// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats #12)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_CANCEL_ADD_RATIO_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="CANCEL_ADD_RATIO_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Niveaux near"; sc.Input[0].SetInt(10);
    sc.Input[1].Name="02. EMA %"; sc.Input[1].SetInt(85);
    sc.DrawZeros=false; return;
  }
  static double prevB[64], prevA[64]; int N=sc.Input[0].GetInt(); if(N>64) N=64;
  s_MarketDepthEntry md{}; double adds=0,canc=0;
  for(int i=0;i<N; ++i){
    double qb=0, qa=0;
    if(i<sc.GetBidMarketDepthNumberOfLevels()){ sc.GetBidMarketDepthEntryAtLevel(md,i); qb=md.Quantity; }
    if(i<sc.GetAskMarketDepthNumberOfLevels()){ sc.GetAskMarketDepthEntryAtLevel(md,i); qa=md.Quantity; }
    if(prevB[i]>0){ if(qb>prevB[i]) adds += qb-prevB[i]; else canc += prevB[i]-qb; }
    if(prevA[i]>0){ if(qa>prevA[i]) adds += qa-prevA[i]; else canc += prevA[i]-qa; }
    prevB[i]=qb; prevA[i]=qa;
  }
  double a=sc.Input[1].GetInt()/100.0; static double ema=0;
  double ratio = (adds+canc>0? canc/(adds+canc):0.0);
  ema = a*ratio+(1-a)*ema;
  sc.Subgraph[1][sc.ArraySize-1]=ema;
}
