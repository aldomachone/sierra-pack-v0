#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"
#include "UtilitaireZones.h"   // suppose : znDist(), Zone {lo,hi,type,strength}
#include "UtilitairePivots.h"  // PivotOut
#include "UtilitaireOHLC.h"    // OHLC helpers
#include "UtilitaireVWAP.h"    // VWAP helpers
#include "UtilitaireRanges.h"  // Ranges helpers

// ============================================================================
// UtilitaireConfluence.h — Score de confluence multi‑sources + features (v1)
// ----------------------------------------------------------------------------
//  • Entièrement inline, aucune allocation, aucun état global.
//  • Compat v0 conservée : cfConfluenceScore(zones, n, price, minDist),
//    cfNearestZone(price, zones[], n, idx, dist), cfScoreWeighted(...).
//  • Ajouts :
//      - Modèle de « niveaux » hétérogènes (pivots, OHLC, VWAP, OR/IB, zones),
//        avec poids par type et décroissance 1/(ε+distance).
//      - Détection du niveau le plus proche et métadonnées associées.
//      - Normalisation optionnelle par l’échelle de ticks.

//      - Paquet de 16 features MLP pour la confluence locale.
// ----------------------------------------------------------------------------
// Conventions :
//  - price et levels en prix natif Sierra.
//  - tickSize facultatif, utilisé pour convertir les distances en ticks.
//  - « distance » d(x,level) = |x - levelMid| pour un niveau ponctuel, ou
//    0 si x est à l’intérieur d’une zone [lo,hi].
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Compat v0 — inchangé
// ---------------------------------------------------------------------------
inline float 	cfConfluenceScore			(const Zone* zs, int count, float price, float minDist)
{
  float s = 0.0f; const float md = std::max(1e-6f, minDist);
  for (int i = 0; i < count; ++i) s += 1.0f / std::max(md, (float)znDist(zs[i], price));
  return du::sanitize(s);
}

inline void 	cfNearestZone				(double price, const double* zones, int n, int& idx, double& dist)
{
  idx = -1; dist = 0.0; if (!zones || n <= 0) return;
  double best = 1e300; int bi = -1;
  for (int i = 0; i < n; ++i) {
    const double d = std::fabs(zones[i] - price);
    if (d < best) { best = d; bi = i; }
  }
  idx = bi; dist = (bi >= 0) ? best : 0.0;
}

inline double 	cfScoreWeighted				(const double* items, const double* w, int n)
{
  if (!items || n <= 0) return 0.0; double s = 0.0;
  for (int i = 0; i < n; ++i) s += items[i] * (w ? w[i] : 1.0);
  return du::sanitize(s);
}

// ---------------------------------------------------------------------------
// v1 — enrichi : modèle de niveau unifié
// ---------------------------------------------------------------------------
enum 			CfType : int {
  CF_VWAP = 0,
  CF_PIVOT_P,
  CF_PIVOT_R1, CF_PIVOT_R2, CF_PIVOT_R3, CF_PIVOT_R4,
  CF_PIVOT_S1, CF_PIVOT_S2, CF_PIVOT_S3, CF_PIVOT_S4,
  CF_OHLC_O, CF_OHLC_H, CF_OHLC_L, CF_OHLC_C,
  CF_RANGE_OR_HI, CF_RANGE_OR_LO,
  CF_RANGE_IB_HI, CF_RANGE_IB_LO,
  CF_ZONE_LO, CF_ZONE_HI,
  CF_CUSTOM
};

struct 			CfLevel
{
  double value = 0.0;   // prix du niveau, ou milieu de zone
  double lo    = 0.0;   // bornes pour zones, sinon = value
  double hi    = 0.0;
  double w     = 1.0;   // poids de ce niveau
  int    type  = CF_CUSTOM;
};

// Distance (prix → niveau) en prix natif. Si zone [lo,hi], distance = 0 si inside.
inline double 	cf_distance_native			(double price, const CfLevel& L)
{
  if (L.hi > L.lo) {
    if (price >= L.lo && price <= L.hi) return 0.0;
    const double mid = 0.5 * (L.lo + L.hi);
    return std::fabs(price - mid);
  }
  const double c = (L.value > 0.0 ? L.value : 0.5 * (L.lo + L.hi));
  return std::fabs(price - c);
}

inline double 	cf_distance_ticks			(double price, const CfLevel& L, double tickSize)
{
  const double d = cf_distance_native(price, L);
  return (tickSize > 0.0) ? (d / tickSize) : d;
}

// Kernel 1/(eps + distance)
inline double 	cf_kernel_inv				(double distTicks, double epsTicks)
{
  const double e = epsTicks > 0.0 ? epsTicks : 1e-6;
  return 1.0 / (e + std::max(0.0, distTicks));
}

// Score total = Σ w_i * K( dist_i )
inline double 	cf_score_levels				(double price, const CfLevel* L, int n, double tickSize, double epsTicks)
{
  if (!L || n <= 0) return 0.0; double s = 0.0;
  for (int i = 0; i < n; ++i) {
    const double d  = cf_distance_ticks(price, L[i], tickSize);
    const double ki = cf_kernel_inv(d, epsTicks);
    s += L[i].w * ki;
  }
  return du::sanitize(s);
}

