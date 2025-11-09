#pragma once
#include "sierrachart.h"
#include "UtilitaireDom.h"

namespace du {

// Imbalance near-mid-far v0
inline float imbalNearMinusFarPct(const SCStudyInterfaceRef& sc, int levels, int nearN, int midN)
{
  const auto b = du::sumBands(sc, 1, levels, nearN, midN);
  float n=0,m=0,f=0; du::normalizePct(b.nearBid+b.nearAsk, b.midBid+b.midAsk, b.farBid+b.farAsk, n,m,f);
  return n - f;
}

} // namespace du
