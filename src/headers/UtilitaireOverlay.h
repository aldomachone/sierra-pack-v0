#pragma once
#include "sierrachart.h"

namespace du {

inline void 	markerTransparent		(SCSubgraphRef sg, int size)												{
  sg.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
  sg.LineWidth = size;
  sg.DrawZeros = 0;
  sg.DisplayAsMainPriceGraphValue = 0;
  sg.PrimaryColor   = RGB(0,255,0);
  sg.SecondaryColor = RGB(255,0,0);
}

// Labels “dernière barre” uniquement
inline void 	lastBarOnlyNameValue	(SCSubgraphRef sg, bool on)													{
  sg.DrawStyle = on ? DRAWSTYLE_SUBGRAPH_NAME_AND_VALUE_LABELS_ONLY : DRAWSTYLE_IGNORE;
  sg.DrawZeros = 0;
}

inline void 	ovSetMarker				(SCStudyInterfaceRef sc, SCSubgraphRef sg, int idx, double y, float size)	{
  sg[idx] = (float)y; sg.DataColor[idx] = RGB(255,255,255); sg.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE; sg.LineWidth=size;
}
inline void 	ovLastBarOnly			(SCStudyInterfaceRef, SCSubgraphRef, bool /*visible*/)						{}
inline void 	ovZeroDoesNotMoveScale	(SCStudyInterfaceRef, SCSubgraphRef)										{}
inline void 	ovRightAlignLabels		(SCStudyInterfaceRef, const char* /*sgName*/, bool /*showValue*/)			{}
inline void 	ovDrawUnderMain			(SCStudyInterfaceRef sc, bool yes)											{ sc.DrawStudyUnderneathMainPriceGraph = yes?1:0; }

} // namespace du
