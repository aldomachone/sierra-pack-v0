#pragma once

// =============================================================================
// UtilitaireContexte.h — Agrégateur CONTEXTE (v1 enrichi, drop-in, sans rupture)
// -----------------------------------------------------------------------------
// Rôle : point d’entrée unique pour les utilitaires de contexte HTF/prix.
//  • Conserve strictement les includes existants.
//  • Ajoute un cadre de versionnage neutre et des indicateurs de capacités.
//  • Fournit un plan d’offsets pour composer un buffer de features « contexte »
//    sans couplage direct aux modules (VWAP/Profile/OHLC/Range/Pivots/Fibo/FVG/Zone/Reload/MTF).
//  • Zero logique de calcul ici → aucun risque de build croisé.
// =============================================================================

#include "UtilitaireVWAP.h"
#include "UtilitaireProfile.h"
#include "UtilitaireOHLC.h"
#include "UtilitaireRanges.h"
#include "UtilitaireZones.h"
#include "UtilitaireFVG.h"
#include "UtilitairePivots.h"
#include "UtilitaireFibo.h"
#include "UtilitaireReloadZone.h"
#include "UtilitaireMTF.h"

// -----------------------------------------------------------------------------
// Versionnage & capacités (ne créent aucune dépendance supplémentaire)
// -----------------------------------------------------------------------------
#ifndef DU_CTX_PACK_VER
  #define DU_CTX_PACK_VER  1
#endif

#ifndef DU_CTX_HAVE_VWAP
  #define DU_CTX_HAVE_VWAP 1
#endif
#ifndef DU_CTX_HAVE_PROFILE
  #define DU_CTX_HAVE_PROFILE 1
#endif
#ifndef DU_CTX_HAVE_OHLC
  #define DU_CTX_HAVE_OHLC 1
#endif
#ifndef DU_CTX_HAVE_RANGE
  #define DU_CTX_HAVE_RANGE 1
#endif
#ifndef DU_CTX_HAVE_PIVOT
  #define DU_CTX_HAVE_PIVOT 1
#endif
#ifndef DU_CTX_HAVE_FIBO
  #define DU_CTX_HAVE_FIBO 1
#endif
#ifndef DU_CTX_HAVE_FVG
  #define DU_CTX_HAVE_FVG 1
#endif
#ifndef DU_CTX_HAVE_ZONE
  #define DU_CTX_HAVE_ZONE 1
#endif
#ifndef DU_CTX_HAVE_RELOAD
  #define DU_CTX_HAVE_RELOAD 1
#endif
#ifndef DU_CTX_HAVE_MTF
  #define DU_CTX_HAVE_MTF 1
#endif

// -----------------------------------------------------------------------------
// Espace de nom léger pour constantes/plan d’offsets du « pack Contexte »
// -----------------------------------------------------------------------------
namespace du { namespace ctx {

  // Codes et bornes génériques
  enum : int { OK = 0, ERR_ARGS = 1 };
  enum : int { MAX_FEATURES_HINT = 128 }; // plafond indicatif

  // Tailles de features par module (v1).
  // Contrat :
  //   • Si DU_CTX_HAVE_xxx == 0 → FEAT_xxx_V1 = 0 (module absent).
  //   • Sinon, valeur par défaut ou override via -DDU_CTX_FEAT_* ou #define
  //     avant l’include de ce header.
  //   • Une valeur négative (ex. -1) signifie « non exposé ici » et ne
  //     contribue pas à FEAT_PACK_V1_HINT.

  // VWAP
#if !DU_CTX_HAVE_VWAP
  constexpr int FEAT_VWAP_V1 = 0;
#elif !defined(DU_CTX_FEAT_VWAP_V1)
  constexpr int FEAT_VWAP_V1 = 16;   // cf. UtilitaireVWAP.h::vp_features_v1/équivalents
#else
  constexpr int FEAT_VWAP_V1 = DU_CTX_FEAT_VWAP_V1;
#endif

  // PROFILE
#if !DU_CTX_HAVE_PROFILE
  constexpr int FEAT_PROFILE_V1 = 0;
#elif !defined(DU_CTX_FEAT_PROFILE_V1)
  constexpr int FEAT_PROFILE_V1 = 16; // cf. UtilitaireProfile.h::vp_features_v1
#else
  constexpr int FEAT_PROFILE_V1 = DU_CTX_FEAT_PROFILE_V1;
#endif

  // OHLC
#if !DU_CTX_HAVE_OHLC
  constexpr int FEAT_OHLC_V1 = 0;
#elif !defined(DU_CTX_FEAT_OHLC_V1)
  constexpr int FEAT_OHLC_V1 = 12;   // ex.: O/H/L/C niveaux + breaks
#else
  constexpr int FEAT_OHLC_V1 = DU_CTX_FEAT_OHLC_V1;
#endif

  // RANGE
#if !DU_CTX_HAVE_RANGE
  constexpr int FEAT_RANGE_V1 = 0;
#elif !defined(DU_CTX_FEAT_RANGE_V1)
  constexpr int FEAT_RANGE_V1 = 12;  // IB, range session, position relative, etc.
#else
  constexpr int FEAT_RANGE_V1 = DU_CTX_FEAT_RANGE_V1;
#endif

  // PIVOT
#if !DU_CTX_HAVE_PIVOT
  constexpr int FEAT_PIVOT_V1 = 0;
#elif !defined(DU_CTX_FEAT_PIVOT_V1)
  constexpr int FEAT_PIVOT_V1 = 16;  // pack R/S + états croisement
#else
  constexpr int FEAT_PIVOT_V1 = DU_CTX_FEAT_PIVOT_V1;
#endif

