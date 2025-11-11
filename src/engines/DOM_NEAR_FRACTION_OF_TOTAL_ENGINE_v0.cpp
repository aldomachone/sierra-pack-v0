// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT5: DOM Dynamics Advanced)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_NEAR_FRACTION_OF_TOTAL_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_NEAR_FRACTION_OF_TOTAL_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1;
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
    sc.Input[0].Name="01. Near N"; sc.Input[0].SetInt(6);
    sc.Input[1].Name="02. Profondeur max"; sc.Input[1].SetInt(30);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); int L=sc.Input[1].GetInt(); s_MarketDepthEntry md{}; double bn=0,an=0, bt=0,at=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<L; ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); bt+=md.Quantity; if(i<N) bn+=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<L; ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); at+=md.Quantity; if(i<N) an+=md.Quantity; }
  sc.Subgraph[1][sc.ArraySize-1]=(bt>0? bn/bt:0.0); sc.Subgraph[2][sc.ArraySize-1]=(at>0? an/at:0.0);
}
