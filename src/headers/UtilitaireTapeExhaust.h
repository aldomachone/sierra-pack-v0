#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// ============================================================================
// UtilitaireTapeExhaust.h — Détection d'« exhaustion » sur le Tape (v1)
// ----------------------------------------------------------------------------
// Portée : header inline, sans allocation, sans état global. Zéro dépendance
// autre que Sierra Chart + UtilitaireSanitize.h.
// Idées clés :
//  - « Exhaustion » = chute significative de la cadence (pace) vs sa tendance
//    récente (EMA), avec garde‑fous min d'échantillons et réfractaire.
//  - Outils fournis : score, état cumulatif, M‑sur‑N minimal (via minSamples),
//    z‑score EMA/variance (Welford), confidence, et extraction de features MLP.
//  - Tout est agnostique de SCStudyInterfaceRef : on passe simplement les
//    valeurs de cadence et l'horodatage en millisecondes.
// ----------------------------------------------------------------------------
// Conventions :
//  - « pace » = événements par seconde (ex. trades/s). Si vous avez l'intervalle
//    inter‑arrivée en millisecondes (Δt_ms), pace = 1000.0 / Δt_ms.
//  - Les pourcentages sont retournés en [%].
//  - nowMs = temps courant en millisecondes (entier signé 64 bits recommandé).
//  - Toutes les fonctions sont inline. Pas de new/delete.
// ----------------------------------------------------------------------------
// Compat v0 :
//  - teExhaustScore(paceNow, paceEma)
//  - teExhaust(dropPct, minSamples)
//  - teFatigue(emaRate, lowThr)
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Constantes légères
// ---------------------------------------------------------------------------
constexpr double kTiny     = 1e-12;
constexpr double kEpsPct   = 1e-9;   // pour éviter les divisions par 0 sur %
constexpr double kMsInSec  = 1000.0;

// ---------------------------------------------------------------------------
// Helpers math
// ---------------------------------------------------------------------------
inline double clamp(double x, double lo, double hi)            { return x < lo ? lo : (x > hi ? hi : x); }
inline double clamp01(double x)                                { return clamp(x, 0.0, 1.0); }
inline double pct_drop(double ref, double now)                 { return ref <= kTiny ? 0.0 : (ref - now) * 100.0 / ref; }
inline double ema_alpha_from_half_life_ms(double halfLifeMs, double dtMs)
{
  if (halfLifeMs <= 0.0 || dtMs <= 0.0) return 1.0; // dégénérescence = tout poids au dernier échantillon
  // alpha = 1 - exp(-ln(2) * dt/HL)
  const double x = -0.6931471805599453 * (dtMs / halfLifeMs);
  const double a = 1.0 - std::exp(x);
  return clamp01(a);
}

// ---------------------------------------------------------------------------
// Compatibilité v0 (API simple)
// ---------------------------------------------------------------------------
inline float teExhaustScore(float paceNow, float paceEma)
{
  const float d = paceEma <= 0 ? 0.0f : (paceEma - paceNow) / paceEma;
  return du::sanitize(static_cast<float>(100.0f * d)); // %
}

inline bool teExhaust(double dropPct, int minSamples) { return dropPct > 0.0 && minSamples > 0; }
inline bool teFatigue(double emaRate, double lowThr)  { return emaRate < lowThr; }

// ---------------------------------------------------------------------------
// État cumulatif — suivi EMA + variance (Welford) + réfractaire + métriques
// ---------------------------------------------------------------------------
struct TeExhaustParams
{
  // Fenêtrage/EMA
  double halfLifeMs      = 1500.0;   // demi‑vie EMA (1.5 s par défaut)
  int    warmupMinN      = 16;       // échantillons min avant détection

  // Seuils d'exhaustion
  double minDropPct      = 35.0;     // chute minimale vs EMA, en %
  double maxEmaRateOK    = 12.0;     // fatigue: EMA en‑dessous de ce pace (/s)
  double minNegZScore    = 1.25;     // optionnel: z <= -1.25 autorise le trigger

  // Garde‑fous temporels
  int    refractoryMs    = 600;      // verrou après un trigger

