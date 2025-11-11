// ============================================================================
// Pack v0 — Signals v1
// Markers only. Inputs FR 01..N contigus. ValueFormat=26. No StudyDescription.
// Each signal reads engine subgraphs via Study Reference ID + SG indices.
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_TAPE_RUNS_STATE_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_TAPE_RUNS_STATE_v1"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="State"; sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false; sc.Subgraph[1].PrimaryColor = RGB(0,200,0);
    sc.Subgraph[1].DisplayAsMainPriceGraphValue=0;
    sc.Input[0].Name="01. ID étude source"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG persistance"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. Seuil haut"; sc.Input[2].SetFloat(0.7f);
    sc.Input[3].Name="04. Seuil bas";  sc.Input[3].SetFloat(0.3f);
    return;
  }
  int src=sc.Input[0].GetStudyID(), sg=sc.Input[1].GetInt();
  if(src<=0 || sg<1) return;
  SCFloatArrayRef S; sc.GetStudyArrayUsingID(src,sg,S);
  int i=sc.ArraySize-1; if(!S.Arrays) return;
  double v=S[i]; if(v>=sc.Input[2].GetFloat() || v<=sc.Input[3].GetFloat()) sc.Subgraph[1][i]=2.5f; else sc.Subgraph[1][i]=0.0f;
}

