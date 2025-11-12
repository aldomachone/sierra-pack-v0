#pragma once
#include "sierrachart.h"

// =============================================================================
// Pack_v0.h — Umbrella headers (v2 enrichi, drop‑in, sans rupture)
// -----------------------------------------------------------------------------
// Objectif : point d'entrée unique, paramétrable, pour toutes les familles
// (Core, I/O/MLP, DOM, Tape, Contexte, Zones/Niveaux, MTF/Overlay, Mémoire/Risque).
// • Conserve les toggles PACK_V0_ENABLE_* existants. Valeurs par défaut = ON.
// • Ajoute versionnage, capacités, compat featSig et agrégateurs Tape/Contexte.
// • Aucune dépendance forte : si un agrégateur n'est pas présent, fallback vers
//   les includes unitaires déjà en place.
// • Aucun symbole global, aucune allocation.
// =============================================================================

// ===== Versions & schémas ====================================================
#ifndef PACK_V0_VERSION
  #define PACK_V0_VERSION      0x00010001  // 1.0.1
#endif
#ifndef PACK_V0_SCHEMA_REV
  #define PACK_V0_SCHEMA_REV   1           // schéma d'offsets/featSig pack
#endif
#ifndef PACK_V0_FEATSIG_V1
  #define PACK_V0_FEATSIG_V1   1           // signature v1 (72–96 features cible)
#endif

// ===== Toggles familles (1 = on, 0 = off) ===================================
#ifndef PACK_V0_ENABLE_CORE
  #define PACK_V0_ENABLE_CORE       1
#endif
#ifndef PACK_V0_ENABLE_MLP
  #define PACK_V0_ENABLE_MLP        1
#endif
#ifndef PACK_V0_ENABLE_DOM
  #define PACK_V0_ENABLE_DOM        1
#endif
#ifndef PACK_V0_ENABLE_TAPE
  #define PACK_V0_ENABLE_TAPE       1
#endif
#ifndef PACK_V0_ENABLE_CONTEXTE
  #define PACK_V0_ENABLE_CONTEXTE   1
#endif
#ifndef PACK_V0_ENABLE_ZONES
  #define PACK_V0_ENABLE_ZONES      1
#endif
#ifndef PACK_V0_ENABLE_MTF
  #define PACK_V0_ENABLE_MTF        1
#endif
#ifndef PACK_V0_ENABLE_MEMORY_RISK
  #define PACK_V0_ENABLE_MEMORY_RISK 1
#endif

// Option : utiliser les agrégateurs (si présents)
#ifndef PACK_V0_USE_AGGREGATORS
  #define PACK_V0_USE_AGGREGATORS 1
#endif

// ===== Core temps / utilitaires =============================================
#if PACK_V0_ENABLE_CORE
  #include "UtilitaireTempo.h"
  #include "UtilitaireSanitize.h"
  #include "UtilitaireCompat.h"
  // Aides pack (log-once, clamp simple)
  #ifndef PACK_V0_LOG_ONCE
    #define PACK_V0_LOG_ONCE(sc, key, msg) do{ du::duLogOnce(sc, key, msg); }while(0)
  #endif
#endif

// ===== I/O & MLP =============================================================
#if PACK_V0_ENABLE_MLP
  #include "UtilitaireMLP.h"
#endif

// ===== DOM ==================================================================
#if PACK_V0_ENABLE_DOM
  #include "UtilitaireDom.h"
  #include "UtilitaireDomSignal.h"
  #include "UtilitaireDomImbalSweep.h"
  #include "UtilitaireDomSignalMomentum.h"
  #include "UtilitaireDomVacuum.h"
  #include "UtilitaireStopRun.h"
#endif

// ===== Tape (agrégateur si dispo, sinon unitaires) ===========================
#if PACK_V0_ENABLE_TAPE
  #if PACK_V0_USE_AGGREGATORS
    // Essaye l'agrégateur; si absent, DU_TAPE_PACK_VER ne sera pas défini → fallback
    #include "UtilitaireTape.h"
  #endif
  #ifndef DU_TAPE_PACK_VER
    // Fallback includes unitaires
    #include "UtilitaireTapePace.h"
    #include "UtilitaireTapeBVC.h"
    #include "UtilitaireTapeSweep.h"
    #include "UtilitaireTapeExhaust.h"
  #endif
#endif

// ===== Contexte (agrégateur si dispo, sinon unitaires) =======================
#if PACK_V0_ENABLE_CONTEXTE
  #if PACK_V0_USE_AGGREGATORS
    #include "UtilitaireContexte.h"
  #endif
  #ifndef DU_CTX_PACK_VER
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
  #endif
#endif

// ===== Zones / Niveaux =======================================================
#if PACK_V0_ENABLE_ZONES
  #include "UtilitairePivots.h"
  #include "UtilitaireFibo.h"
  #include "UtilitaireReloadZone.h"
  #include "UtilitaireFVG.h"
  #include "UtilitaireZones.h"
  #include "UtilitaireConfluence.h"
#endif

// ===== MTF / Overlay =========================================================
#if PACK_V0_ENABLE_MTF
  #include "UtilitaireMTF.h"
  #include "UtilitaireOverlay.h"
#endif

// ===== Mémoire / Réponse / Risque ===========================================
#if PACK_V0_ENABLE_MEMORY_RISK
  #include "UtilitaireMDHG.h"
  #include "UtilitairePRF.h"
  #include "UtilitaireLiqRisk.h"
#endif

// ===== Expositions pack (facultatives) =======================================
namespace du { namespace pack {
  // Capabilities flags exposés (stables)
  enum : int {
    HAVE_CORE       = PACK_V0_ENABLE_CORE,
    HAVE_MLP        = PACK_V0_ENABLE_MLP,
    HAVE_DOM        = PACK_V0_ENABLE_DOM,
    HAVE_TAPE       = PACK_V0_ENABLE_TAPE,
    HAVE_CTX        = PACK_V0_ENABLE_CONTEXTE,
    HAVE_ZONES      = PACK_V0_ENABLE_ZONES,
    HAVE_MTF        = PACK_V0_ENABLE_MTF,
    HAVE_MEM_RISK   = PACK_V0_ENABLE_MEMORY_RISK
  };

  // Offsets d'agrégation utiles si agrégateurs présents
  #if PACK_V0_ENABLE_TAPE
    #ifdef DU_TAPE_PACK_VER
      using TapeOffsetsV1 = du::tape::OffsetsV1;
      inline TapeOffsetsV1 tape_plan_offsets_v1(){ return du::tape::plan_offsets_v1(); }
    #endif
  #endif
  #if PACK_V0_ENABLE_CONTEXTE
    #ifdef DU_CTX_PACK_VER
      using CtxOffsetsV1  = du::ctx::OffsetsV1;
      inline CtxOffsetsV1  ctx_plan_offsets_v1(){ return du::ctx::plan_offsets_v1(); }
    #endif
  #endif

  // Helpers de contrôle non bloquants
  inline int version()      { return (int)PACK_V0_VERSION; }
  inline int schema_rev()   { return (int)PACK_V0_SCHEMA_REV; }
  inline int featsig_v1()   { return (int)PACK_V0_FEATSIG_V1; }
}}

// ===== Notes =================================================================
// • Si vous souhaitez réduire les dépendances lors de builds ciblés, définissez
//   PACK_V0_ENABLE_* = 0 en tête de vos .cpp avant cet include.
// • Les agrégateurs améliorent l'ergonomie (offsets de features pack)
//   mais ne sont pas requis. Le code tombe en « includes unitaires » sinon.
// • Aucun symbole global ajouté par ce header. Pas de sc.StudyDescription.
