#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// ============================================================================
// UtilitaireFVG.h — Fair Value Gaps (détection, merge, métriques, features)
// v1 enrichi, 100% inline, aucune allocation, aucun état global. Compat v0 OK.
// ----------------------------------------------------------------------------
// Conventions rapides
// • FVG « up » quand Low[n-1] > High[n-2]  → gap haussier (zone [High[n-2], Low[n-1]]).
// • FVG « dn » quand High[n-1] < Low[n-2] → gap baissier (zone [High[n-1], Low[n-2]]).
// • Les bornes (lo,hi) sont toujours normalisées avec lo <= hi.
// • minTicks optionnel pour filtrer la largeur minimale en ticks.
// ----------------------------------------------------------------------------
// Compat v0 conservée:
//   struct Fvg { bool up,dn; float hi,lo; };
//   struct fvgRec { double lo, hi; };
//   Fvg    fvgDetect(SCStudyInterfaceRef sc, int n);
//   int    fvgDetect(const double* candles,int n,int minTicks,fvgRec* out,int maxOut);
//   double fvgFillPct(double price, const fvgRec&);
// Ajouts v1:
//   - Helpers: fvgWidth, fvgContains, fvgSignedDist, fvgNormalize, fvgMergeAdjacent.
//   - Extraction bar‑based compacte: fvg_scan_series(sc, first, last, tickSize,...).
//   - Emballage en « zones » compatibles confluence et 12 features MLP.
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Compat v0
// ---------------------------------------------------------------------------
struct 			Fvg 				{ bool up=false, dn=false; float hi=0, lo=0; };
struct 			fvgRec 				{ double lo{0}, hi{0}; };

// FVG 3‑bar (index courant = n). Renvoie un Fvg simple pour la barre n.
inline 			Fvg fvgDetect		(const SCStudyInterfaceRef& sc, int n)
{
  Fvg f{}; if (n < 2) return f;
  const double Hm2 = sc.High[n-2];
  const double Lm2 = sc.Low [n-2];
  const double Hm1 = sc.High[n-1];
  const double Lm1 = sc.Low [n-1];

  // Up‑gap si Low[n-1] > High[n-2]
  if (Lm1 > Hm2) { f.up = true; f.lo = (float)Hm2; f.hi = (float)Lm1; }
  // Down‑gap si High[n-1] < Low[n-2]
  if (Hm1 < Lm2) { f.dn = true; f.lo = (float)Hm1; f.hi = (float)Lm2; }
  // lo <= hi garanti par la logique ci‑dessus
  return f;
}

// ---------------------------------------------------------------------------
// Enrichi v1 — helpers génériques
// ---------------------------------------------------------------------------
inline void 	fvgNormalize		(fvgRec& z){ if (z.hi < z.lo) { const double t=z.lo; z.lo=z.hi; z.hi=t; } }
inline double 	fvgWidth 			(const fvgRec& z){ return z.hi - z.lo; }
inline bool   	fvgContains			(const fvgRec& z, double p){ return (p >= z.lo && p <= z.hi); }
inline double 	fvgSignedDist		(const fvgRec& z, double p){
	const double c = 0.5*(z.lo+z.hi); if (fvgContains(z,p)) return 0.0; return (p < c) ? (z.lo - p) : (p - z.hi); }

// Merge adjacents/chevauchants avec tolérance absolue (prix), sur place.
inline int 		fvgMergeAdjacent	(fvgRec* zs, int n, double tol)
{
  if (!zs || n <= 0) return 0; for (int i=0;i<n;++i) fvgNormalize(zs[i]);
  // tri par lo
  for(int i=0;i<n-1;++i) for(int j=i+1;j<n;++j) if (zs[j].lo < zs[i].lo) { const fvgRec t=zs[i]; zs[i]=zs[j]; zs[j]=t; }
  int w=0; fvgRec cur = zs[0];
  for(int i=1;i<n;++i){ if (zs[i].lo <= cur.hi + tol) { cur.hi = std::max(cur.hi, zs[i].hi); } else { zs[w++] = cur; cur = zs[i]; } }
  zs[w++] = cur; return w;
}

