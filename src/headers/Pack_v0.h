#pragma once
#include "sierrachart.h"

// === Core temps / infra ===
#include "UtilitaireTempo.h"
#include "UtilitaireSanitize.h"
#include "UtilitaireCompat.h"

// === I/O & MLP ===
#include "UtilitaireMLP.h"

// === DOM / Tape (Phase 0) ===
#include "UtilitaireDom.h"
#include "UtilitaireDomSignal.h"
#include "UtilitaireTapePace.h"
#include "UtilitaireTapeBVC.h"
#include "UtilitaireTapeSweep.h"
#include "UtilitaireTapeExhaust.h"

// === Contexte (Phase 0 minutes/HTF) ===
#include "UtilitaireVWAP.h"
#include "UtilitaireProfile.h"
#include "UtilitaireOHLC.h"
#include "UtilitaireRanges.h"

// === Zones / Niveaux (Phase 1, pré-créés pour éviter refactors) ===
#include "UtilitairePivots.h"
#include "UtilitaireFibo.h"
#include "UtilitaireReloadZone.h"
#include "UtilitaireFVG.h"
#include "UtilitaireZones.h"
#include "UtilitaireConfluence.h"
#include "UtilitaireMTF.h"
#include "UtilitaireOverlay.h"

// === Mémoire / Risque / Réponse (Phase 2) ===
#include "UtilitaireMDHG.h"
#include "UtilitairePRF.h"
#include "UtilitaireLiqRisk.h"

// === Algos DOM avancés (réserves, optionnels) ===
#include "UtilitaireDomSignalMomentum.h"
#include "UtilitaireDomImbalSweep.h"
#include "UtilitaireDomVacuum.h"
#include "UtilitaireStopRun.h"
