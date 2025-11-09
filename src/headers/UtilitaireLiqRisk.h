#pragma once
#include "sierrachart.h"
#include "UtilitaireDom.h"

// Risque de liquidité v0: spread relatif / profondeur near
namespace du {

inline float lrRiskScore(const SCStudyInterfaceRef& sc, int nearLevels)
{
  const float bid = (float)sc.Bid[sc.Index];
  const float ask = (float)sc.Ask[sc.Index];
  const float spread = max(0.0f, ask - bid);

  double depth=0;
  for (int li=0; li<nearLevels; ++li) depth += du::readBidQtyAt(sc, li) + du::readAskQtyAt(sc, li);
  if (depth <= 0) return 0.0f;

  return 1000.0f * spread / (float)depth; // échelle arbitraire
}

} // namespace du
