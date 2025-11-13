#pragma once
#include "sierrachart.h"

/*
===============================================================================
 PackUtilitaires_v0.h — Umbrella headers (v6)

 Rôle:
 - Point d'entrée unique pour tous les Utilitaire*.h du Pack_v0.
 - Active/désactive des familles (DOM, TAPE, CONTEXTE, ZONES, MTF, MEMORY/RISK,
   MLP, CORE) via des macros PACK_V0_ENABLE_*.
 - Utilise des agrégateurs (UtilitaireTape.h, UtilitaireContexte.h, futur
   UtilitaireDomPack.h) quand ils existent, sinon fallback automatique vers
   les headers unitaires.

How to use (résumé):
 - Dans un .cpp, avant ce header:
      #define PACK_V0_ENABLE_DOM      1   // DOM on
      #define PACK_V0_ENABLE_TAPE     0   // TAPE off
      #define PACK_V0_ENABLE_CONTEXTE 1   // CONTEXTE on
      #include "PackUtilitaires_v0.h"

 - Pour forcer les fallback unitaires (ignorer UtilitaireTape.h / Contexte.h):
      #define PACK_V0_USE_AGGREGATORS 0

Matrice de builds à tester (QA minimale):
 - FULL_PACK     : tous PACK_V0_ENABLE_* = 1
 - DOM_ONLY      : CORE=1, DOM=1, le reste=0
 - TAPE_ONLY     : CORE=1, TAPE=1, le reste=0
 - CONTEXT_ONLY  : CORE=1, CONTEXTE=1, ZONES=1, MTF=1, le reste=0
===============================================================================
*/

// ============================================================================
// Standard C++ (garde C++17 robuste)
// ============================================================================

#ifndef PACK_V0_REQUIRE_CPP17
  #define PACK_V0_REQUIRE_CPP17 1
#endif

#if PACK_V0_REQUIRE_CPP17
  // _MSVC_LANG est plus fiable que __cplusplus sous MSVC.
  #if defined(_MSVC_LANG)
    #define PACK_V0_CPP_LANG _MSVC_LANG
  #else
    #define PACK_V0_CPP_LANG __cplusplus
  #endif

  #if !defined(PACK_V0_CPP_LANG) || (PACK_V0_CPP_LANG < 201703L)
    #error "Pack_v0: C++17 requis (désactivez PACK_V0_REQUIRE_CPP17=0 si nécessaire)."
  #endif
#endif

// ============================================================================
// Versions & schémas (métadonnées globales pack)
// ============================================================================

#ifndef PACK_V0_VERSION
  #define PACK_V0_VERSION      0x00010003   // 1.0.3
#endif

#ifndef PACK_V0_SCHEMA_REV
  #define PACK_V0_SCHEMA_REV   1
#endif

#ifndef PACK_V0_FEATSIG_V1
  #define PACK_V0_FEATSIG_V1   1
#endif

// ============================================================================
// Toggles familles (toutes les macros sont préfixées PACK_V0_ ou DU_)
// ============================================================================

#ifndef PACK_V0_ENABLE_CORE
  #define PACK_V0_ENABLE_CORE         1
#endif
#ifndef PACK_V0_ENABLE_MLP
  #define PACK_V0_ENABLE_MLP          1
#endif
#ifndef PACK_V0_ENABLE_DOM
  #define PACK_V0_ENABLE_DOM          1
#endif
#ifndef PACK_V0_ENABLE_TAPE
  #define PACK_V0_ENABLE_TAPE         1
#endif
#ifndef PACK_V0_ENABLE_CONTEXTE
  #define PACK_V0_ENABLE_CONTEXTE     1
#endif
#ifndef PACK_V0_ENABLE_ZONES
  #define PACK_V0_ENABLE_ZONES        1
#endif
#ifndef PACK_V0_ENABLE_MTF
  #define PACK_V0_ENABLE_MTF          1
#endif
#ifndef PACK_V0_ENABLE_MEMORY_RISK
  #define PACK_V0_ENABLE_MEMORY_RISK  1
#endif

// Option future : pack DOM agrégé (UtilitaireDomPack.h)
#ifndef PACK_V0_ENABLE_DOM_PACK
  #define PACK_V0_ENABLE_DOM_PACK 0
#endif

// Utiliser les agrégateurs (UtilitaireTape.h / UtilitaireContexte.h / DomPack) ?
#ifndef PACK_V0_USE_AGGREGATORS
  #define PACK_V0_USE_AGGREGATORS 1
