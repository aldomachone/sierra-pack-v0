#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// ============================================================================
// UtilitaireRanges.h — Ranges session/rolling, OR/IB et features (v1 enrichi)
// ----------------------------------------------------------------------------
// - Entièrement inline, aucune allocation, aucun état global.
// - Compat v0 conservée: rgRangePos(), rgIBInfo, rgIB(...), rgSessionRange(), rgPosInRange().
// - Ajouts majeurs:
//    • Rolling range multi‑barres + position relative
//    • Session Hi/Lo, Opening Range (OR) et Initial Balance (IB) en minutes
//    • Distances en ticks vers bornes/mid, états de croisement
//    • Features MLP standardisées
// - À appeler en mode bar‑based (1 appel par barre).
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Compat v0
// ---------------------------------------------------------------------------
struct 			RangeOut 				{ float lo=0, hi=0, pos=0; };
struct 			rgIBInfo 				{ double hi{0}, lo{0}; };

// Range & position relative sur N dernières barres (close vs [lo,hi])
inline 			RangeOut rgRangePos		(const SCStudyInterfaceRef& sc, int lookback)
{
  RangeOut r{};
  const int n = lookback <= 1 ? 1 : lookback;
  const int i1 = sc.Index;
  const int i0 = i1 - (n - 1) >= 0 ? i1 - (n - 1) : 0;
  r.lo = (float)sc.Low[i0]; r.hi = (float)sc.High[i0];
  for (int i=i0; i<=i1; ++i) { if (sc.Low[i] < r.lo) r.lo=(float)sc.Low[i]; if (sc.High[i]>r.hi) r.hi=(float)sc.High[i]; }
  const float p = (float)sc.Close[i1];
  const float w = (float)std::max(1e-9, (double)r.hi - (double)r.lo);
  r.pos = (p - r.lo) / w; // [0..1]
  return r;
}

inline rgIBInfo rgIB					(long long /*sessionStart*/, const double* /*firstHourHL*/) { return {}; } // compat v0 dummy
inline double   rgSessionRange			(double hi,double lo) { return hi-lo; }
inline double   rgPosInRange			(double price,double hi,double lo) { if(hi<=lo) return 0.5; return (price-lo)/((hi-lo)); }

// ---------------------------------------------------------------------------
// v1 — enrichi
// ---------------------------------------------------------------------------
struct 			RgParams
{
  bool   resetOnNewDay   = true;   // reset auto au changement YMD
  bool   useSessionTimes = true;   // reset sur IsStartOfTradingDay
  int    orMinutes       = 5;      // Opening Range durée en minutes
  int    ibMinutes       = 60;     // Initial Balance durée en minutes
  double tickSize        = 0.0;    // pour distances en ticks
};

struct 			RgState
{
  // Session courante
  int        lastYMD      = 0;      // YYYYMMDD
  SCDateTime sessionOpen  = 0.0;    // horodatage barre d'ouverture détectée
  bool       sessionInit  = false;  // a‑t‑on initialisé la session ?

  // Extrêmes session
  double sessHi = 0.0;
  double sessLo = 0.0;

  // Opening Range (OR)
  bool   orLocked = false;          // une fois OR figée
  double orHi = 0.0;
  double orLo = 0.0;

  // Initial Balance (IB)
  bool   ibLocked = false;          // une fois IB figée
  double ibHi = 0.0;
  double ibLo = 0.0;

  // Derniers dérivés
  double lastClose = 0.0;

  void reset()
  {
    sessionOpen = 0.0; sessionInit=false; lastYMD=0;
    sessHi=sessLo=0.0; orLocked=false; orHi=orLo=0.0; ibLocked=false; ibHi=ibLo=0.0; lastClose=0.0;
  }
};

// Helpers temps
inline int 		ymd						(const SCDateTime& t) { return t.GetDateYMD(); }
inline int 		minutes_diff			(const SCDateTime& a, const SCDateTime& b)
{
  // Sierra: SCDateTime est un jour julien fractionnaire → Δmin = Δj * 24*60
  const double dm = ((double)a - (double)b) * 1440.0;
  return (int)((dm >= 0.0 ? dm : -dm) + 0.5) * (dm >= 0.0 ? 1 : -1);
}

