// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #10)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_FILLPROB_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="FILLPROB_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Subgraph[5].Name = "SG05";
    sc.Subgraph[5].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[5].DrawZeros = false;
    sc.Subgraph[5].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[6].Name = "SG06";
    sc.Subgraph[6].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[6].DrawZeros = false;
    sc.Subgraph[6].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Niveaux near"; sc.Input[0].SetInt(8);
    sc.Input[1].Name="02. Lambda distance"; sc.Input[1].SetFloat(6.0f);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); double lam=sc.Input[1].GetFloat(); if(lam<=0) lam=6.0;
  s_MarketDepthEntry md{}; double qb=0, qa=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<N; ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); qb += md.Quantity*exp(-(i)/(lam)); }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N; ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); qa += md.Quantity*exp(-(i)/(lam)); }
  double pBuy = (qa+qb>0? qa/(qa+qb):0.5);
  sc.Subgraph[1][sc.ArraySize-1]=pBuy;
}
