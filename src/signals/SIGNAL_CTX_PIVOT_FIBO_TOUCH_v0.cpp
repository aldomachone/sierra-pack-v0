// ============================================================================
// Pack v0 — Signals v2
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_CTX_PIVOT_FIBO_TOUCH_v2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_CTX_PIVOT_FIBO_TOUCH_v2"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="Touch"; sc.Subgraph[1].DrawStyle=DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false;
    sc.Input[0].Name="01. ID étude confl."; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG distance ticks"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. Seuil ticks"; sc.Input[2].SetInt(2);
    return;
  }
  int id=sc.Input[0].GetStudyID(); int sg=sc.Input[1].GetInt(); if(id<=0) return;
  SCFloatArrayRef D; sc.GetStudyArrayUsingID(id,sg,D); int i=sc.ArraySize-1; if(!D.Arrays) return;
  sc.Subgraph[1][i]=(D[i]<=sc.Input[2].GetInt()? 2.5f: 0.0f);
}

