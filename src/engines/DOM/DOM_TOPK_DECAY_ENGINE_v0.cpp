// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_TOPK_DECAY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_TOPK_DECAY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Scanner niveaux"; sc.Input[0].SetInt(40);
    sc.Input[1].Name="02. K"; sc.Input[1].SetInt(3);
    sc.Input[2].Name="03. Décroissance %/lvl"; sc.Input[2].SetInt(90);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(), K=sc.Input[1].GetInt(); double w=sc.Input[2].GetInt()/100.0;
  s_MarketDepthEntry md{}; const int MAX=120; double buf[MAX]; int m=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<N;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); if(m<MAX) buf[m++]=md.Quantity*pow(w,i); }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); if(m<MAX) buf[m++]=md.Quantity*pow(w,i); }
  double acc=0; for(int k=0;k<K && k<m;++k){ int best=k; for(int j=k+1;j<m;++j) if(buf[j]>buf[best]) best=j; double tmp=buf[k]; buf[k]=buf[best]; buf[best]=tmp; acc+=buf[k]; }
  sc.Subgraph[1][sc.ArraySize-1]=acc;
}
