// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT5: DOM Dynamics Advanced)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_PULL_STACK_RATIO_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_PULL_STACK_RATIO_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Niveaux near"; sc.Input[0].SetInt(8);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); s_MarketDepthEntry md{}; static double pb=0,pa=0; static double bprev=0, aprev=0;
  double b=0,a=0; for(int i=0;i<N && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<N && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity; }
  if(sc.Index==0){ bprev=b; aprev=a; }
  double pull = (b<bprev? (bprev-b): (a<aprev? (aprev-a): 0.0));
  double stack = (b>bprev? (b-bprev): (a>aprev? (a-aprev): 0.0));
  bprev=b; aprev=a;
  sc.Subgraph[1][sc.ArraySize-1]=(stack>0? pull/stack:0.0);
}
