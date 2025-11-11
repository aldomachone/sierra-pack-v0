#pragma once
#include "sierrachart.h"
#include "UtilitaireSanize.h"
#include "UtilitaireTempo.h"      // du::toMs
#include "UtilitaireTapeBVC.h"    // du::bvcDelta

// =============================================================================
// UtilitaireStopRun.h — Détection stop‑run: score, gates, hystérésis, état (v1)
// -----------------------------------------------------------------------------
// • 100% inline, aucune allocation, aucun état global.
// • Compat v0 maintenue (srTriggerSimple, duStopRunGate, duStopRunScore).
// • Ajouts:
//   - Score composite OFI/Sweep/Vacuum/Imbalance + |Δ| (BVC) lissé EMA.
//   - Gates MicroPrice/Pace optionnels.
//   - Hystérésis On/Off, réfractaire, cooldown, phase state‑machine.
//   - Direction du run, compteurs, métriques temps.
//   - 16 features MLP standardisées.
// -----------------------------------------------------------------------------
// Convention: les entrées de score sont déjà normalisées par l'appelant.
//             tick engines: appeler sr_update_tick() à chaque tick.
// =============================================================================

namespace du {

// -----------------------------------------------------------------------------
// Compat v0 (inchangé)
// -----------------------------------------------------------------------------
inline bool srTriggerSimple(const SCStudyInterfaceRef& sc, int idx, float thrAbsDelta)
{ return fabsf(du::bvcDelta(sc, idx)) >= thrAbsDelta; }

inline bool duStopRunGate(int /*micropriceState*/, bool /*paceGate*/){ return true; }
inline double duStopRunScore(double ofi,double sweep,double vacuum,double imbalance)
{ return 0.4*ofi + 0.3*sweep + 0.2*vacuum + 0.1*imbalance; }

// -----------------------------------------------------------------------------
// v1 — enrichi
// -----------------------------------------------------------------------------
struct SrParams
{
  // Lissage et pondération
  double alphaPct      = 70.0;   // EMA du score
  double wOFI          = 0.35;
  double wSweep        = 0.30;
  double wVacuum       = 0.20;
  double wImbalance    = 0.10;
  double wAbsDelta     = 0.05;   // bonus par |Δ| BVC

  // Seuils et timings
  double thrOn         = 1.00;   // déclenchement
  double thrOff        = 0.60;   // relâchement
  int    refractoryMs  = 100;    // N ticks consécutifs Δt court ≈ run soutenu
  int    cooldownMs    = 400;    // après trigger
  double minAbsDelta   = 1.0;    // |Δ| minimal pour valider

  // Gates optionnels
  int    requireMicroOK = 1;     // 0/1
  int    requirePaceOK  = 1;     // 0/1
};

enum SrPhase : int { SR_IDLE=0, SR_ARMED=1, SR_TRIGGERED=2, SR_COOLDOWN=3 };

struct SrState
{
  // Score
  double score      = 0.0;  // instantané
  double scoreEma   = 0.0;  // lissé

  // Δ et direction
  double absDelta   = 0.0;  // |BVC Δ|
  int    dir        = 0;    // +1 up run, −1 down run

  // Gates
  int microOK = 1; int paceOK = 1;

  // Phase & temps
  int        phase          = SR_IDLE;
  long long  tLast          = 0;      // ms du dernier tick
  long long  tTriggered     = 0;      // ms du dernier trigger
  long long  tArmed         = 0;      // premier passage au‑dessus de thrOn

  // Compteurs
  int consecOver = 0;  // ticks consécutifs scoreEma ≥ thrOn
  int consecUnder= 0;  // ticks consécutifs scoreEma ≤ thrOff
  int nTriggers  = 0;  // total triggers