// ---------------------------------------------------------------------------
// Mise à jour bar‑based (à appeler une fois par barre)
// ---------------------------------------------------------------------------
inline void 	rg_update_bar			(SCStudyInterfaceRef sc, const RgParams& p, RgState& st)
{
  const int i = sc.Index;
  const SCDateTime t = sc.BaseDateTimeIn[i];
  const int day = ymd(t);

  // Détection nouvelle journée / début de session
  if (i == 0) { st.reset(); st.lastYMD = day; }
  if (p.resetOnNewDay && day != st.lastYMD) { st.reset(); st.lastYMD = day; }
  if (p.useSessionTimes && sc.IsStartOfTradingDay) { st.reset(); st.lastYMD = day; }

  // Init session à la première barre utile
  if (!st.sessionInit) { st.sessionInit = true; st.sessionOpen = t; st.sessHi = sc.High[i]; st.sessLo = sc.Low[i]; }

  // Update extrêmes session
  if (sc.High[i] > st.sessHi) st.sessHi = sc.High[i];
  if (sc.Low [i] < st.sessLo) st.sessLo = sc.Low[i];

  // Opening Range tant que pas expirée
  const int minsFromOpen = minutes_diff(t, st.sessionOpen);
  if (!st.orLocked)
  {
    // OR = hi/lo depuis sessionOpen jusqu'à orMinutes inclus
    const int i0 = [&](){
      int j=i; for (; j>=0; --j) { if (ymd(sc.BaseDateTimeIn[j])!=st.lastYMD) { j=j+1; break; } if (j==0) { /* ok */ } }
      return j<0?0:j;
    }();
    double hi = sc.High[i0], lo = sc.Low[i0];
    for (int j=i0; j<=i; ++j) { if (sc.High[j]>hi) hi=sc.High[j]; if (sc.Low[j]<lo) lo=sc.Low[j]; }
    st.orHi = hi; st.orLo = lo;
    if (minsFromOpen >= p.orMinutes) st.orLocked = true;
  }

  // Initial Balance tant que pas expirée
  if (!st.ibLocked)
  {
    const int i0 = [&](){
      int j=i; for (; j>=0; --j) { if (ymd(sc.BaseDateTimeIn[j])!=st.lastYMD) { j=j+1; break; } if (j==0) { /* ok */ } }
      return j<0?0:j;
    }();
    double hi = sc.High[i0], lo = sc.Low[i0];
    for (int j=i0; j<=i; ++j) { if (sc.High[j]>hi) hi=sc.High[j]; if (sc.Low[j]<lo) lo=sc.Low[j]; }
    st.ibHi = hi; st.ibLo = lo;
    if (minsFromOpen >= p.ibMinutes) st.ibLocked = true;
  }

  st.lastClose = sc.Close[i];
}

// ---------------------------------------------------------------------------
// Dérivés et utilitaires
// ---------------------------------------------------------------------------
inline double 	rg_mid					(double lo, double hi) { return 0.5*(lo+hi); }
inline int    	rg_cross				(double price, double level) { return price < level ? 0 : (price > level ? 2 : 1); }

inline void 	rg_dist_ticks			(double price, double tickSize, double lo, double hi,
                          double& dLo, double& dHi, double& dMid)
{
  if (tickSize <= 0.0) { dLo=dHi=dMid=0.0; return; }
  const double mid = rg_mid(lo,hi);
  dLo = (price - lo) / tickSize;
  dHi = (price - hi) / tickSize;
  dMid= (price - mid)/ tickSize;
}

// Position relative [0..1]
inline double 	rg_pos01				(double price, double lo, double hi)
{
  if (hi <= lo) return 0.5; const double w = hi - lo; const double x = (price - lo) / w; return x<0?0:(x>1?1:x);
}

// Rolling range brut sur N barres (écrit hi/lo)
inline void 	rg_range_last_n			(const SCStudyInterfaceRef& sc, int n, double& outLo, double& outHi)
{
  const int i1=sc.Index; const int i0 = std::max(0, i1-(std::max(1,n)-1));
  double hi=sc.High[i0], lo=sc.Low[i0];
  for(int i=i0;i<=i1;++i){ if(sc.High[i]>hi) hi=sc.High[i]; if(sc.Low[i]<lo) lo=sc.Low[i]; }
  outLo=lo; outHi=hi;
}

// ---------------------------------------------------------------------------
// Features MLP (v1) — 16 features
// 0: sess_lo  1: sess_hi  2: sess_mid  3: sess_pos01  4: sess_range
// 5: or_lo    6: or_hi    7: or_mid    8: or_pos01    9: or_locked{0,1}
// 10: ib_lo   11: ib_hi   12: ib_mid   13: ib_pos01   14: ib_locked{0,1}
// 15: last_close
// Distances en ticks disponibles via rg_dist_ticks si besoin.
// ---------------------------------------------------------------------------
inline int 	rg_features_v1				(const RgState& st, double price, const RgParams& p, double* out)
{
  if (!out) return 0;
  const double sMid = rg_mid(st.sessLo, st.sessHi);
  const double oMid = rg_mid(st.orLo,   st.orHi);
  const double iMid = rg_mid(st.ibLo,   st.ibHi);

  out[0]  = du::sanitize(st.sessLo);
  out[1]  = du::sanitize(st.sessHi);
  out[2]  = du::sanitize(sMid);
  out[3]  = du::sanitize(rg_pos01(price, st.sessLo, st.sessHi));
  out[4]  = du::sanitize(st.sessHi - st.sessLo);
  out[5]  = du::sanitize(st.orLo);
  out[6]  = du::sanitize(st.orHi);
  out[7]  = du::sanitize(oMid);
  out[8]  = du::sanitize(rg_pos01(price, st.orLo, st.orHi));
  out[9]  = st.orLocked ? 1.0 : 0.0;
  out[10] = du::sanitize(st.ibLo);
  out[11] = du::sanitize(st.ibHi);
  out[12] = du::sanitize(iMid);
  out[13] = du::sanitize(rg_pos01(price, st.ibLo, st.ibHi));
  out[14] = st.ibLocked ? 1.0 : 0.0;
  out[15] = du::sanitize(st.lastClose);
  return 16;
}

} // namespace du
