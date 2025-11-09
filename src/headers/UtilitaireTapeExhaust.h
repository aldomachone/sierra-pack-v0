#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

namespace du {

// Exhaustion v0: chute de pace vs EMA
inline float teExhaustScore(float paceNow, float paceEma)
{
  const float d = paceEma <= 0 ? 0.0f : (paceEma - paceNow) / paceEma;
  return du::sanitize(100.0f * d); // %
}

} // namespace du
