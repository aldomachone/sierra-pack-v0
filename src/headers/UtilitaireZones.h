#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// ============================================================================
// UtilitaireZones.h — Zones de prix, opérations, nearest et features (v1)
// ----------------------------------------------------------------------------
// • Entièrement inline, aucune allocation, aucun état global.
// • Compat v0 conservée : Zone {lo,hi}, znContains(), znCenter(), znDist().
// • Ajouts :
//   - Métadonnées (type, force), distance signée, clamp/expand/contract.
//   - Union/Intersection/Merge avec tolérance (ticks), normalisation (lo<=hi).
//   - Recherche nearest zone, tri par centre, filtrage par largeur/force.
//   - Packing en niveaux CfLevel‑like (lo/hi/weight), features MLP (16).
//   - Sérialisation simple (toString/fromString) pour logs.
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Compat v0
// ---------------------------------------------------------------------------
struct Zone { double lo=0.0, hi=0.0; int type=0; double strength=1.0; };

inline bool  znContains(const Zone& z, double p) { return (z.lo<=z.hi) ? (p>=z.lo && p<=z.hi) : (p>=z.hi && p<=z.lo); }
inline double znCenter (const Zone& z)           { return 0.5*((z.lo+z.hi)); }
inline double znDist   (const Zone& z, double p) {
  const double a = z.lo, b = z.hi; const double lo = a<b?a:b, hi=a<b?b:a;
  if (p<lo) return lo - p; if (p>hi) return p - hi; return 0.0; }

// ---------------------------------------------------------------------------
// Enrichi v1 — helpers
// ---------------------------------------------------------------------------
inline void znNormalize(Zone& z) { if (z.hi < z.lo) { const double t=z.lo; z.lo=z.hi; z.hi=t; } }
inline double znWidth(const Zone& z) { return (z.hi>=z.lo)? (z.hi - z.lo) : (z.lo - z.hi); }
inline double znSignedDist(const Zone& z, double p)
{ const double c=znCenter(z); if (p>=z.lo && p<=z.hi) return 0.0; return (p<c)? (z.lo-p) : (p-z.hi); }
inline double znDistTicks(const Zone& z, double p, double tick) { return tick>0.0 ? znDist(z,p)/tick : znDist(z,p); }

inline Zone znClamp(const Zone& z, double lo, double hi)
{ Zone r=z; znNormalize(r); const double L = lo<hi?lo:hi, H=lo<hi?hi:lo; if (r.lo<L) r.lo=L; if (r.hi>H) r.hi=H; return r; }
inline Zone znExpand(const Zone& z, double d) { Zone r=z; if (r.lo<=r.hi){ r.lo-=d; r.hi+=d; } else { r.lo+=d; r.hi-=d; } return r; }
inline Zone znContract(const Zone& z, double d) { return znExpand(z, -d); }

inline bool znIsValid(const Zone& z) { return std::isfinite(z.lo) && std::isfinite(z.hi); }
inline Zone znFromPoint(double p, double halfWidth, int type=0, double strength=1.0)
{ Zone z{p-halfWidth, p+halfWidth, type, strength}; znNormalize(z); return z; }

// Union / Intersection (non mutantes)
inline Zone znUnion(const Zone& a, const Zone& b)
{ Zone A=a,B=b; znNormalize(A); znNormalize(B); return Zone{ std::min(A.lo,B.lo), std::max(A.hi,B.hi), A.type, std::max(A.strength,B.strength)}; }
inline bool znIntersect(const Zone& a, const Zone& b, Zone& out)
{
  Zone A=a,B=b; znNormalize(A); znNormalize(B); const double lo = std::max(A.lo,B.lo), hi=std::min(A.hi,B.hi);
  if (lo<=hi) { out = Zone{lo,hi, A.type, 0.5*(A.strength+B.strength)}; return true; } return false;
}

// Merge d’un tableau trié par lo si chevauchement ou gap <= tol
inline int znMergeAdjacent(Zone* zs, int n, double tol)
{
  if (!zs || n<=0) return 0; int w=0; znNormalize(zs[0]); for (int i=1;i<n;++i) znNormalize(zs[i]);
  // tri rapide par lo
  for (int i=0;i<n-1;++i) for (int j=i+1;j<n;++j) if (zs[j].lo < zs[i].lo) { Zone t=zs[i]; zs[i]=zs[j]; zs[j]=t; }
  Zone cur = zs[0];
  for (int i=1;i<n;++i) {
    const bool overlap = (zs[i].lo <= cur.hi + tol);
    if (overlap) { cur.hi = std::max(cur.hi, zs[i].hi); cur.strength = std::max(cur.strength, zs[i].strength); }
    else { zs[w++] = cur; cur = zs[i]; }
  }
  zs[w++] = cur; return w;
}

