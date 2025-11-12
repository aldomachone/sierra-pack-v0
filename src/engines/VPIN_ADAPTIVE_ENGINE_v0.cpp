// ============================================================================
// Pack v0 — Engines v0 (lot Extra-HFT #15)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VPIN_ADAPTIVE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="VPIN_ADAPTIVE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[3].Name = "SG03";
    sc.Subgraph[3].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[3].DrawZeros = false;
    sc.Subgraph[3].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[4].Name = "SG04";
    sc.Subgraph[4].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[4].DrawZeros = false;
    sc.Subgraph[4].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Seau volume base"; sc.Input[0].SetInt(1000);
    sc.Input[1].Name="02. Fenêtre seaux"; sc.Input[1].SetInt(20);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<2 || sc.ArraySize==0) return;
  static double bucket=0, buy=0, sell=0; static double ring[256]; static int rp=0,rn=0;
  int V=sc.Input[0].GetInt(); int W=sc.Input[1].GetInt(); if(W>256) W=256;
  double last=0; for(int i=0;i<ts.Size(); ++i){ const auto& e=ts[i]; if(e.Type!=SC_TS_TRADES) continue; int sgn=(last>0? (e.Price>last? +1:(e.Price<last? -1:0)):0); last=e.Price; if(sgn>=0) buy+=e.Volume; else sell+=e.Volume; bucket+=e.Volume;
    if(bucket>=V){ double vpin=fabs(buy-sell)/(buy+sell+1e-9); ring[rp]=vpin; rp=(rp+1)%W; if(rn<W) ++rn; bucket=0; buy=0; sell=0; } }
  double acc=0; for(int i=0;i<rn;++i) acc+=ring[i]; double vp=(rn>0? acc/rn:0.0); sc.Subgraph[1][sc.ArraySize-1]=vp;
}
