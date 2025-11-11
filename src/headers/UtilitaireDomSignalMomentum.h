#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"   // du::sanitize
#include "UtilitaireTempo.h"      // du::toMs
#include "UtilitaireDom.h"        // du::readBidQtyAt, du::readAskQtyAt (si utilisé par appelant)

// ============================================================================
// UtilitaireDomSignalMomentum.h — Momentum DOM (diff Bid/Ask) v1 enrichi
// ---------------------------------------------------------------------------
// • 100% inline, aucune allocation, aucun état global.
// • Compat v0 conservée :
//     - momPrice1(sc,i)
//     - duMomentumNear(now, prev, avail, alphaPct, outScore)
//     - duMomentumSigned(bidMom, askMom)
//     - duMomentumGate(score, p1, p2, warmupN)
// • Ajouts v1 :
//     - Bandes Near/Mid/Far avec pondération [0.60, 0.30, 0.10].
//     - Normalisations: par base (∑ qty), par ticks (optionnel), z‑score rolling.
//     - EMA des scores, hystérésis On/Off, réfractaire N, cooldown ms.
//     - Helpers de features MLP (16) pour le momentum.
//     - Sûreté: clamps NaN/Inf, garde « avail ».
// ---------------------------------------------------------------------------
// Conventions :
//  - Les tableaux now[] et prev[] représentent des quantités par niveau (>=0).
//  - avail = nombre de niveaux valides (0→avail‑1). Toute lecture hors bornes est ignorée.
//  - Tous les calculs sont purement arithmétiques. La lecture DOM se fait ailleurs.
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Compat v0 — inchangé
// ---------------------------------------------------------------------------
inline float 	momPrice1				(const SCStudyInterfaceRef& sc, int i)
{
  if (i <= 0) return 0.0f; return (float)(sc.Close[i] - sc.Close[i-1]);
}

inline void 	duMomentumNear			(const double* now, const double* prev, int avail, double alphaPct, double* outScore)
{
  double s = 0.0; const int N = avail < 0 ? 0 : avail; for (int i=0; i<N; ++i) { const double d = (now?now[i]:0.0) - (prev?prev[i]:0.0); s += d; }
  if (outScore) *outScore = s * (alphaPct/100.0);
}

inline double 	duMomentumSigned		(double bidMom, double askMom) { return bidMom - askMom; }

inline bool 	duMomentumGate			(double score, double p1, double p2, int /*warmupN*/) { return score > p1 || score > p2; }

// ---------------------------------------------------------------------------
// v1 — enrichi
// ---------------------------------------------------------------------------
struct 			DmBands { int startNear=0, cntNear=10; int startMid=10, cntMid=20; int startFar=30, cntFar=30; }; // jusqu’à 60

struct 			DmParams
{
  DmBands bands{};          // partition profondeur
  double  wNear=0.60, wMid=0.30, wFar=0.10; // pondérations
  double  alphaPct=70.0;    // EMA des scores
  double  hOn=0.8, hOff=0.4; // hystérésis sur scoreEma norm
  int     refractoryN=2;    // longueur minimale au‑dessus de hOn
  int     cooldownMs=250;   // post‑évènement
  int     useZScore=1;      // 0/1 z‑score rolling
};

struct 			DmStats // accumulateurs pour z‑score rolling simple (une voie)
{
  double mean=0.0, m2=0.0; int n=0;
  inline void reset(){ mean=0.0; m2=0.0; n=0; }
  inline void push(double x){ n+=1; const double d=x-mean; mean += d/n; m2 += d*(x-mean); }
  inline double std() const { return (n>1? std::sqrt(std::max(0.0, m2/(n-1))) : 0.0); }
};

enum 			DmPhase:int { DM_IDLE=0, DM_ARMED=1, DM_TRIGGERED=2, DM_COOLDOWN=3 };

struct 			DmState
{
  // Scores bruts
  double near=0, mid=0, far=0;         // delta quantités par bande (Bid–Ask)
  double weighted=0;                    // combinaison pondérée
  double weightedEma=0;                 // EMA

  // Normalisations
  double baseNear=0, baseMid=0, baseFar=0; // ∑ qty bandes (Bid+Ask)
  double norm=0, normEma=0;                 // score normalisé (par base)
  double z=0;                               // z‑score optionnel

  // Hystérésis / phase
  int consecOver=0, consecUnder=0; int phase=DM_IDLE; long long tTrig=0, tLast=0;

  // Z‑score rolling
  DmStats zs{};

  // Util
  inline void reset_soft(){ near=mid=far=weighted=weightedEma=0; baseNear=baseMid=baseFar=0; norm=normEma=0; z=0; consecOver=consecUnder=0; }
  inline void reset_all(){ reset_soft(); phase=DM_IDLE; tTrig=tLast=0; zs.reset(); }
};

// Somme d’un intervalle borné [start .. start+count-1] pour (now-prev)
inline double 	dm_band_delta			(const double* now, const double* prev, int avail, int start, int count)
{
  if (!now && !prev) return 0.0; if (count<=0) return 0.0; if (start<0) start=0;
  const int end = start + count - 1; double s=0.0; for (int i=start; i<=end && i<avail; ++i) s += (now?now[i]:0.0) - (prev?prev[i]:0.0);
  return s;
}

