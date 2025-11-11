#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// =============================================================================
// UtilitaireProfile.h — Volume Profile: POC, VAH/VAL, histos, features (v1)
// -----------------------------------------------------------------------------
// • 100% inline, aucune allocation, aucun état global.
// • Deux modes de construction:
//    1) Fallback bar-based (approx) : répartit le volume de la barre sur [L..H]
//    2) VAP (si disponible)         : lit sc.VolumeAtPriceForBars (précis)
// • Sorties clés: POC prix/vol, VAH/VAL à % cumulable, skewness, nearest dist.
// • Buffers passés par l’appelant: hist[] (nBins), edges[] optionnel.
// • Compat v0 conservée (vpPocSimple, vpPoc, vpVa, vpShift).
// =============================================================================

namespace du {

// -----------------------------------------------------------------------------
// Compat v0
// -----------------------------------------------------------------------------
struct ProfileOut { float poc=0, vMax=0; };

inline ProfileOut vpPocSimple(const SCStudyInterfaceRef& sc, int lookback)
{
  ProfileOut out{};
  const int i0 = std::max(0, sc.Index - std::max(1, lookback) + 1);
  int iBest = i0; float vBest = (float)sc.Volume[i0];
  for (int i=i0; i<=sc.Index; ++i){ const float v=(float)sc.Volume[i]; if(v>vBest){vBest=v;iBest=i;} }
  out.poc  = (float)sc.Close[iBest];
  out.vMax = vBest; return out;
}

inline int   vpPoc(const double* hist,int n){ int idx=0; double mx=-1e300; for(int i=0;i<n;++i){ if(hist[i]>mx){mx=hist[i]; idx=i;} } return idx; }
inline void  vpVa (const double* /*hist*/, int /*n*/, double /*pct*/, int& lo, int& hi){ lo=0; hi=0; }
inline int   vpShift(int currPOC,int prevPOC){ return currPOC-prevPOC; }

// -----------------------------------------------------------------------------
// v1 — Enrichi
// -----------------------------------------------------------------------------
struct VpParams
{
  double  tickSize      = 0.0;   // taille de tick
  double  pctVA         = 0.68;  // % du volume couvert par la VA (0.68, 0.70, 0.95)
  bool    useVAP        = true;  // tenter d’utiliser VolumeAtPriceForBars
  bool    clampZeros    = true;  // sanitize histogramme
};

struct VpStats
{
  // Histogramme agrégé
  double totalVol = 0.0;
  int    pocIdx   = -1; // index dans hist[]
  double pocPrice = 0.0;
  double pocVol   = 0.0;

  // Vale Area
  int    vaLoIdx  = 0;
  int    vaHiIdx  = 0;
  double vaLo     = 0.0;
  double vaHi     = 0.0;

