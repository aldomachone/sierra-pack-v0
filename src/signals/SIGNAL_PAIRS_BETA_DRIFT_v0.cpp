// ============================================================================
// Pack v0 — Signals v2
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_PAIRS_BETA_DRIFT_v2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_PAIRS_BETA_DRIFT_v2"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="BetaDrift"; sc.Subgraph[1].DrawStyle=DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false;
    sc.Input[0].Name="01. ID étude Beta"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG beta"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. Fenêtre moyenne"; sc.Input[2].SetInt(50);
    sc.Input[3].Name="04. Seuil |β - β̄|"; sc.Input[3].SetFloat(0.2f);
    return;
  }
  int id=sc.Input[0].GetStudyID(); int sg=sc.Input[1].GetInt(); if(id<=0) return;
  SCFloatArrayRef B; sc.GetStudyArrayUsingID(id,sg,B); int i=sc.ArraySize-1; if(!B.Arrays) return;
  int W=sc.Input[2].GetInt(); if(i<W) return; double m=0; for(int k=i-W+1;k<=i;++k){ m+=B[k]; } m/=W;
  sc.Subgraph[1][i]=(fabs(B[i]-m)>=sc.Input[3].GetFloat()? 2.5f: 0.0f);
}

