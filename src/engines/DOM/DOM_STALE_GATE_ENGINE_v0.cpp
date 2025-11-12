// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_STALE_GATE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited=sc.GetPersistentInt(1); double& last=sc.GetPersistentDouble(2); double& lastT=sc.GetPersistentDouble(3);
  if(sc.SetDefaults){
    sc.GraphName="DOM_STALE_GATE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Δmin qty"; sc.Input[0].SetInt(1);
    sc.Input[1].Name="02. Délai max ms"; sc.Input[1].SetInt(200);
    sc.DrawZeros=false; return;
  }
  if(!inited||sc.IsFullRecalculation){ inited=1; last=0; lastT=0; }
  s_MarketDepthEntry md{}; double sum=0;
  int N=20; for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<N;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); sum+=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); sum+=md.Quantity; }
  double now=sc.CurrentSystemDateTime;
  double ageMs=(lastT>0? (now-lastT)*86400000.0:0.0);
  int ok=1; if(fabs(sum-last)<sc.Input[0].GetInt() && ageMs>sc.Input[1].GetInt()) ok=0;
  last=sum; lastT=now;
  sc.Subgraph[1][sc.ArraySize-1]=ok; sc.Subgraph[2][sc.ArraySize-1]=ageMs;
}
