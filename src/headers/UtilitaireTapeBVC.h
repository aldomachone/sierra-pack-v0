#pragma once
#include "sierrachart.h"

// Bulk Volume Classification v0 (proxy barre): utilise Up/DownVolume
namespace du {

inline void bvcBar(const SCStudyInterfaceRef& sc, int idx, float& buyVol, float& sellVol)
{
  buyVol  = (float)sc.UpVolume[idx];
  sellVol = (float)sc.DownVolume[idx];
}

// Delta = buy - sell
inline float bvcDelta(const SCStudyInterfaceRef& sc, int idx)
{
  return (float)sc.UpVolume[idx] - (float)sc.DownVolume[idx];
}

} // namespace du
