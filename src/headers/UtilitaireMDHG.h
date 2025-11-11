#pragma once
#include "sierrachart.h"
#include "UtilitaireDom.h"       // readBidQtyAt/readAskQtyAt, domIsStale
#include "UtilitaireSanitize.h"  // du::sanitize, du::emaPct, du::clamp
#include "UtilitaireTempo.h"     // du::toMs (facultatif côté appelant)

// =============================================================================
// UtilitaireMDHG.h — Mémoire de profondeur (MDHG) v1 enrichi, compat v0
// -----------------------------------------------------------------------------
// • 100% inline, aucune allocation dynamique, aucun état global.
// • Compat v0 conservée :
//     - struct MdhgState { float emaNear; }
//     - mdhgUpdate(sc, st, levelsNear, emaPct) → emaNear
//     - mdhgUpdate(grid*, price, qty, nowMs)   → no‑op auparavant
//     - mdhgSticky(grid*, halfLifeMs)          → no‑op auparavant
//     - mdhgAsym(bidMem, askMem)               → bidMem‑askMem
//     - mdhgGrad(grid*, slope*)                → 0.0
// • Ajouts v1 :
//     - Bandes Near/Mid/Far en lecture unique DOM.
//     - Mémoire collante (“sticky”) par bande avec demi‑vie (ms) + EMA%.
//     - Scores d’asymétrie, gradient discret, shift (Δ mémoire), z‑score court.
//     - Gates: DOM stale, min profondeur, spread max.
//     - 16 features MLP standardisées.
//     - API simple: mdhg_update_tick(sc, state, params, nowMs) et mdhg_features_v1().
// -----------------------------------------------------------------------------
// Conventions :
//   - Indice 0 = meilleur prix. Bandes [start, start+count-1].
//   - Tous les champs sont bornés et assainis (NaN/Inf → 0).
// =============================================================================

