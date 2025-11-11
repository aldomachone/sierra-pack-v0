// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #14)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DEPTH_GINI_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DEPTH_GINI_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); if(N>128) N=128;
  s_MarketDepthEntry md{}; static double v[256]; int n=0; double sum=0;
  for(int i=0;i<N; ++i){
    double qb=0, qa=0;
    if(i<sc.GetBidMarketDepthNumberOfLevels()){ sc.GetBidMarketDepthEntryAtLevel(md,i); qb=md.Quantity; }
    if(i<sc.GetAskMarketDepthNumberOfLevels()){ sc.GetAskMarketDepthEntryAtLevel(md,i); qa=md.Quantity; }
    v[n++]=qb; v[n++]=qa; sum+=qb+qa;
  }
  if(n<2 || sum<=0){ sc.Subgraph[1][sc.ArraySize-1]=0; return; }
  // sort ascending
  for(int i=0;i<n-1;++i){ int mi=i; for(int j=i+1;j<n;++j) if(v[j]<v[mi]) mi=j; double t=v[i]; v[i]=v[mi]; v[mi]=t; }
  double cum=0; double B=0; for(int i=0;i<n;++i){ cum+=v[i]; B += cum; }
  double G = ( (n+1.0)/n - 2.0*B/(n*sum) );
  sc.Subgraph[1][sc.ArraySize-1]=G;
}
