#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"
#include "UtilitaireTempo.h"      // du::toMs
#include "UtilitaireDom.h"        // du::readBidQtyAt(), du::readAskQtyAt(), du::domIsStale()

// =============================================================================
// UtilitaireDomVacuum.h — Détection de "vacuum" DOM: chutes de liquidité
// v1 enrichi, 100% inline, aucune allocation, aucun état global. Compat v0 OK.
// -----------------------------------------------------------------------------
// Compat v0 conservée :
//   float  vacNearDrop(sc, nearLevels, prevNearSum)
//   bool   duVacuumDrop(prev[], now[], avail, thrAbs, thrPct, minLifeMs, nowMs)
//   double duVacuumScore(nearLoss, farLoss)
// -----------------------------------------------------------------------------
// Ajouts v1 :
//   - Lecture snapshot unique Bid/Ask par tick, avec disponibilité réelle.
//   - Agrégats par bandes Near/Mid/Far et métriques de pertes absolues/relatives.
//   - Moteur vacuum avec hystérésis, minLifeMs, cooldown, score EMA.
//   - Gates optionnels: DOM stale, minDepth, spread.
//   - 16 features MLP standardisées.
// -----------------------------------------------------------------------------
// Conventions: indices de profondeur 0 = meilleur prix. Bandes [start, start+count-1].
// =============================================================================

namespace du {

// -----------------------------------------------------------------------------
// Compat v0 (inchangé)
// -----------------------------------------------------------------------------
inline float 	vacNearDrop					(const SCStudyInterfaceRef& sc, int nearLevels, double prevNearSum)
{
  double now=0.0; const int N = nearLevels<0? 0 : nearLevels;
  for (int li=0; li<N; ++li) now += du::readBidQtyAt(sc, li) + du::readAskQtyAt(sc, li);
  return (float)(prevNearSum - now); // >0 si chute
}

inline bool 	duVacuumDrop				(const double* prev,const double* now,int avail,double thrAbs,double thrPct,int /*minLifeMs*/,long long /*nowMs*/)
{
  double loss=0, base=0; for(int i=0;i<avail;++i){ const double p = prev?prev[i]:0.0; const double n = now?now[i]:0.0; if (p>n) loss += (p-n); base += p; }
  return loss>thrAbs || (base>1e-12 && loss/base > thrPct);
}

inline double 	duVacuumScore				(double nearLoss,double farLoss){ return 0.7*nearLoss + 0.3*farLoss; }

// -----------------------------------------------------------------------------
// v1 — enrichi
// -----------------------------------------------------------------------------
struct 			DvBands
{
  // Bandes côté BID et ASK sont symétriques, on lit les deux et on somme.
  int startNear = 0;  int countNear = 10;
  int startMid  = 10; int countMid  = 20;
  int startFar  = 30; int countFar  = 30; // jusqu'à 59 → 60 niveaux
};

struct 			DvParams
{
  DvBands bands{};
  double alphaPct     = 70.0;   // EMA du score
  double thrAbs       = 50.0;   // perte absolue minimale (quantités)
  double thrPct       = 0.20;   // perte relative vs base
  double thrOnScore   = 1.0;    // score pour armement
  double thrOffScore  = 0.6;    // score pour relâchement
  double minLifeMs    = 150.0;  // durée minimale d’un épisode de vacuum
  int    cooldownMs   = 300;    // délai post‑événement
  int    minAvail     = 10;     // profondeur minimale requise
  int    requireFresh = 1;      // refuser si DOM "stale"
  double spreadMaxTks = 4.0;    // gate spread
};

enum 			DvPhase : int 				{ DV_IDLE=0, DV_ARMED=1, DV_TRIGGERED=2, DV_COOLDOWN=3 };

struct 			DvAgg
{
  // Sums par bande (Bid+Ask)
  double near=0, mid=0, far=0, total=0;
};

struct 			DvLoss
{
  // Pertes punitives p>=n ? p-n : 0 ; rel = loss / max(p,eps)
  double nearAbs=0, midAbs=0, farAbs=0, totAbs=0;
  double nearRel=0, midRel=0, farRel=0, totRel=0;
};

struct 			DvState
{
  // Derniers agrégats
  DvAgg  prev{};     // tick précédent
  DvAgg  now{};      // courant
  DvLoss loss{};     // pertes calculées prev→now

