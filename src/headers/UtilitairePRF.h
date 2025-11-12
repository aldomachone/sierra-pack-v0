#pragma once
#include <cmath>
#include "sierrachart.h"
#include "UtilitaireSanitize.h"   // du::sanitize
#include "UtilitaireTempo.h"      // du::toMs (si l'appelant veut passer nowMs)

// =============================================================================
// UtilitairePRF.h — Réponse / demi‑vie (v1 enrichi, compat v0)
// -----------------------------------------------------------------------------
// • 100% inline, aucune allocation, aucun état global.
// • Compat v0 préservée :
//     - prfAlphaFromHalfLifeSec(float halfLifeSec, float dtSec)
//     - prfEmaHL(x, prev, halfLifeSec, dtSec)
//     - prfResponse(dtMs, halfLifeMs)
//     - prfSlope(series, n)
//     - prfHalfLife(targetPct, dtMs)
// • Ajouts v1 :
//     - Paramètres & état de réponse PRF (alpha demi‑vie, niveau, vitesse, accel).
//     - Estimation demi‑vie en ligne (méthode log‑ratio sur seuils) + régression simple.
//     - Impulse/step helper et décroissance exponentielle contrôlée.
//     - Hystérésis, réfractaire, cooldown.
//     - 16 features MLP standardisées.
// -----------------------------------------------------------------------------
// Conventions :
//   - « Réponse » = filtre 1er ordre EMA piloté par demi‑vie (HL) et échantillonnage dt.
//   - On expose une API purement arithmétique, indépendante d'ACSIL, sauf types basiques.
// =============================================================================

namespace du {

// -----------------------------------------------------------------------------
// ===== Compat v0 (inchangée, mais sécurisée) =================================
// -----------------------------------------------------------------------------
inline float 		prfAlphaFromHalfLifeSec			(float halfLifeSec, float dtSec)
{
  if (!(halfLifeSec>0.0f)) halfLifeSec = 0.1f;
  if (!(dtSec      >0.0f)) dtSec       = 0.01f;
  const float a = 1.0f - std::exp(-dtSec * 0.69314718f / halfLifeSec);
  return a;
}

inline float 		prfEmaHL						(float x, float prev, float halfLifeSec, float dtSec){
	const float a = prfAlphaFromHalfLifeSec(halfLifeSec, dtSec); return a*x + (1.0f - a)*prev; }

inline double 		prfResponse						(long long dtMs, long long halfLifeMs){
	if (halfLifeMs<=0) return 0.0; return 1.0 - std::exp(-std::log(2.0) * (double)dtMs / (double)halfLifeMs); }

// Pente simple : régression sur indices 0..n-1
inline double 		prfSlope						(const double* series, int n)
{
  if(!series || n<=1) return 0.0; double sx=0, sy=0, sxx=0, sxy=0; for(int i=0;i<n;++i){ const double x=(double)i, y=series[i]; sx+=x; sy+=y; sxx+=x*x; sxy+=x*y; }
  const double den = n*sxx - sx*sx; if (std::fabs(den) < 1e-12) return 0.0; return (n*sxy - sx*sy) / den;
}

inline long long 	prfHalfLife						(double targetPct, long long dtMs){
	if(!(targetPct>0.0) || !(targetPct<1.0) || dtMs<=0) return 0; return (long long)(- (double)dtMs * std::log(1.0 - targetPct) / std::log(2.0)); }

// -----------------------------------------------------------------------------
// ===== v1 — paramètres, état et utilitaires =================================
// -----------------------------------------------------------------------------
struct 				PrfParams
{
  double hl_ms      = 250.0;   // demi‑vie en millisecondes
  double dt_ms      = 50.0;    // pas nominal entre updates
  double hOn        = 0.80;    // hystérésis ON sur |norm|
  double hOff       = 0.40;    // hystérésis OFF
  int    refracN    = 2;       // réfractaire n updates au‑dessus de hOn
  int    cooldownMs = 250;     // délai post‑événement
};

enum 				PrfPhase:int { PRF_IDLE=0, PRF_ARMED=1, PRF_TRIGGERED=2, PRF_COOLDOWN=3 };

struct 				PrfState
{
  // Niveau, vitesse, accélération
  double y=0.0, yPrev=0.0, v=0.0, a=0.0;
  // Normalisations
  double yNorm=0.0, yNormEma=0.0; // |y| normalisé par EMA(|y|)
  double emaAbs=0.0; int emaInited=0;
  // Phase/temps
  int phase=PRF_IDLE; int consecOver=0, consecUnder=0; long long tLast=0, tTrig=0;
  // Demi‑vie estimée (log‑ratio simple) et pente de y
  double hlEstMs=0.0; double slopeY=0.0;
  // Buffer court pour slope
  double buf[32]; int nBuf=0;

