// ============================================================================
// Pack v0 — Signals v1
// Markers only. Inputs FR 01..N contigus. ValueFormat=26. No StudyDescription.
// Each signal reads engine subgraphs via Study Reference ID + SG indices.
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_TAPE_HAWKES_SPIKE_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_TAPE_HAWKES_SPIKE_v1"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="Spike"; sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false; sc.Subgraph[1].PrimaryColor = RGB(255,128,0);
    sc.Subgraph[1].DisplayAsMainPriceGraphValue=0;
    sc.Input[0].Name="01. ID étude source"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG intensité (ex: l1)"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. Seuil spike"; sc.Input[2].SetFloat(5.0f);
    return;
  }
  int src = sc.Input[0].GetStudyID(); int sg = sc.Input[1].GetInt(); float thr = sc.Input[2].GetFloat();
  if(src<=0 || sg<1) return;
  SCFloatArrayRef A; sc.GetStudyArrayUsingID(src, sg, A);
  int i=sc.ArraySize-1; if(!A.Arrays) return;
  double v=A[i]; if(v>thr){ sc.Subgraph[1][i]= (float)(v - thr + 1.0); } else { sc.Subgraph[1][i]=0.0f; }
}

