#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// ============================================================================
// UtilitaireFibo.h — Retracements/Extensions, nearest, packing, features (v1)
// ----------------------------------------------------------------------------
// • 100% inline, aucune allocation, aucun état global.
// • Compat v0 conservée : FiboSet, fbRetrace(float hi,lo), fbLevels(hi,lo,out),
//   fbSnap(price, levels[], n, idx, dist).
// • Ajouts majeurs :
//   - Retracements complets (23.6, 38.2, 50, 61.8, 78.6) + Extensions (127.2,
//     138.2, 161.8, 200, 261.8) pour swing Up/Down.
//   - Trend & position relative dans le swing, distances en ticks.
//   - Nearest level, packaging niveaux, séries rapides bar‑based.
//   - Features MLP standardisées (14).
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Compat v0
// ---------------------------------------------------------------------------
struct 			FiboSet 					{ float lv236=0, lv382=0, lv500=0, lv618=0, lv786=0; };

// Retracements entre swingHi et swingLo (up si hi>=lo) — compat v0
inline 			FiboSet fbRetrace	(float swingHi, float swingLo)
{
  FiboSet f{};
  const bool up = swingHi >= swingLo;
  const float a = up ? swingHi : swingLo; // top
  const float b = up ? swingLo : swingHi; // bottom
  const float d = a - b;
  f.lv236 = a - 0.236f*d;
  f.lv382 = a - 0.382f*d;
  f.lv500 = a - 0.500f*d;
  f.lv618 = a - 0.618f*d;
  f.lv786 = a - 0.786f*d;
  return f;
}

inline void 	fbLevels					(double hi,double lo,double* out){
	if(!out) return; const double r = hi-lo; out[0]=hi; out[1]=hi-0.618*r; out[2]=lo; }

inline void 	fbSnap						(double price,const double* lv,int n,int& idx,double& dist)
{
  idx=-1; dist=0.0; if(!lv||n<=0){return;} double best=1e300; int bi=-1;
  for(int i=0;i<n;++i){ const double d = std::fabs(price-lv[i]); if(d<best){best=d; bi=i;} }
  idx=bi; dist=(bi>=0? best:0.0);
}

// ---------------------------------------------------------------------------
// Enrichi v1 — structures et helpers
// ---------------------------------------------------------------------------
struct 			FiboFull
{
  // Retracements standard (top→bottom pour Up; bottom→top pour Down)
  double r236=0, r382=0, r500=0, r618=0, r786=0;
  // Extensions au‑delà du swing
  double x1272=0, x1382=0, x1618=0, x2000=0, x2618=0;
};

inline int 		fbTrendFromSwings			(double hi,double lo){
	return (hi>=lo)? +1 : -1; }

inline double 	fbRange						(double hi,double lo) { return hi-lo; }
inline double 	fbMid						(double hi,double lo)   { return 0.5*(hi+lo); }
inline double 	fbPos01						(double price,double hi,double lo){
	const double w=hi-lo; if(w<=0) return 0.5; const double x=(price-lo)/w; return x<0?0:(x>1?1:x); }

// Retracements + extensions, Up si hi>=lo, sinon Down automatiquement
inline 			FiboFull fbComputeFull		(double hi,double lo)
{
  FiboFull F{}; const bool up = (hi>=lo); const double a = up? hi:lo; const double b = up? lo:hi; const double d=a-b;
  // retracements entre a et b
  F.r236 = a - 0.236*d; F.r382 = a - 0.382*d; F.r500 = a - 0.500*d; F.r618 = a - 0.618*d; F.r786 = a - 0.786*d;
  // extensions à partir du swing
  // conventions courantes : 1.272, 1.382, 1.618, 2.000, 2.618 de l’amplitude
  if(up){
    F.x1272 = a + 0.272*d; F.x1382 = a + 0.382*d; F.x1618 = a + 0.618*d; F.x2000 = a + 1.000*d; F.x2618 = a + 1.618*d;
  }else{
    F.x1272 = b - 0.272*d; F.x1382 = b - 0.382*d; F.x1618 = b - 0.618*d; F.x2000 = b - 1.000*d; F.x2618 = b - 1.618*d;
  }
  return F;
}

// Packing des niveaux dans un buffer (ordre retracements puis extensions)
inline int 		fbPackLevels				(const FiboFull& F, double* out, int maxOut)
{
  if(!out||maxOut<=0) return 0; int k=0;
  auto PUT=[&](double v){ if(k<maxOut) out[k++]=v; };
  PUT(F.r236); PUT(F.r382); PUT(F.r500); PUT(F.r618); PUT(F.r786);
  PUT(F.x1272); PUT(F.x1382); PUT(F.x1618); PUT(F.x2000); PUT(F.x2618);
  return k;
}

// Nearest avec conversion optionnelle en ticks
inline int 		fbNearest					(double price,const double* lv,int n,double tickSize,int& idx,double& distTicks)
{
  idx=-1; distTicks=0.0; if(!lv||n<=0) return -1; double best=1e300; int bi=-1;
  for(int i=0;i<n;++i){ const double d=std::fabs(price-lv[i]); if(d<best){best=d; bi=i;} }
  idx=bi; distTicks = (bi>=0? (tickSize>0.0? best/tickSize : best) : 0.0); return bi;
}

// Séries bar‑based minimales: récupère hi/lo d’un intervalle et renvoie FiboFull
inline 			FiboFull fbFromSeries_HiLo	(const SCStudyInterfaceRef& sc, int first, int last)
{
  if(first<0) first=0; if(last<first) last=first; double hi=sc.High[first], lo=sc.Low[first];
  for(int i=first;i<=last;++i){ if(sc.High[i]>hi) hi=sc.High[i]; if(sc.Low[i]<lo) lo=sc.Low[i]; }
  return fbComputeFull(hi,lo);
}

// ---------------------------------------------------------------------------
// Features MLP v1 — 14 features
// 0: trend{+1/-1}, 1: swing_hi, 2: swing_lo, 3: range, 4: mid,
// 5: pos01 (prix vs swing), 6: nearest_idx, 7: dist_ticks_nearest,
// 8: r618, 9: r382, 10: r786, 11: x1272, 12: x1618, 13: x2000
// ---------------------------------------------------------------------------
inline int 		fb_features_v1				(double price,double hi,double lo,double tickSize,const FiboFull& F,double* out)
{
  if(!out) return 0; const int trend = fbTrendFromSwings(hi,lo); double lv[10]; const int n=fbPackLevels(F,lv,10);
  int idx=-1; double distT=0.0; fbNearest(price, lv, n, tickSize, idx, distT);
  out[0]  = (double)trend;
  out[1]  = du::sanitize(hi);
  out[2]  = du::sanitize(lo);
  out[3]  = du::sanitize(hi-lo);
  out[4]  = du::sanitize(fbMid(hi,lo));
  out[5]  = du::sanitize(fbPos01(price,hi,lo));
  out[6]  = (double)idx;
  out[7]  = du::sanitize(distT);
  out[8]  = du::sanitize(F.r618);
  out[9]  = du::sanitize(F.r382);
  out[10] = du::sanitize(F.r786);
  out[11] = du::sanitize(F.x1272);
  out[12] = du::sanitize(F.x1618);
  out[13] = du::sanitize(F.x2000);
  return 14;
}

} // namespace du
