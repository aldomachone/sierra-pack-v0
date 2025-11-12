// ============================================================================
// Pack v0 — Signals v2
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_TAPE_COX_BURST_v2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_TAPE_COX_BURST_v2"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="Burst"; sc.Subgraph[1].DrawStyle=DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false;
    sc.Input[0].Name="01. ID étude Cox"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG lambda"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. z-quantile"; sc.Input[2].SetFloat(1.5f);
    return;
  }
  int id=sc.Input[0].GetStudyID(); int sg=sc.Input[1].GetInt(); if(id<=0) return;
  SCFloatArrayRef L; sc.GetStudyArrayUsingID(id,sg,L); int i=sc.ArraySize-1; if(!L.Arrays||i<40) return;
  int W=40; double m=0,v=0; for(int k=i-W+1;k<=i;++k){ m+=L[k]; } m/=W; for(int k=i-W+1;k<=i;++k){ double d=L[k]-m; v+=d*d; } v/=W;
  double z=(v>0? (L[i]-m)/sqrt(v):0.0); sc.Subgraph[1][i]=(z>sc.Input[2].GetFloat()? 2.5f: 0.0f);
}