  // Confidence
  double confDropScale   = 1.0;      // pondère l'impact du drop
  double confEmaScale    = 0.5;      // pondère l'impact du ratio ema
};

struct TeExhaustState
{
  // Base
  long long lastMs       = 0;        // timestamp du dernier update
  int       n            = 0;        // nombre d'échantillons valides

  // EMA pace
  double emaRate         = 0.0;      // EMA du pace

  // Welford pour z‑score sur pace
  double mean            = 0.0;
  double m2              = 0.0;      // somme des carrés centrés

  // Extrêmes locaux
  double maxRate         = 0.0;
  double minRate         = 1e300;

  // Dernier drop calculé
  double lastDropPct     = 0.0;

  // Temporisation post‑trigger
  long long lockUntilMs  = 0;

  // Telemetry
  int runLen             = 0;        // longueur de la séquence de drops
  double lastZ           = 0.0;      // z‑score du dernier pace

  // Reset doux (par exemple nouvelle session)
  void reset()
  {
    lastMs      = 0; n = 0; emaRate = 0.0; mean = 0.0; m2 = 0.0;
    maxRate     = 0.0; minRate = 1e300; lastDropPct = 0.0;
    lockUntilMs = 0; runLen = 0; lastZ = 0.0;
  }
};

// Mise à jour Welford
inline void welford_update(TeExhaustState& st, double x)
{
  st.n += 1;
  const double d  = x - st.mean;
  st.mean        += d / st.n;
  st.m2          += d * (x - st.mean);
}

inline double welford_var(const TeExhaustState& st)
{
  return (st.n > 1) ? (st.m2 / (st.n - 1)) : 0.0;
}

// Score de confiance 0..1 en fonction du drop, de l'EMA et du nombre d'échantillons
inline double te_confidence(const TeExhaustState& st, const TeExhaustParams& p)
{
  if (st.n <= 0 || st.emaRate <= 0.0) return 0.0;
  // Normalisation simple
  const double drop01 = clamp01(st.lastDropPct / (p.minDropPct + 50.0));
  const double ema01  = clamp01((p.maxEmaRateOK - st.emaRate) / (p.maxEmaRateOK + 1.0));
  const double n01    = clamp01((st.n - p.warmupMinN) / 64.0);
  const double s      = p.confDropScale * drop01 + p.confEmaScale * ema01 + 0.25 * n01;
  return clamp01(s);
}

// Mise à jour complète :
//  - nowMs        : horodatage courant en ms
//  - paceNow      : cadence instantanée (/s)
//  - params       : seuils/HL
//  - outTriggered : true si un événement "exhaustion" est détecté
//  - outScorePct  : intensité de la chute en % (vs EMA)
//  - outZ         : z‑score courant
inline void te_update(
  TeExhaustState& st,
  long long nowMs,
  double paceNow,
  const TeExhaustParams& params,
  bool& outTriggered,
  double& outScorePct,
  double& outZ)
{
  outTriggered = false;
  outScorePct  = 0.0;
  outZ         = 0.0;

  if (paceNow <= 0.0) {
    // Pace nul → on compte mais sans déstabiliser la variance de manière négative
    // On laisse le mécanisme standard traiter x = 0.0
  }

  // Temps écoulé pour alpha
  double dtMs = (st.lastMs > 0) ? double(nowMs - st.lastMs) : 0.0;
  if (dtMs < 0.0) dtMs = 0.0; // robustesse en cas d'horloge non monotone

  const double a = ema_alpha_from_half_life_ms(params.halfLifeMs, dtMs);

  // EMA pace
  if (st.n == 0) {
    st.emaRate = paceNow;
  } else {
    st.emaRate = du::sanitize(a * paceNow + (1.0 - a) * st.emaRate);
  }

  // Welford + extrêmes
  welford_update(st, paceNow);
  st.maxRate = paceNow > st.maxRate ? paceNow : st.maxRate;
  st.minRate = paceNow < st.minRate ? paceNow : st.minRate;

  // z‑score
  const double var = welford_var(st);
  const double std = (var > 0.0) ? std::sqrt(var) : 0.0;
  outZ             = (std > 0.0) ? ((paceNow - st.mean) / std) : 0.0;
  st.lastZ         = outZ;

  // Drop vs EMA
  const double emaRef     = (st.emaRate > kTiny) ? st.emaRate : kTiny;
  const double dropPctNow = (emaRef - paceNow) * 100.0 / emaRef;
  st.lastDropPct          = du::sanitize(dropPctNow);
  outScorePct             = st.lastDropPct;

  // Run length de drops consécutifs
  st.runLen = (dropPctNow >= 0.0) ? (st.runLen + 1) : 0;

  // Temporisation post‑trigger
  const bool locked = (nowMs < st.lockUntilMs);

  // Conditions de tir
  const bool warmOK  = (st.n >= params.warmupMinN);
  const bool dropOK  = (st.lastDropPct >= params.minDropPct);
  const bool emaOK   = (st.emaRate <= params.maxEmaRateOK);
  const bool zOK     = (params.minNegZScore > 0.0) ? (outZ <= -params.minNegZScore) : true;

  if (!locked && warmOK && dropOK && (emaOK || zOK)) {
    outTriggered    = true;
    st.lockUntilMs  = nowMs + params.refractoryMs;
  }

  st.lastMs = nowMs;
}

