// ============================================================================
// Pack v0 — Engines v0 (Pairs/Discovery)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PAIRS_BETA_INTRADAY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PAIRS_BETA_INTRADAY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(240);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<W) return;
  double mx=0,my=0; for(int i=idx-W+1;i<=idx;++i){ mx+=sc.Open[i]; my+=sc.Close[i]; } mx/=W; my/=W;
  double num=0, den=0; for(int i=idx-W+1;i<=idx;++i){ double ax=sc.Open[i]-mx; double ay=sc.Close[i]-my; num+=ax*ay; den+=ax*ax; }
  sc.Subgraph[1][idx]=(den>0? num/den:0.0);
}
