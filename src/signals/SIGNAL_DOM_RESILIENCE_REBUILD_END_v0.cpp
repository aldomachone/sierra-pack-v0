// ============================================================================
// Pack v0 — Signals v2
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_DOM_RESILIENCE_REBUILD_END_v2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_DOM_RESILIENCE_REBUILD_END_v2"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="RebuildEnd"; sc.Subgraph[1].DrawStyle=DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false;
    sc.Input[0].Name="01. ID étude resilience"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG ratioB"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. SG ratioA"; sc.Input[2].SetInt(2);
    sc.Input[3].Name="04. SG rebuilding flag"; sc.Input[3].SetInt(4);
    return;
  }
  int id=sc.Input[0].GetStudyID(); if(id<=0) return;
  SCFloatArrayRef RB,RA,FL; sc.GetStudyArrayUsingID(id,sc.Input[1].GetInt(),RB); sc.GetStudyArrayUsingID(id,sc.Input[2].GetInt(),RA); sc.GetStudyArrayUsingID(id,sc.Input[3].GetInt(),FL);
  int i=sc.ArraySize-1; if(!RB.Arrays||!RA.Arrays||!FL.Arrays||i<1) return;
  bool end = (FL[i-1]>0.5 && FL[i]<0.5) && (RB[i]>=1.0 && RA[i]>=1.0);
  sc.Subgraph[1][i]= end? 3.0f: 0.0f;
}

