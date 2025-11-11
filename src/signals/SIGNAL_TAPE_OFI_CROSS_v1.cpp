// ============================================================================
// Pack v0 — Signals v1
// Markers only. Inputs FR 01..N contigus. ValueFormat=26. No StudyDescription.
// Each signal reads engine subgraphs via Study Reference ID + SG indices.
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_TAPE_OFI_CROSS_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_TAPE_OFI_CROSS_v1"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="Cross"; sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false; sc.Subgraph[1].PrimaryColor = RGB(0,140,255);
    sc.Subgraph[1].DisplayAsMainPriceGraphValue=0;
    sc.Input[0].Name="01. ID étude source"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG court"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. SG long"; sc.Input[2].SetInt(2);
    return;
  }
  int src=sc.Input[0].GetStudyID(), s1=sc.Input[1].GetInt(), s2=sc.Input[2].GetInt();
  if(src<=0 || s1<1 || s2<1) return;
  SCFloatArrayRef A,B; sc.GetStudyArrayUsingID(src,s1,A); sc.GetStudyArrayUsingID(src,s2,B);
  int i=sc.ArraySize-1; if(!A.Arrays || !B.Arrays || i<1) return;
  double c=A[i], p=A[i-1]; double C=B[i], P=B[i-1];
  if(p<=P && c>P) sc.Subgraph[1][i]=3.0f; else if(p>=P && c<P) sc.Subgraph[1][i]=3.0f; else sc.Subgraph[1][i]=0.0f;
}

