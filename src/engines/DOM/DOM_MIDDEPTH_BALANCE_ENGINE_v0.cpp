// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT5: DOM Dynamics Advanced)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_MIDDEPTH_BALANCE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_MIDDEPTH_BALANCE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Start"; sc.Input[0].SetInt(5);
    sc.Input[1].Name="02. Window"; sc.Input[1].SetInt(10);
    sc.DrawZeros=false; return;
  }
  int s= sc.Input[0].GetInt(); int w=sc.Input[1].GetInt();
  s_MarketDepthEntry md{}; double b=0,a=0;
  for(int i=s;i<s+w && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=s;i<s+w && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity; }
  sc.Subgraph[1][sc.ArraySize-1]=(b+a>0? (b-a)/(b+a):0.0);
}
