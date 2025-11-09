#pragma once
#include "sierrachart.h"
#include "UtilitaireTempo.h"
#include "UtilitaireSanitize.h"

namespace du {

// Pace v0 = (UpVol+DownVol) / durée barre en secondes
inline float tpPaceBar(const SCStudyInterfaceRef& sc, int idx)
{
  const double vol = (double)sc.UpVolume[idx] + (double)sc.DownVolume[idx];
  const double dt  = max(1.0, (double)(sc.SecondsPerBar)); // fallback
  return (float)du::sanitize((float)(vol / dt));
}

// Inter-arrival temps & EMA
inline int   tpInterArrivalMs(long long lastMs, long long nowMs) { return (int)max(0LL, nowMs - lastMs); }
inline float tpInterArrivalEma(float x, float prev, int pct)     { return du::emaPct(x, prev, du::clamp(pct,1,99)); }

} // namespace du
