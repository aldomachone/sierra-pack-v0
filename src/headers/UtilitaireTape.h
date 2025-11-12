#pragma once

// =============================================================================
// UtilitaireTape.h — Agrégateur TAPE (v1 enrichi, drop‑in, sans rupture)
// -----------------------------------------------------------------------------
// Rôle : point d’entrée unique pour les utilitaires T&S (Tape) du pack.
//  • Conserve strictement les includes existants.
//  • Ajoute un petit cadre de compat/version, sans dépendances nouvelles.
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
  enum : int { OK=0, ERR_ARGS=1 };
  enum : int { MAX_FEATURES_HINT = 64 };   // indice plafond pour buffers d’agrégation

  // Placeholders pour tailles de features v1. Laissez −1 si non connu ici.
  // Vous pouvez les spécialiser côté projet via -DDU_TAPE_FEAT_* ou #define avant include.
  #ifndef DU_TAPE_FEAT_PACE_V1
    constexpr int FEAT_PACE_V1   = 16;  // défini par UtilitaireTapePace.h
  #else
    constexpr int FEAT_PACE_V1   = DU_TAPE_FEAT_PACE_V1;
  #endif
  #ifndef DU_TAPE_FEAT_BVC_V1
    constexpr int FEAT_BVC_V1    = -1;
  #else
    constexpr int FEAT_BVC_V1    = DU_TAPE_FEAT_BVC_V1;
  #endif
  #ifndef DU_TAPE_FEAT_SWEEP_V1
    constexpr int FEAT_SWEEP_V1  = -1;
  #else
    constexpr int FEAT_SWEEP_V1  = DU_TAPE_FEAT_SWEEP_V1;
  #endif
  #ifndef DU_TAPE_FEAT_EXHAUST_V1
    constexpr int FEAT_EXHAUST_V1= -1;
  #else
    constexpr int FEAT_EXHAUST_V1= DU_TAPE_FEAT_EXHAUST_V1;
  #endif

  // Somme « best‑effort » pour donner un ordre de grandeur de buffer.
  constexpr int FEAT_PACK_V1_HINT =
      (FEAT_PACE_V1   > 0 ? FEAT_PACE_V1   : 0) +
      (FEAT_BVC_V1    > 0 ? FEAT_BVC_V1    : 0) +
      (FEAT_SWEEP_V1  > 0 ? FEAT_SWEEP_V1  : 0) +
      (FEAT_EXHAUST_V1> 0 ? FEAT_EXHAUST_V1: 0);

  // -------------------------------------------------------------------------
  // Helper d’agrégation « optionnel » minimaliste
  //   • Décide uniquement des offsets d’écriture dans un buffer features[]
  //   • N’appelle aucune fonction spécifique des modules → zéro couplage
  // Usage : appelez vos tp/bvc/sweep/exhaust_features_v1() vous‑même et écrivez
  //         dans features[outPace], features[outBvc], etc.
  // -------------------------------------------------------------------------
  struct OffsetsV1 { int pace=0, bvc=0, sweep=0, exhaust=0, total=0; };

  inline OffsetsV1 plan_offsets_v1()
  {
    OffsetsV1 o{}; int cur=0;
    if (FEAT_PACE_V1   > 0) { o.pace    = cur; cur += FEAT_PACE_V1; }
    if (FEAT_BVC_V1    > 0) { o.bvc     = cur; cur += FEAT_BVC_V1; }
    if (FEAT_SWEEP_V1  > 0) { o.sweep   = cur; cur += FEAT_SWEEP_V1; }
    if (FEAT_EXHAUST_V1> 0) { o.exhaust = cur; cur += FEAT_EXHAUST_V1; }
    o.total = cur; return o;
  }

}} // namespace du::tape
