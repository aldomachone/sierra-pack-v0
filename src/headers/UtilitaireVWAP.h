#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// ============================================================================
// UtilitaireVWAP.h — VWAP/AVWAP, bandes et features (v1 enrichi)
// ----------------------------------------------------------------------------
// - Entièrement inline, aucune allocation, aucun état global.
// - Deux modes d’alimentation :
//     1) Bar-based (OHLC/Volume via SCStudyInterfaceRef)
//     2) Tick-based (prix, qty) pour un flux Time&Sales agrégé.
// - Resets robustes : nouvelle journée, ancre personnalisée (AVWAP), session.
// - Bandes type « écart‑type » autour du VWAP, z‑score, distance en ticks.
// - Export de features MLP standardisées.
// - Conserve l’API v0 minimaliste (typical, vwap, vwVwap, vwBands, vwCrossState).
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// v0 — Compat minimaliste
// ---------------------------------------------------------------------------
struct 			VwapState 		{ double cumPV=0, cumV=0; int lastDay=0; };

inline float 	typical			(const SCStudyInterfaceRef& sc, int i)
{
  return (float)((sc.High[i] + sc.Low[i] + sc.Close[i]) / 3.0);
}

inline float 	vwap			(SCStudyInterfaceRef sc, VwapState& st)
{
  const int i = sc.Index; const int day = sc.BaseDateTimeIn[i].GetDateYMD();
  if (i == 0 || day != st.lastDay) { st.cumPV = 0; st.cumV = 0; st.lastDay = day; }
  const double v  = (double)sc.Volume[i] > 0.0 ? (double)sc.Volume[i] : 0.0;
  const double tp = (double)typical(sc, i);
  st.cumPV += tp * v; st.cumV += v;
  return (float)(st.cumV > 1e-12 ? st.cumPV / st.cumV : tp);
}

inline double	vwVwap			(double sumPQ, double sumQ) { return sumQ > 1e-12 ? sumPQ / sumQ : 0.0; }
inline void  	vwBands			(double vwap, double stdev, double k, double& upper, double& lower) { upper = vwap + k * stdev; lower = vwap - k * stdev; }
inline int   	vwCrossState	(double price, double vwap) { return price < vwap ? 0 : (price > vwap ? 2 : 1); }

// ---------------------------------------------------------------------------
// v1 — Enrichi : paramètres et état avancés
// ---------------------------------------------------------------------------
struct 			VwapParams
{
  // Reset & ancrage
  bool   resetOnNewDay   = true;   // reset automatique au changement de date YMD
  bool   useSessionTimes = false;  // si vrai, reset à l’ouverture de session
  int    anchorHour      = -1;     // AVWAP ancré HH:MM (local chart). -1 = désactivé
  int    anchorMinute    = 0;

  // Bandes
  double bandsK          = 1.0;    // multiplicateur d’écart‑type

  // Sécurité
  double tickSize        = 0.0;    // pour calculer distance en ticks (optionnel)
};

struct 			VwapAdvState
{
  // Cumuls
  double sumPV = 0.0;  // Σ price*vol (typical*vol si bar‑based)
  double sumV  = 0.0;  // Σ vol

  // Welford sur prix pour stdev (autour de la moyenne simple des prix)
  // Note: pour les bandes VWAP, on utilise classiquement stdev des prix (ou du typical).
  // Ici on calcule la variance non pondérée pour stabilité et coût réduit.
  double meanP = 0.0;  // moyenne simple du prix observé (typical ou tick price)
  double m2P   = 0.0;  // Σ (x-mean)^2
  int    n     = 0;    // nombre d’échantillons

  // Dernières valeurs
  double lastPrice = 0.0;
  double lastVwap  = 0.0;
  double lastStdev = 0.0;
  double lastUpper = 0.0;
  double lastLower = 0.0;

  // Ancrages et resets
  int  lastYMD = 0;        // dernier jour YMD traité
  bool anchored = false;   // indique si l’ancre personnalisée est active

  // Reset complet
  void reset()
  {
    sumPV = sumV = 0.0; meanP = m2P = 0.0; n = 0;
    lastPrice = lastVwap = lastStdev = lastUpper = lastLower = 0.0;
    anchored = false;
  }
};

// ---------------------------------------------------------------------------
// Helpers internes
// ---------------------------------------------------------------------------
inline void 	welford_update	(double x, double& mean, double& m2, int& n)
{
  n += 1; const double d = x - mean; mean += d / n; m2 += d * (x - mean);
}

inline double 	welford_std		(double mean, double m2, int n)
{
  return (n > 1 && m2 > 0.0) ? std::sqrt(m2 / (n - 1)) : 0.0;
}

inline bool 	is_anchor_time	(const SCDateTime& t, int hh, int mm)
{
  if (hh < 0) return false; // ancre désactivée
  return t.GetHour() == hh && t.GetMinute() == mm;
}

