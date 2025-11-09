#pragma once
#include "sierrachart.h"

// Fair Value Gap v0 (3-bar gap)
namespace du {

struct Fvg { bool up=false, dn=false; float hi=0, lo=0; };

// FVG up si Low[n-1] > High[n-3], dn si High[n-1] < Low[n-3]
inline Fvg fvgDetect(const SCStudyInterfaceRef& sc, int n /* index courant */)
{
  Fvg f{};
  if (n < 2) return f;
  const float h0 = (float)sc.High[n-2];
  const float l0 = (float)sc.Low [n-2];
  const float h2 = (float)sc.High[n];
  const float l2 = (float)sc.Low [n];

  // Up-gap si Low[n-1] > High[n-2]
  if (sc.Low[n-1] > h0) { f.up = true; f.hi = (float)sc.Low[n-1]; f.lo = h0; }
  // Down-gap si High[n-1] < Low[n-2]
  if (sc.High[n-1] < l0) { f.dn = true; f.lo = (float)sc.High[n-1]; f.hi = l0; }
  (void)h2; (void)l2; // non utilisés v0
  return f;
}

} // namespace du
