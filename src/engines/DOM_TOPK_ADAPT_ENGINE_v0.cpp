// ============================================================================
// Pack v0 — Engines v0 (DOM_CORE_V2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_TOPK_ADAPT_ENGINE_v2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_TOPK_ADAPT_ENGINE_v2"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1;
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
    sc.Input[0].Name="01. Profondeur max"; sc.Input[0].SetInt(30);
    sc.Input[1].Name="02. K"; sc.Input[1].SetInt(3);
    sc.DrawZeros=false; return;
  }
  int L=sc.Input[0].GetInt(); int K=sc.Input[1].GetInt(); if(K<1) K=1; if(K>32) K=32;
  s_MarketDepthEntry md{}; double bb[64], aa[64]; int nb=0,na=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<L && nb<64; ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); bb[nb++]=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<L && na<64; ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); aa[na++]=md.Quantity; }
  for(int i=0;i<nb-1;++i){ int mi=i; for(int j=i+1;j<nb;++j) if(bb[j]>bb[mi]) mi=j; double t=bb[i]; bb[i]=bb[mi]; bb[mi]=t; }
  for(int i=0;i<na-1;++i){ int mi=i; for(int j=i+1;j<na;++j) if(aa[j]>aa[mi]) mi=j; double t=aa[i]; aa[i]=aa[mi]; aa[mi]=t; }
  double sb=0,sa=0; for(int i=0;i<nb && i<K;++i) sb+=bb[i]; for(int i=0;i<na && i<K;++i) sa+=aa[i];
  sc.Subgraph[1][sc.ArraySize-1]=sb; sc.Subgraph[2][sc.ArraySize-1]=sa; sc.Subgraph[3][sc.ArraySize-1]=sb-sa;
}