// Nearest zone (renvoie index et distance absolue)
inline int znNearest(const Zone* zs, int n, double price, double* outDist=nullptr)
{
  if (!zs || n<=0) { if(outDist) *outDist=0.0; return -1; }
  double best=1e300; int bi=-1; for(int i=0;i<n;++i){ const double d=znDist(zs[i],price); if(d<best){best=d; bi=i;} }
  if(outDist) *outDist=best; return bi;
}

// Tri par centre croissant
inline void znSortByCenter(Zone* zs, int n)
{ for(int i=0;i<n-1;++i) for(int j=i+1;j<n;++j) if (znCenter(zs[j]) < znCenter(zs[i])) { Zone t=zs[i]; zs[i]=zs[j]; zs[j]=t; } }

// Filtrage
inline int znFilterMinWidth(Zone* zs, int n, double minWidth)
{ if(!zs||n<=0) return 0; int w=0; for(int i=0;i<n;++i){ if(znWidth(zs[i])>=minWidth) zs[w++]=zs[i]; } return w; }
inline int znFilterMinStrength(Zone* zs, int n, double minS)
{ if(!zs||n<=0) return 0; int w=0; for(int i=0;i<n;++i){ if(zs[i].strength>=minS) zs[w++]=zs[i]; } return w; }

// Packing dans buffers niveaux (style CfLevel)
inline int znPackLevels(const Zone* zs, int n, double* centers, double* lows, double* highs, double* weights, int maxOut)
{
  if(!zs||n<=0||maxOut<=0) return 0; int k=0; for(int i=0;i<n && k<maxOut; ++i){ centers[k]=znCenter(zs[i]); lows[k]=zs[i].lo; highs[k]=zs[i].hi; weights[k]=zs[i].strength; ++k; } return k;
}

// Sérialisation simple "lo,hi,type,strength"
inline void znToString(const Zone& z, SCString& out)
{ out.Format("%.10f,%.10f,%d,%.6f", z.lo, z.hi, z.type, z.strength); }
inline bool znFromString(const SCString& s, Zone& z)
{
  double lo=0,hi=0,st=1.0; int tp=0; if (4 != sscanf(s.GetChars(), "%lf,%lf,%d,%lf", &lo,&hi,&tp,&st)) return false; z.lo=lo; z.hi=hi; z.type=tp; z.strength=st; znNormalize(z); return true;
}

// ---------------------------------------------------------------------------
// Features MLP (v1) — 16 features pour un prix donné
// 0: n_zones, 1: nearest_dist, 2: nearest_width, 3: nearest_strength,
// 4: inside_count, 5: mean_width, 6: max_width, 7: min_width,
// 8: mean_strength, 9: max_strength, 10: min_strength,
// 11: signed_dist_nearest, 12: dist_ticks_nearest (si tick>0),
// 13: price_minus_center_nearest, 14: lo_nearest, 15: hi_nearest
// ---------------------------------------------------------------------------
inline int zn_features_v1(const Zone* zs, int n, double price, double tick, double* out)
{
  if(!out) return 0; if(!zs||n<=0){ for(int i=0;i<16;++i) out[i]=0.0; return 16; }
  double dist=0.0; const int iNear = znNearest(zs,n,price,&dist);
  const Zone& z = zs[iNear];
  int inside=0; double sumW=0, sumS=0, maxW=0, minW=1e300, maxS=0, minS=1e300;
  for(int i=0;i<n;++i){ const double w=znWidth(zs[i]); const double s=zs[i].strength; if(znContains(zs[i],price)) inside++; sumW+=w; sumS+=s; if(w>maxW)maxW=w; if(w<minW)minW=w; if(s>maxS)maxS=s; if(s<minS)minS=s; }
  const double meanW = sumW / n; const double meanS = sumS / n;

  out[0]  = (double)n;
  out[1]  = du::sanitize(dist);
  out[2]  = du::sanitize(znWidth(z));
  out[3]  = du::sanitize(z.strength);
  out[4]  = (double)inside;
  out[5]  = du::sanitize(meanW);
  out[6]  = du::sanitize(maxW);
  out[7]  = du::sanitize(minW);
  out[8]  = du::sanitize(meanS);
  out[9]  = du::sanitize(maxS);
  out[10] = du::sanitize(minS);
  const double sdist = znSignedDist(z, price);
  out[11] = du::sanitize(sdist);
  out[12] = du::sanitize(tick>0.0? dist/tick : dist);
  out[13] = du::sanitize(price - znCenter(z));
  out[14] = du::sanitize(z.lo);
  out[15] = du::sanitize(z.hi);
  return 16;
}

} // namespace du
