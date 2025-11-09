#pragma once
#include "sierrachart.h"

namespace du {

inline void markerTransparent(SCSubgraphRef sg, int size){
  sg.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
  sg.LineWidth = size;
  sg.DrawZeros = 0;
  sg.DisplayAsMainPriceGraphValue = 0;
  sg.PrimaryColor   = RGB(0,255,0);
  sg.SecondaryColor = RGB(255,0,0);
}

// Labels “dernière barre” uniquement
inline void lastBarOnlyNameValue(SCSubgraphRef sg, bool on){
  sg.DrawStyle = on ? DRAWSTYLE_SUBGRAPH_NAME_AND_VALUE_LABELS_ONLY : DRAWSTYLE_IGNORE;
  sg.DrawZeros = 0;
}

} // namespace du