// ---------------------------------------------------------------------------
// Détection array‑based compacte (compat signe v0)
// candles: tableau OHLC intercalé de taille 4*n, ordre [O,H,L,C] par barre.
// n: nombre de barres. minTicks: largeur mini (en ticks) si tickSize>0 sinon en prix.
// out: tableau de fvgRec. maxOut: taille max de out[]. Retourne le nb écrit.
// ---------------------------------------------------------------------------
inline int 		fvgDetect			(const double* candles, int n, int minTicks, fvgRec* out, int maxOut,
                     double tickSize = 0.0)
{
  if (!candles || !out || n <= 0 || maxOut <= 0) return 0; int k=0;
  auto H=[&](int i){ return candles[4*i+1]; };
  auto L=[&](int i){ return candles[4*i+2]; };
  const double minAbs = (tickSize > 0.0 && minTicks > 0) ? (minTicks * tickSize) : std::max(0, minTicks);
  for (int i=2; i<n && k<maxOut; ++i)
  {
    const double Hm2 = H(i-2), Lm2 = L(i-2), Hm1 = H(i-1), Lm1 = L(i-1);
    // Up
    if (Lm1 > Hm2) {
      fvgRec z{Hm2, Lm1}; if (fvgWidth(z) >= minAbs) { out[k++] = z; }
    }
    // Down
    if (Hm1 < Lm2) {
      fvgRec z{Hm1, Lm2}; if (fvgWidth(z) >= minAbs) { out[k++] = z; }
    }
  }
  return k;
}

// Pourcentage de « comblement » par rapport à la zone [lo,hi]
// price à l’intérieur → 100 %. En dehors → proportion négative à 0.
inline double 	fvgFillPct			(double price, const fvgRec& z)
{
  if (z.hi <= z.lo) return 0.0; if (price <= z.lo) return 0.0; if (price >= z.hi) return 100.0;
  return 100.0 * (price - z.lo) / (z.hi - z.lo);
}

// ---------------------------------------------------------------------------
// Scan bar‑based: collecte des FVG sur un intervalle [first..last]
// Écrit jusqu’à maxOut zones triées et merge avec tolérance tol.
// ---------------------------------------------------------------------------
inline int 		fvg_scan_series		(SCStudyInterfaceRef sc, int first, int last, fvgRec* out, int maxOut,
                           double tickSize = 0.0, int minTicks = 0, double tol = 0.0)
{
  if (!out || maxOut <= 0) return 0; if (first < 2) first = 2; if (last < first) return 0;
  const int N = last - first + 1; const int cap = std::min(N*2, maxOut); // *2 car up+down possibles

  int k = 0;
  const double minAbs = (tickSize > 0.0 && minTicks > 0) ? (minTicks * tickSize) : std::max(0, minTicks);

  for (int i = first; i <= last && k < cap; ++i)
  {
    const double Hm2 = sc.High[i-2];
    const double Lm2 = sc.Low [i-2];
    const double Hm1 = sc.High[i-1];
    const double Lm1 = sc.Low [i-1];

    if (Lm1 > Hm2) { fvgRec z{Hm2, Lm1}; if (fvgWidth(z) >= minAbs) out[k++] = z; }
    if (Hm1 < Lm2) { fvgRec z{Hm1, Lm2}; if (fvgWidth(z) >= minAbs) out[k++] = z; }
  }

  // Merge + trim
  k = fvgMergeAdjacent(out, k, tol);
  if (k > maxOut) k = maxOut;
  return k;
}

// ---------------------------------------------------------------------------
// Features MLP v1 (12)
// Contexte: prix courant "price" et tableau de FVG déjà détectés/mergés.
// Ordre:
// 0: n_fvg, 1: nearest_dist, 2: nearest_width, 3: nearest_fill_pct,
// 4: total_covered_span, 5: mean_width, 6: max_width, 7: min_width,
// 8: inside_flag{0,1}, 9: signed_dist_nearest, 10: lo_nearest, 11: hi_nearest
// ---------------------------------------------------------------------------
inline int 		fvg_features_v1		(const fvgRec* zs, int n, double price, double* out)
{
  if (!out) return 0; for (int i=0;i<12;++i) out[i]=0.0; if (!zs || n<=0) return 12;
  // nearest
  int bi = -1; double best = 1e300; for (int i=0;i<n; ++i){ const double d = znDist(*(const du::Zone*)&zs[i], price); if (d < best){ best=d; bi=i; } }
  const fvgRec z = zs[bi];
  // stats
  double sumW=0, maxW=0, minW=1e300; double covered=0.0; int inside=0;
  for (int i=0;i<n; ++i){ const double w=fvgWidth(zs[i]); sumW+=w; if(w>maxW)maxW=w; if(w<minW)minW=w; if(fvgContains(zs[i],price)) inside=1; covered += w; }
  const double meanW = sumW / n;

  out[0]  = (double)n;
  out[1]  = du::sanitize(best);
  out[2]  = du::sanitize(fvgWidth(z));
  out[3]  = du::sanitize(fvgFillPct(price, z));
  out[4]  = du::sanitize(covered);
  out[5]  = du::sanitize(meanW);
  out[6]  = du::sanitize(maxW);
  out[7]  = du::sanitize(minW);
  out[8]  = (double)inside;
  out[9]  = du::sanitize(fvgSignedDist(z, price));
  out[10] = du::sanitize(z.lo);
  out[11] = du::sanitize(z.hi);
  return 12;
}

} // namespace du
