// ============================================================================
// Pack v0 — Signals v2
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_DOM_LATENCY_SPREAD_OK_v2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_DOM_LATENCY_SPREAD_OK_v2"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="OK"; sc.Subgraph[1].DrawStyle=DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false;
    sc.Input[0].Name="01. ID étude gate"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG Gate"; sc.Input[1].SetInt(1);
    return;
  }
  int id=sc.Input[0].GetStudyID(); int sg=sc.Input[1].GetInt(); if(id<=0) return;
  SCFloatArrayRef G; sc.GetStudyArrayUsingID(id,sg,G); int i=sc.ArraySize-1; if(!G.Arrays) return;
  sc.Subgraph[1][i]= (G[i]>0.5? 2.0f: 0.0f);
}