// Conversion rapide inter‑arrivée → pace (/s)
inline double te_pace_from_inter_ms(double deltaMs)
{
  if (deltaMs <= 0.0) return 0.0;
  return kMsInSec / deltaMs;
}

// Paquet feature pour MLP : remplit un buffer (taille >= 12).
// Retourne le nombre de features écrites.
// Ordre proposé (v1) — unités entre parenthèses :
//  0: pace_ema (/s)
//  1: pace_mean (/s)
//  2: pace_std  (/s)
//  3: drop_pct  (%)
//  4: z_score   (z)
//  5: run_len   (ticks)
//  6: max_rate  (/s)
//  7: min_rate  (/s)
//  8: conf01    (ratio)
//  9: ema_ok    {0,1}
// 10: drop_ok   {0,1}
// 11: warm_ok   {0,1}
inline int te_features_v1(const TeExhaustState& st, const TeExhaustParams& p, double* out)
{
  if (!out) return 0;
  const double var = welford_var(st);
  const double std = (var > 0.0) ? std::sqrt(var) : 0.0;

  out[0]  = du::sanitize(st.emaRate);
  out[1]  = du::sanitize(st.mean);
  out[2]  = du::sanitize(std);
  out[3]  = du::sanitize(st.lastDropPct);
  out[4]  = du::sanitize(st.lastZ);
  out[5]  = double(st.runLen);
  out[6]  = du::sanitize(st.maxRate);
  out[7]  = du::sanitize((st.minRate == 1e300) ? 0.0 : st.minRate);
  out[8]  = du::sanitize(te_confidence(st, p));
  out[9]  = (st.emaRate <= p.maxEmaRateOK) ? 1.0 : 0.0;
  out[10] = (st.lastDropPct >= p.minDropPct) ? 1.0 : 0.0;
  out[11] = (st.n >= p.warmupMinN) ? 1.0 : 0.0;
  return 12;
}

// Raccourci monosource: inter‑arrivée → update → trigger
inline bool te_step_from_delta_ms(
  TeExhaustState& st,
  long long nowMs,
  double interArrivalMs,
  const TeExhaustParams& params,
  double& outScorePct,
  double& outZ)
{
  const double pace = te_pace_from_inter_ms(interArrivalMs);
  bool trig = false; double score = 0.0; double z = 0.0;
  te_update(st, nowMs, pace, params, trig, score, z);
  outScorePct = score; outZ = z;
  return trig;
}

// Reset gardant éventuellement EMA/variance si souhaité
inline void te_soft_reset(TeExhaustState& st, bool keepStats)
{
  const double keepEma   = st.emaRate;
  const double keepMean  = st.mean;
  const double keepM2    = st.m2;
  const int    keepN     = st.n;

  st.reset();
  if (keepStats) {
    st.emaRate = keepEma;
    st.mean    = keepMean;
    st.m2      = keepM2;
    st.n       = keepN;
  }
}

} // namespace du