// Recherche du niveau le plus proche
inline int 		cf_nearest_level			(double price, const CfLevel* L, int n, double tickSize, double& outDistTicks)
{
  outDistTicks = 0.0; if (!L || n <= 0) return -1;
  double best = 1e300; int bi = -1;
  for (int i = 0; i < n; ++i) {
    const double d = cf_distance_ticks(price, L[i], tickSize);
    if (d < best) { best = d; bi = i; }
  }
  outDistTicks = (bi >= 0) ? best : 0.0;
  return bi;
}

// Construction d’un petit tableau de niveaux standard à partir de sorties déjà calculées
inline int 		cf_pack_levels_from_sources	(
  double price,
  const du::PivotOut* piv,
  const du::VwapAdvState* vw,
  const du::RgState* rg,
  const du::OHLC* ohlcSimple,
  const du::Zone* zones, int nZones,
  CfLevel* out, int maxOut)
{
  int k = 0;
  auto push = [&](double v, int type, double w){ if (k < maxOut) { out[k++] = CfLevel{v, v, v, w, type}; } };
  auto pushZ = [&](double lo, double hi, int type, double w){ if (k < maxOut) { out[k++] = CfLevel{0.5*(lo+hi), lo, hi, w, type}; } };

  // VWAP
  if (vw) push(vw->lastVwap, CF_VWAP, 1.0);

  // Pivots (si fournis)
  if (piv) {
    push(piv->L[0], CF_PIVOT_P, 1.2);
    for (int i = 1; i <= 4; ++i) if (piv->L[i]  != 0.0) push(piv->L[i],  CF_PIVOT_R1 + (i - 1), 1.0);
    for (int i = 5; i <= 8; ++i) if (piv->L[i]  != 0.0) push(piv->L[i],  CF_PIVOT_S1 + (i - 5), 1.0);
  }

  // OHLC simple (période courante)
  if (ohlcSimple) {
    push(ohlcSimple->O, CF_OHLC_O, 0.8);
    push(ohlcSimple->H, CF_OHLC_H, 1.0);
    push(ohlcSimple->L, CF_OHLC_L, 1.0);
    push(ohlcSimple->C, CF_OHLC_C, 0.8);
  }

  // OR/IB issus de Ranges
  if (rg) {
    push(rg->orHi, CF_RANGE_OR_HI, 0.9);
    push(rg->orLo, CF_RANGE_OR_LO, 0.9);
    push(rg->ibHi, CF_RANGE_IB_HI, 0.9);
    push(rg->ibLo, CF_RANGE_IB_LO, 0.9);
  }

  // Zones utilisateur
  for (int i = 0; i < nZones; ++i) pushZ(zones[i].lo, zones[i].hi, CF_ZONE_LO, zones[i].strength > 0 ? zones[i].strength : 1.0);

  return k;
}

// ---------------------------------------------------------------------------
// Features MLP v1 — 16 features autour du prix courant
// 0: score_total, 1: dist_nearest_ticks, 2: type_nearest, 3: w_nearest,
// 4: n_inside (nb de zones contenant le prix), 5: score_vwap_only,
// 6: score_pivots_only, 7: score_ohlc_only, 8: score_range_only,
// 9: score_zones_only, 10..15 réservées pour extensions.
// ---------------------------------------------------------------------------
inline int 		cf_features_v1				(double price, const CfLevel* L, int n, double tickSize, double epsTicks, double* out)
{
  if (!out) return 0;
  out[0] = du::sanitize(cf_score_levels(price, L, n, tickSize, epsTicks));

  double dNear = 0.0; const int iNear = cf_nearest_level(price, L, n, tickSize, dNear);
  out[1] = du::sanitize(dNear);
  out[2] = (double)(iNear >= 0 ? L[iNear].type : -1);
  out[3] = du::sanitize(iNear >= 0 ? L[iNear].w : 0.0);

  // Compte combien de zones contiennent le prix
  int nInside = 0; for (int i = 0; i < n; ++i) if (L[i].hi > L[i].lo && price >= L[i].lo && price <= L[i].hi) nInside++;
  out[4] = (double)nInside;

  auto scoreFilter = [&](auto pred){ double s=0.0; for(int i=0;i<n;++i){ if(pred(L[i])){ const double d=cf_distance_ticks(price,L[i],tickSize); s+= L[i].w * cf_kernel_inv(d, epsTicks);} } return du::sanitize(s); };
  out[5] = scoreFilter([](const CfLevel& x){ return x.type == CF_VWAP; });
  out[6] = scoreFilter([](const CfLevel& x){ return x.type>=CF_PIVOT_P && x.type<=CF_PIVOT_S4; });
  out[7] = scoreFilter([](const CfLevel& x){ return x.type>=CF_OHLC_O && x.type<=CF_OHLC_C; });
  out[8] = scoreFilter([](const CfLevel& x){ return x.type==CF_RANGE_OR_HI || x.type==CF_RANGE_OR_LO || x.type==CF_RANGE_IB_HI || x.type==CF_RANGE_IB_LO; });
  out[9] = scoreFilter([](const CfLevel& x){ return x.type==CF_ZONE_LO || x.type==CF_ZONE_HI; });

  for (int i = 10; i < 16; ++i) out[i] = 0.0;
  return 16;
}

} // namespace du
