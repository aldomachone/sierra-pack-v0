// ============================================================================
// Pack v0 — Signals v2
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_TAPE_BVC_IMBAL_ALERT_v2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_TAPE_BVC_IMBAL_ALERT_v2"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="Imbalance"; sc.Subgraph[1].DrawStyle=DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false;
    sc.Input[0].Name="01. ID étude BVC"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG buy"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. SG sell"; sc.Input[2].SetInt(2);
    sc.Input[3].Name="04. Seuil |buy-sell|"; sc.Input[3].SetFloat(200.0f);
    return;
  }
  int id=sc.Input[0].GetStudyID(); int sb=sc.Input[1].GetInt(); int ss=sc.Input[2].GetInt(); if(id<=0) return;
  SCFloatArrayRef B,S; sc.GetStudyArrayUsingID(id,sb,B); sc.GetStudyArrayUsingID(id,ss,S); int i=sc.ArraySize-1; if(!B.Arrays||!S.Arrays) return;
  double d=fabs(B[i]-S[i]); sc.Subgraph[1][i]=(d>=sc.Input[3].GetFloat()? 2.0f: 0.0f);
}

