#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// ============================================================================
// UtilitaireOHLC.h — Niveaux O/H/L/C multi‑TF + features (v1 enrichi)
// ----------------------------------------------------------------------------
//  - Entièrement inline, aucune allocation, aucun état global.
//  - Scan efficace des bornes O/H/L/C pour Daily, Weekly, Monthly, Yearly.
//  - Offsets négatifs supportés (0 = période courante, 1 = précédente, etc.).
//  - API v0 conservée : olDaily(), olLevelsTF(), olBreakState().
//  - Ajouts : position dans le range, distances en ticks, cross/break states,
//    features MLP.
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Compat v0
// ---------------------------------------------------------------------------
struct OHLC { float O=0, H=0, L=0, C=0; };

inline OHLC olDaily(const SCStudyInterfaceRef& sc)
{
  OHLC o{};
  const int i = sc.Index;
  const int d = sc.BaseDateTimeIn[i].GetDateYMD();

  int first = i;
  for (int j = i; j >= 0; --j) {
    if (sc.BaseDateTimeIn[j].GetDateYMD() != d) { first = j + 1; break; }
    if (j == 0) first = 0;
  }

  o.O = (float)sc.Open[first];
  o.H = (float)sc.High[first];
  o.L = (float)sc.Low [first];
  for (int j = first; j <= i; ++j) {
    if (sc.High[j] > o.H) o.H = (float)sc.High[j];
    if (sc.Low [j] < o.L) o.L = (float)sc.Low [j];
  }
  o.C = (float)sc.Close[i];
  return o;
}

inline void olLevelsTF(int /*tf*/, double& O,double& H,double& L,double& C) { O=H=L=C=0.0; }
inline int  olBreakState(double price,double level) { if(price>level) return 2; if(price<level) return 0; return 1; }

// ---------------------------------------------------------------------------
// v1 — enrichi
// ---------------------------------------------------------------------------
enum OlTF : int { OL_Daily=0, OL_Weekly=1, OL_Monthly=2, OL_Yearly=3 };

// Helpers temps
inline int ymd(const SCDateTime& t)        { return t.GetDateYMD(); }
inline int ym (const SCDateTime& t)        { return t.GetYear()*100 + t.GetMonth(); }
inline int y  (const SCDateTime& t)        { return t.GetYear(); }
inline int dow(const SCDateTime& t)        { return t.GetDayOfWeek(); } // 0=Dimanche (Sierra), 1=Lundi, ...

// Frontières de période (début de période → key)
inline int period_key(const SCDateTime& t, OlTF tf)
{
  switch(tf)
  {
    case OL_Daily:   return ymd(t);
    case OL_Weekly:  {
      // Heuristique stable: semaine commence le lundi.
      // On code la clé comme YYYY*100 + ISO_like_week (approx). Pour stabilité
      // intra‑session, on s'appuie sur année + semaine calendaire approchée.
      // Simplification: semaineKey = yyyymmdd du lundi correspondant.
      const int d = dow(t); // 0..6, 1=Lundi
      const int shift = (d==0?6:d-1); // nb de jours à soustraire pour revenir au lundi
      SCDateTime monday = t;
      monday.SubtractDays(shift);
      return ymd(monday);
    }
    case OL_Monthly: return ym(t);
    case OL_Yearly:  return y(t);
  }
  return ymd(t);
}

// Scan d'une période donnée par sa clé (retourne firstIdx, lastIdx inclus)
inline bool scan_period_bounds(const SCStudyInterfaceRef& sc, int iLast, int key, OlTF tf, int& first, int& last)
{
  if (iLast < 0) return false;
  last = iLast;
  first = iLast;
  for (int j = iLast; j >= 0; --j)
  {
    const int k = period_key(sc.BaseDateTimeIn[j], tf);
    if (k != key) { first = j + 1; break; }
    first = j;
  }
  return first <= last;
}

// Trouve la période d'offset nPeriods en arrière par rapport à l'index iRef.
// offset=0 courant, 1 précédent, ... Renvoie aussi la clé trouvée.
inline bool find_period_by_offset(const SCStudyInterfaceRef& sc, int iRef, OlTF tf, int offset, int& outKey, int& outFirst, int& outLast)
{
  if (iRef < 0) return false;
  int seen = 0;
  int curKey = period_key(sc.BaseDateTimeIn[iRef], tf);
  int first = 0, last = 0;
  if (!scan_period_bounds(sc, iRef, curKey, tf, first, last)) return false;
  if (offset == 0) { outKey = curKey; outFirst = first; outLast = last; return true; }

  int j = first - 1;
  while (j >= 0)
  {
    const int keyJ = period_key(sc.BaseDateTimeIn[j], tf);
    // Avance jusqu'au changement de clé
    int keyEnd = keyJ; int f= j, l= j;
    // Recule jusqu'au début de cette période
    for (; f >= 0; --f)
    {
      const int kj = period_key(sc.BaseDateTimeIn[f], tf);
      if (kj != keyEnd) { f = f + 1; break; }
      if (f == 0) { /* atteint le début */ }
    }
    if (f < 0) f = 0;
    // f..j est la période; met à jour j pour continuer
    seen += 1;
    if (seen == offset) { outKey = keyEnd; outFirst = f; outLast = j; return true; }
    j = f - 1;
  }
  return false;
}