  // Util
  void reset_soft(){ score=scoreEma=0; absDelta=0; dir=0; consecOver=consecUnder=0; microOK=paceOK=1; }
  void reset_all() { reset_soft(); phase=SR_IDLE; tLast=tTriggered=tArmed=0; nTriggers=0; }
};

inline double sr_clamp(double x,double lo,double hi){ if(x<lo) return lo; if(x>hi) return hi; return x; }
inline double sr_ema(double prev,double x,double aPct){ const double a = aPct>0.0? aPct/100.0 : 0.0; return prev + a*(x - prev); }

// Calcul du score composite instantané
inline double sr_score_instant(const SrParams& p, double ofi,double sweep,double vacuum,double imbalance,double absDelta)
{
  return p.wOFI*ofi + p.wSweep*sweep + p.wVacuum*vacuum + p.wImbalance*imbalance + p.wAbsDelta*absDelta;
}

inline bool sr_gates_ok(const SrParams& p, int microOK, int paceOK)
{ return (p.requireMicroOK? microOK!=0 : true) && (p.requirePaceOK? paceOK!=0 : true); }

// Mise à jour par tick. nowMs monotone.
// Retourne true si trigger sur CE tick. outDir reçoit {+1,-1}.
inline bool sr_update_tick(SrState& st, const SrParams& p,
                           long long nowMs,
                           double ofi,double sweep,double vacuum,double imbalance,
                           double absDeltaFromBvc,
                           int microOK,int paceOK,
                           int& outDir)
{
  outDir = 0; st.tLast = nowMs; st.absDelta = du::sanitize(absDeltaFromBvc); st.microOK = microOK; st.paceOK = paceOK;

  // Score instant + EMA
  st.score   = du::sanitize(sr_score_instant(p, ofi, sweep, vacuum, imbalance, std::fabs(st.absDelta)));
  st.scoreEma= sr_ema(st.scoreEma, st.score, p.alphaPct);

  // Direction proposée par signe OFI + complément |Δ|
  st.dir = (ofi>0 ? +1 : (ofi<0 ? -1 : (st.absDelta>=p.minAbsDelta? (imbalance>=0? +1 : -1) : 0)));

  // Cooldown actif ?
  if (st.phase == SR_COOLDOWN) {
    if (nowMs - st.tTriggered >= p.cooldownMs) { st.phase = SR_IDLE; st.consecOver=st.consecUnder=0; }
    else return false;
  }

  // Gates
  const bool gates = sr_gates_ok(p, microOK, paceOK);

  // Hystérésis
  if (st.scoreEma >= p.thrOn && gates) { st.consecOver++; st.consecUnder=0; if (st.phase == SR_IDLE) { st.phase=SR_ARMED; st.tArmed=nowMs; } }
  else if (st.scoreEma <= p.thrOff || !gates) { st.consecUnder++; st.consecOver=0; if (st.phase==SR_ARMED) st.phase=SR_IDLE; }

  // Réfractaire: considérer un run valide si maintien au‑dessus de thrOn pendant refractoryMs
  const bool refractoryOK = (st.phase==SR_ARMED) && ((nowMs - st.tArmed) >= p.refractoryMs);

  // Condition de déclenchement
  const bool cond = refractoryOK && (std::fabs(st.absDelta) >= p.minAbsDelta) && st.dir!=0;
  if (cond) {
    st.phase = SR_TRIGGERED; st.tTriggered = nowMs; st.nTriggers += 1; outDir = st.dir; st.phase = SR_COOLDOWN; return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
// Features MLP v1 — 16 features
// 0 score, 1 score_ema, 2 abs_delta, 3 dir, 4 microOK, 5 paceOK, 6 phase,
// 7 consecOver, 8 consecUnder, 9 dt_since_arm_ms, 10 dt_since_trig_ms,
// 11 n_triggers, 12 thrOn, 13 thrOff, 14 refractory_ms, 15 cooldown_ms
// -----------------------------------------------------------------------------
inline int sr_features_v1(const SrState& st, const SrParams& p, long long nowMs, double* out)
{
  if(!out) return 0; const long long dtArm = (st.tArmed>0? nowMs - st.tArmed : 0); const long long dtTrig = (st.tTriggered>0? nowMs - st.tTriggered : 0);
  out[0]=du::sanitize(st.score);
  out[1]=du::sanitize(st.scoreEma);
  out[2]=du::sanitize(st.absDelta);
  out[3]=(double)st.dir;
  out[4]=(double)st.microOK;
  out[5]=(double)st.paceOK;
  out[6]=(double)st.phase;
  out[7]=(double)st.consecOver;
  out[8]=(double)st.consecUnder;
  out[9]=(double)dtArm;
  out[10]=(double)dtTrig;
  out[11]=(double)st.nTriggers;
  out[12]=du::sanitize(p.thrOn);
  out[13]=du::sanitize(p.thrOff);
  out[14]=(double)p.refractoryMs;
  out[15]=(double)p.cooldownMs; return 16;
}

} // namespace du
