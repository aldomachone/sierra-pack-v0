// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT4: Risk&Regime, Math&Finance)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VARIANCE_RATIO_LOWMIDHIGH_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="VARIANCE_RATIO_LOWMIDHIGH_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Pas large m"; sc.Input[0].SetInt(4);
    sc.Input[1].Name="02. Fenêtre bars"; sc.Input[1].SetInt(200);
    sc.DrawZeros=false; return;
  }
  int m=sc.Input[0].GetInt(); int idx=sc.ArraySize-1; int W=sc.Input[1].GetInt(); if(idx<2||m<2) return; int s=(idx-W>m? idx-W: m);
  double RV=0,RVm=0; for(int i=s;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; RV+=r*r; }
  for(int i=s;i<=idx;i+=m){ if(i-m<0) continue; double r=sc.Close[i]-sc.Close[i-m]; RVm+=r*r; }
  double ratio = (RVm/m>0? RV/(RVm/m):0.0);
  sc.Subgraph[1][idx]=ratio;
}