  // Moments grossiers
  double meanIdx  = 0.0;   // moyenne pondérée sur indices
  double skew     = 0.0;   // moment 3 normalisé
  double kurt     = 0.0;   // moment 4 normalisé
};

// Map prix → index de bin
inline int vp_bin_index(double price, double lo, double tick, int nBins)
{
  if (tick <= 0.0) return 0; const double x = (price - lo) / tick; int i = (int)std::floor(x + 0.5);
  if (i < 0) i = 0; if (i > nBins-1) i = nBins-1; return i;
}

// Construction fallback bar-based: on répartit le volume entre Low..High
inline void vp_build_hist_bar_based(const SCStudyInterfaceRef& sc, int first, int last,
                                   double loPrice, double tick, double* hist, int nBins)
{
  if (!hist || nBins<=0) return; for(int i=0;i<nBins;++i) hist[i]=0.0;
  for (int i = std::max(0, first); i <= last; ++i)
  {
    const double L = sc.Low[i], H = sc.High[i];
    if (!(H>=L)) continue; const double V = (double)sc.Volume[i]; if (V<=0.0) continue;
    const int li = vp_bin_index(L, loPrice, tick, nBins);
    const int hi = vp_bin_index(H, loPrice, tick, nBins);
    const int span = std::max(1, hi - li + 1);
    const double vper = V / span;
    for (int k=0, j=li; j<=hi && j<nBins; ++j,++k) hist[j] += vper;
  }
}

// Construction précise via VAP (si VolumeAtPriceForBars actif)
inline bool vp_build_hist_vap(const SCStudyInterfaceRef& sc, int first, int last,
                              double loPrice, double tick, double* hist, int nBins)
{
  if (!hist || nBins<=0) return false; for(int i=0;i<nBins;++i) hist[i]=0.0;
  if (sc.VolumeAtPriceForBars == nullptr) return false;

  for (int i = std::max(0, first); i <= last; ++i)
  {
    const int priceCount = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(i);
    for (int p=0; p<priceCount; ++p)
    {
      s_VolumeAtPriceV2 row{};
      if (!sc.VolumeAtPriceForBars->GetVAPRowAtIndex(i, p, row)) continue;
      const double price = row.PriceInTicks * tick + sc.TickSize * 0.0 + 0.0; // PriceInTicks * TickSize
      const double vol   = (double)row.Volume;
      if (vol <= 0.0) continue;
      const int bi = vp_bin_index(price, loPrice, tick, nBins);
      hist[bi] += vol;
    }
  }
  return true;
}

// Sanitize histogramme (clamp NaN/neg)
inline void vp_hist_sanitize(double* hist, int n)
{
  if(!hist) return; for(int i=0;i<n;++i){ if(!std::isfinite(hist[i]) || hist[i]<0.0) hist[i]=0.0; }
}

// Totaux + POC
inline void vp_poc_and_total(const double* hist, int n, int& pocIdx, double& total, double& pocVol)
{
  total=0.0; pocIdx=0; double mx=-1e300; pocVol=0.0; for(int i=0;i<n;++i){ const double v=hist[i]; total+=v; if(v>mx){mx=v; pocIdx=i;} }
  pocVol = (n>0? hist[pocIdx] : 0.0);
}

// Vale Area par expansion autour du POC jusqu’à couvrir pctVA du volume
inline void vp_value_area(const double* hist, int n, int pocIdx, double pctVA, int& vaLo, int& vaHi)
{
  vaLo = vaHi = pocIdx; if(n<=0) return; double total=0.0; for(int i=0;i<n;++i) total+=hist[i];
  const double target = pctVA <= 0.0 ? 0.0 : (pctVA >= 1.0 ? total : pctVA * total);
  double sum = hist[pocIdx]; int L=pocIdx, H=pocIdx;
  while (sum < target && (L>0 || H<n-1))
  {
    const double lVal = (L>0? hist[L-1] : -1.0);
    const double hVal = (H<n-1? hist[H+1] : -1.0);
    if (hVal >= lVal) { if (H<n-1){ ++H; sum += hVal; } else if (L>0){ --L; sum += lVal; } }
    else               { if (L>0){ --L; sum += lVal; } else if (H<n-1){ ++H; sum += hVal; } }
  }
  vaLo=L; vaHi=H;
}

// Moments sur indices (skew/kurt) normalisés par variance^1.5 et variance^2
inline void vp_moments_idx(const double* hist, int n, double& meanI, double& skew, double& kurt)
{
  meanI=0.0; skew=0.0; kurt=0.0; double sum=0.0; for(int i=0;i<n;++i) sum+=hist[i]; if(sum<=0.0) return;
  for(int i=0;i<n;++i) meanI += i*hist[i]; meanI/=sum; double m2=0.0,m3=0.0,m4=0.0;
  for(int i=0;i<n;++i){ const double d=i-meanI; const double w=hist[i]; m2+=w*d*d; m3+=w*d*d*d; m4+=w*d*d*d*d; }
  m2/=sum; m3/=sum; m4/=sum; const double s = (m2>1e-12? std::sqrt(m2) : 0.0);
  skew = (s>0.0? m3/(s*s*s) : 0.0); kurt = (m2>0.0? m4/(m2*m2) : 0.0);
}

// Pipeline complet de construction + stats
inline void vp_build_and_stats(SCStudyInterfaceRef sc, int first, int last,
                               double loPrice, double tick, double* hist, int nBins,
                               const VpParams& p, VpStats& st)
{
  bool okVAP=false; if(p.useVAP) okVAP = vp_build_hist_vap(sc, first, last, loPrice, tick, hist, nBins);
  if(!okVAP) vp_build_hist_bar_based(sc, first, last, loPrice, tick, hist, nBins);
  if(p.clampZeros) vp_hist_sanitize(hist, nBins);

  // Stats
  vp_poc_and_total(hist, nBins, st.pocIdx, st.totalVol, st.pocVol);
  st.pocPrice = loPrice + st.pocIdx * tick;
  vp_value_area(hist, nBins, st.pocIdx, p.pctVA, st.vaLoIdx, st.vaHiIdx);
  st.vaLo = loPrice + st.vaLoIdx * tick; st.vaHi = loPrice + st.vaHiIdx * tick;
  vp_moments_idx(hist, nBins, st.meanIdx, st.skew, st.kurt);
}

// Distance en ticks depuis un prix vers POC/VA bornes
inline void vp_distances_ticks(const VpStats& st, double price, double tick, double& dPOC, double& dVAL, double& dVAH)
{
  if (tick<=0.0){ dPOC=dVAL=dVAH=0.0; return; }
  dPOC = (price - st.pocPrice)/tick; dVAL = (price - st.vaLo)/tick; dVAH = (price - st.vaHi)/tick;
}

// -----------------------------------------------------------------------------
// Features MLP v1 — 16 features
// 0: poc_price, 1: poc_vol, 2: va_lo, 3: va_hi, 4: va_width, 5: total_vol,
// 6: mean_idx, 7: skew, 8: kurt, 9: dPOC_ticks, 10: dVAL_ticks, 11: dVAH_ticks,
// 12: in_VA{0,1}, 13: poc_ratio (poc_vol/total), 14: va_coverage (≈pctVA atteint), 15: bins
// -----------------------------------------------------------------------------
inline int vp_features_v1(const VpStats& st, double price, double tick, int nBins, double* out)
{
  if(!out) return 0; double dPOC=0,dVAL=0,dVAH=0; vp_distances_ticks(st, price, tick, dPOC, dVAL, dVAH);
  const int inVA = (st.vaLo<=st.vaHi && price>=st.vaLo && price<=st.vaHi)? 1 : 0;
  const double vaWidth = st.vaHi - st.vaLo;
  const double pocRatio = (st.totalVol>0.0? st.pocVol/st.totalVol : 0.0);
  // la couverture réelle dépend de hist, non conservée ici → approx = param cible
  out[0]=du::sanitize(st.pocPrice);
  out[1]=du::sanitize(st.pocVol);
  out[2]=du::sanitize(st.vaLo);
  out[3]=du::sanitize(st.vaHi);
  out[4]=du::sanitize(vaWidth);
  out[5]=du::sanitize(st.totalVol);
  out[6]=du::sanitize(st.meanIdx);
  out[7]=du::sanitize(st.skew);
  out[8]=du::sanitize(st.kurt);
  out[9]=du::sanitize(dPOC);
  out[10]=du::sanitize(dVAL);
  out[11]=du::sanitize(dVAH);
  out[12]=(double)inVA;
  out[13]=du::sanitize(pocRatio);
  out[14]=0.0; // placeholder pour coverage réel si calculé côté appelant
  out[15]=(double)nBins; return 16;
}

} // namespace du