// Calcule O/H/L/C pour une période identifiée par (first..last)
inline void compute_ohlc_range(const SCStudyInterfaceRef& sc, int first, int last, double& O, double& H, double& L, double& C)
{
  if (first < 0 || last < 0 || first > last) { O=H=L=C=0.0; return; }
  O = sc.Open[first];
  H = sc.High[first];
  L = sc.Low [first];
  for (int j = first; j <= last; ++j) {
    if (sc.High[j] > H) H = sc.High[j];
    if (sc.Low [j] < L) L = sc.Low [j];
  }
  C = sc.Close[last];
}

// API pratique : OHLC pour TF + offset (0 = courant)
inline bool olGet(const SCStudyInterfaceRef& sc, OlTF tf, int offset, double& O, double& H, double& L, double& C, int* outFirst=nullptr, int* outLast=nullptr)
{
  const int i = sc.Index;
  int key=0, first=0, last=0;
  if (!find_period_by_offset(sc, i, tf, offset, key, first, last)) { O=H=L=C=0.0; return false; }
  compute_ohlc_range(sc, first, last, O, H, L, C);
  if (outFirst) *outFirst = first; if (outLast) *outLast = last;
  return true;
}

// États de croisement/rupture
inline int olCross(double price, double level) { return price < level ? 0 : (price > level ? 2 : 1); }
inline int olBreak(double price, double level) { return olCross(price, level); }

// Position relative du prix dans le range [L,H]
inline double olPosInRange(double price, double L, double H)
{
  if (H <= L) return 0.5; const double x = (price - L) / (H - L); return x < 0.0 ? 0.0 : (x > 1.0 ? 1.0 : x);
}

// Distances en ticks vers O/H/L/C
inline void olDistancesTicks(double price, double tickSize, double O, double H, double L, double C,
                             double& dO, double& dH, double& dL, double& dC)
{
  if (tickSize <= 0.0) { dO=dH=dL=dC=0.0; return; }
  dO = (price - O) / tickSize; dH = (price - H) / tickSize; dL = (price - L) / tickSize; dC = (price - C) / tickSize;
}

// Features MLP v1 (12 features) :
// 0 O, 1 H, 2 L, 3 C, 4 posInRange, 5 dO, 6 dH, 7 dL, 8 dC, 9 range(H-L), 10 mid, 11 crossState@C
inline int ol_features_v1(double price, double tickSize, double O, double H, double L, double C, double* out)
{
  if (!out) return 0;
  double dO, dH, dL, dC; olDistancesTicks(price, tickSize, O, H, L, C, dO, dH, dL, dC);
  const double range = H - L;
  const double mid   = 0.5 * (H + L);
  out[0]=du::sanitize(O); out[1]=du::sanitize(H); out[2]=du::sanitize(L); out[3]=du::sanitize(C);
  out[4]=du::sanitize(olPosInRange(price, L, H));
  out[5]=du::sanitize(dO); out[6]=du::sanitize(dH); out[7]=du::sanitize(dL); out[8]=du::sanitize(dC);
  out[9]=du::sanitize(range); out[10]=du::sanitize(mid); out[11]=(double)olCross(price, C);
  return 12;
}

// Raccourcis utiles : O/H/L/C des N dernières périodes (écrit dans des buffers)
inline int ol_last_n(const SCStudyInterfaceRef& sc, OlTF tf, int nMax, double* O, double* H, double* L, double* C)
{
  if (nMax <= 0) return 0; int wrote = 0; int i = sc.Index; int key, first, last;
  // Période courante
  key = period_key(sc.BaseDateTimeIn[i], tf);
  if (scan_period_bounds(sc, i, key, tf, first, last)) { compute_ohlc_range(sc, first, last, O[0], H[0], L[0], C[0]); wrote = 1; }
  // Périodes précédentes
  int j = first - 1; int k = 1;
  while (j >= 0 && k < nMax) {
    const int keyJ = period_key(sc.BaseDateTimeIn[j], tf);
    int f=j, l=j; for (; f>=0; --f) { if (period_key(sc.BaseDateTimeIn[f], tf)!=keyJ) { f=f+1; break; } if (f==0) {/*ok*/} }
    if (f<0) f=0; compute_ohlc_range(sc, f, j, O[k], H[k], L[k], C[k]);
    j = f - 1; k += 1; wrote += 1;
  }
  return wrote;
}

} // namespace du
