#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// ============================================================================
// UtilitaireTapeSweep.h — Détection et scoring des sweeps sur le Tape (v1 enrichi)
// ----------------------------------------------------------------------------
//  - Version entièrement inline, sans allocation, sans état global.
//  - Lecture simple via Up/DownVolume ou Time&Sales.
//  - Ajout : cumul local, run‑length, vitesse d’exécution, ratio Up/Dn, score,
//    déclencheur intelligent avec seuil dynamique, compatibilité MLP features.
//  - Inspiré des logiques HFT : un « sweep » = rafale rapide de trades unilatéraux
//    (achat ou vente) surpassant les volumes moyens récents.
// ----------------------------------------------------------------------------
// Fournit :
//   twSweepScore(), twSweepTrigger(), twSweepRun(), twSweepUpdate(),
//   twSweepFeatures_v1() — ensemble de 10 features standardisées.
// ----------------------------------------------------------------------------
// Requiert : UtilitaireSanitize.h
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Types et constantes
// ---------------------------------------------------------------------------
struct TwSweepParams
{
  double halfLifeMs = 500.0;     // demi‑vie EMA du volume (ms)
  int    warmupMinN = 8;         // échantillons min avant déclenchement
  double thrMult    = 3.0;       // multiplicateur seuil vs EMA
  int    minRun     = 3;         // longueur mini d’une rafale
  int    maxGapMs   = 250;       // écart max entre ticks successifs
};

struct TwSweepState
{
  // Suivi EMA et variance
  double emaVol   = 0.0;
  double meanVol  = 0.0;
  double m2Vol    = 0.0;
  int    n        = 0;

  // Dernière observation
  long long lastMs = 0;
  double lastVol   = 0.0;
  double lastSide  = 0.0; // +1 = Buy, −1 = Sell

  // Run actuel
  int runLen     = 0;
  double runSide = 0.0;
  long long runStartMs = 0;
  long long runLastMs  = 0;

  // Compteurs globaux
  int totalRuns = 0;
  int totalSweeps = 0;

  // Reset
  void reset()
  {
    emaVol = meanVol = m2Vol = 0.0; n = 0;
    lastMs = 0; lastVol = 0.0; lastSide = 0.0;
    runLen = 0; runSide = 0.0; runStartMs = runLastMs = 0;
    totalRuns = totalSweeps = 0;
  }
};

constexpr double kMsInSec = 1000.0;
constexpr double kTiny    = 1e-12;

// ---------------------------------------------------------------------------
// Math helpers
// ---------------------------------------------------------------------------
inline double clamp(double x, double lo, double hi) { return x < lo ? lo : (x > hi ? hi : x); }
inline double ema_alpha(double hlMs, double dtMs)
{
  if (hlMs <= 0.0 || dtMs <= 0.0) return 1.0;
  const double x = -0.6931471805599453 * (dtMs / hlMs);
  return clamp(1.0 - std::exp(x), 0.0, 1.0);
}

// ---------------------------------------------------------------------------
// Scoring simple bar‑based (compat v0)
// ---------------------------------------------------------------------------
inline float twSweepScore(const SCStudyInterfaceRef& sc, int idx)
{
  const float uv = (float)sc.UpVolume[idx];
  const float dv = (float)sc.DownVolume[idx];
  return uv > dv ? uv : dv;
}

inline bool twSweepTrigger(const SCStudyInterfaceRef& sc, int idx, float thr)
{
  return twSweepScore(sc, idx) >= thr;
}

