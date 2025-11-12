#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"
#include "UtilitaireTempo.h"     // du::toMs if needed by caller
#include "UtilitaireDom.h"       // du::readBidQtyAt(), du::readAskQtyAt(), du::domIsStale()

// =============================================================================
// UtilitaireLiqRisk.h — Risque de liquidité (v1 enrichi, compat v0)
// -----------------------------------------------------------------------------
// • 100% inline, aucune allocation, aucun état global.
// • Compat v0 préservée: lrRiskScore(), lrNearIndex(), lrCollapseFlag(), lrRecoveryHalfLife().
// • Ajouts v1:
//   - Lecture unique DOM avec bandes Near/Mid/Far.
//   - Scores: spreadTicks, depthNear/Mid/Far/Total, risk = spreadTicks / depthNear.
//   - Effondrement (collapse) et récupération (recovery half‑life) basés sur série depth.
//   - Gates: DOM stale, profondeur minimale, spread max.
//   - 16 features MLP standardisées.
//   - API simple: lr_update_tick(), lr_features_v1().
// -----------------------------------------------------------------------------
// Conventions: indices de profondeur 0 = meilleur prix. Bandes [start, start+count-1].
// =============================================================================

namespace du {

// -----------------------------------------------------------------------------
// ===== Compat v0 inchangée ===================================================
// -----------------------------------------------------------------------------
inline float 	lrRiskScore			(const SCStudyInterfaceRef& sc, int nearLevels)
{
  const float bid = (float)sc.Bid[sc.Index];
  const float ask = (float)sc.Ask[sc.Index];
  const float spread = (ask>bid? ask - bid : 0.0f);
  double depth=0.0; const int N = nearLevels<0? 0 : nearLevels;
  for (int li=0; li<N; ++li) depth += du::readBidQtyAt(sc, li) + du::readAskQtyAt(sc, li);
  if (depth <= 0.0) return 0.0f; return (float)( (spread>0.0f? spread:0.0f) * 1000.0 / depth );
}

inline int  	lrNearIndex			(double price, const double* zones, int n)
{ int idx=0; double dist=1e300; for(int i=0;i<n;++i){ const double d = std::fabs(price - (zones?zones[i]:0.0)); if(d<dist){ dist=d; idx=i; } } return idx; }

inline bool 	lrCollapseFlag		(double depthNear, double depthFar, double thr)	{ return (depthNear + depthFar) < thr; }

inline double 	lrRecoveryHalfLife	(const double* depthSeries, int n)
{ // estimation simple: temps pour remonter de moitié entre min et base
  if(!depthSeries || n<3) return 0.0; const double base = depthSeries[0]; double mn=base; int iMin=0;
  for(int i=1;i<n;++i){ if(depthSeries[i] < mn){ mn=depthSeries[i]; iMin=i; } }
  const double target = mn + 0.5*(base - mn); for(int i=iMin;i<n;++i){ if(depthSeries[i] >= target) return (double)(i - iMin); }
  return 0.0;
}

// -----------------------------------------------------------------------------
// ===== v1 enrichi ============================================================
// -----------------------------------------------------------------------------
struct 			LrBands 			{ int startNear=0, cntNear=10; int startMid=10, cntMid=20; int startFar=30, cntFar=30; };

struct 			LrParams
{
  LrBands bands{};           // partition profondeur
  int     minAvail   = 10;   // profondeur minimale requise
  double  spreadMaxT = 8.0;  // spread max en ticks (gate)
  int     requireFresh=1;    // gate DOM stale
  int     seriesLen  = 64;   // taille max série depth pour recovery
};

enum 			LrPhase:int 		{ LR_OK=0, LR_COLLAPSE=1, LR_RECOVERING=2 };

struct 			LrState
{
  // Profondeurs par bande (Bid+Ask)
  double near=0.0, mid=0.0, far=0.0, total=0.0;
  // Prix & spread
  double bid=0.0, ask=0.0, tick=0.0, spreadTks=0.0;
  // Score de risque simple
  double risk=0.0; // spreadTicks / near
  // Phase
  int phase=LR_OK; long long tLast=0; int avail=0;
  // Série depth pour recovery (côté near)
  // L’appelant peut gérer l’horodatage externe; ici on empile valeurs scalaires
  int seriesN=0; double depthSeries[128];

