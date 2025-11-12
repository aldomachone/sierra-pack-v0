// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_SPREAD_ELASTICITY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SPREAD_ELASTICITY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(40);
    sc.DrawZeros=false; return;
  }
  if(sc.GetBidMarketDepthNumberOfLevels()<1 || sc.GetAskMarketDepthNumberOfLevels()<1) return;
  s_MarketDepthEntry md{}; sc.GetBidMarketDepthEntryAtLevel(md,0); double pb=md.Price; sc.GetAskMarketDepthEntryAtLevel(md,0); double pa=md.Price;
  double spr=(sc.TickSize>0? (pa-pb)/sc.TickSize:0.0);
  int W=sc.Input[0].GetInt(); int idx=sc.ArraySize-1; int s=(idx-W>1? idx-W:1);
  double rv=0; for(int i=s;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; rv+=r*r; } rv/=W;
  sc.Subgraph[1][idx] = (rv>0? spr/rv : 0.0);
}
