// File: _PACK_v0.cpp
#include "Pack_v0.h"
SCDLLName("PACK_v0")

SCSFExport scsf__PACK_V0_INFO(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphRegion = 0; sc.AutoLoop = 0; sc.UpdateAlways = 0; sc.ValueFormat = 26; sc.FreeDLL = 0;
    sc.Subgraph[0].Name = "Pack_v0 Version";
    sc.Subgraph[0].DrawStyle = DRAWSTYLE_SUBGRAPH_NAME_AND_VALUE_LABELS_ONLY;
    sc.Subgraph[0].DrawZeros = 0;
    sc.DisplayStudyInputValues = false;
    return;
  }
  if(sc.Index==0){
    sc.Subgraph[0][sc.Index] = 1.0f; // placeholder version
  }
}