#endif

// ============================================================================
// Contraintes de configuration (exemples de combinaisons interdites)
// ============================================================================

// DOM sans CORE n'est pas supporté (utilise Tempo/Sanitize/Compat).
#if PACK_V0_ENABLE_DOM && !PACK_V0_ENABLE_CORE
  #error "Pack_v0: PACK_V0_ENABLE_DOM=1 requiert PACK_V0_ENABLE_CORE=1 (UtilitaireACSILCore, Tempo, Sanitize, Compat)."
#endif

// MLP sans CORE est déconseillé (log / temps / sanitize). Ajustable si besoin.
#if PACK_V0_ENABLE_MLP && !PACK_V0_ENABLE_CORE
  #error "Pack_v0: PACK_V0_ENABLE_MLP=1 requiert PACK_V0_ENABLE_CORE=1."
#endif

// (On pourrait ajouter ici d'autres static_assert si nécessaire à l'avenir.)

// ============================================================================
// LOG_ONCE — garantie globale
// ============================================================================
// Par défaut, PACK_V0_LOG_ONCE est un no-op pour éviter toute casse si CORE est off.
// Le bloc CORE redéfinira ensuite la macro avec un fallback AddMessageToLog ou
// du::duLogOnce si disponible.

#ifndef PACK_V0_LOG_ONCE
  #define PACK_V0_LOG_ONCE(sc, key, msg) do{}while(0)
#endif

// ============================================================================
// CORE
// - UtilitaireACSILCore : wrappers SC / types communs
// - UtilitaireTempo     : nowMs, jitter clamp, clock skew
// - UtilitaireSanitize  : clamp, anti-NaN/Inf
// - UtilitaireCompat    : SG/inputs/featSig compat
// - UtilitaireEngineContract : conventions engines/signaux
// ============================================================================

#if PACK_V0_ENABLE_CORE
  #include "UtilitaireACSILCore.h"
  #include "UtilitaireTempo.h"
  #include "UtilitaireSanitize.h"
  #include "UtilitaireCompat.h"
  #include "UtilitaireEngineContract.h"

  // Fallback LOG_ONCE si du::duLogOnce n'est pas fourni ailleurs.
  namespace du { namespace detail {
    inline void _pack_log_fallback(SCStudyInterfaceRef sc, const char* key, const char* msg){
      SCString s("PACK_V0[");
      s += key;
      s += "]: ";
      s += msg;
      sc.AddMessageToLog(s, 0);
    }
  }}

  #undef PACK_V0_LOG_ONCE
  #if defined(DU_HAVE_LOG_ONCE)
    // du::duLogOnce(sc, "KEY", "message...")
    #define PACK_V0_LOG_ONCE(sc, key, msg) do{ du::duLogOnce(sc, key, msg); }while(0)
  #else
    // Fallback: AddMessageToLog avec la clé intégrée dans le message.
    #define PACK_V0_LOG_ONCE(sc, key, msg) do{ du::detail::_pack_log_fallback(sc, key, msg); }while(0)
  #endif
#endif

// ============================================================================
// MLP / I/O
// - UtilitaireMLP : CSV/BIN, #META/#PARAMS, featSig, rotation, Snappy éventuel
// ============================================================================

#if PACK_V0_ENABLE_MLP
  #include "UtilitaireMLP.h"
#endif

// ============================================================================
// TAPE
// - Agrégateur UtilitaireTape.h si présent (DU_TAPE_PACK_VER >= 1).
//   Contrat minimal: 
//     #define DU_TAPE_PACK_VER 1
//     namespace du::tape {
//       struct OffsetsV1;
//       OffsetsV1 plan_offsets_v1();
//     }
// - Fallback unitaire: Pace / BVC / Sweep / Exhaust.
// ============================================================================

#if PACK_V0_ENABLE_TAPE
  #if PACK_V0_USE_AGGREGATORS
    #if defined(__has_include)
      #if __has_include("UtilitaireTape.h")
        #define DU_TAPE_PACK_HEADER_PRESENT 1
        #include "UtilitaireTape.h"
      #endif
    #endif
  #endif

  #ifndef DU_TAPE_PACK_VER
    // Fallback unitaire
    #include "UtilitaireTapePace.h"
    #include "UtilitaireTapeBVC.h"
    #include "UtilitaireTapeSweep.h"
    #include "UtilitaireTapeExhaust.h"
  #else
    static_assert(DU_TAPE_PACK_VER >= 1, "UtilitaireTape.h trop ancien (DU_TAPE_PACK_VER < 1).");
  #endif