  inline void reset_soft(){ yPrev=y; v=0; a=0; }
  inline void reset_all(){ y=yPrev=v=a=0; yNorm=yNormEma=0; emaAbs=0; emaInited=0; phase=PRF_IDLE; consecOver=consecUnder=0; tLast=tTrig=0; hlEstMs=0; slopeY=0; nBuf=0; }
};

// alpha EMA à partir d'une demi‑vie en ms et d'un pas dt en ms
inline double 		prf_alpha_from_hl_ms			(double hl_ms, double dt_ms){
	if(!(hl_ms>0.0)) hl_ms=1.0; if(!(dt_ms>0.0)) dt_ms=1.0; return 1.0 - std::exp(-(std::log(2.0)) * dt_ms / hl_ms); }

// Pas de filtre EMA 1er ordre
inline double 		prf_step						(double x, PrfState& st, const PrfParams& p)
{
  const double a = prf_alpha_from_hl_ms(p.hl_ms, p.dt_ms);
  st.yPrev = st.y; st.y = a*x + (1.0 - a)*st.yPrev; st.v = st.y - st.yPrev; st.a = st.v - (st.yPrev - (st.yPrev - st.v));
  // normalisation |y|
  const double absy = std::fabs(st.y);
  if(!st.emaInited){ st.emaAbs = absy; st.emaInited=1; }
  else { st.emaAbs = std::max(0.0, (1.0 - a)*st.emaAbs + a*absy); }
  st.yNorm = (st.emaAbs>1e-12? st.y / st.emaAbs : 0.0);
  st.yNormEma = (1.0 - a)*st.yNormEma + a*st.yNorm;
  // buffer slope
  if(st.nBuf < (int)(sizeof(st.buf)/sizeof(st.buf[0]))) st.buf[st.nBuf++] = st.y; else { for(int i=1;i<st.nBuf;++i) st.buf[i-1]=st.buf[i]; st.buf[st.nBuf-1]=st.y; }
  st.slopeY = prfSlope(st.buf, st.nBuf);
  return st.y;
}

// Estimation HL sur décroissance exponentielle : y(t) = y0 * 2^{-t/HL}
// À partir de deux amplitudes y1,y2 espacées de dt cumulés (ms)
inline double 		prf_estimate_hl_ms_from_pair	(double y1, double y2, double dt_ms)
{
  const double a1 = std::fabs(y1), a2 = std::fabs(y2); if(!(a1>1e-12) || !(a2>1e-12) || !(dt_ms>0.0)) return 0.0;
  const double r  = a1 / a2; if(!(r>1.0)) return 0.0; return dt_ms / (std::log2(r));
}

// Hystérésis + réfractaire + cooldown sur yNormEma
inline bool 		prf_hysteresis_step				(PrfState& st, const PrfParams& p, long long nowMs)
{
  if (st.phase == PRF_COOLDOWN) { if (nowMs - st.tTrig >= p.cooldownMs) st.phase = PRF_IDLE; else return false; }
  const double x = std::fabs(st.yNormEma);
  const bool over = (x >= p.hOn); const bool under = (x <= p.hOff);
  if (over)  { st.consecOver++; st.consecUnder=0; if (st.phase==PRF_IDLE) st.phase=PRF_ARMED; }
  if (under) { st.consecUnder++; st.consecOver=0; if (st.phase==PRF_ARMED) st.phase=PRF_IDLE; }
  if (st.phase==PRF_ARMED && st.consecOver >= p.refracN) { st.phase=PRF_TRIGGERED; st.tTrig=nowMs; st.phase=PRF_COOLDOWN; return true; }
  return false;
}

// Boucle utilitaire complète: applique l'entrée x, met à jour phase, retourne true si trigger
inline bool 		prf_update_tick					(double x, PrfState& st, const PrfParams& p, long long nowMs){
	st.tLast = nowMs; prf_step(x, st, p); return prf_hysteresis_step(st, p, nowMs); }

// -----------------------------------------------------------------------------
// Features MLP v1 — 16 features
// 0 y, 1 v, 2 a, 3 yNorm, 4 yNormEma, 5 emaAbs, 6 phase, 7 consecOver,
// 8 consecUnder, 9 dt_since_trig_ms, 10 slopeY, 11 hlEstMs, 12 alpha, 13 dt_ms,
// 14 hl_ms_cfg, 15 y_abs
// -----------------------------------------------------------------------------
inline int 			prf_features_v1					(const PrfState& st, const PrfParams& p, long long nowMs, double* out)
{
  if(!out) return 0; const long long dtT = (st.tTrig>0? nowMs - st.tTrig : 0);
  out[0]=du::sanitize(st.y); out[1]=du::sanitize(st.v); out[2]=du::sanitize(st.a);
  out[3]=du::sanitize(st.yNorm); out[4]=du::sanitize(st.yNormEma); out[5]=du::sanitize(st.emaAbs);
  out[6]=(double)st.phase; out[7]=(double)st.consecOver; out[8]=(double)st.consecUnder; out[9]=(double)dtT;
  out[10]=du::sanitize(st.slopeY); out[11]=du::sanitize(st.hlEstMs);
  const double alpha = prf_alpha_from_hl_ms(p.hl_ms, p.dt_ms); out[12]=du::sanitize(alpha);
  out[13]=du::sanitize(p.dt_ms); out[14]=du::sanitize(p.hl_ms); out[15]=du::sanitize(std::fabs(st.y));
  return 16;
}

} // namespace du
