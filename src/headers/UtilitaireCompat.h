#pragma once
#include "sierrachart.h"
#include <cstdint>
#include <cstring>
#include <algorithm>

// =============================================================================
// UtilitaireCompat.h — Compatibilité indices SG/Inputs/featSig (v2 enrichi)
// -----------------------------------------------------------------------------
// • 100% inline, aucune allocation globale, aucun état statique persistant.
// • Compat v0 préservée :
//      - duCompatIdx(old,&new)
//      - duCompatSg(...)
//      - duCompatInput(...)
//      - duCompatCheckFeatSig(expected, actual)  // version sans log
// • Ajouts v2 :
//   - Registres familiaux SG (DOM/TAPE/CONTEXT/…)
//   - Mapping « identité » par défaut avec crochet pour versions futures.
//   - Recherche Input par nom « 01. Nom » avec normalisation.
//   - Helpers pour accéder à des arrays d’autres études (GetStudyArray*).
// -----------------------------------------------------------------------------
// NOTE CONTRAT SG FAMILIES : les plages DOM/TAPE/CONTEXT/MDHG/PRF/LIQRISK
// doivent rester synchronisées avec ce qui est décrit dans UtilitaireEngineContract.h
// et dans la doc Pack_v0 (DOM: 1–24, TAPE: 1–16, CONTEXT: 1–8, MDHG: 1–16,
// PRF: 1–8, LIQRISK: 1–8).
// =============================================================================

