// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT4: Risk&Regime, Math&Finance)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_MOMENTUM_TREND_RATIO_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="MOMENTUM_TREND_RATIO_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre courte"; sc.Input[0].SetInt(10);
    sc.Input[1].Name="02. Fenêtre longue"; sc.Input[1].SetInt(50);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; if(idx<2) return;
  auto mean=[&](int W){ int s=(idx-W>0? idx-W:0); double m=0; int n=0; for(int i=s;i<=idx;++i){ m+=sc.Close[i]; ++n; } return m/(double)(n>0?n:1); };
  double shortM = mean(sc.Input[0].GetInt());
  double longM  = mean(sc.Input[1].GetInt());
  sc.Subgraph[1][idx]=(longM!=0? shortM/longM:0.0);
}
