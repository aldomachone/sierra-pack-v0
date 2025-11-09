#pragma once
#include "sierrachart.h"
#include "UtilitaireDom.h"

// Vacuum v0: baisse nette des quantités near
namespace du {

inline float vacNearDrop(const SCStudyInterfaceRef& sc, int nearLevels, double prevNearSum)
{
  double now=0; for (int li=0; li<nearLevels; ++li) now += du::readBidQtyAt(sc, li) + du::readAskQtyAt(sc, li);
  return (float)(prevNearSum - now); // >0 si chute
}

} // namespace du
