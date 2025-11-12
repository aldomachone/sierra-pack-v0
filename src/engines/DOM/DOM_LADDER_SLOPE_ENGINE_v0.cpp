// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT5: DOM Dynamics Advanced)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_LADDER_SLOPE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_LADDER_SLOPE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1;
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
    sc.Input[0].Name="01. Niveaux"; sc.Input[0].SetInt(12);
    sc.DrawZeros=false; return;
  }
  int L=sc.Input[0].GetInt(); s_MarketDepthEntry md{}; double x=0,y=0,xx=0,xy=0; int n=0;
  for(int i=0;i<L && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); double u=i+1; double v=md.Quantity; x+=u; y+=v; xx+=u*u; xy+=u*v; ++n; }
  double den=n*xx-x*x; double sa=(den!=0? (n*xy-x*y)/den:0.0);
  x=0;y=0;xx=0;xy=0;n=0;
  for(int i=0;i<L && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); double u=i+1; double v=md.Quantity; x+=u; y+=v; xx+=u*u; xy+=u*v; ++n; }
  den=n*xx-x*x; double sb=(den!=0? (n*xy-x*y)/den:0.0);
  sc.Subgraph[1][sc.ArraySize-1]=sb; sc.Subgraph[2][sc.ArraySize-1]=sa; sc.Subgraph[3][sc.ArraySize-1]=sb-sa;
}
