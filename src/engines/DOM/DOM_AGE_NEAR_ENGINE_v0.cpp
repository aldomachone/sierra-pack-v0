// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_AGE_NEAR_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_AGE_NEAR_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Subgraph[7].Name = "SG07";
    sc.Subgraph[7].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[7].DrawZeros = false;
    sc.Subgraph[7].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[8].Name = "SG08";
    sc.Subgraph[8].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[8].DrawZeros = false;
    sc.Subgraph[8].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Niveaux suivis"; sc.Input[0].SetInt(4);
    sc.DrawZeros=false; return;
  }
  int K=sc.Input[0].GetInt(); if(K>8) K=8;
  static double lastB[8]={0}, lastA[8]={0}; static int ageB[8]={0}, ageA[8]={0};
  s_MarketDepthEntry md{};
  for(int i=0;i<K;++i){
    double qb=0, qa=0;
    if(i<sc.GetBidMarketDepthNumberOfLevels()){ sc.GetBidMarketDepthEntryAtLevel(md,i); qb=md.Quantity; }
    if(i<sc.GetAskMarketDepthNumberOfLevels()){ sc.GetAskMarketDepthEntryAtLevel(md,i); qa=md.Quantity; }
    if(qb!=lastB[i]){ lastB[i]=qb; ageB[i]=0; } else { ++ageB[i]; }
    if(qa!=lastA[i]){ lastA[i]=qa; ageA[i]=0; } else { ++ageA[i]; }
    if(i<4){ sc.Subgraph[i+1][sc.ArraySize-1]=ageB[i]; } // SG1..SG4 = Bid ages
  }
  // SG5..SG8 = Ask ages if available
  for(int i=0;i<min(K,4);++i){ sc.Subgraph[i+5][sc.ArraySize-1]=ageA[i]; }
}
