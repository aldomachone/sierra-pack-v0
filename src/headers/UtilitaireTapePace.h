#pragma once
#include "sierrachart.h"
#include "UtilitaireTempo.h"
#include "UtilitaireSanitize.h"

namespace du {

inline float pacePerSecBar(const SCStudyInterfaceRef& sc, int idx){
  const double vol = (double)sc.UpVolume[idx] + (double)sc.DownVolume[idx];
  const double dt  = max(1, sc.SecondsPerBar);
  return (float)(vol / dt);
}

inline int interArrivalMs(long long last, long long now){ return (int)max(0LL, now-last); }

} // namespace du
