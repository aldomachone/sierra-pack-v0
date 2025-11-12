// ============================================================================
// Pack v0 — Signals v1
// Markers only. Inputs FR 01..N contigus. ValueFormat=26. No StudyDescription.
// Each signal reads engine subgraphs via Study Reference ID + SG indices.
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_CTX_CONFLUENCE_NEAR_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_CTX_CONFLUENCE_NEAR_v1"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="Confluence"; sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false; sc.Subgraph[1].PrimaryColor = RGB(255,215,0);
    sc.Subgraph[1].DisplayAsMainPriceGraphValue=0;
    sc.Input[0].Name="01. ID étude source"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG score zone"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. Seuil zone"; sc.Input[2].SetFloat(1.0f);
    return;
  }
  int src=sc.Input[0].GetStudyID(), sg=sc.Input[1].GetInt(); float thr=sc.Input[2].GetFloat();
  if(src<=0 || sg<1) return;
  SCFloatArrayRef Z; sc.GetStudyArrayUsingID(src,sg,Z);
  int i=sc.ArraySize-1; if(!Z.Arrays) return;
  sc.Subgraph[1][i]=(Z[i]>=thr? 3.0f: 0.0f);
}

