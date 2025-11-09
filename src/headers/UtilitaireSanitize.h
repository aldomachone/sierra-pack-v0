#pragma once
#include <cmath>
#include "sierrachart.h"

// === Sanitize / Clamp (v0) ===
namespace du {

  template<typename T>
  inline T clamp(T x, T lo, T hi)
  {
    return x < lo ? lo : (x > hi ? hi : x);
  }

  // sanitize : remplace NaN/Inf par 0
  inline float sanitize(float v) {
    if (!std::isfinite(v)) return 0.0f;
    return v;
  }
  inline double sanitize(double v) {
    if (!std::isfinite(v)) return 0.0;
    return v;
  }

  // emaPct : EMA avec facteur (%) entre 1 et 99
  inline float emaPct(float x, float prev, int pct)
  {
    pct = clamp(pct, 1, 99);
    const float a = (float)pct / 100.0f;
    return a * x + (1.0f - a) * prev;
  }

} // namespace du
