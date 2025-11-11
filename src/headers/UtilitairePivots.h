#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"
#include "UtilitaireOHLC.h" // olGet()

// ============================================================================
// UtilitairePivots.h — Pivots multi‑formules + multi‑TF + helpers avancés (v1.1)
// ----------------------------------------------------------------------------
//  • 100% inline, aucune allocation, aucun état global.
//  • Compat v0 maintenue (pvClassic, pvCompute, pvState).
//  • Formules : Classic, Fibonacci, Camarilla, Woodie, DeMark.
//  • Multi‑TF + offset via OHLC scan (Daily/Weekly/Monthly/Yearly).
//  • Outils : pack niveaux, nearest en ticks, extraction directe depuis sc.
//  • Features MLP v1 (16) conservées.
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Compat v0
// ---------------------------------------------------------------------------
struct PivotsClassic { float P=0, R1=0, S1=0, R2=0, S2=0, R3=0, S3=0; };

inline PivotsClassic pvClassic(float H, float L, float C)
{
  PivotsClassic p{}; const float P = (H + L + C) / 3.0f; p.P = P;
  p.R1 = 2*P - L;        p.S1 = 2*P - H;
  p.R2 = P + (H - L);    p.S2 = P - (H - L);
  p.R3 = H + 2*(P - L);  p.S3 = L - 2*(H - P);
  return p;
}

inline void pvCompute(int /*set*/, int /*tf*/, double O,double H,double L,double C, double* out)
{ if(!out) return; out[0] = (H+L+C)/3.0; }

inline int pvState(double price,double level){ return price>level? +1 : price<level? -1 : 0; }

// ---------------------------------------------------------------------------
// v1.1 — enrichi
// ---------------------------------------------------------------------------

enum PivotSet : int { PV_CLASSIC=0, PV_FIBO=1, PV_CAMARILLA=2, PV_WOODIE=3, PV_DEMARK=4 };
enum PivotTF  : int { PV_DAILY=0, PV_WEEKLY=1, PV_MONTHLY=2, PV_YEARLY=3 };

struct PivotOut
{
  // Ordre indices : 0:P, 1:R1,2:R2,3:R3,4:R4, 5:S1,6:S2,7:S3,8:S4
  double L[9] {0};
};

// Helpers
inline int  pv_cross(double price,double level){ return price<level?0:(price>level?2:1); }
inline void pv_dist_ticks(double price,double tick,double level,double& d){ d = tick>0.0? (price-level)/tick : 0.0; }
inline double mid(double a,double b){ return 0.5*(a+b); }

// ----------------------- Formules -----------------------
inline void pv_compute_classic(double H,double L,double C,PivotOut& o)
{
  const double P = (H+L+C)/3.0; o.L[0]=P;
  o.L[1] = 2*P - L;          o.L[5] = 2*P - H;
  o.L[2] = P + (H - L);      o.L[6] = P - (H - L);
  o.L[3] = H + 2*(P - L);    o.L[7] = L - 2*(H - P);
}

inline void pv_compute_fibo(double H,double L,double C,PivotOut& o)
{
  const double P=(H+L+C)/3.0; o.L[0]=P; const double R = H-L;
  o.L[1]=P+0.382*R; o.L[2]=P+0.618*R; o.L[3]=P+1.000*R; o.L[4]=P+1.618*R;
  o.L[5]=P-0.382*R; o.L[6]=P-0.618*R; o.L[7]=P-1.000*R; o.L[8]=P-1.618*R;
}

inline void pv_compute_camarilla(double H,double L,double C,PivotOut& o)
{
  const double R=H-L; const double k1=1.1; // variante classique
  o.L[0]=C; // centre = Close
  o.L[1]=C + (R*k1)/12.0; o.L[2]=C + (R*k1)/6.0;  o.L[3]=C + (R*k1)/4.0;  o.L[4]=C + (R*k1)/2.0;
  o.L[5]=C - (R*k1)/12.0; o.L[6]=C - (R*k1)/6.0;  o.L[7]=C - (R*k1)/4.0;  o.L[8]=C - (R*k1)/2.0;
}

