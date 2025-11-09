#pragma once
#include "sierrachart.h"

namespace du {

// Score "sweep" v0: max(UpVol, DownVol)
inline float twSweepScore(const SCStudyInterfaceRef& sc, int idx)
{
  const float uv = (float)sc.UpVolume[idx];
  const float dv = (float)sc.DownVolume[idx];
  return uv > dv ? uv : dv;
}

// Déclenchement si dépasse seuil
inline bool twSweepTrigger(const SCStudyInterfaceRef& sc, int idx, float thr)
{
  return twSweepScore(sc, idx) >= thr;
}

} // namespace du