  // FIBO
#if !DU_CTX_HAVE_FIBO
  constexpr int FEAT_FIBO_V1 = 0;
#elif !defined(DU_CTX_FEAT_FIBO_V1)
  constexpr int FEAT_FIBO_V1 = 12;   // ratios, distances, confluence
#else
  constexpr int FEAT_FIBO_V1 = DU_CTX_FEAT_FIBO_V1;
#endif

  // FVG
#if !DU_CTX_HAVE_FVG
  constexpr int FEAT_FVG_V1 = 0;
#elif !defined(DU_CTX_FEAT_FVG_V1)
  constexpr int FEAT_FVG_V1 = 12;    // gaps, fill %, voisinage
#else
  constexpr int FEAT_FVG_V1 = DU_CTX_FEAT_FVG_V1;
#endif

  // ZONE
#if !DU_CTX_HAVE_ZONE
  constexpr int FEAT_ZONE_V1 = 0;
#elif !defined(DU_CTX_FEAT_ZONE_V1)
  constexpr int FEAT_ZONE_V1 = 12;   // nearest zone, score de confluence
#else
  constexpr int FEAT_ZONE_V1 = DU_CTX_FEAT_ZONE_V1;
#endif

  // RELOAD
#if !DU_CTX_HAVE_RELOAD
  constexpr int FEAT_RELOAD_V1 = 0;
#elif !defined(DU_CTX_FEAT_RELOAD_V1)
  constexpr int FEAT_RELOAD_V1 = 8;  // zones reload/supply-demand
#else
  constexpr int FEAT_RELOAD_V1 = DU_CTX_FEAT_RELOAD_V1;
#endif

  // MTF
#if !DU_CTX_HAVE_MTF
  constexpr int FEAT_MTF_V1 = 0;
#elif !defined(DU_CTX_FEAT_MTF_V1)
  constexpr int FEAT_MTF_V1 = 8;     // drapeaux HTF, états D/W/M
#else
  constexpr int FEAT_MTF_V1 = DU_CTX_FEAT_MTF_V1;
#endif

  // Somme « best-effort » pour dimensionner un buffer unique
  constexpr int FEAT_PACK_V1_HINT =
      (FEAT_VWAP_V1    > 0 ? FEAT_VWAP_V1    : 0) +
      (FEAT_PROFILE_V1 > 0 ? FEAT_PROFILE_V1 : 0) +
      (FEAT_OHLC_V1    > 0 ? FEAT_OHLC_V1    : 0) +
      (FEAT_RANGE_V1   > 0 ? FEAT_RANGE_V1   : 0) +
      (FEAT_PIVOT_V1   > 0 ? FEAT_PIVOT_V1   : 0) +
      (FEAT_FIBO_V1    > 0 ? FEAT_FIBO_V1    : 0) +
      (FEAT_FVG_V1     > 0 ? FEAT_FVG_V1     : 0) +
      (FEAT_ZONE_V1    > 0 ? FEAT_ZONE_V1    : 0) +
      (FEAT_RELOAD_V1  > 0 ? FEAT_RELOAD_V1  : 0) +
      (FEAT_MTF_V1     > 0 ? FEAT_MTF_V1     : 0);

  static_assert(
      FEAT_PACK_V1_HINT <= MAX_FEATURES_HINT,
      "UtilitaireContexte.h: FEAT_PACK_V1_HINT dépasse MAX_FEATURES_HINT, "
      "ajustez les tailles FEAT_* ou MAX_FEATURES_HINT."
  );

  // -------------------------------------------------------------------------
  // Plan d’offsets simple pour écrire les features de chaque module à la suite
  // Usage :
  //   auto off = du::ctx::plan_offsets_v1();
  //   // puis chaque module écrit ses features dans features[off.vwap], etc.
  // Sentinelle :
  //   • offset = -1  → module absent / non utilisé.
  // -------------------------------------------------------------------------
  struct OffsetsV1 {
    int vwap    = -1;
    int profile = -1;
    int ohlc    = -1;
    int range   = -1;
    int pivot   = -1;
    int fibo    = -1;
    int fvg     = -1;
    int zone    = -1;
    int reload  = -1;
    int mtf     = -1;
    int total   =  0;
  };

  constexpr OffsetsV1 plan_offsets_v1() noexcept
  {
    OffsetsV1 o{};
    int cur = 0;

    if (FEAT_VWAP_V1    > 0) { o.vwap    = cur; cur += FEAT_VWAP_V1; }
    if (FEAT_PROFILE_V1 > 0) { o.profile = cur; cur += FEAT_PROFILE_V1; }
    if (FEAT_OHLC_V1    > 0) { o.ohlc    = cur; cur += FEAT_OHLC_V1; }
    if (FEAT_RANGE_V1   > 0) { o.range   = cur; cur += FEAT_RANGE_V1; }
    if (FEAT_PIVOT_V1   > 0) { o.pivot   = cur; cur += FEAT_PIVOT_V1; }
    if (FEAT_FIBO_V1    > 0) { o.fibo    = cur; cur += FEAT_FIBO_V1; }
    if (FEAT_FVG_V1     > 0) { o.fvg     = cur; cur += FEAT_FVG_V1; }
    if (FEAT_ZONE_V1    > 0) { o.zone    = cur; cur += FEAT_ZONE_V1; }
    if (FEAT_RELOAD_V1  > 0) { o.reload  = cur; cur += FEAT_RELOAD_V1; }
    if (FEAT_MTF_V1     > 0) { o.mtf     = cur; cur += FEAT_MTF_V1; }

    o.total = cur;
    return o;
  }

}} // namespace du::ctx
