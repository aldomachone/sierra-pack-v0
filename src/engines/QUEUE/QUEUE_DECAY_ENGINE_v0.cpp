// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_QUEUE_DECAY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="QUEUE_DECAY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Niveaux near"; sc.Input[0].SetInt(8);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); static double prev[128]; s_MarketDepthEntry md{}; double dec=0,base=0;
  for(int i=0;i<N; ++i){ double q=0; if(i<sc.GetBidMarketDepthNumberOfLevels()){ sc.GetBidMarketDepthEntryAtLevel(md,i); q+=md.Quantity; } if(i<sc.GetAskMarketDepthNumberOfLevels()){ sc.GetAskMarketDepthEntryAtLevel(md,i); q+=md.Quantity; } if(prev[i]>0 && q<prev[i]) dec += prev[i]-q; base += prev[i]>0? prev[i]:0; prev[i]=q; }
  double rate = (base>0? dec/base : 0.0);
  sc.Subgraph[1][sc.ArraySize-1]=rate;
}
