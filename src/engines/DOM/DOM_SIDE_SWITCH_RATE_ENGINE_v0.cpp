// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT5: DOM Dynamics Advanced)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_SIDE_SWITCH_RATE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_SIDE_SWITCH_RATE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.DrawZeros=false; return;
  }
  static int dir=0; int flips=0;
  if(sc.GetBidMarketDepthNumberOfLevels()>0 && sc.GetAskMarketDepthNumberOfLevels()>0){
    s_MarketDepthEntry b,a; sc.GetBidMarketDepthEntryAtLevel(b,0); sc.GetAskMarketDepthEntryAtLevel(a,0);
    int d = (a.Price>b.Price? +1: (a.Price<b.Price? -1: dir));
    if(dir==0) dir=d; else if(d!=dir){ dir=d; flips=1; }
  }
  sc.Subgraph[1][sc.ArraySize-1]=flips;
}
