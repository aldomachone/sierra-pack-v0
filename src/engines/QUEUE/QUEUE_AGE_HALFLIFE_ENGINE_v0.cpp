// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #14)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_QUEUE_AGE_HALFLIFE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="QUEUE_AGE_HALFLIFE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. EMA %"; sc.Input[0].SetInt(85);
    sc.DrawZeros=false; return;
  }
  static double lastQ=0; static double age=0; static double ema=0;
  double a=sc.Input[0].GetInt()/100.0;
  if(sc.GetBidMarketDepthNumberOfLevels()>0 && sc.GetAskMarketDepthNumberOfLevels()>0){
    s_MarketDepthEntry md{}; sc.GetBidMarketDepthEntryAtLevel(md,0); double qb=md.Quantity; sc.GetAskMarketDepthEntryAtLevel(md,0); double qa=md.Quantity;
    double q=qb+qa;
    if(lastQ==0) lastQ=q;
    if(q>=lastQ) age=0; else age+=1;
    lastQ=q;
    ema = a*age + (1-a)*ema;
    sc.Subgraph[1][sc.ArraySize-1]=ema;
  }
}
