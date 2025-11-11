// ============================================================================
// Pack v0 — Signals v1
// Markers only. Inputs FR 01..N contigus. ValueFormat=26. No StudyDescription.
// Each signal reads engine subgraphs via Study Reference ID + SG indices.
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_VOL_BNS_JUMP_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_VOL_BNS_JUMP_v1"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="Jump"; sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false; sc.Subgraph[1].PrimaryColor = RGB(255,0,255);
    sc.Subgraph[1].DisplayAsMainPriceGraphValue=0;
    sc.Input[0].Name="01. ID étude source"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG stat"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. Seuil"; sc.Input[2].SetFloat(0.0f);
    return;
  }
  int src=sc.Input[0].GetStudyID(); int sg=sc.Input[1].GetInt(); float thr=sc.Input[2].GetFloat();
  if(src<=0 || sg<1) return;
  SCFloatArrayRef J; sc.GetStudyArrayUsingID(src,sg,J);
  int i=sc.ArraySize-1; if(!J.Arrays) return;
  sc.Subgraph[1][i]=(J[i]>thr? 3.0f: 0.0f);
}

