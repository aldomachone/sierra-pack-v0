#pragma once
#include "sierrachart.h"

// Momentum prix v0
namespace du {

inline float momPrice1(const SCStudyInterfaceRef& sc, int i)
{
  if (i<=0) return 0.0f;
  return (float)(sc.Close[i] - sc.Close[i-1]);
}

} // namespace du
