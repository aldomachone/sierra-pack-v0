#pragma once
#include "sierrachart.h"

// ============================================================================
// UtilitaireTempo.h — Helpers de temps/horloge pour ACSIL (Sierra Chart)
// - Conversions SCDateTime → millisecondes
// - Clamps anti-jitter
// - Détection nouvelle journée de trading
// - Estimation skew horloge (système vs chart)
// - Temps de session écoulé
// - Debounce sans état global
// - Utilitaires génériques (clamp, diff sûrs)
// ----------------------------------------------------------------------------
// Conventions :
// - Toutes les fonctions sont inline, sans allocation, sans static global.
// - Les noms sont courts, préfixés "du" pour éviter les collisions.
// - Les conversions SCDateTime se font via cast double * 86'400'000.
// ============================================================================

namespace du
{
// ---------------------------------------------------------------------------
// Constantes et conversions de base
// ---------------------------------------------------------------------------
constexpr double kDayToMs = 24.0 * 3600.0 * 1000.0;
constexpr double kSecToMs = 1000.0;

// Convertit un SCDateTime (jour julien fractionnaire Sierra) en millisecondes.
inline long long toMs(SCDateTime t) noexcept
{
  const double ms = static_cast<double>(t) * kDayToMs;
  return static_cast<long long>(ms + 0.5);
}

// ms depuis epoch selon l’horloge système (Sierra fournit CurrentSystemDateTimeMS).
inline long long nowMs(const SCStudyInterfaceRef& sc) noexcept
{
  return toMs(sc.CurrentSystemDateTimeMS);
}

// Alias de compatibilité pour les anciens codes (V9G, Stop-Run, etc.).
inline long long duNowMs(const SCStudyInterfaceRef& sc) noexcept
{
  return nowMs(sc);
}

// ms de la barre idx selon l’horloge du chart.
inline long long barTimeMs(const SCStudyInterfaceRef& sc, int idx) noexcept
{
  if (sc.ArraySize <= 0)
    return 0;

  int maxIndex = sc.ArraySize - 1;
  if (idx < 0)
    idx = 0;
  else if (idx > maxIndex)
    idx = maxIndex;

  return toMs(sc.BaseDateTimeIn[idx]);
}

// Nanosecondes système (non dispo nativement dans ACSIL). Placeholder neutre.
inline long long duNowNs(SCStudyInterfaceRef) noexcept
{
  return 0;
}

// ---------------------------------------------------------------------------
// Jours / sessions
// ---------------------------------------------------------------------------

// Vrai si la barre courante est dans une nouvelle journée (chart clock).
inline bool newTradingDay(const SCStudyInterfaceRef& sc, int& lastYmd) noexcept
{
  if (sc.ArraySize <= 0)
    return false;

  int idx = sc.Index;
  if (idx < 0)
    idx = 0;
  else if (idx >= sc.ArraySize)
    idx = sc.ArraySize - 1;

  const int ymd = sc.BaseDateTimeIn[idx].GetDateYMD();
  if (ymd != lastYmd)
  {
    lastYmd = ymd;
    return true;
  }
  return false;
}

// Variante “à la volée” sur la dernière barre connue.
inline bool duIsNewTradingDay(SCStudyInterfaceRef sc, int lastDateYMD) noexcept
{
  if (sc.ArraySize <= 0)
    return false;

  const int last = sc.ArraySize - 1;
  return sc.BaseDateTimeIn[last].GetDateYMD() != lastDateYMD;
}

// Retourne l’index de la première barre de la journée courante. O(N backscan) borné.
inline int firstIndexOfToday(const SCStudyInterfaceRef& sc) noexcept
{
  if (sc.ArraySize <= 0)
    return 0;

  int i0 = sc.Index;
  if (i0 < 0)
    i0 = 0;
  else if (i0 >= sc.ArraySize)
    i0 = sc.ArraySize - 1;

  const int ymd = sc.BaseDateTimeIn[i0].GetDateYMD();
  int i = i0;
  while (i > 0 && sc.BaseDateTimeIn[i - 1].GetDateYMD() == ymd)
    --i;
  return i;
}

// Durée écoulée de la session courante en ms, entre première barre du jour
// et horloge “chart” de la barre courante.
inline long long duSessionElapsedMs(SCStudyInterfaceRef sc) noexcept
{
  if (sc.ArraySize <= 0)
    return 0;

  const int       iStart = firstIndexOfToday(sc);
  const long long tStart = barTimeMs(sc, iStart);
  const long long tNow   = barTimeMs(sc, sc.Index);
  return tNow >= tStart ? (tNow - tStart) : 0;
}

// ---------------------------------------------------------------------------
// Jitter / clamps
// ---------------------------------------------------------------------------

// Clamp simple d’un delta de latence en ms.
inline int jitterClampMs(int dtMs, int maxJitterMs) noexcept
{
  if (dtMs < 0) return 0;
  if (dtMs > maxJitterMs) return maxJitterMs;
  return dtMs;
}

// Clamp “saut max” entre deux timestamps ms.
inline long long duJitterClamp(long long prevMs,
                               long long curMs,
                               long long maxJumpMs) noexcept
{
  const long long diff = curMs - prevMs;
  if (diff > maxJumpMs)  return prevMs + maxJumpMs;
  if (diff < -maxJumpMs) return prevMs - maxJumpMs;
  return curMs;
}

// Template clamp générique.
template <typename T>
inline T duClamp(T v, T lo, T hi) noexcept
{
  return v < lo ? lo : (v > hi ? hi : v);
}

// Différence ms sûre, non-négative.
inline long long duSafeDiffMs(long long newerMs, long long olderMs) noexcept
{
  return newerMs > olderMs ? (newerMs - olderMs) : 0LL;
}

// ---------------------------------------------------------------------------
// Échelles temporelles chart
// ---------------------------------------------------------------------------

// Secondes équivalentes à N barres.
// - Si SecondsPerBar > 0, on l’utilise directement.
// - Sinon on approxime via la différence temporelle des deux dernières barres.
// - Fallback = N * 1 s.
inline double secFromBars(const SCStudyInterfaceRef& sc, int bars) noexcept
{
  if (bars <= 0)
    return 0.0;

  if (sc.SecondsPerBar > 0)
    return static_cast<double>(bars) * static_cast<double>(sc.SecondsPerBar);

  int i = sc.Index;
  if (i > 0 && sc.ArraySize > 1)
  {
    const long long t1 = barTimeMs(sc, i);
    const long long t0 = barTimeMs(sc, i - 1);
    const long long dt = duSafeDiffMs(t1, t0);
    if (dt > 0)
      return static_cast<double>(bars) * (static_cast<double>(dt) / kSecToMs);
  }
  return static_cast<double>(bars); // fallback 1 s/bar
}

// ---------------------------------------------------------------------------
// Skew horloge
// ---------------------------------------------------------------------------

// Estime l’écart horloge système vs horloge chart en secondes.
// Signé : positif si système “en avance” vs chart.
inline double duClockSkewEstimate(SCStudyInterfaceRef sc,
                                  long long chartNowMs) noexcept
{
  const long long sysMs  = nowMs(sc);
  const long long skewMs = static_cast<long long>(sysMs) - static_cast<long long>(chartNowMs);
  return static_cast<double>(skewMs) / kSecToMs;
}

// ---------------------------------------------------------------------------
// Debounce sans état global
// ---------------------------------------------------------------------------

// Debounce paramétrique :
// - lastFireMs : timestamp ms du dernier “fire” (à conserver par l’appelant)
// - nowMsVal   : horloge ms courante
// - minGapMs   : écart minimal entre deux triggers
// - state      : compteur d’événements déclenchés
// Retourne true si un déclenchement est autorisé et met à jour lastFireMs + state.
inline bool duDebounceMs(long long& lastFireMs,
                         long long  nowMsVal,
                         long long  minGapMs,
                         int&       state) noexcept
{
  if (minGapMs <= 0)
  {
    lastFireMs = nowMsVal;
    ++state;
    return true;
  }

  const long long dt = duSafeDiffMs(nowMsVal, lastFireMs);
  if (dt < minGapMs)
    return false;

  lastFireMs = nowMsVal;
  ++state;
  return true;
}

// Variante compatible avec signature initiale “state only” :
// - Stocke le dernier fire dans un PersistentInt64 indexé par l’appelant.
inline bool duDebounceMs_Persist(SCStudyInterfaceRef sc,
                                 int                  persistIdxForLastMs,
                                 int&                 state,
                                 long long            nowMsVal,
                                 long long            minGapMs) noexcept
{
  long long& lastMs = sc.GetPersistentInt64(persistIdxForLastMs);
  return duDebounceMs(lastMs, nowMsVal, minGapMs, state);
}

// ---------------------------------------------------------------------------
// Helpers divers
// ---------------------------------------------------------------------------

// Vrai une fois par nouvelle barre. lastIndex est fourni par l’appelant.
inline bool duIsNewBar(const SCStudyInterfaceRef& sc, int& lastIndex) noexcept
{
  if (sc.Index != lastIndex)
  {
    lastIndex = sc.Index;
    return true;
  }
  return false;
}

// Proxy pratique sur le flag plateforme.
inline bool duIsFullRecalculation(const SCStudyInterfaceRef& sc) noexcept
{
  return sc.IsFullRecalculation;
}

// Incrémente avec saturation [lo, hi].
template <typename T>
inline void duSaturatingAdd(T& v, T delta, T lo, T hi) noexcept
{
  if (delta > 0 && v > hi - delta) { v = hi; return; }
  if (delta < 0 && v < lo - delta) { v = lo; return; }
  v += delta;
}

} // namespace du
