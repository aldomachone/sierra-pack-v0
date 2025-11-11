// ============================================================================
// Pack v0 — Signals v1
// Markers only. Inputs FR 01..N contigus. ValueFormat=26. No StudyDescription.
// Each signal reads engine subgraphs via Study Reference ID + SG indices.
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_CTX_FVG_MARK_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_CTX_FVG_MARK_v1"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="FVG"; sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false; sc.Subgraph[1].PrimaryColor = RGB(128,128,255);
    sc.Subgraph[1].DisplayAsMainPriceGraphValue=0;
    sc.Input[0].Name="01. ID étude source"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG score"; sc.Input[1].SetInt(1);
    return;
  }
  int src=sc.Input[0].GetStudyID(); int sg=sc.Input[1].GetInt(); if(src<=0) return;
  SCFloatArrayRef S; sc.GetStudyArrayUsingID(src,sg,S);
  int i=sc.ArraySize-1; if(!S.Arrays) return;
  if(fabs(S[i])>0.5) sc.Subgraph[1][i]=2.0f; else sc.Subgraph[1][i]=0.0f;
}

