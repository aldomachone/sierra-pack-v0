// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #6)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_QUEUE_IMBALANCE_SLOPE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  double& prev=sc.GetPersistentDouble(1);
  if(sc.SetDefaults){
    sc.GraphName="DOM_QUEUE_IMBALANCE_SLOPE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Scanner niveaux"; sc.Input[0].SetInt(20);
    sc.Input[1].Name="02. EMA %"; sc.Input[1].SetInt(85);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); double a=sc.Input[1].GetInt()/100.0;
  s_MarketDepthEntry md{}; double sum=0, wsum=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<N;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); sum+=md.Quantity; wsum+=md.Quantity*(i+1); }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); sum-=md.Quantity; wsum-=md.Quantity*(i+1); }
  double slope = (sum!=0? wsum/sum:0.0);
  if(sc.IsFullRecalculation) prev=slope;
  double ds = slope-prev; prev=slope;
  static double ema=0; ema=a*ds+(1-a)*ema;
  sc.Subgraph[1][sc.ArraySize-1]=ema;
}
