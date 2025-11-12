// ============================================================================
// Pack v0 — Signals v2
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_DOM_QUEUE_HAZARD_TURN_v2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_DOM_QUEUE_HAZARD_TURN_v2"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="Turn"; sc.Subgraph[1].DrawStyle=DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false;
    sc.Input[0].Name="01. ID étude queue-hazard"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG BidHaz"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. SG AskHaz"; sc.Input[2].SetInt(2);
    return;
  }
  int id=sc.Input[0].GetStudyID(); int sB=sc.Input[1].GetInt(); int sA=sc.Input[2].GetInt(); if(id<=0) return;
  SCFloatArrayRef B,A; sc.GetStudyArrayUsingID(id,sB,B); sc.GetStudyArrayUsingID(id,sA,A); int i=sc.ArraySize-1; if(!B.Arrays||!A.Arrays||i<2) return;
  double dB=B[i]-B[i-1], pB=B[i-1]-B[i-2]; double dA=A[i]-A[i-1], pA=A[i-1]-A[i-2];
  bool turn = ((dB>0 && pB<0) || (dB<0 && pB>0) || (dA>0 && pA<0) || (dA<0 && pA>0));
  sc.Subgraph[1][i]= turn? 2.5f: 0.0f;
}

