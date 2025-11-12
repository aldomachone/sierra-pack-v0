// ============================================================================
// Pack v0 — Signals v2
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_DOM_QUOTE_STUFFING_SPIKE_v2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_DOM_QUOTE_STUFFING_SPIKE_v2"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="Stuffing"; sc.Subgraph[1].DrawStyle=DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false;
    sc.Input[0].Name="01. ID étude stuffing"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG rate"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. z-quantile"; sc.Input[2].SetFloat(2.0f);
    return;
  }
  int id=sc.Input[0].GetStudyID(); int sg=sc.Input[1].GetInt(); if(id<=0||sg<1) return;
  SCFloatArrayRef R; sc.GetStudyArrayUsingID(id,sg,R); int i=sc.ArraySize-1; if(!R.Arrays||i<30) return;
  int W=30; double m=0,v=0; for(int k=i-W+1;k<=i;++k){ m+=R[k]; } m/=W; for(int k=i-W+1;k<=i;++k){ double d=R[k]-m; v+=d*d; } v/=W;
  double z=(v>0? (R[i]-m)/sqrt(v):0.0); sc.Subgraph[1][i]=(z>=sc.Input[2].GetFloat()? 3.0f: 0.0f);
}

