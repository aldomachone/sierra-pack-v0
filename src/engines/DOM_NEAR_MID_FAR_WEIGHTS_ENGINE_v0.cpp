// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT5: DOM Dynamics Advanced)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_NEAR_MID_FAR_WEIGHTS_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_NEAR_MID_FAR_WEIGHTS_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1;
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
    sc.Input[0].Name="01. Near"; sc.Input[0].SetInt(4);
    sc.Input[1].Name="02. Mid"; sc.Input[1].SetInt(8);
    sc.Input[2].Name="03. Far"; sc.Input[2].SetInt(16);
    sc.DrawZeros=false; return;
  }
  int n=sc.Input[0].GetInt(), m=sc.Input[1].GetInt(), f=sc.Input[2].GetInt();
  s_MarketDepthEntry md{}; double bn=0,an=0,bm=0,am=0,bf=0,af=0;
  for(int i=0;i<n && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); bn+=md.Quantity; }
  for(int i=0;i<n && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); an+=md.Quantity; }
  for(int i=n;i<m && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); bm+=md.Quantity; }
  for(int i=n;i<m && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); am+=md.Quantity; }
  for(int i=m;i<f && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); bf+=md.Quantity; }
  for(int i=m;i<f && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); af+=md.Quantity; }
  sc.Subgraph[1][sc.ArraySize-1]=bn-an; sc.Subgraph[2][sc.ArraySize-1]=bm-am; sc.Subgraph[3][sc.ArraySize-1]=bf-af;
}
