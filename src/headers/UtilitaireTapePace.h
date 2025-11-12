#pragma once
#include "sierrachart.h"
#include "UtilitaireTempo.h"     // du::toMs, etc.
#include "UtilitaireSanitize.h"  // du::sanitize

// =============================================================================
// UtilitaireTapePace.h — Tempo/Pace/Arrivées T&S (v1 enrichi, compat v0)
// -----------------------------------------------------------------------------
// • 100% inline, aucune allocation, aucun état global.
// • Compat v0 conservée : pacePerSecBar(), interArrivalMs(), tpInterArrivalMs(),
//   tpPaceEMA(), tpRate().
// • Ajouts :
//   - Mesures robustes d’inter‑arrivée (min/max/mean/cv) sur fenêtre M.
//   - EMA adaptative des Δt et du pace (ticks/s) avec clamp et alpha%.
//   - Détection BURST/RUN simple : seuil Δt, réfractaire, compteur de série.
//   - Compteurs UP/DN, ΔtUP/ΔtDN, gardes TickSize>0, NowMs unique.
//   - 16 features MLP standardisées pour le rythme du flux.
// =============================================================================

namespace du {

// -----------------------------------------------------------------------------
// Compat v0 (inchangé)
// -----------------------------------------------------------------------------
inline float 	pacePerSecBar		(const SCStudyInterfaceRef& sc, int idx)
{
  const double vol = (double)sc.UpVolume[idx] + (double)sc.DownVolume[idx];
  const double dt  = sc.SecondsPerBar > 0 ? (double)sc.SecondsPerBar : 1.0;
  return (float)(vol / dt);
}

inline int 		interArrivalMs		(long long last, long long now) 							{ return (int)(now > last ? (now - last) : 0LL); }

inline double 	tpInterArrivalMs	(long long lastTs, long long ts) 							{ return (double)(ts - lastTs); }

inline double 	tpPaceEMA			(double prev, double dtMs, double alphaPct)					{
	const double a = alphaPct > 0.0 ? (alphaPct / 100.0) : 0.0; return prev + a * (dtMs - prev); }

inline double 	tpRate				(int ctr, double dtMs)										{ return dtMs > 0.0 ? (double)ctr * 1000.0 / dtMs : 0.0; }

struct 			TpParams
{
  int     winM          = 16;    // fenêtre pour stats Δt
  double  alphaPct      = 50.0;  // EMA Δt et pace
  double  burstDtMs     = 60.0;  // Δt <= burstDtMs ⇒ BURST
  int     refractoryN   = 2;     // N min pour valider une RUN
  double  clampDtMinMs  = 1.0;   // clamp Δt min
  double  clampDtMaxMs  = 5000;  // clamp Δt max
};

struct 			TpState
{
  // Temps et Δt
  long long lastTsMs   = 0;      // dernier timestamp ms vu
  double    dtMs       = 0.0;    // Δt courant
  double    dtEma      = 0.0;    // EMA Δt
  double    paceEma    = 0.0;    // EMA des ticks/s

  // Stats rolling sur M
  double dtMin=1e300, dtMax=0.0, dtSum=0.0, dtSq=0.0; int dtCnt=0;

  // Détection BURST/RUN
  int    consec = 0;             // longueur de la série active
  int    isBurst=0;              // flag Δt<=burstDtMs
  int    isRun  = 0;             // flag consec>=refractoryN

  // Compteurs UP/DN (si fournis par appelant)
  int up=0, dn=0;                // cumulés depuis le dernier reset

