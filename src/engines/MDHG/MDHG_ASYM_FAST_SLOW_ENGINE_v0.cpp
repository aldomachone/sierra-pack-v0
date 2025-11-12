// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #4)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_MDHG_ASYM_FAST_SLOW_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="MDHG_ASYM_FAST_SLOW_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Near niveaux"; sc.Input[0].SetInt(12);
    sc.Input[1].Name="02. EMA% fast"; sc.Input[1].SetInt(90);
    sc.Input[2].Name="03. EMA% slow"; sc.Input[2].SetInt(70);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); double af=sc.Input[1].GetInt()/100.0, as=sc.Input[2].GetInt()/100.0;
  s_MarketDepthEntry md{}; double b=0,a=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<N;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity; }
  double tot=b+a; double x=(tot>0? (b-a)/tot:0.0);
  static double ef=0, es=0; ef=af*x+(1-af)*ef; es=as*x+(1-as)*es;
  sc.Subgraph[1][sc.ArraySize-1]=ef; sc.Subgraph[2][sc.ArraySize-1]=es; sc.Subgraph[3][sc.ArraySize-1]=ef-es;
}
