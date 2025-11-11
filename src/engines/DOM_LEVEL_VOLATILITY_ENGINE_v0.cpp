// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT5: DOM Dynamics Advanced)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_LEVEL_VOLATILITY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_LEVEL_VOLATILITY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Niveaux"; sc.Input[0].SetInt(8);
    sc.DrawZeros=false; return;
  }
  int L=sc.Input[0].GetInt(); s_MarketDepthEntry md{}; static double prev[64]={0}; double acc=0; int n=0;
  for(int i=0;i<L && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); acc+=fabs(md.Quantity - prev[i]); prev[i]=md.Quantity; ++n; }
  for(int i=0;i<L && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); acc+=fabs(md.Quantity - prev[i+L]); prev[i+L]=md.Quantity; ++n; }
  sc.Subgraph[1][sc.ArraySize-1]=(n>0? acc/n:0.0);
}
