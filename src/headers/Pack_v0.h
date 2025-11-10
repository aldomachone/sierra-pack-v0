#pragma once
#include "sierrachart.h"

// ===========================
// Pack v0 — Umbrella headers
// ===========================
// Toggle familles (1 = on, 0 = off). Tu peux les définir au projet ou en tête des .cpp.
#ifndef PACK_V0_ENABLE_CORE
  #define PACK_V0_ENABLE_CORE     1
#endif
#ifndef PACK_V0_ENABLE_MLP
  #define PACK_V0_ENABLE_MLP      1
#endif
#ifndef PACK_V0_ENABLE_DOM
  #define PACK_V0_ENABLE_DOM      1
#endif
#ifndef PACK_V0_ENABLE_TAPE
  #define PACK_V0_ENABLE_TAPE     1
#endif
#ifndef PACK_V0_ENABLE_CONTEXTE
  #define PACK_V0_ENABLE_CONTEXTE 1
#endif
#ifndef PACK_V0_ENABLE_ZONES
  #define PACK_V0_ENABLE_ZONES    1
#endif
#ifndef PACK_V0_ENABLE_MTF
  #define PACK_V0_ENABLE_MTF      1
#endif
#ifndef PACK_V0_ENABLE_MEMORY_RISK
  #define PACK_V0_ENABLE_MEMORY_RISK 1
#endif

// -------- Core temps / utilitaires
#if PACK_V0_ENABLE_CORE
  #include "UtilitaireTempo.h"
  #include "UtilitaireSanitize.h"
  #include "UtilitaireCompat.h"
#endif

// -------- I/O & MLP
#if PACK_V0_ENABLE_MLP
  #include "UtilitaireMLP.h"
#endif

// -------- DOM
#if PACK_V0_ENABLE_DOM
  #include "UtilitaireDom.h"
  #include "UtilitaireDomSignal.h"
  #include "UtilitaireDomImbalSweep.h"
  #include "UtilitaireDomSignalMomentum.h"
  #include "UtilitaireDomVacuum.h"
  #include "UtilitaireStopRun.h"
#endif

// -------- Tape
#if PACK_V0_ENABLE_TAPE
  #include "UtilitaireTapePace.h"
  #include "UtilitaireTapeBVC.h"
  #include "UtilitaireTapeSweep.h"
  #include "UtilitaireTapeExhaust.h"
#endif

// -------- Contexte
#if PACK_V0_ENABLE_CONTEXTE
  #include "UtilitaireVWAP.h"
  #include "UtilitaireProfile.h"
  #include "UtilitaireOHLC.h"
  #include "UtilitaireRanges.h"
#endif

// -------- Zones / Niveaux
#if PACK_V0_ENABLE_ZONES
  #include "UtilitairePivots.h"
  #include "UtilitaireFibo.h"
  #include "UtilitaireReloadZone.h"
  #include "UtilitaireFVG.h"
  #include "UtilitaireZones.h"
  #include "UtilitaireConfluence.h"
#endif

// -------- MTF / Overlay
#if PACK_V0_ENABLE_MTF
  #include "UtilitaireMTF.h"
  #include "UtilitaireOverlay.h"
#endif

// -------- Mémoire / Réponse / Risque
#if PACK_V0_ENABLE_MEMORY_RISK
  #include "UtilitaireMDHG.h"
  #include "UtilitairePRF.h"
  #include "UtilitaireLiqRisk.h"
#endif

// Version du pack (simple macro pour trace/compat)
#ifndef PACK_V0_VERSION
  #define PACK_V0_VERSION 0x00010000  // 1.0.0
#endif
