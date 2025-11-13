#pragma once
#include "sierrachart.h"

// ============================================================================
// UtilitaireEngineContract.h — Contrat SG / Features v1 (Pack_v0)
//
// Rôle :
// - Stabiliser les index SG par famille d'ENGINE (DOM, TAPE, CONTEXTE, ZONES,
//   MDHG, PRF, LIQ, etc.).
// - Fournir une API compacte pour récupérer les plages de SG (DOM/TAPE/CTX…),
//   avec une énumération symbolique des familles.
// - Point central pour publier la #LAYOUT des features (hook possible vers
//   feat_registry_v1.csv via macro DU_FEATREG_REGISTER_LAYOUT).
// - Aucun état global. Inline / constexpr only. Pas de sc.StudyDescription.
// - À inclure par tous les ENGINE headless et par les SIGNALS lecteurs.
//
// ---------------------------------------------------------------------------
// Matrice SG v1 (par étude ENGINE, pas global à un seul study) :
//
//   ENGINE              | SG range | Count
//   --------------------+----------+------
//   DOM_ENGINE_V2       |  1 .. 24 |  24
//   TAPE_ENGINE_V1      |  1 .. 16 |  16
//   VWAP_ENGINE         |  1 ..  8 |   8
//   PROFILE_ENGINE      |  1 ..  8 |   8
//   OHLC_ENGINE         |  1 ..  8 |   8
//   RANGE_ENGINE        |  1 ..  8 |   8
//   PIVOT_ENGINE        |  1 ..  8 |   8
//   FIBO_ENGINE         |  1 ..  8 |   8
//   FVG_ENGINE          |  1 ..  8 |   8
//   ZONE_ENGINE         |  1 ..  8 |   8
//   MDHG_ENGINE         |  1 .. 16 |  16
//   PRF_ENGINE          |  1 ..  8 |   8
//   LIQRISK_ENGINE      |  1 ..  8 |   8
//
// Chaque ENGINE vit dans sa propre étude ACSIL, donc pas de conflit de SG
// entre familles, seulement un contrat par ENGINE.
// ============================================================================

namespace du {

// ============================================================================
// 1) Version / schéma (alignement avec PackUtilitaires_v0.h)
// ============================================================================

// Si le header umbrella n'est pas inclus, on fournit des valeurs par défaut
#ifndef PACK_V0_SCHEMA_REV
  #define PACK_V0_SCHEMA_REV 1
#endif

#ifndef PACK_V0_FEATSIG_V1
  #define PACK_V0_FEATSIG_V1 1
#endif

// Contrat courant : schéma et featSig v1 uniquement
constexpr int DU_SCHEMA_REV_ENGINECONTRACT = 1;
constexpr int DU_FEATSIG_V1_ENGINECONTRACT = 1;

static_assert(PACK_V0_SCHEMA_REV >= DU_SCHEMA_REV_ENGINECONTRACT,
              "Pack_v0: PACK_V0_SCHEMA_REV trop ancien pour UtilitaireEngineContract.h");

static_assert(PACK_V0_FEATSIG_V1 == DU_FEATSIG_V1_ENGINECONTRACT,
              "Pack_v0: UtilitaireEngineContract.h est conçu pour FEATSIG v1 uniquement.");

// ============================================================================
// 2) Plages de Subgraphs par ENGINE (mapping v1 figé)
// ============================================================================

constexpr int DU_SG_MAX = 24;

// Plage générique
struct DU_SgRange {
  int first;
  int last;

