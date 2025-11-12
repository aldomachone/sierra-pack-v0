// ============================================================================
// Pack v0 — Signals v2
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_PAIRS_ADF_RHO_BREAK_v2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_PAIRS_ADF_RHO_BREAK_v2"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="ADFbreak"; sc.Subgraph[1].DrawStyle=DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false;
    sc.Input[0].Name="01. ID étude ADF proxy"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG rho"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. Seuil rho"; sc.Input[2].SetFloat(0.9f);
    return;
  }
  int id=sc.Input[0].GetStudyID(); int sg=sc.Input[1].GetInt(); if(id<=0) return;
  SCFloatArrayRef R; sc.GetStudyArrayUsingID(id,sg,R); int i=sc.ArraySize-1; if(!R.Arrays||i<1) return;
  bool brk = (R[i-1]>=sc.Input[2].GetFloat() && R[i]<sc.Input[2].GetFloat());
  sc.Subgraph[1][i]= brk? 2.5f: 0.0f;
}

