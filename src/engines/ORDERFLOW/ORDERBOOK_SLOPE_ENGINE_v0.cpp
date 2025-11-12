// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ORDERBOOK_SLOPE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ORDERBOOK_SLOPE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Niveaux max"; sc.Input[0].SetInt(20);
    sc.DrawZeros=false; return;
  }
  int L=sc.Input[0].GetInt(); s_MarketDepthEntry md{};
  auto slope=[&](bool bid){ double x=0,y=0,xx=0,xy=0; int n=0; int N= bid? sc.GetBidMarketDepthNumberOfLevels(): sc.GetAskMarketDepthNumberOfLevels();
    for(int i=1;i<=N && i<=L; ++i){ if(bid) sc.GetBidMarketDepthEntryAtLevel(md,i-1); else sc.GetAskMarketDepthEntryAtLevel(md,i-1); double q=md.Quantity; if(q<=0) continue; double u=log((double)i); double v=log(q); x+=u; y+=v; xx+=u*u; xy+=u*v; ++n; } double den=n*xx-x*x; return (den!=0? (n*xy-x*y)/den:0.0); };
  sc.Subgraph[1][sc.ArraySize-1]=slope(true); sc.Subgraph[2][sc.ArraySize-1]=slope(false);
}
