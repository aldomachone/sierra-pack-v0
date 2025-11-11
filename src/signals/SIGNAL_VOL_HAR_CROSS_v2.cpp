// ============================================================================
// Pack v0 — Signals v2
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_VOL_HAR_CROSS_v2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_VOL_HAR_CROSS_v2"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="HARcross"; sc.Subgraph[1].DrawStyle=DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false;
    sc.Input[0].Name="01. ID étude HAR"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG value"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. Fenêtre m"; sc.Input[2].SetInt(40);
    return;
  }
  int id=sc.Input[0].GetStudyID(); int sg=sc.Input[1].GetInt(); if(id<=0) return;
  SCFloatArrayRef H; sc.GetStudyArrayUsingID(id,sg,H); int i=sc.ArraySize-1; if(!H.Arrays) return;
  int W=sc.Input[2].GetInt(); if(i<W) return;
  double m=0; for(int k=i-W+1;k<=i;++k){ m+=H[k]; } m/=W;
  double prev=0; for(int k=i-W;k<i;++k){ prev+=H[k]; } prev/=W;
  bool cross = (prev<=m && H[i]>m) || (prev>=m && H[i]<m);
  sc.Subgraph[1][i]= cross? 2.5f: 0.0f;
}