// ---------------------------------------------------------------------------
// Mise à jour BAR‑BASED (OHLC+Volume) — à appeler une fois par barre
// ---------------------------------------------------------------------------
inline void 	vw_update_bar	(SCStudyInterfaceRef sc, const VwapParams& p, VwapAdvState& st)
{
  const int i = sc.Index;
  const SCDateTime t = sc.BaseDateTimeIn[i];
  const int ymd = t.GetDateYMD();

  // Resets
  if (i == 0) { st.reset(); st.lastYMD = ymd; }
  if (p.resetOnNewDay && ymd != st.lastYMD) { st.reset(); st.lastYMD = ymd; }
  if (p.useSessionTimes && sc.IsStartOfTradingDay) { st.reset(); st.lastYMD = ymd; }
  if (is_anchor_time(t, p.anchorHour, p.anchorMinute)) { st.reset(); st.anchored = true; st.lastYMD = ymd; }

  // Données de barre
  const double v  = sc.Volume[i] > 0 ? (double)sc.Volume[i] : 0.0;
  const double tp = (sc.High[i] + sc.Low[i] + sc.Close[i]) / 3.0;

  // Cumuls
  st.sumPV += tp * v; st.sumV += v;
  welford_update(tp, st.meanP, st.m2P, st.n);

  // Sorties
  const double vw = st.sumV > 1e-12 ? st.sumPV / st.sumV : tp;
  const double sd = welford_std(st.meanP, st.m2P, st.n);
  double up, lo; vwBands(vw, sd, p.bandsK, up, lo);

  st.lastPrice = tp; st.lastVwap = vw; st.lastStdev = sd; st.lastUpper = up; st.lastLower = lo;
}

// ---------------------------------------------------------------------------
// Mise à jour TICK‑BASED (prix, qty) — à appeler à chaque trade agrégé
// ---------------------------------------------------------------------------
inline void 	vw_update_tick	(double price, double qty, const SCDateTime& ts, const VwapParams& p, VwapAdvState& st)
{
  const int ymd = ts.GetDateYMD();
  if (st.n == 0) st.lastYMD = ymd;
  if (p.resetOnNewDay && ymd != st.lastYMD) { st.reset(); st.lastYMD = ymd; }
  if (is_anchor_time(ts, p.anchorHour, p.anchorMinute)) { st.reset(); st.anchored = true; st.lastYMD = ymd; }

  const double v  = qty > 0.0 ? qty : 0.0;
  const double pr = price > 0.0 ? price : st.lastPrice;

  st.sumPV += pr * v; st.sumV += v;
  welford_update(pr, st.meanP, st.m2P, st.n);

  const double vw = st.sumV > 1e-12 ? st.sumPV / st.sumV : pr;
  const double sd = welford_std(st.meanP, st.m2P, st.n);
  double up, lo; vwBands(vw, sd, p.bandsK, up, lo);

  st.lastPrice = pr; st.lastVwap = vw; st.lastStdev = sd; st.lastUpper = up; st.lastLower = lo;
}

// ---------------------------------------------------------------------------
// Accesseurs et dérivés
// ---------------------------------------------------------------------------
inline double 	vw_value		(const VwapAdvState& st)   { return st.lastVwap; }
inline double 	vw_stdev		(const VwapAdvState& st)   { return st.lastStdev; }
inline void   	vw_bands		(const VwapAdvState& st, double& up, double& lo) { up = st.lastUpper; lo = st.lastLower; }

inline double 	vw_dist_ticks	(const VwapAdvState& st, double tickSize)
{
  if (tickSize <= 0.0) return 0.0; return (st.lastPrice - st.lastVwap) / tickSize;
}

inline double 	vw_zscore		(const VwapAdvState& st)
{
  return st.lastStdev > 0.0 ? (st.lastPrice - st.lastVwap) / st.lastStdev : 0.0;
}

inline int 		vw_cross		(const VwapAdvState& st)
{
  return vwCrossState(st.lastPrice, st.lastVwap);
}

// ---------------------------------------------------------------------------
// Features MLP (v1) — écrit 10 features
// 0: vwap, 1: stdev, 2: upper, 3: lower, 4: last_price,
// 5: zscore, 6: cross_state {0,1,2}, 7: sumV, 8: dist_ticks, 9: anchored{0,1}
// ---------------------------------------------------------------------------
inline int 		vw_features_v1	(const VwapAdvState& st, const VwapParams& p, double* out)
{
  if (!out) return 0;
  out[0] = du::sanitize(st.lastVwap);
  out[1] = du::sanitize(st.lastStdev);
  out[2] = du::sanitize(st.lastUpper);
  out[3] = du::sanitize(st.lastLower);
  out[4] = du::sanitize(st.lastPrice);
  out[5] = du::sanitize(vw_zscore(st));
  out[6] = (double)vw_cross(st);
  out[7] = du::sanitize(st.sumV);
  out[8] = (p.tickSize > 0.0) ? du::sanitize(vw_dist_ticks(st, p.tickSize)) : 0.0;
  out[9] = st.anchored ? 1.0 : 0.0;
  return 10;
}

} // namespace du