// Somme des bases (Bid+Ask) sur une bande. L’appelant fournit base déjà lue si besoin.
inline double 	dm_band_base			(const double* nowB, const double* nowA, int avail, int start, int count)
{
  if ((!nowB && !nowA) || count<=0) return 0.0; if (start<0) start=0; const int end=start+count-1; double s=0.0;
  for (int i=start; i<=end && i<avail; ++i) s += (nowB?nowB[i]:0.0) + (nowA?nowA[i]:0.0);
  return s;
}

inline double 	dm_ema					(double prev, double x, double aPct){ const double a = aPct>0.0? aPct/100.0 : 0.0; return prev + a*(x - prev); }

// Mise à jour à partir d’instantanés Bid/Ask (now vs prev) — entièrement arithmétique
// nowB/A, prevB/A: tableaux de quantités >=0 ; avail: niveaux valides
// Retourne le score signé normalisé (normEma).
inline double 	dm_update_from_arrays	(DmState& st, const DmParams& p,
                                    const double* nowB, const double* nowA,
                                    const double* prevB, const double* prevA,
                                    int avail, long long nowMs)
{
  st.tLast = nowMs;
  // Deltas par bandes
  st.near = dm_band_delta(nowB, prevB, avail, p.bands.startNear, p.bands.cntNear)
          - dm_band_delta(nowA, prevA, avail, p.bands.startNear, p.bands.cntNear);
  st.mid  = dm_band_delta(nowB, prevB, avail, p.bands.startMid , p.bands.cntMid )
          - dm_band_delta(nowA, prevA, avail, p.bands.startMid , p.bands.cntMid );
  st.far  = dm_band_delta(nowB, prevB, avail, p.bands.startFar , p.bands.cntFar )
          - dm_band_delta(nowA, prevA, avail, p.bands.startFar , p.bands.cntFar );

  // Base par bandes (Bid+Ask) pour normalisation
  st.baseNear = dm_band_base(nowB, nowA, avail, p.bands.startNear, p.bands.cntNear);
  st.baseMid  = dm_band_base(nowB, nowA, avail, p.bands.startMid , p.bands.cntMid );
  st.baseFar  = dm_band_base(nowB, nowA, avail, p.bands.startFar , p.bands.cntFar );

  // Combinaison pondérée → EMA
  const double wsum = p.wNear * st.near + p.wMid * st.mid + p.wFar * st.far;
  st.weighted    = du::sanitize(wsum);
  st.weightedEma = dm_ema(st.weightedEma, st.weighted, p.alphaPct);

  // Normalisation par base agrégée (évite explosion quand profondeur faible)
  const double base = std::max(1e-9, st.baseNear + st.baseMid + st.baseFar);
  st.norm    = du::sanitize(st.weighted / base);
  st.normEma = dm_ema(st.normEma, st.norm, p.alphaPct);

  // z‑score optionnel sur normEma
  if (p.useZScore) {
    st.zs.push(st.normEma);
    const double sd = st.zs.std(); st.z = (sd>0.0? (st.normEma - st.zs.mean)/sd : 0.0);
  } else st.z = 0.0;

  return st.normEma;
}

// Hystérésis + réfractaire + cooldown. Retourne true si « momentum validé » sur CE tick.
inline bool 	dm_hysteresis_step		(DmState& st, const DmParams& p, long long nowMs)
{
  // Cooldown
  if (st.phase == DM_COOLDOWN) {
    if (nowMs - st.tTrig >= p.cooldownMs) st.phase = DM_IDLE; else return false;
  }

  // Hystérésis sur normEma
  if (st.normEma >= p.hOn) { st.consecOver++; st.consecUnder=0; if (st.phase==DM_IDLE) st.phase=DM_ARMED; }
  if (st.normEma <= p.hOff){ st.consecUnder++; st.consecOver=0; if (st.phase==DM_ARMED) st.phase=DM_IDLE; }

  // Déclenchement si réfractaire OK
  const bool ok = (st.phase==DM_ARMED) && (st.consecOver >= p.refractoryN);
  if (ok) { st.phase=DM_TRIGGERED; st.tTrig=nowMs; st.phase=DM_COOLDOWN; return true; }
  return false;
}

// ---------------------------------------------------------------------------
// Features MLP v1 — 16 features
// 0 near, 1 mid, 2 far, 3 weighted, 4 weightedEma, 5 baseNear, 6 baseMid,
// 7 baseFar, 8 norm, 9 normEma, 10 z, 11 consecOver, 12 consecUnder,
// 13 phase, 14 dt_since_trig_ms, 15 avail_hint
// ---------------------------------------------------------------------------
inline int 		dm_features_v1			(const DmState& st, int avail, long long nowMs, double* out)
{
  if (!out) return 0; const long long dtT = (st.tTrig>0? nowMs - st.tTrig : 0);
  out[0]=du::sanitize(st.near); out[1]=du::sanitize(st.mid); out[2]=du::sanitize(st.far);
  out[3]=du::sanitize(st.weighted); out[4]=du::sanitize(st.weightedEma);
  out[5]=du::sanitize(st.baseNear); out[6]=du::sanitize(st.baseMid); out[7]=du::sanitize(st.baseFar);
  out[8]=du::sanitize(st.norm); out[9]=du::sanitize(st.normEma); out[10]=du::sanitize(st.z);
  out[11]=(double)st.consecOver; out[12]=(double)st.consecUnder; out[13]=(double)st.phase; out[14]=(double)dtT; out[15]=(double)avail;
  return 16;
}

} // namespace du