  constexpr int count() const noexcept {
    return (last >= first) ? (last - first + 1) : 0;
  }
};

// Raccourcis de plages standard
constexpr DU_SgRange DU_SG_1_8()  noexcept { return { 1,  8}; }
constexpr DU_SgRange DU_SG_1_16() noexcept { return { 1, 16}; }
constexpr DU_SgRange DU_SG_1_24() noexcept { return { 1, 24}; }

// Mapping canonique par ENGINE (v1)
// DOM_ENGINE_V2
constexpr DU_SgRange DU_SG_DOM()     noexcept { return DU_SG_1_24(); }

// TAPE_ENGINE_V1
constexpr DU_SgRange DU_SG_TAPE()    noexcept { return DU_SG_1_16(); }

// Contexte lent (VWAP / PROFILE / OHLC / RANGE / PIVOT / FIBO / FVG / ZONE)
// Chaque ENGINE de contexte utilise indépendamment SG 1..8 dans son propre study.
constexpr DU_SgRange DU_SG_VWAP()    noexcept { return DU_SG_1_8(); }
constexpr DU_SgRange DU_SG_PROFILE() noexcept { return DU_SG_1_8(); }
constexpr DU_SgRange DU_SG_OHLC()    noexcept { return DU_SG_1_8(); }
constexpr DU_SgRange DU_SG_RANGE()   noexcept { return DU_SG_1_8(); }
constexpr DU_SgRange DU_SG_PIVOT()   noexcept { return DU_SG_1_8(); }
constexpr DU_SgRange DU_SG_FIBO()    noexcept { return DU_SG_1_8(); }
constexpr DU_SgRange DU_SG_FVG()     noexcept { return DU_SG_1_8(); }
constexpr DU_SgRange DU_SG_ZONE()    noexcept { return DU_SG_1_8(); }

// Alias "historiques" pour compatibilité (CONTEXTE / ZONES génériques)
constexpr DU_SgRange DU_SG_CONTEXT() noexcept { return DU_SG_1_8(); }
constexpr DU_SgRange DU_SG_ZONES()   noexcept { return DU_SG_1_8(); }

// Mémoire / Réponse / Risque
constexpr DU_SgRange DU_SG_MDHG()    noexcept { return DU_SG_1_16(); } // Mémoire DOM (heatmap)
constexpr DU_SgRange DU_SG_PRF()     noexcept { return DU_SG_1_8();  } // Réponse prix
constexpr DU_SgRange DU_SG_LIQ()     noexcept { return DU_SG_1_8();  } // Risque de liquidité

// ============================================================================
// 2 bis) Enum symbolique des familles (EngineFamily)
// ============================================================================

enum class EngineFamily : int
{
  Dom,
  Tape,
  Vwap,
  Profile,
  Ohlc,
  Range,
  Pivot,
  Fibo,
  Fvg,
  Zone,
  Context,  // alias générique
  Zones,    // alias générique
  Mdhg,
  Prf,
  Liq
};

constexpr const char* duEngineFamilyName(EngineFamily f) noexcept
{
  switch (f)
  {
    case EngineFamily::Dom:     return "DOM_ENGINE_V2";
    case EngineFamily::Tape:    return "TAPE_ENGINE_V1";
    case EngineFamily::Vwap:    return "VWAP_ENGINE";
    case EngineFamily::Profile: return "PROFILE_ENGINE";
    case EngineFamily::Ohlc:    return "OHLC_ENGINE";
    case EngineFamily::Range:   return "RANGE_ENGINE";
    case EngineFamily::Pivot:   return "PIVOT_ENGINE";
    case EngineFamily::Fibo:    return "FIBO_ENGINE";
    case EngineFamily::Fvg:     return "FVG_ENGINE";
    case EngineFamily::Zone:    return "ZONE_ENGINE";
    case EngineFamily::Context: return "CONTEXT_ENGINE";
    case EngineFamily::Zones:   return "ZONES_ENGINE";
    case EngineFamily::Mdhg:    return "MDHG_ENGINE";
    case EngineFamily::Prf:     return "PRF_ENGINE";
    case EngineFamily::Liq:     return "LIQRISK_ENGINE";
    default:                    return "UNKNOWN_ENGINE";
  }
}

constexpr DU_SgRange duGetRange(EngineFamily f) noexcept
{
  switch (f)
  {
    case EngineFamily::Dom:     return DU_SG_DOM();
    case EngineFamily::Tape:    return DU_SG_TAPE();
    case EngineFamily::Vwap:    return DU_SG_VWAP();
    case EngineFamily::Profile: return DU_SG_PROFILE();
    case EngineFamily::Ohlc:    return DU_SG_OHLC();
    case EngineFamily::Range:   return DU_SG_RANGE();
    case EngineFamily::Pivot:   return DU_SG_PIVOT();
    case EngineFamily::Fibo:    return DU_SG_FIBO();
    case EngineFamily::Fvg:     return DU_SG_FVG();
    case EngineFamily::Zone:    return DU_SG_ZONE();
    case EngineFamily::Context: return DU_SG_CONTEXT();
    case EngineFamily::Zones:   return DU_SG_ZONES();
    case EngineFamily::Mdhg:    return DU_SG_MDHG();
    case EngineFamily::Prf:     return DU_SG_PRF();
    case EngineFamily::Liq:     return DU_SG_LIQ();
    default:                    return DU_SgRange{0, -1};
  }
}

// ============================================================================
// 3) Vérifications compile-time (cohérence interne du contrat)
// ============================================================================

constexpr int duSgCount(DU_SgRange r) noexcept { return r.count(); }

static_assert(DU_SG_DOM().last    <= DU_SG_MAX, "DU_SG_DOM dépasse DU_SG_MAX");
static_assert(DU_SG_TAPE().last   <= DU_SG_MAX, "DU_SG_TAPE dépasse DU_SG_MAX");
static_assert(DU_SG_VWAP().last   <= DU_SG_MAX, "DU_SG_VWAP dépasse DU_SG_MAX");
static_assert(DU_SG_PROFILE().last<= DU_SG_MAX, "DU_SG_PROFILE dépasse DU_SG_MAX");
static_assert(DU_SG_OHLC().last   <= DU_SG_MAX, "DU_SG_OHLC dépasse DU_SG_MAX");
static_assert(DU_SG_RANGE().last  <= DU_SG_MAX, "DU_SG_RANGE dépasse DU_SG_MAX");
static_assert(DU_SG_PIVOT().last  <= DU_SG_MAX, "DU_SG_PIVOT dépasse DU_SG_MAX");
static_assert(DU_SG_FIBO().last   <= DU_SG_MAX, "DU_SG_FIBO dépasse DU_SG_MAX");
static_assert(DU_SG_FVG().last    <= DU_SG_MAX, "DU_SG_FVG dépasse DU_SG_MAX");
static_assert(DU_SG_ZONE().last   <= DU_SG_MAX, "DU_SG_ZONE dépasse DU_SG_MAX");
static_assert(DU_SG_MDHG().last   <= DU_SG_MAX, "DU_SG_MDHG dépasse DU_SG_MAX");
static_assert(DU_SG_PRF().last    <= DU_SG_MAX, "DU_SG_PRF dépasse DU_SG_MAX");
static_assert(DU_SG_LIQ().last    <= DU_SG_MAX, "DU_SG_LIQ dépasse DU_SG_MAX");

// Comptages attendus (mapping figé v1)
static_assert(duSgCount(DU_SG_DOM())    == 24, "DOM_ENGINE_V2 doit exposer 24 SG.");
static_assert(duSgCount(DU_SG_TAPE())   == 16, "TAPE_ENGINE_V1 doit exposer 16 SG.");
static_assert(duSgCount(DU_SG_VWAP())   ==  8, "VWAP_ENGINE doit exposer 8 SG.");
static_assert(duSgCount(DU_SG_PROFILE())==  8, "PROFILE_ENGINE doit exposer 8 SG.");
static_assert(duSgCount(DU_SG_OHLC())   ==  8, "OHLC_ENGINE doit exposer 8 SG.");
static_assert(duSgCount(DU_SG_RANGE())  ==  8, "RANGE_ENGINE doit exposer 8 SG.");
static_assert(duSgCount(DU_SG_PIVOT())  ==  8, "PIVOT_ENGINE doit exposer 8 SG.");
static_assert(duSgCount(DU_SG_FIBO())   ==  8, "FIBO_ENGINE doit exposer 8 SG.");
static_assert(duSgCount(DU_SG_FVG())    ==  8, "FVG_ENGINE doit exposer 8 SG.");
static_assert(duSgCount(DU_SG_ZONE())   ==  8, "ZONE_ENGINE doit exposer 8 SG.");
static_assert(duSgCount(DU_SG_MDHG())   == 16, "MDHG_ENGINE doit exposer 16 SG.");
static_assert(duSgCount(DU_SG_PRF())    ==  8, "PRF_ENGINE doit exposer 8 SG.");
static_assert(duSgCount(DU_SG_LIQ())    ==  8, "LIQRISK_ENGINE doit exposer 8 SG.");

// ============================================================================
// 4) Helpers d’usage pour les SIGNALS / ENGINES
// ============================================================================

// Vérification runtime très légère d'une plage (défensive)
inline void duCheckSgRange(const SCStudyInterfaceRef sc, DU_SgRange r)
{
  if (r.first < 1 || r.last < r.first || r.last > SC_MAX_NUM_STUDY_SUBGRAPHS)
  {
    sc.AddMessageToLog("DU: SG range invalide (first/last).", 1);
  }
}

// Helper générique pour tester si un SG index appartient à une famille
constexpr bool duSgInRange(DU_SgRange r, int sgIndex) noexcept
{
  return (sgIndex >= r.first && sgIndex <= r.last);
}

// Convertir un index SG global en index local 0..N-1 dans la plage
constexpr int duSgLocalIndex(DU_SgRange r, int sgIndex) noexcept
{
  return sgIndex - r.first; // l'appelant doit garantir duSgInRange(r, sgIndex)
}

// Raccourcis lisibles pour les SIGNALS (usage direct sans enum si souhaité)
constexpr DU_SgRange duGetDomRange()     noexcept { return DU_SG_DOM();    }
constexpr DU_SgRange duGetTapeRange()    noexcept { return DU_SG_TAPE();   }
constexpr DU_SgRange duGetVwapRange()    noexcept { return DU_SG_VWAP();   }
constexpr DU_SgRange duGetProfileRange() noexcept { return DU_SG_PROFILE();}
constexpr DU_SgRange duGetOhlcRange()    noexcept { return DU_SG_OHLC();   }
constexpr DU_SgRange duGetRangeRange()   noexcept { return DU_SG_RANGE();  }
constexpr DU_SgRange duGetPivotRange()   noexcept { return DU_SG_PIVOT();  }
constexpr DU_SgRange duGetFiboRange()    noexcept { return DU_SG_FIBO();   }
constexpr DU_SgRange duGetFvgRange()     noexcept { return DU_SG_FVG();    }
constexpr DU_SgRange duGetZoneRange()    noexcept { return DU_SG_ZONE();   }
constexpr DU_SgRange duGetMdhgRange()    noexcept { return DU_SG_MDHG();   }
constexpr DU_SgRange duGetPrfRange()     noexcept { return DU_SG_PRF();    }
constexpr DU_SgRange duGetLiqRange()     noexcept { return DU_SG_LIQ();    }

// ============================================================================
// 5) Publication des noms de features (#LAYOUT pour CSV / registry)
//
// Usage typique côté ENGINE :
//
//   static const char* kDomFeatureNames[duSgCount(DU_SG_DOM())] = {
//     "DOM_NearBid", "DOM_NearAsk", ...
//   };
//   duPublishFeatureLayout(sc, "DOM_ENGINE_V2", DU_SG_DOM(), kDomFeatureNames);
//
// L’appel génère une ligne de log type :
//   DU:LAYOUT:DOM_ENGINE_V2:24:DOM_NearBid,DOM_NearAsk,...
//
// Ces lignes pourront être parsées offline pour alimenter feat_registry_v1.csv.
// Un hook compile-time optionnel permet aussi de brancher directement sur un
// registre interne (voir macro DU_FEATREG_REGISTER_LAYOUT).
// ============================================================================

inline void duPublishFeatureLayout(const SCStudyInterfaceRef sc,
                                   const char* family,
                                   DU_SgRange  r,
                                   const char* const* names)
{
  const int count = r.count();

  SCString msg;
  msg.Format("DU:LAYOUT:%s:%d:", family, count);

  for (int i = 0; i < count; ++i)
  {
    msg += names[i];
    if (i + 1 < count)
      msg += ",";
  }

  sc.AddMessageToLog(msg, 0);

#ifdef DU_FEATREG_REGISTER_LAYOUT
  // Hook optionnel vers un système de registry interne.
  // À définir côté MLP / infra, par exemple :
  //   #define DU_FEATREG_REGISTER_LAYOUT(fam, range, namesArr) \
  //       du::mlp::RegisterFeatureLayout((fam), (range), (namesArr))
  DU_FEATREG_REGISTER_LAYOUT(family, r, names);
#endif
}

// Overload de compat : ancienne signature (sans DU_SgRange)
inline void duPublishFeatureLayout(const SCStudyInterfaceRef sc,
                                   const char* family,
                                   const char* const* names,
                                   int count)
{
  DU_SgRange r{1, count};
  duPublishFeatureLayout(sc, family, r, names);
}

} // namespace du
