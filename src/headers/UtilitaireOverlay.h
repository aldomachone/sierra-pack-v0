#pragma once
#include "sierrachart.h"

// Helpers d'affichage pour ne pas déplacer l'échelle
namespace du {

inline void ovMarkerTransparent(SCSubgraphRef sg, int size)
{
  sg.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
  sg.LineWidth = size;
  sg.DrawZeros = 0;
  sg.DisplayAsMainPriceGraphValue = 0;
}

} // namespace du
