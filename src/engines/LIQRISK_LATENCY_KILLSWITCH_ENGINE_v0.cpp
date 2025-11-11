// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #5)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_LIQRISK_LATENCY_KILLSWITCH_ENGINE_v0(SCStudyInterfaceRef sc)
{
  double& lastSum=sc.GetPersistentDouble(1); double& lastT=sc.GetPersistentDouble(2);
  if(sc.SetDefaults){
    sc.GraphName="LIQRISK_LATENCY_KILLSWITCH_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Δmin qty"; sc.Input[0].SetInt(1);
    sc.Input[1].Name="02. Délai max ms"; sc.Input[1].SetInt(250);
    sc.DrawZeros=false; return;
  }
  s_MarketDepthEntry md{}; double sum=0; int N=20;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<N;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); sum+=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); sum+=md.Quantity; }
  double now=sc.CurrentSystemDateTime; if(lastT<=0) lastT=now;
  double ageMs=(now-lastT)*86400000.0; int flag=0;
  if(fabs(sum-lastSum)>=sc.Input[0].GetInt()){ lastSum=sum; lastT=now; ageMs=0; }
  if(ageMs>sc.Input[1].GetInt()) flag=1;
  sc.Subgraph[1][sc.ArraySize-1]=flag; sc.Subgraph[2][sc.ArraySize-1]=ageMs;
}
