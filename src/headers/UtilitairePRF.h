#pragma once
#include "sierrachart.h"

// Réponse / demi-vie v0
namespace du {

inline float prfAlphaFromHalfLifeSec(float halfLifeSec, float dtSec)
{
  halfLifeSec = max(0.1f, halfLifeSec);
  dtSec       = max(0.01f, dtSec);
  const float a = 1.0f - expf(-dtSec * 0.69314718f / halfLifeSec);
  return a;
}

inline float prfEmaHL(float x, float prev, float halfLifeSec, float dtSec)
{
  const float a = prfAlphaFromHalfLifeSec(halfLifeSec, dtSec);
  return a*x + (1.0f - a)*prev;
}

} // namespace du