// ---------------------------------------------------------------------------
// Mise à jour à partir d’un tick ou Time&Sales (volUp, volDn, nowMs)
// ---------------------------------------------------------------------------
inline void twSweepUpdate(TwSweepState& st, long long nowMs, double volUp, double volDn, const TwSweepParams& p,
                          bool& outTriggered, double& outScore)
{
  outTriggered = false;
  const double vol = du::sanitize(volUp + volDn);
  const double side = (volUp > volDn) ? +1.0 : (volDn > volUp) ? -1.0 : 0.0;

  const double dtMs = (st.lastMs > 0) ? double(nowMs - st.lastMs) : 0.0;
  const double a = ema_alpha(p.halfLifeMs, dtMs);

  if (st.n == 0)
  {
    st.emaVol = vol;
    st.meanVol = vol;
  }
  else
  {
    st.emaVol  = du::sanitize(a * vol + (1.0 - a) * st.emaVol);
    const double d = vol - st.meanVol;
    st.meanVol += d / (++st.n);
    st.m2Vol   += d * (vol - st.meanVol);
  }
  st.n = st.n <= 0 ? 1 : st.n;

  const double var = (st.n > 1) ? st.m2Vol / (st.n - 1) : 0.0;
  const double std = (var > 0.0) ? std::sqrt(var) : 0.0;

  const double thrAbs = st.emaVol * p.thrMult;
  const double score = (thrAbs > 0.0) ? vol / thrAbs : 0.0;
  outScore = du::sanitize(score);

  // Détection run : séquence de même signe rapprochée dans le temps
  bool newRun = false;
  if (side != 0.0)
  {
    if (st.runLen == 0 || side != st.runSide || nowMs - st.runLastMs > p.maxGapMs)
    {
      st.runLen = 1; st.runSide = side; st.runStartMs = nowMs; newRun = true;
    }
    else
    {
      st.runLen += 1;
    }
    st.runLastMs = nowMs;
  }

  if (st.runLen >= p.minRun && vol >= thrAbs)
  {
    outTriggered = true;
    st.totalSweeps += 1;
  }
  if (newRun) st.totalRuns += 1;

  st.lastVol = vol;
  st.lastSide = side;
  st.lastMs = nowMs;
}

// ---------------------------------------------------------------------------
// Run detection explicite (alternative array‑based)
// ---------------------------------------------------------------------------
inline bool twRunDetect(const double* sides, int n, int minLen, long long maxGapMs, const long long* timesMs)
{
  if (!sides || !timesMs || n <= 0) return false;
  int run = 1;
  for (int i = 1; i < n; ++i)
  {
    const bool close = (timesMs[i] - timesMs[i - 1]) <= maxGapMs;
    if (sides[i] == sides[i - 1] && close)
    {
      if (++run >= minLen) return true;
    }
    else run = 1;
  }
  return false;
}

// ---------------------------------------------------------------------------
// Features MLP — 10 features standardisées
// 0: ema_vol, 1: mean_vol, 2: last_vol, 3: run_len, 4: run_side,
// 5: score, 6: total_sweeps, 7: total_runs, 8: last_side, 9: std_vol.
// ---------------------------------------------------------------------------
inline int twSweepFeatures_v1(const TwSweepState& st, double* out)
{
  if (!out) return 0;
  const double var = (st.n > 1) ? st.m2Vol / (st.n - 1) : 0.0;
  const double std = (var > 0.0) ? std::sqrt(var) : 0.0;

  out[0] = du::sanitize(st.emaVol);
  out[1] = du::sanitize(st.meanVol);
  out[2] = du::sanitize(st.lastVol);
  out[3] = double(st.runLen);
  out[4] = du::sanitize(st.runSide);
  out[5] = du::sanitize(st.lastVol / (st.emaVol + kTiny));
  out[6] = double(st.totalSweeps);
  out[7] = double(st.totalRuns);
  out[8] = du::sanitize(st.lastSide);
  out[9] = du::sanitize(std);
  return 10;
}

// ---------------------------------------------------------------------------
// Fonctions simples pour compatibilité v0
// ---------------------------------------------------------------------------
inline bool twSweep(const int* /*tsArr*/, int /*n*/, double /*thrQty*/, double /*thrRate*/) { return false; }

} // namespace du