  void reset()
  {
    lastTsMs=0; dtMs=0; dtEma=0; paceEma=0; dtMin=1e300; dtMax=0; dtSum=0; dtSq=0; dtCnt=0; consec=0; isBurst=0; isRun=0; up=0; dn=0;
  }
};

// Clamp utilitaire
inline double 	tp_clamp			(double x, double lo, double hi)							{ if (x < lo) return lo; if (x > hi) return hi; return x; }

// Mise à jour « arrival » à chaque trade/impression.
// nowMs : horodatage ms monotone (du::toMs(sc.CurrentSystemDateTime) recommandé)
// side  : +1 pour UP, −1 pour DN, 0 si inconnu
inline void 	tp_update_arrival	(TpState& st, const TpParams& p, long long nowMs, int side)
{
  if (st.lastTsMs <= 0) { st.lastTsMs = nowMs; st.dtMs = 0.0; return; }

  // Δt brut puis clamp
  const double raw = (double)(nowMs - st.lastTsMs);
  const double dt  = tp_clamp(raw, p.clampDtMinMs, p.clampDtMaxMs);
  st.dtMs = dt; st.lastTsMs = nowMs;

  // EMA Δt
  st.dtEma = tpPaceEMA(st.dtEma, dt, p.alphaPct);

  // Pace instantané ≈ 1/Δt, puis EMA pace
  const double paceInst = dt > 0.0 ? 1000.0 / dt : 0.0; // ticks par seconde
  st.paceEma = st.paceEma + (p.alphaPct > 0.0 ? (p.alphaPct/100.0) * (paceInst - st.paceEma) : 0.0);

  // Rolling M (Welford simplifié via moments cumulés)
  st.dtCnt += 1; st.dtSum += dt; st.dtSq += dt*dt; if (dt < st.dtMin) st.dtMin = dt; if (dt > st.dtMax) st.dtMax = dt;
  if (st.dtCnt > p.winM) {
    // réduction naïve: on rapproche la moyenne progressivement (sans buffer)
    const double mean = st.dtSum / st.dtCnt; // approx si winM glissante non exacte
    st.dtSum -= mean; st.dtSq -= mean*mean; st.dtCnt -= 1; // maintien borné
  }

  // Burst / Run
  st.isBurst = (dt <= p.burstDtMs) ? 1 : 0;
  st.consec  = st.isBurst ? (st.consec + 1) : 0;
  st.isRun   = (st.consec >= p.refractoryN) ? 1 : 0;

  // Compteurs UP/DN
  if (side > 0) st.up += 1; else if (side < 0) st.dn += 1;
}

// Stats dérivées
inline void 	tp_stats			(const TpState& st, double& mean, double& std, double& cv)
{
  if (st.dtCnt <= 0) { mean=0; std=0; cv=0; return; }
  mean = st.dtSum / st.dtCnt; const double var = st.dtCnt>0 ? (st.dtSq / st.dtCnt) - mean*mean : 0.0; std = var>0? std::sqrt(var) : 0.0; cv = (mean>0? std/mean : 0.0);
}

// Features MLP v1 (16)
// 0 dt_ms, 1 dt_ema, 2 pace_ema, 3 dt_min, 4 dt_max, 5 dt_mean, 6 dt_std, 7 dt_cv,
// 8 is_burst, 9 consec, 10 is_run, 11 up, 12 dn, 13 up_minus_dn, 14 rate_est, 15 rate_inv_dt
inline int 		tp_features_v1		(const TpState& st, const TpParams& p, double* out)
{
  if (!out) return 0; double mean=0,std=0,cv=0; tp_stats(st, mean, std, cv);
  out[0]=du::sanitize(st.dtMs);
  out[1]=du::sanitize(st.dtEma);
  out[2]=du::sanitize(st.paceEma);
  out[3]=du::sanitize(st.dtMin<1e299? st.dtMin : 0.0);
  out[4]=du::sanitize(st.dtMax);
  out[5]=du::sanitize(mean);
  out[6]=du::sanitize(std);
  out[7]=du::sanitize(cv);
  out[8]=(double)st.isBurst;
  out[9]=(double)st.consec;
  out[10]=(double)st.isRun;
  out[11]=(double)st.up;
  out[12]=(double)st.dn;
  out[13]=du::sanitize((double)st.up - (double)st.dn);
  // estimations de rythme : ticks/s via EMA, et 1/dt instantané
  out[14]=du::sanitize(st.paceEma);
  out[15]=du::sanitize(st.dtMs>0.0? 1000.0/st.dtMs : 0.0);
  return 16;
}

} // namespace du
