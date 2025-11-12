// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT5: DOM Dynamics Advanced)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_DEPTH_GRADIENT_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_DEPTH_GRADIENT_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1;
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
    sc.Input[0].Name="01. Max niveaux"; sc.Input[0].SetInt(20);
    sc.DrawZeros=false; return;
  }
  int L=sc.Input[0].GetInt(); s_MarketDepthEntry md{}; double gb=0,ga=0; int n=0;
  for(int i=1;i<L && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ s_MarketDepthEntry md0, md1; sc.GetBidMarketDepthEntryAtLevel(md0,i-1); sc.GetBidMarketDepthEntryAtLevel(md1,i); gb += (md1.Quantity - md0.Quantity); ++n; }
  for(int i=1;i<L && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ s_MarketDepthEntry md0, md1; sc.GetAskMarketDepthEntryAtLevel(md0,i-1); sc.GetAskMarketDepthEntryAtLevel(md1,i); ga += (md1.Quantity - md0.Quantity); }
  sc.Subgraph[1][sc.ArraySize-1]=(n>0? gb/n:0.0); sc.Subgraph[2][sc.ArraySize-1]=(n>0? ga/n:0.0);
}
