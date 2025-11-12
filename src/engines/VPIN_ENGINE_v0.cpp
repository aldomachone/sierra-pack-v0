// ============================================================================
// Pack v0 — Engines v0 (lot HFT formulas #11)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VPIN_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="VPIN_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Seau volume"; sc.Input[0].SetInt(1000);
    sc.Input[1].Name="02. Fenêtre seaux"; sc.Input[1].SetInt(20);
    sc.DrawZeros=false; return;
  }
  static double bucketVol=0, buyV=0, sellV=0; static int buckets=0;
  static double ring[256]; static int rp=0, rN=0;
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<2) return;
  int V=sc.Input[0].GetInt(); int W=sc.Input[1].GetInt(); if(W>256) W=256;
  double last=0;
  // consume recent T&S incrementally
  static int lastSz=0; int start=(lastSz>0? lastSz-1:0); lastSz=ts.Size();
  for(int i=start;i<ts.Size(); ++i){
    const auto& e=ts[i]; if(e.Type!=SC_TS_TRADES) continue;
    int sgn=(last>0? (e.Price>last? +1:(e.Price<last? -1:0)):0); last=e.Price;
    if(sgn>=0) buyV += e.Volume; else sellV += e.Volume;
    bucketVol += e.Volume;
    if(bucketVol >= V){
      double vpin = fabs(buyV - sellV) / (buyV + sellV + 1e-9);
      ring[rp]=vpin; rp=(rp+1)%W; if(rN<W) ++rN;
      bucketVol=0; buyV=0; sellV=0; ++buckets;
    }
  }
  double acc=0; for(int i=0;i<rN;++i) acc+=ring[i];
  double vpinW = (rN>0? acc/rN : 0.0);
  if(sc.ArraySize>0) sc.Subgraph[1][sc.ArraySize-1]=vpinW;
}