namespace du {

// ---- Types de base ----------------------------------------------------------

enum duSide       : int { DU_BID = 0, DU_ASK = 1 };
enum duGateMode   : int { DU_FREEZE = 0, DU_ZERO = 1, DU_CONTINUE = 2 };
enum duCrossState : int { DU_BELOW = 0, DU_CROSSUP = 1, DU_ABOVE = 2, DU_CROSSDOWN = 3 };

struct duZone
{
  double lo{0.0};
  double hi{0.0};
};

constexpr double DU_EPS = 1e-12;

// ---- Familles SG et bornes figées -----------------------------------------

enum duSgFamily : int
{
  SGF_NONE    = 0,
  SGF_DOM     = 1, // 1..24
  SGF_TAPE    = 2, // 1..16
  SGF_CONTEXT = 3, // 1..8 (VWAP/PROFILE/OHLC/RANGE/PIVOT/FIBO/FVG/ZONE)
  SGF_MDHG    = 4, // 1..16
  SGF_PRF     = 5, // 1..8
  SGF_LIQRISK = 6  // 1..8
};

constexpr int sgfLo(duSgFamily f) noexcept
{
  switch (f)
  {
    case SGF_DOM:     return 1;
    case SGF_TAPE:    return 1;
    case SGF_CONTEXT: return 1;
    case SGF_MDHG:    return 1;
    case SGF_PRF:     return 1;
    case SGF_LIQRISK: return 1;
    default:          return 0;
  }
}

constexpr int sgfHi(duSgFamily f) noexcept
{
  switch (f)
  {
    case SGF_DOM:     return 24;
    case SGF_TAPE:    return 16;
    case SGF_CONTEXT: return 8;
    case SGF_MDHG:    return 16;
    case SGF_PRF:     return 8;
    case SGF_LIQRISK: return 8;
    default:          return 0;
  }
}

// ---- Hachage léger et log-once --------------------------------------------

// FNV-1a 32-bit, utilisable en constexpr (C++17+).
constexpr uint32_t duHash32(const char* s) noexcept
{
  if (!s)
    return 0u;

  uint32_t h = 2166136261u;
  while (*s)
  {
    h ^= static_cast<uint8_t>(*s);
    h *= 16777619u;
    ++s;
  }
  return h;
}

inline void duLogOnce(SCStudyInterfaceRef sc,
                      const char*        key,
                      const char*        msg)
{
  const uint32_t h    = duHash32(key);
  const int      slot = 10000 + static_cast<int>(h & 0x7FFF);
  int&           seen = sc.GetPersistentInt(slot);

  if (!seen)
  {
    sc.AddMessageToLog(msg, 0);
    seen = 1;
  }
}

// ---- Accès arrays d’autres études -----------------------------------------

inline bool getStudyArray(const SCStudyInterfaceRef& sc,
                          int                        studyId,
                          int                        subgraphIdx,
                          SCFloatArray&              out)
{
  return sc.GetStudyArrayUsingID(studyId, subgraphIdx, out) != 0;
}

inline bool getStudyArrayFromChart(const SCStudyInterfaceRef& sc,
                                   int                        chart,
                                   int                        studyId,
                                   int                        subgraphIdx,
                                   SCFloatArray&              out)
{
  sc.GetStudyArrayFromChartUsingID(chart, studyId, subgraphIdx, out);
  return out.GetArraySize() > 0;
}

inline bool tryGetStudyValue(const SCStudyInterfaceRef& sc,
                             int                        studyId,
                             int                        subgraphIdx,
                             int                        barIndex,
                             float&                     v)
{
  SCFloatArray arr;
  if (!getStudyArray(sc, studyId, subgraphIdx, arr))
    return false;

  const int n = arr.GetArraySize();
  if (barIndex < 0 || barIndex >= n)
    return false;

  v = arr[barIndex];
  return true;
}

// ---- Compat indices SG -----------------------------------------------------
// v2 : identité par défaut. Crochet pour remap futur selon schéma/versions.
// Retourne true et écrit newIdx même si identité.

inline bool duCompatIdx(int oldIdx, int* newIdx) noexcept
{
  if (newIdx)
    *newIdx = oldIdx;
  return true;
}

// Variante avec borne familiale. Coupe aux bornes si hors plage.
inline bool duCompatIdx(int        oldIdx,
                        duSgFamily fam,
                        int        /*schemaVer*/,
                        int*       newIdx) noexcept
{
  const int lo  = sgfLo(fam);
  const int hi  = sgfHi(fam);
  int       idx = oldIdx;

  if (lo > 0 && hi > 0)
    idx = std::max(lo, std::min(hi, idx));

  if (newIdx)
    *newIdx = idx;

  return true;
}

// ---- Compat SG fixés -------------------------------------------------------
// Par défaut retourne l’index fourni. Sert de point d’injection si une famille migre.
inline int duCompatSg(SCStudyInterfaceRef /*sc*/,
                      int                 family,
                      int                 sgFixed) noexcept
{
  duSgFamily f  = static_cast<duSgFamily>(family);
  const int  lo = sgfLo(f);
  const int  hi = sgfHi(f);

  if (lo == 0 && hi == 0)
    return sgFixed;

  return std::max(lo, std::min(hi, sgFixed));
}

// ---- Compat Inputs : recherche par nom normalisé ---------------------------

inline void trimLeftDigitsDotSpace(const char*& p) noexcept
{
  while (*p && ((*p >= '0' && *p <= '9') || *p == '.' || *p == ' '))
    ++p;
}

// Comparaison insensible à la casse, en ignorant un éventuel préfixe "01. ".
inline bool nameEqualsNorm(const char* a, const char* b) noexcept
{
  if (!a || !b)
    return false;

  const char* pa = a;
  const char* pb = b;
  trimLeftDigitsDotSpace(pa);
  trimLeftDigitsDotSpace(pb);

  for (;;)
  {
    char ca = *pa;
    char cb = *pb;

    if (ca >= 'A' && ca <= 'Z')
      ca = static_cast<char>(ca - 'A' + 'a');
    if (cb >= 'A' && cb <= 'Z')
      cb = static_cast<char>(cb - 'A' + 'a');

    if (ca != cb)
      return false;
    if (ca == 0)
      return true;

    ++pa;
    ++pb;
  }
}

inline int findInputIndexByName(const SCStudyInterfaceRef& sc,
                                const char*                wanted)
{
  if (!wanted)
    return -1;

  for (int i = 0; i < SC_INPUTS_AVAILABLE; ++i)
  {
    const char* nm = sc.Input[i].GetName();
    if (!nm || !*nm)
      continue;

    if (nameEqualsNorm(nm, wanted))
      return i;
  }
  return -1;
}

inline int duCompatInput(const SCStudyInterfaceRef& sc,
                         const char*                wantedName,
                         int                        fallbackIdx)
{
  const int k = findInputIndexByName(sc, wantedName);
  return (k >= 0 ? k : fallbackIdx);
}

// ---- Feature signature ------------------------------------------------------

// Version v0 : pas de log, juste un test simple.
inline bool duCompatCheckFeatSig(int expected,
                                 int actual) noexcept
{
  return expected == actual;
}

// Version v2 : avec SC pour log via duLogOnce.
inline bool duCompatCheckFeatSig(SCStudyInterfaceRef sc,
                                 int                 expected,
                                 int                 actual)
{
  if (expected == actual)
    return true;

  SCString m;
  m.Format(
      "[Compat] featSig mismatch: expected=%d actual=%d. Continues in compat mode.",
      expected, actual);
  duLogOnce(sc, "featSigMismatch", m.GetChars());
  return false;
}

// ---- Sécurité bornes SG/Inputs --------------------------------------------

inline int clampSgIndex(int idx) noexcept
{
  if (idx < 0)
    idx = 0;
  if (idx >= SC_SUBGRAPH_MAX)
    idx = SC_SUBGRAPH_MAX - 1;
  return idx;
}

inline int clampInputIndex(int idx) noexcept
{
  if (idx < 0)
    idx = 0;
  if (idx >= SC_INPUTS_AVAILABLE)
    idx = SC_INPUTS_AVAILABLE - 1;
  return idx;
}

// ---- Cross helpers simples --------------------------------------------------
// Note : par design, si prev == level et now > level, on retourne DU_ABOVE
// (et non DU_CROSSUP). Les seuls véritables "cross" sont :
//  - DU_CROSSUP   : prev < level, now > level
//  - DU_CROSSDOWN : prev > level, now < level

inline duCrossState crossState(double prev,
                               double now,
                               double level) noexcept
{
  const bool wasBelow = prev < level;
  const bool nowAbove = now > level;
  const bool nowBelow = now < level;

  if (wasBelow && nowAbove)
    return DU_CROSSUP;
  if (!wasBelow && nowBelow)
    return DU_CROSSDOWN;

  return nowAbove ? DU_ABOVE : DU_BELOW;
}

// ---- Gate mode to string ----------------------------------------------------

inline const char* gateModeName(duGateMode m) noexcept
{
  switch (m)
  {
    case DU_FREEZE: return "FREEZE";
    case DU_ZERO:   return "ZERO";
    default:        return "CONTINUE";
  }
}

} // namespace du
