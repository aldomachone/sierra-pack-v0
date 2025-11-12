// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #5)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_ORDER_AGE_PROXY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& age=sc.GetPersistentInt(1); double& pb=sc.GetPersistentDouble(2); double& pa=sc.GetPersistentDouble(3);
  if(sc.SetDefaults){
    sc.GraphName="DOM_ORDER_AGE_PROXY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.DrawZeros=false; return;
  }
  s_MarketDepthEntry md{}; double b0=0,a0=0;
  if(sc.GetBidMarketDepthNumberOfLevels()>0){ sc.GetBidMarketDepthEntryAtLevel(md,0); b0=md.Quantity; }
  if(sc.GetAskMarketDepthNumberOfLevels()>0){ sc.GetAskMarketDepthEntryAtLevel(md,0); a0=md.Quantity; }
  if(sc.IsFullRecalculation){ age=0; pb=b0; pa=a0; }
  if(b0!=pb || a0!=pa){ age=0; pb=b0; pa=a0; } else { ++age; }
  sc.Subgraph[1][sc.ArraySize-1]=age;
}