inline void pv_compute_woodie(double H,double L,double C,double O,PivotOut& o)
{
  const double P=(H+L+2*C)/4.0; o.L[0]=P;
  o.L[1]=2*P - L;          o.L[5]=2*P - H;
  o.L[2]=P + (H - L);      o.L[6]=P - (H - L);
  o.L[3]=H + 2*(P - L);    o.L[7]=L - 2*(H - P);
}

inline void pv_compute_demark(double H,double L,double C,double O,PivotOut& o)
{
  double X = 0.0;
  if (C < O)      X = H + 2.0*L + C;
  else if (C > O) X = 2.0*H + L + C;
  else            X = H + L + 2.0*C;
  const double P = X / 4.0; o.L[0]=P;
  o.L[1] = X/2.0 - L; // R1
  o.L[5] = X/2.0 - H; // S1
}

inline void pv_compute(PivotSet set,double O,double H,double L,double C,PivotOut& o)
{
  switch(set){
    case PV_CLASSIC:   pv_compute_classic(H,L,C,o); break;
    case PV_FIBO:      pv_compute_fibo(H,L,C,o);    break;
    case PV_CAMARILLA: pv_compute_camarilla(H,L,C,o);break;
    case PV_WOODIE:    pv_compute_woodie(H,L,C,O,o); break;
    case PV_DEMARK:    pv_compute_demark(H,L,C,O,o); break;
  }
}

// ----------------------- Multi‑TF depuis sc -----------------------
inline bool pv_from_sc(const SCStudyInterfaceRef& sc, du::OlTF tf, int offset, PivotSet set,
                       double& O,double& H,double& L,double& C, PivotOut& out)
{
  if (!du::olGet(sc, (du::OlTF)tf, offset, O,H,L,C)) { O=H=L=C=0.0; for(double& x: out.L) x=0.0; return false; }
  pv_compute(set,O,H,L,C,out); return true;
}

// ----------------------- Packing / Nearest -----------------------
inline int pv_pack_levels(const PivotOut& po, double* out, int maxOut)
{
  if(!out||maxOut<=0) return 0; int k=0; for(int i=0;i<9 && k<maxOut; ++i) out[k++]=po.L[i]; return k;
}

inline int pv_nearest(double price, const PivotOut& po, double tickSize, int& idx, double& distTicks)
{
  idx=-1; distTicks=0.0; double best=1e300; int bi=-1; for(int i=0;i<9;++i){ const double v=po.L[i]; if(v==0.0) continue; const double d=std::fabs(price-v); if(d<best){best=d; bi=i;} }
  idx=bi; distTicks = (bi>=0? (tickSize>0.0? best/tickSize : best) : 0.0); return bi;
}

// ----------------------- Features MLP v1 (16) -----------------------
inline int pv_features_v1(const PivotOut& po, double price, double tick, double H, double L, double* out)
{
  if(!out) return 0; const double P = po.L[0];
  out[0]=du::sanitize(P);
  out[1]=du::sanitize(po.L[1]); out[2]=du::sanitize(po.L[2]); out[3]=du::sanitize(po.L[3]); out[4]=du::sanitize(po.L[4]);
  out[5]=du::sanitize(po.L[5]); out[6]=du::sanitize(po.L[6]); out[7]=du::sanitize(po.L[7]); out[8]=du::sanitize(po.L[8]);
  out[9] = tick>0.0? du::sanitize((price-P)/tick) : 0.0;      // dist ticks à P
  out[10]= (double)pv_cross(price,P);                         // cross @P
  const double band = H - L; out[11]=du::sanitize(band);
  out[12]=du::sanitize(mid(H,L));
  double span = 0.0; for(int k=1;k<=8;++k){ const double d = std::fabs(po.L[k]-P); if(d>span) span=d; }
  out[13]=du::sanitize(span);
  out[14]=0.0; out[15]=0.0;
  return 16;
}

} // namespace du