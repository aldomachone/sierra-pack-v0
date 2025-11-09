#pragma once
#include <cmath>
#include "sierrachart.h"

namespace du {

// ms depuis epoch (horloge système)
inline long long nowMs(const SCStudyInterfaceRef& sc){
  const SCDateTimeMS t = sc.CurrentSystemDateTimeMS;
  const double ms = (double)t * 24.0 * 3600.0 * 1000.0;
  return (long long)(ms + 0.5);
}

// ms de la barre idx (horloge chart)
inline long long barTimeMs(const SCStudyInterfaceRef& sc, int idx){
  const double ms = (double)sc.BaseDateTimeIn[idx] * 24.0 * 3600.0 * 1000.0;
  return (long long)(ms + 0.5);
}

// vrai si nouvelle journée de trading
inline bool newTradingDay(const SCStudyInterfaceRef& sc, int& lastYmd){
  const int ymd = sc.BaseDateTimeIn[sc.Index].GetDateYMD();
  if (ymd != lastYmd){ lastYmd = ymd; return true; }
  return false;
}

// clamp jitter (latence) en ms
inline int jitterClampMs(int dtMs, int maxJitterMs){
  if (dtMs < 0) return 0;
  if (dtMs > maxJitterMs) return maxJitterMs;
  return dtMs;
}

// secondes équivalentes à N barres
inline double secFromBars(const SCStudyInterfaceRef& sc, int bars){
  return (double)bars * (double)max(1, sc.SecondsPerBar);
}

} // namespace du