#endif

// ============================================================================
// CONTEXTE
// - Agrégateur UtilitaireContexte.h si présent (DU_CTX_PACK_VER >= 1).
//   Contrat minimal:
//     #define DU_CTX_PACK_VER 1
//     namespace du::ctx {
//       struct OffsetsV1;
//       OffsetsV1 plan_offsets_v1();
//     }
//   + Si MTF est inclus dans UtilitaireContexte.h, celui-ci doit définir
//     #define DU_PACK_MTF_INCLUDED_FROM_CTX 1
//
// - Fallback "contexte large": VWAP / PROFILE / OHLC / RANGES, plus zones
//   (Pivots/Fibo/Reload/FVG/Zones) et MTF.
// ============================================================================

#if PACK_V0_ENABLE_CONTEXTE
  #if PACK_V0_USE_AGGREGATORS
    #if defined(__has_include)
      #if __has_include("UtilitaireContexte.h")
        #define DU_CTX_PACK_HEADER_PRESENT 1
        #include "UtilitaireContexte.h"
      #endif
    #endif
  #endif

  #ifndef DU_CTX_PACK_VER
    // Fallback unitaire — Contexte large
    #include "UtilitaireVWAP.h"
    #include "UtilitaireProfile.h"
    #include "UtilitaireOHLC.h"
    #include "UtilitaireRanges.h"
    #include "UtilitaireZones.h"
    #include "UtilitaireFVG.h"
    #include "UtilitairePivots.h"
    #include "UtilitaireFibo.h"
    #include "UtilitaireReloadZone.h"
    // MTF est inclus ici : on marque que MTF vient déjà du contexte
    #define DU_PACK_MTF_INCLUDED_FROM_CTX 1
    #include "UtilitaireMTF.h"
  #else
    static_assert(DU_CTX_PACK_VER >= 1, "UtilitaireContexte.h trop ancien (DU_CTX_PACK_VER < 1).");
  #endif
#endif

// ============================================================================
// ZONES / NIVEAUX
// - Pivots, Fibo, Reload, FVG, Zones, Confluence.
// - NB: il y a recouvrement avec le fallback CONTEXTE (qui inclut déjà certains
//   headers de zones). Les include guards des Utilitaire*.h évitent les doublons.
// ============================================================================

#if PACK_V0_ENABLE_ZONES
  #include "UtilitairePivots.h"
  #include "UtilitaireFibo.h"
  #include "UtilitaireReloadZone.h"
  #include "UtilitaireFVG.h"
  #include "UtilitaireZones.h"
  #include "UtilitaireConfluence.h"
#endif

// ============================================================================
// MTF / OVERLAY
// - UtilitaireMTF : logique multi-timeframe.
// - UtilitaireOverlay : helpers overlay, lag_ms, validity flags.
// - DU_PACK_MTF_INCLUDED_FROM_CTX peut être défini par le fallback CONTEXTE ou
//   par UtilitaireContexte.h si ce dernier inclut déjà UtilitaireMTF.h.
// ============================================================================

#if PACK_V0_ENABLE_MTF
  #if !defined(DU_PACK_MTF_INCLUDED_FROM_CTX)
    #include "UtilitaireMTF.h"
  #endif
  #include "UtilitaireOverlay.h"
#endif

// ============================================================================
// MÉMOIRE / RÉPONSE / RISQUE
// - UtilitaireMDHG   : mémoire DOM (heatmap globale).
// - UtilitairePRF    : post-response feature.
// - UtilitaireLiqRisk: risque de liquidité / collapses / recovery.
// ============================================================================

#if PACK_V0_ENABLE_MEMORY_RISK
  #include "UtilitaireMDHG.h"
  #include "UtilitairePRF.h"
  #include "UtilitaireLiqRisk.h"
#endif

// ============================================================================
// DOM — Microstructure rapide
// - Option pack DOM (UtilitaireDomPack.h) via PACK_V0_ENABLE_DOM_PACK.
//   Contrat minimal:
//     #define DU_DOM_PACK_VER 1
//     // (interfaces exposées à définir dans UtilitaireDomPack.h)
// - Fallback unitaire: UtilitaireDom*, ImbalSweep, Momentum, Vacuum, StopRun.
// ============================================================================

