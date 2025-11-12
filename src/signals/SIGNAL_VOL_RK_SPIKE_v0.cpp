// ============================================================================
// Pack v0 — Signals v1
// Markers only. Inputs FR 01..N contigus. ValueFormat=26. No StudyDescription.
// Each signal reads engine subgraphs via Study Reference ID + SG indices.
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_VOL_RK_SPIKE_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_VOL_RK_SPIKE_v1"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="RK spike"; sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false; sc.Subgraph[1].PrimaryColor = RGB(0,255,255);
    sc.Subgraph[1].DisplayAsMainPriceGraphValue=0;
    sc.Input[0].Name="01. ID étude source"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG realized kernel"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. Quantile proxy"; sc.Input[2].SetFloat(1.5f);
    return;
  }
  int src=sc.Input[0].GetStudyID(), sg=sc.Input[1].GetInt(); float q=sc.Input[2].GetFloat();
  if(src<=0 || sg<1) return;
  SCFloatArrayRef R; sc.GetStudyArrayUsingID(src,sg,R);
  int i=sc.ArraySize-1; if(!R.Arrays || i<50) return;
  double m=0,v=0; int W=50; for(int k=i-W+1;k<=i;++k){ m+=R[k]; } m/=W; for(int k=i-W+1;k<=i;++k){ double d=R[k]-m; v+=d*d; } v/=W;
  double z=(v>0? (R[i]-m)/sqrt(v):0.0); sc.Subgraph[1][i]=(z>q? 2.5f: 0.0f);
}