  // Score
  double score=0.0;  // instantané
  double scoreEma=0.0; // lissé

  // Phase & temps
  int        phase = DV_IDLE;
  long long  tFirstLoss = 0;  // ms premier tick où loss>=seuils
  long long  tLast      = 0;  // ms dernier update
  long long  tTrig      = 0;  // ms dernier trigger

  // Compteurs
  int consecOver=0;
  int consecUnder=0;
  int nTriggers=0;

  void reset_soft(){ prev=DvAgg{}; now=DvAgg{}; loss=DvLoss{}; score=scoreEma=0; consecOver=consecUnder=0; }
  void reset_all (){ reset_soft(); phase=DV_IDLE; tFirstLoss=tLast=tTrig=0; nTriggers=0; }
};

// Utilitaires -----------------------------------------------------------------
inline double 	dv_sum_range_read			(const SCStudyInterfaceRef& sc, bool isBid, int start, int count)
{
  if (start<0) start=0; if (count<=0) return 0.0; const int end = start + count - 1; double s=0.0;
  for (int li=start; li<=end; ++li) s += isBid? du::readBidQtyAt(sc, li) : du::readAskQtyAt(sc, li);
  return s;
}

inline 			DvAgg dv_read_agg_bands		(const SCStudyInterfaceRef& sc, const DvBands& b)
{
  DvAgg a{};
  const double bN = dv_sum_range_read(sc, /*bid*/true , b.startNear, b.countNear);
  const double aN = dv_sum_range_read(sc, /*bid*/false, b.startNear, b.countNear);
  const double bM = dv_sum_range_read(sc, /*bid*/true , b.startMid , b.countMid );
  const double aM = dv_sum_range_read(sc, /*bid*/false, b.startMid , b.countMid );
  const double bF = dv_sum_range_read(sc, /*bid*/true , b.startFar , b.countFar );
  const double aF = dv_sum_range_read(sc, /*bid*/false, b.startFar , b.countFar );
  a.near = bN + aN; a.mid = bM + aM; a.far = bF + aF; a.total = a.near + a.mid + a.far; return a;
}

inline 			DvLoss dv_compute_loss		(const DvAgg& P, const DvAgg& N)
{
  auto rel = [](double loss, double base){ return base>1e-12? loss/base : 0.0; };
  DvLoss L{}; L.nearAbs = std::max(0.0, P.near - N.near); L.midAbs  = std::max(0.0, P.mid - N.mid);
  L.farAbs  = std::max(0.0, P.far  - N.far ); L.totAbs  = std::max(0.0, P.total- N.total);
  L.nearRel = rel(L.nearAbs, P.near); L.midRel = rel(L.midAbs, P.mid); L.farRel = rel(L.farAbs, P.far); L.totRel = rel(L.totAbs, P.total);
  return L;
}

inline double 	dv_score					(const DvLoss& L)
{ // pondération near/mid/far
  return 0.60*L.nearRel + 0.30*L.midRel + 0.10*L.farRel + 0.001*L.totAbs; // bonus absolu léger
}

inline double 	dv_ema						(double prev,double x,double aPct){ const double a = aPct>0.0? aPct/100.0 : 0.0; return prev + a*(x - prev); }

// Gates simples: stale DOM, profondeur min, spread max
inline bool 	dv_gates_ok					(const SCStudyInterfaceRef& sc, const DvParams& p)
{
  if (p.requireFresh && du::domIsStale(sc)) return false;
  // profondeur dispo
  const int avail = sc.GetMarketDepthNumberOfLevels(); if (avail < p.minAvail) return false;
  // spread
  const double bid = sc.Bid[sc.Index]; const double ask = sc.Ask[sc.Index]; const double ts = sc.TickSize;
  if (ts>0.0 && bid>0.0 && ask>0.0) { const double spr = (ask-bid)/ts; if (spr > p.spreadMaxTks) return false; }
  return true;
}

// Mise à jour par tick. Retourne true si événement vacuum déclenché sur CE tick.
inline bool 	dv_update_tick				(SCStudyInterfaceRef sc, DvState& st, const DvParams& p, long long nowMs, int& outPhase)
{
  outPhase = st.phase; st.tLast = nowMs;

  if (!dv_gates_ok(sc, p)) { st.consecUnder++; st.consecOver=0; if (st.phase==DV_ARMED) st.phase=DV_IDLE; return false; }

  // Lecture unique
  st.now = dv_read_agg_bands(sc, p.bands);

  // Première init si prev vide
  if (st.prev.total <= 0.0) { st.prev = st.now; st.loss = DvLoss{}; st.score = st.scoreEma = 0.0; return false; }

  // Pertes prev→now
  st.loss = dv_compute_loss(st.prev, st.now);
  st.prev = st.now; // shift pour le tick suivant

  // Score instant + EMA
  st.score   = du::sanitize(dv_score(st.loss));
  st.scoreEma= dv_ema(st.scoreEma, st.score, p.alphaPct);

  // Hystérésis & minLife
  const bool over = (st.scoreEma >= p.thrOnScore) && (st.loss.totAbs > p.thrAbs || st.loss.totRel > p.thrPct);
  const bool under= (st.scoreEma <= p.thrOffScore);

  if (over) { st.consecOver++; st.consecUnder=0; if (st.phase==DV_IDLE){ st.phase=DV_ARMED; st.tFirstLoss = nowMs; } }
  if (under){ st.consecUnder++; st.consecOver=0; if (st.phase==DV_ARMED){ st.phase=DV_IDLE; st.tFirstLoss=0; } }

  // Cooldown actif ?
  if (st.phase==DV_COOLDOWN) {
    if (nowMs - st.tTrig >= p.cooldownMs) st.phase = DV_IDLE; else { outPhase = st.phase; return false; }
  }

  // Condition de déclenchement
  const bool lifeOK = (st.phase==DV_ARMED) && (st.tFirstLoss>0) && ((nowMs - st.tFirstLoss) >= p.minLifeMs);
  if (lifeOK && over) {
    st.phase = DV_TRIGGERED; st.tTrig = nowMs; st.nTriggers += 1; outPhase = st.phase; st.phase = DV_COOLDOWN; return true;
  }

  outPhase = st.phase; return false;
}

// -----------------------------------------------------------------------------
// Features MLP v1 — 16 features
// 0 score, 1 score_ema, 2 nearAbs, 3 midAbs, 4 farAbs, 5 totAbs,
// 6 nearRel, 7 midRel, 8 farRel, 9 totRel, 10 phase, 11 consecOver,
// 12 consecUnder, 13 dt_since_first_ms, 14 dt_since_trig_ms, 15 total_now
// -----------------------------------------------------------------------------
inline int 		dv_features_v1				(const DvState& st, const DvParams& /*p*/, long long nowMs, double* out)
{
  if(!out) return 0; const long long dtFirst = (st.tFirstLoss>0? nowMs - st.tFirstLoss : 0); const long long dtTrig = (st.tTrig>0? nowMs - st.tTrig : 0);
  out[0] = du::sanitize(st.score);
  out[1] = du::sanitize(st.scoreEma);
  out[2] = du::sanitize(st.loss.nearAbs);
  out[3] = du::sanitize(st.loss.midAbs);
  out[4] = du::sanitize(st.loss.farAbs);
  out[5] = du::sanitize(st.loss.totAbs);
  out[6] = du::sanitize(st.loss.nearRel);
  out[7] = du::sanitize(st.loss.midRel);
  out[8] = du::sanitize(st.loss.farRel);
  out[9] = du::sanitize(st.loss.totRel);
  out[10]= (double)st.phase;
  out[11]= (double)st.consecOver;
  out[12]= (double)st.consecUnder;
  out[13]= (double)dtFirst;
  out[14]= (double)dtTrig;
  out[15]= du::sanitize(st.now.total);
  return 16;
}

} // namespace du
