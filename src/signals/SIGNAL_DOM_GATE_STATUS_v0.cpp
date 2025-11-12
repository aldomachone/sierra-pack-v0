// ============================================================================
// Pack v0 — Signals v1
// Markers only. Inputs FR 01..N contigus. ValueFormat=26. No StudyDescription.
// Each signal reads engine subgraphs via Study Reference ID + SG indices.
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_DOM_GATE_STATUS_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_DOM_GATE_STATUS_v1"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="Gate"; sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false; sc.Subgraph[1].PrimaryColor = RGB(255,255,0);
    sc.Subgraph[1].DisplayAsMainPriceGraphValue=0;
    sc.Input[0].Name="01. ID étude source"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG Gate"; sc.Input[1].SetInt(1);
    return;
  }
  int src=sc.Input[0].GetStudyID(); int sg=sc.Input[1].GetInt(); if(src<=0 || sg<1) return;
  SCFloatArrayRef G; sc.GetStudyArrayUsingID(src,sg,G);
  int i=sc.ArraySize-1; if(!G.Arrays) return;
  sc.Subgraph[1][i] = (G[i]>0.5? 2.0f: 0.0f);
}

