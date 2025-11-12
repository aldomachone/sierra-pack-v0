// ============================================================================
// Pack v0 — Signals v1
// Markers only. Inputs FR 01..N contigus. ValueFormat=26. No StudyDescription.
// Each signal reads engine subgraphs via Study Reference ID + SG indices.
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_DOM_IMBAL_SHAPE_BIAS_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_DOM_IMBAL_SHAPE_BIAS_v1"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="Bias"; sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false; sc.Subgraph[1].PrimaryColor = RGB(0,150,255);
    sc.Subgraph[1].DisplayAsMainPriceGraphValue=0;
    sc.Input[0].Name="01. ID étude source"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG Near"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. SG Mid";  sc.Input[2].SetInt(2);
    sc.Input[3].Name="04. SG Far";  sc.Input[3].SetInt(3);
    sc.Input[4].Name="05. Seuil abs"; sc.Input[4].SetFloat(0.15f);
    return;
  }
  int src=sc.Input[0].GetStudyID(); if(src<=0) return;
  SCFloatArrayRef N,M,F; sc.GetStudyArrayUsingID(src,sc.Input[1].GetInt(),N); sc.GetStudyArrayUsingID(src,sc.Input[2].GetInt(),M); sc.GetStudyArrayUsingID(src,sc.Input[3].GetInt(),F);
  int i=sc.ArraySize-1; if(!N.Arrays || !M.Arrays || !F.Arrays) return;
  double bias = 0.5*N[i] + 0.35*M[i] + 0.15*F[i];
  sc.Subgraph[1][i] = (fabs(bias)>=sc.Input[4].GetFloat()? (float)(fabs(bias)*6.0f): 0.0f);
}