#if PACK_V0_ENABLE_DOM
  #if PACK_V0_ENABLE_DOM_PACK && PACK_V0_USE_AGGREGATORS
    #if defined(__has_include)
      #if __has_include("UtilitaireDomPack.h")
        #define DU_DOM_PACK_HEADER_PRESENT 1
        #include "UtilitaireDomPack.h"
      #endif
    #endif
  #endif

  #ifndef DU_DOM_PACK_VER
    // Fallback unitaire
    #include "UtilitaireDom.h"
    #include "UtilitaireDomSignal.h"
    #include "UtilitaireDomImbalSweep.h"
    #include "UtilitaireDomSignalMomentum.h"
    #include "UtilitaireDomVacuum.h"
    #include "UtilitaireStopRun.h"
  #else
    static_assert(DU_DOM_PACK_VER >= 1, "UtilitaireDomPack.h trop ancien (DU_DOM_PACK_VER < 1).");
  #endif
#endif

// ============================================================================
// Espace du::pack — introspection pack
// - Helpers constexpr pour savoir ce qui est compilé.
// - Accès aux OffsetsV1 des agrégateurs TAPE/CONTEXTE si disponibles.
// - Métadonnées pack (version, schema, featSig v1).
// - log_config(sc) pour logguer la config effective une fois par chart.
// ============================================================================

namespace du { namespace pack {

  // Disponibilités (constexpr → aucun coût)
  constexpr bool has_core()       { return PACK_V0_ENABLE_CORE         != 0; }
  constexpr bool has_mlp()        { return PACK_V0_ENABLE_MLP          != 0; }
  constexpr bool has_dom()        { return PACK_V0_ENABLE_DOM          != 0; }
  constexpr bool has_tape()       { return PACK_V0_ENABLE_TAPE         != 0; }
  constexpr bool has_ctx()        { return PACK_V0_ENABLE_CONTEXTE     != 0; }
  constexpr bool has_zones()      { return PACK_V0_ENABLE_ZONES        != 0; }
  constexpr bool has_mtf()        { return PACK_V0_ENABLE_MTF          != 0; }
  constexpr bool has_memrisk()    { return PACK_V0_ENABLE_MEMORY_RISK  != 0; }

  // Offsets TAPE si agrégateur présent (UtilitaireTape.h)
  #if defined(DU_TAPE_PACK_VER)
    #define DU_PACK_HAVE_TAPE_OFFSETS 1
    using TapeOffsetsV1 = du::tape::OffsetsV1;
    inline TapeOffsetsV1 tape_plan_offsets_v1(){ return du::tape::plan_offsets_v1(); }
  #endif

  // Offsets CONTEXTE si agrégateur présent (UtilitaireContexte.h)
  #if defined(DU_CTX_PACK_VER)
    #define DU_PACK_HAVE_CTX_OFFSETS 1
    using CtxOffsetsV1 = du::ctx::OffsetsV1;
    inline CtxOffsetsV1 ctx_plan_offsets_v1(){ return du::ctx::plan_offsets_v1(); }
  #endif

  // (Éventuellement plus tard: offsets DOM si pack DOM exposé.)

  // Métadonnées pack (fonctions + constantes)
  constexpr int version()    { return (int)PACK_V0_VERSION; }
  constexpr int schema_rev() { return (int)PACK_V0_SCHEMA_REV; }
  constexpr int featsig_v1() { return (int)PACK_V0_FEATSIG_V1; }

  constexpr int kVersion   = (int)PACK_V0_VERSION;
  constexpr int kSchemaRev = (int)PACK_V0_SCHEMA_REV;
  constexpr int kFeatSigV1 = (int)PACK_V0_FEATSIG_V1;

  // Helper de log de configuration (facultatif, coût nul si non appelé).
  inline void log_config(SCStudyInterfaceRef sc){
    SCString s("PACK_V0: ver=");
    s += kVersion;
    s += " schema=";
    s += kSchemaRev;
    s += " core=";
    s += has_core();
    s += " dom=";
    s += has_dom();
    s += " tape=";
    s += has_tape();
    s += " ctx=";
    s += has_ctx();
    s += " zones=";
    s += has_zones();
    s += " mtf=";
    s += has_mtf();
    s += " memrisk=";
    s += has_memrisk();
    PACK_V0_LOG_ONCE(sc, "PACK_V0_CONFIG", s);
  }

}} // namespace du::pack
