#pragma once

// =============================================================================
// UtilitaireTape.h — Agrégateur TAPE (v1)
// -----------------------------------------------------------------------------
// Rôle : point d’entrée unique pour les utilitaires T&S (Tape) du pack.
//  • Conserve strictement les includes existants.
//  • Ajoute un petit cadre de compat/version, sans dépendances nouvelles
//    en mode normal (self-test optionnel uniquement).
//  • N’impose aucun symbole croisé : zéro appel indirect → zéro risque de build.
//  • Fournit des helpers optionnels et des constantes neutres.
// =============================================================================

#include "UtilitaireTapePace.h"    // TpParams, TpState, tp_update_arrival(), tp_features_v1(...)
#include "UtilitaireTapeBVC.h"     // selon votre fichier (delta, features, ...)
#include "UtilitaireTapeSweep.h"   // idem
#include "UtilitaireTapeExhaust.h" // idem

// -----------------------------------------------------------------------------
// Versionnage & capacités (non bloquant). Ces macros n’ajoutent aucune dépendance.
// -----------------------------------------------------------------------------
#ifndef DU_TAPE_PACK_VER
  #define DU_TAPE_PACK_VER  1
#endif

#ifndef DU_TAPE_HAVE_PACE
  #define DU_TAPE_HAVE_PACE 1
#endif
#ifndef DU_TAPE_HAVE_BVC
  #define DU_TAPE_HAVE_BVC  1  // mettez 0 si vous retirez BVC du build
#endif
#ifndef DU_TAPE_HAVE_SWEEP
  #define DU_TAPE_HAVE_SWEEP 1
#endif
#ifndef DU_TAPE_HAVE_EXHAUST
  #define DU_TAPE_HAVE_EXHAUST 1
#endif

// -----------------------------------------------------------------------------
// Espace de nom léger pour constantes génériques du « pack Tape »
// -----------------------------------------------------------------------------
namespace du { namespace tape {

  // Codes retour neutres et bornes génériques
  enum : int { OK = 0, ERR_ARGS = 1 };
  enum : int { MAX_FEATURES_HINT = 64 };   // indice plafond pour buffers d’agrégation

  // -------------------------------------------------------------------------
  // Placeholders pour tailles de features v1.
  // - FEAT_*_V1 reflètent le nombre de slots écrits par chaque module.
  // - Vous pouvez les spécialiser côté projet via -DDU_TAPE_FEAT_* ou #define
  //   avant l’include de ce header.
  // - Lien avec HAVE_* : si DU_TAPE_HAVE_xxx == 0 → FEAT_xxx_V1 = 0.
  // -------------------------------------------------------------------------

  // PACE
#if !DU_TAPE_HAVE_PACE
  constexpr int FEAT_PACE_V1 = 0;
#elif !defined(DU_TAPE_FEAT_PACE_V1)
  constexpr int FEAT_PACE_V1 = 16;     // défaut : à adapter si besoin
#else
  constexpr int FEAT_PACE_V1 = DU_TAPE_FEAT_PACE_V1;
#endif

  // BVC
#if !DU_TAPE_HAVE_BVC
  constexpr int FEAT_BVC_V1 = 0;
#elif !defined(DU_TAPE_FEAT_BVC_V1)
  constexpr int FEAT_BVC_V1 = -1;      // inconnu ici → ne contribue pas au hint
#else
  constexpr int FEAT_BVC_V1 = DU_TAPE_FEAT_BVC_V1;
#endif

  // SWEEP
#if !DU_TAPE_HAVE_SWEEP
  constexpr int FEAT_SWEEP_V1 = 0;
#elif !defined(DU_TAPE_FEAT_SWEEP_V1)
  constexpr int FEAT_SWEEP_V1 = -1;
#else
  constexpr int FEAT_SWEEP_V1 = DU_TAPE_FEAT_SWEEP_V1;
#endif

  // EXHAUST
#if !DU_TAPE_HAVE_EXHAUST
  constexpr int FEAT_EXHAUST_V1 = 0;
#elif !defined(DU_TAPE_FEAT_EXHAUST_V1)
  constexpr int FEAT_EXHAUST_V1 = -1;
#else
  constexpr int FEAT_EXHAUST_V1 = DU_TAPE_FEAT_EXHAUST_V1;
#endif

  // Somme « best-effort » pour donner un ordre de grandeur de buffer.
  constexpr int FEAT_PACK_V1_HINT =
      (FEAT_PACE_V1    > 0 ? FEAT_PACE_V1    : 0) +
      (FEAT_BVC_V1     > 0 ? FEAT_BVC_V1     : 0) +
      (FEAT_SWEEP_V1   > 0 ? FEAT_SWEEP_V1   : 0) +
      (FEAT_EXHAUST_V1 > 0 ? FEAT_EXHAUST_V1 : 0);

  static_assert(
      FEAT_PACK_V1_HINT <= MAX_FEATURES_HINT,
      "UtilitaireTape.h: FEAT_PACK_V1_HINT dépasse MAX_FEATURES_HINT, "
      "ajustez les tailles FEAT_* ou MAX_FEATURES_HINT."
  );

  // -------------------------------------------------------------------------
  // Helper d’agrégation « optionnel » minimaliste
  //   • Décide uniquement des offsets d’écriture dans un buffer features[]
  //   • N’appelle aucune fonction spécifique des modules → zéro couplage
  //
  // Usage typique :
  //   const auto off = du::tape::plan_offsets_v1();
  //   // features est un buffer d’au moins off.total cases
  //   if (off.pace    >= 0) tp_features_v1(..., &features[off.pace]);
  //   if (off.bvc     >= 0) bvc_features_v1(..., &features[off.bvc]);
  //   ...
  // -------------------------------------------------------------------------
  struct OffsetsV1 {
    int pace    = -1;
    int bvc     = -1;
    int sweep   = -1;
    int exhaust = -1;
    int total   =  0;
  };

  constexpr OffsetsV1 plan_offsets_v1() noexcept
  {
    OffsetsV1 o{};
    int cur = 0;

    if (FEAT_PACE_V1 > 0) {
      o.pace = cur;
      cur += FEAT_PACE_V1;
    }
    if (FEAT_BVC_V1 > 0) {
      o.bvc = cur;
      cur += FEAT_BVC_V1;
    }
    if (FEAT_SWEEP_V1 > 0) {
      o.sweep = cur;
      cur += FEAT_SWEEP_V1;
    }
    if (FEAT_EXHAUST_V1 > 0) {
      o.exhaust = cur;
      cur += FEAT_EXHAUST_V1;
    }

    o.total = cur;
    return o;
  }

}} // namespace du::tape

// -----------------------------------------------------------------------------
// Hook self-test optionnel (ne s’active que si PACK_V0_SELFTEST est défini).
// Vérifie que le nombre de features indicatif tient dans la plage de SG allouée
// à TAPE_ENGINE_V1 (contrat global pack_v0).
// -----------------------------------------------------------------------------
#if defined(PACK_V0_SELFTEST)
  #include "UtilitaireEngineContract.h"
  static_assert(
      ::du::tape::FEAT_PACK_V1_HINT
        <= ::du::duSgCount(::du::duGetRange(::du::EngineFamily::Tape)),
      "UtilitaireTape.h: FEAT_PACK_V1_HINT dépasse le nombre de SG alloué "
      "à TAPE_ENGINE_V1 dans UtilitaireEngineContract.h."
  );
#endif
