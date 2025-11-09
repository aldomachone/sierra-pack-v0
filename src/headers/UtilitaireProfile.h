#pragma once
#include "sierrachart.h"

// Profile v0 très simple: POC = Close de la barre avec Volume max dans un lookback
namespace du {

struct ProfileOut { float poc=0; float vMax=0; };

inline ProfileOut vpPocSimple(const SCStudyInterfaceRef& sc, int lookback)
{
  ProfileOut out{};
  const int i0 = max(0, sc.Index - max(1, lookback) + 1);
  int iBest = i0; float vBest = (float)sc.Volume[i0];
  for (int i=i0; i<=sc.Index; ++i)
  {
    const float v = (float)sc.Volume[i];
    if (v > vBest) { vBest = v; iBest = i; }
  }
  out.poc  = (float)sc.Close[iBest];
  out.vMax = vBest;
  return out;
}

} // namespace du
