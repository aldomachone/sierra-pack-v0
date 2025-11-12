// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #10)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_IMBAL_SKEW_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_IMBAL_SKEW_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Scanner niveaux"; sc.Input[0].SetInt(24);
    sc.DrawZeros=false; return;
  }
  int L=sc.Input[0].GetInt(); s_MarketDepthEntry md{};
  double m=0,s2=0,s3=0; int n=0;
  for(int i=0;i<L; ++i){
    double b=0,a=0;
    if(i<sc.GetBidMarketDepthNumberOfLevels()){ sc.GetBidMarketDepthEntryAtLevel(md,i); b=md.Quantity; }
    if(i<sc.GetAskMarketDepthNumberOfLevels()){ sc.GetAskMarketDepthEntryAtLevel(md,i); a=md.Quantity; }
    double x=(b+a>0? (b-a)/(b+a):0.0);
    ++n; double d=x-m; double m1=m+d/n; s3 += d*(x-m1)*(x-m1) - 3*s2*d/n + 2*d*d*d/n; s2 += d*(x-m1); m=m1;
  }
  double skew = (n>2 && s2!=0? (s3/n)/pow((s2/n),1.5):0.0);
  sc.Subgraph[1][sc.ArraySize-1]=skew;
}