  inline void reset_soft(){ near=mid=far=total=0; bid=ask=tick=spreadTks=0; risk=0; }
  inline void reset_all(){ reset_soft(); phase=LR_OK; tLast=0; avail=0; seriesN=0; }
};

// Utilitaires -----------------------------------------------------------------
inline double 	lr_sum_range_read	(const SCStudyInterfaceRef& sc, bool isBid, int start, int count)
{
  if (count<=0) return 0.0; if (start<0) start=0; const int e=start+count-1; double s=0.0;
  for (int li=start; li<=e; ++li) s += isBid? du::readBidQtyAt(sc,li) : du::readAskQtyAt(sc,li);
  return s;
}

inline bool 	lr_gates_ok			(const SCStudyInterfaceRef& sc, const LrParams& p)
{
  if (p.requireFresh && du::domIsStale(sc)) return false;
  const int avail = sc.GetMarketDepthNumberOfLevels(); if (avail < p.minAvail) return false;
  const double bid = sc.Bid[sc.Index], ask = sc.Ask[sc.Index], ts = sc.TickSize; if (ts>0.0 && bid>0.0 && ask>0.0) {
    const double spr = (ask - bid) / ts; if (spr > p.spreadMaxT) return false; }
  return true;
}

// Mise à jour par tick. nowMs facultatif. Retourne true si collapse ou recovering détecté.
inline bool 	lr_update_tick		(SCStudyInterfaceRef sc, LrState& st, const LrParams& p, long long nowMs, int& outPhase)
{
  outPhase = st.phase; st.tLast = nowMs; st.reset_soft();
  if (!lr_gates_ok(sc, p)) { st.phase = LR_OK; outPhase = st.phase; return false; }

  st.avail = sc.GetMarketDepthNumberOfLevels();
  st.bid = sc.Bid[sc.Index]; st.ask = sc.Ask[sc.Index]; st.tick = sc.TickSize;
  st.spreadTks = (st.tick>0.0 && st.bid>0.0 && st.ask>0.0)? (st.ask - st.bid)/st.tick : 0.0;

  // Lecture unique bandes
  st.near = lr_sum_range_read(sc, true , p.bands.startNear, p.bands.cntNear)
          + lr_sum_range_read(sc, false, p.bands.startNear, p.bands.cntNear);
  st.mid  = lr_sum_range_read(sc, true , p.bands.startMid , p.bands.cntMid )
          + lr_sum_range_read(sc, false, p.bands.startMid , p.bands.cntMid );
  st.far  = lr_sum_range_read(sc, true , p.bands.startFar , p.bands.cntFar )
          + lr_sum_range_read(sc, false, p.bands.startFar , p.bands.cntFar );
  st.total= st.near + st.mid + st.far;

  // Score de risque
  st.risk = (st.near>1e-9? st.spreadTks / st.near : 0.0);

  // Série pour recovery (near)
  if (p.seriesLen > 0) {
    if (st.seriesN < (int)(sizeof(st.depthSeries)/sizeof(st.depthSeries[0]))) {
      st.depthSeries[st.seriesN++] = st.near;
    } else { // shift 1 à gauche
      for(int i=1;i<st.seriesN;++i) st.depthSeries[i-1] = st.depthSeries[i];
      st.depthSeries[st.seriesN-1] = st.near;
    }
  }

  // Détection simple de collapse/recovery: seuils adaptables par appelant via analyse des features
  // Ici, on bascule en COLLAPSE si near << total et spread élevé; sinon RECOVERING si série remonte.
  const bool collapseLikely = (st.near < 0.15 * std::max(1e-9, st.total)) && (st.spreadTks >= 2.0);
  if (collapseLikely) st.phase = LR_COLLAPSE;
  else if (st.seriesN >= 3 && st.depthSeries[st.seriesN-1] > st.depthSeries[st.seriesN-2]) st.phase = LR_RECOVERING;
  else st.phase = LR_OK;

  outPhase = st.phase; return (st.phase != LR_OK);
}

// -----------------------------------------------------------------------------
// Features MLP v1 — 16 features
// 0 near, 1 mid, 2 far, 3 total, 4 bid, 5 ask, 6 tick, 7 spreadTks,
// 8 risk, 9 avail, 10 phase, 11 fracNear=near/total, 12 fracMid, 13 fracFar,
// 14 collapseFlag(heur), 15 recoveryHL (échantillons)
// -----------------------------------------------------------------------------
inline int 	lr_features_v1			(const LrState& st, const LrParams& /*p*/, double* out)
{
  if (!out) return 0; const double tot = std::max(1e-9, st.total);
  out[0] = du::sanitize(st.near);
  out[1] = du::sanitize(st.mid);
  out[2] = du::sanitize(st.far);
  out[3] = du::sanitize(st.total);
  out[4] = du::sanitize(st.bid);
  out[5] = du::sanitize(st.ask);
  out[6] = du::sanitize(st.tick);
  out[7] = du::sanitize(st.spreadTks);
  out[8] = du::sanitize(st.risk);
  out[9] = (double)st.avail;
  out[10]= (double)st.phase;
  out[11]= du::sanitize(st.near / tot);
  out[12]= du::sanitize(st.mid  / tot);
  out[13]= du::sanitize(st.far  / tot);
  const double collapseHeur = (st.near < 0.15*tot && st.spreadTks >= 2.0) ? 1.0 : 0.0;
  out[14]= collapseHeur;
  // Recovery half‑life estimée en nombre d’échantillons depuis le dernier minimum
  double hl = 0.0; if (st.seriesN >= 3) hl = lrRecoveryHalfLife(st.depthSeries, st.seriesN);
  out[15]= du::sanitize(hl);
  return 16;
}

} // namespace du
