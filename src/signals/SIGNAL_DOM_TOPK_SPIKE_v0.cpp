// ============================================================================
// Pack v0 — Signals v1
// Markers only. Inputs FR 01..N contigus. ValueFormat=26. No StudyDescription.
// Each signal reads engine subgraphs via Study Reference ID + SG indices.
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_DOM_TOPK_SPIKE_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_DOM_TOPK_SPIKE_v1"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="Spike"; sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false; sc.Subgraph[1].PrimaryColor = RGB(255,0,0);
    sc.Subgraph[1].DisplayAsMainPriceGraphValue=0;
    sc.Input[0].Name="01. ID étude source"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG BidTopK"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. SG AskTopK"; sc.Input[2].SetInt(2);
    sc.Input[3].Name="04. Seuil diff"; sc.Input[3].SetFloat(100.0f);
    return;
  }
  int src=sc.Input[0].GetStudyID(); int sB=sc.Input[1].GetInt(); int sA=sc.Input[2].GetInt(); float thr=sc.Input[3].GetFloat();
  if(src<=0) return;
  SCFloatArrayRef B,A; sc.GetStudyArrayUsingID(src,sB,B); sc.GetStudyArrayUsingID(src,sA,A);
  int i=sc.ArraySize-1; if(!B.Arrays || !A.Arrays) return;
  double d=B[i]-A[i]; sc.Subgraph[1][i]=(fabs(d)>thr? (float)(fabs(d)/thr*2.0):0.0f);
}