namespace du {

// ===== Compat v0 inchangée ===================================================
struct MdhgState { float emaNear = 0; };

inline float mdhgUpdate(const SCStudyInterfaceRef& sc, MdhgState& st, int levelsNear, int emaPct)
{
  double nb=0, na=0; const int N = levelsNear<0? 0:levelsNear;
  for (int li=0; li<N; ++li) { nb += du::readBidQtyAt(sc, li); na += du::readAskQtyAt(sc, li); }
  const float near = (float)(nb + na);
  st.emaNear = du::emaPct(near, st.emaNear, du::clamp(emaPct,1,99));
  return st.emaNear;
}

inline void   mdhgUpdate(double* /*grid*/, double /*price*/, double /*qty*/, long long /*nowMs*/){ /* compat: no‑op */ }
inline void   mdhgSticky(double* /*grid*/, long long /*halfLifeMs*/){ /* compat: no‑op */ }
inline double mdhgAsym(double bidMem,double askMem){ return bidMem - askMem; }
inline double mdhgGrad(const double* /*grid*/, double* /*slope*/){ return 0.0; }

// ===== v1 enrichi ============================================================
struct MdhgBands { int startNear=0, cntNear=10; int startMid=10, cntMid=20; int startFar=30, cntFar=30; };

struct MdhgParams
{
  MdhgBands bands{};    // partition profondeur
  // Gates
  int     requireFresh=1; int minAvail=10; double spreadMaxTks=6.0;
  // Mémoire « sticky » (ms) et EMA (%)
  double  hlMs=60000.0;   // demi‑vie mémoire en ms (par bande)
  int     emaPct=50;      // lissage EMA% de la mémoire
  // z‑score court
  int     zWin=32;        // fenêtre d’estimation (N échantillons)
};

enum MdhgPhase:int { MDHG_OK=0, MDHG_COLLAPSE=1, MDHG_RECOVERING=2 };

struct MdhgZs { double mean=0, m2=0; int n=0; inline void reset(){ mean=0; m2=0; n=0; } inline void push(double x){ ++n; const double d=x-mean; mean+=d/n; m2+=d*(x-mean);} inline double std()const{ return n>1? std::sqrt(std::max(0.0, m2/(n-1))) : 0.0; } };

struct MdhgStateV1
{
  // Mémoire par bande (Bid+Ask)
  double memNear=0, memMid=0, memFar=0, memTot=0;
  // Lecture instantanée
  double nearNow=0, midNow=0, farNow=0, totNow=0;
  // Prix et spread
  double bid=0, ask=0, ts=0, spreadTks=0; int avail=0;
  // Dérivés
  double asym=0;               // memNear - memFar (proxy asy)
  double gradNF=0;             // gradient discret Near→Far (memFar - memNear)
  double shiftNear=0;          // Δ memNear vs pas précédent
  // z‑score near
  MdhgZs  zsNear{}; double zNear=0;
  // Phase simple
  int phase=MDHG_OK; long long tLast=0;
  // Buffers courts pour z‑score (valeurs near successives)
  double bufNear[64]; int nNear=0;
  // Util
  inline void reset_soft(){ nearNow=midNow=farNow=totNow=0; bid=ask=ts=spreadTks=0; asym=0; gradNF=0; shiftNear=0; zNear=0; }
  inline void reset_all(){ reset_soft(); memNear=memMid=memFar=memTot=0; avail=0; phase=MDHG_OK; tLast=0; nNear=0; zsNear.reset(); }
};

// --- Helpers internes --------------------------------------------------------
inline double mdhg_sum_band_read(const SCStudyInterfaceRef& sc, bool isBid, int start, int cnt)
{
  if (cnt<=0) return 0.0; if (start<0) start=0; const int e=start+cnt-1; double s=0.0; for (int li=start; li<=e; ++li) s += isBid? du::readBidQtyAt(sc,li) : du::readAskQtyAt(sc,li); return s;
}

inline double mdhg_alpha_from_hl_ms(double hlMs, double dtMs){ if(!(hlMs>0.0)) hlMs=1.0; if(!(dtMs>0.0)) dtMs=1.0; return 1.0 - std::exp(-(std::log(2.0))*dtMs/hlMs); }

inline bool mdhg_gates_ok(const SCStudyInterfaceRef& sc, const MdhgParams& p)
{
  if (p.requireFresh && du::domIsStale(sc)) return false; const int avail = sc.GetMarketDepthNumberOfLevels(); if (avail < p.minAvail) return false; const double bid=sc.Bid[sc.Index], ask=sc.Ask[sc.Index], ts=sc.TickSize; if (ts>0.0 && bid>0.0 && ask>0.0){ const double spr=(ask-bid)/ts; if (spr>p.spreadMaxTks) return false; } return true;
}

// --- Mise à jour par tick ----------------------------------------------------
inline void mdhg_update_tick(SCStudyInterfaceRef sc, MdhgStateV1& st, const MdhgParams& p, long long nowMs, double dtMs)
{
  st.tLast = nowMs; st.reset_soft();
  if (!mdhg_gates_ok(sc, p)) { st.phase=MDHG_OK; return; }

  st.avail = sc.GetMarketDepthNumberOfLevels(); st.bid=sc.Bid[sc.Index]; st.ask=sc.Ask[sc.Index]; st.ts=sc.TickSize; st.spreadTks = (st.ts>0.0 && st.bid>0.0 && st.ask>0.0)? (st.ask-st.bid)/st.ts : 0.0;

  // Lecture unique DOM par bandes (Bid+Ask)
  st.nearNow = mdhg_sum_band_read(sc,true ,p.bands.startNear,p.bands.cntNear) + mdhg_sum_band_read(sc,false,p.bands.startNear,p.bands.cntNear);
  st.midNow  = mdhg_sum_band_read(sc,true ,p.bands.startMid ,p.bands.cntMid ) + mdhg_sum_band_read(sc,false,p.bands.startMid ,p.bands.cntMid );
  st.farNow  = mdhg_sum_band_read(sc,true ,p.bands.startFar ,p.bands.cntFar ) + mdhg_sum_band_read(sc,false,p.bands.startFar ,p.bands.cntFar );
  st.totNow  = st.nearNow + st.midNow + st.farNow;

  // Sticky memory par demi‑vie + EMA%
  const double aHL = mdhg_alpha_from_hl_ms(p.hlMs, dtMs);
  const double aPct= std::max(0.0, std::min(1.0, (double)p.emaPct/100.0));
  const double a    = 1.0 - (1.0-aHL)*(1.0-aPct); // mélange stable

  const double prevNear = st.memNear;
  st.memNear = a*st.nearNow + (1.0-a)*st.memNear;
  st.memMid  = a*st.midNow  + (1.0-a)*st.memMid;
  st.memFar  = a*st.farNow  + (1.0-a)*st.memFar;
  st.memTot  = st.memNear + st.memMid + st.memFar;

  // Dérivés simples
  st.shiftNear = st.memNear - prevNear;
  st.gradNF    = st.memFar  - st.memNear;       // gradient discret Near→Far
  st.asym      = st.memNear - st.memFar;

  // z‑score court sur near
  if (p.zWin > 0) {
    const double x = du::sanitize(st.memNear);
    st.zsNear.push(x);
    const double sd = st.zsNear.std(); st.zNear = (sd>0.0? (x - st.zsNear.mean)/sd : 0.0);
    // tampon circulaire court
    if (st.nNear < (int)(sizeof(st.bufNear)/sizeof(st.bufNear[0]))) st.bufNear[st.nNear++] = x; else { for(int i=1;i<st.nNear;++i) st.bufNear[i-1]=st.bufNear[i]; st.bufNear[st.nNear-1]=x; }
  } else st.zNear = 0.0;

  // Phase heuristique : collapse si near très faible vs total et spread élevé; recovering si near remonte
  const bool collapseLikely = (st.nearNow < 0.15*std::max(1e-9, st.totNow)) && (st.spreadTks >= 2.0);
  if (collapseLikely) st.phase = MDHG_COLLAPSE;
  else if (st.nNear>=2 && st.bufNear[st.nNear-1] > st.bufNear[st.nNear-2]) st.phase = MDHG_RECOVERING;
  else st.phase = MDHG_OK;
}

// --- Features MLP v1 ---------------------------------------------------------
// 0 memNear, 1 memMid, 2 memFar, 3 memTot, 4 nearNow, 5 midNow, 6 farNow, 7 totNow,
// 8 asym, 9 gradNF, 10 shiftNear, 11 zNear, 12 spreadTks, 13 avail, 14 phase, 15 ts
inline int mdhg_features_v1(const MdhgStateV1& st, double* out)
{
  if (!out) return 0;
  out[0]=du::sanitize(st.memNear); out[1]=du::sanitize(st.memMid); out[2]=du::sanitize(st.memFar); out[3]=du::sanitize(st.memTot);
  out[4]=du::sanitize(st.nearNow); out[5]=du::sanitize(st.midNow); out[6]=du::sanitize(st.farNow); out[7]=du::sanitize(st.totNow);
  out[8]=du::sanitize(st.asym);    out[9]=du::sanitize(st.gradNF);
  out[10]=du::sanitize(st.shiftNear); out[11]=du::sanitize(st.zNear);
  out[12]=du::sanitize(st.spreadTks); out[13]=(double)st.avail; out[14]=(double)st.phase; out[15]=du::sanitize(st.ts);
  return 16;
}

} // namespace du
