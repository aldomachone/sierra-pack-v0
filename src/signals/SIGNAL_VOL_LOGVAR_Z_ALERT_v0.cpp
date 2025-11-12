// ============================================================================
// Pack v0 — Signals v2
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_SIGNAL_VOL_LOGVAR_Z_ALERT_v2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SIGNAL_VOL_LOGVAR_Z_ALERT_v2"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name="LV z"; sc.Subgraph[1].DrawStyle=DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sc.Subgraph[1].DrawZeros=false;
    sc.Input[0].Name="01. ID étude logvar"; sc.Input[0].SetStudyID(0);
    sc.Input[1].Name="02. SG zscore"; sc.Input[1].SetInt(1);
    sc.Input[2].Name="03. Seuil |z|"; sc.Input[2].SetFloat(2.0f);
    return;
  }
  int id=sc.Input[0].GetStudyID(); int sg=sc.Input[1].GetInt(); if(id<=0) return;
  SCFloatArrayRef Z; sc.GetStudyArrayUsingID(id,sg,Z); int i=sc.ArraySize-1; if(!Z.Arrays) return;
  sc.Subgraph[1][i]=(fabs(Z[i])>=sc.Input[2].GetFloat()? 2.5f: 0.0f);
}

