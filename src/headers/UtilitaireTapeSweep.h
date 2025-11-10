#pragma once
#include "sierrachart.h"

namespace du {

// Score "sweep" v0: max(UpVol, DownVol)
inline float 	twSweepScore	(const SCStudyInterfaceRef& sc, int idx)
{
  const float uv = (float)sc.UpVolume[idx];
  const float dv = (float)sc.DownVolume[idx];
  return uv > dv ? uv : dv;
}

// Déclenchement si dépasse seuil
inline bool 	twSweepTrigger	(const SCStudyInterfaceRef& sc, int idx, float thr)
{
  return twSweepScore(sc, idx) >= thr;
}

inline bool 	twSweep			(const int* /*tsArr*/, int /*n*/, double /*thrQty*/, double /*thrRate*/)	{ return false; }
inline bool 	twRunDetect		(const int* /*sign*/, int /*n*/, int /*minLen*/, long long /*maxGapMs*/)	{ return false; }

} // namespace du
