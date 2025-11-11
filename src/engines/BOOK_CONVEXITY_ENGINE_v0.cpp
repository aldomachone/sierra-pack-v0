// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats #12)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_BOOK_CONVEXITY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="BOOK_CONVEXITY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Scanner niveaux"; sc.Input[0].SetInt(20);
    sc.DrawZeros=false; return;
  }
  int L=sc.Input[0].GetInt(); s_MarketDepthEntry md{};
  auto quad=[&](bool bid){ double x=0,y=0,x2=0,x3=0,x4=0, xy=0, x2y=0; int n=0;
    int N = bid? sc.GetBidMarketDepthNumberOfLevels(): sc.GetAskMarketDepthNumberOfLevels();
    for(int i=1;i<=N && i<=L; ++i){ if(bid) sc.GetBidMarketDepthEntryAtLevel(md,i-1); else sc.GetAskMarketDepthEntryAtLevel(md,i-1);
      if(md.Quantity<=0) continue; double u=log((double)i); double v=log((double)md.Quantity);
      x+=u; y+=v; x2+=u*u; x3+=u*u*u; x4+=u*u*u*u; xy+=u*v; x2y+=u*u*v; ++n; }
    if(n<3) return 0.0;
    double D = (x2*x2 - x*x3)*n + 2*x*x2*x - n*x4 - x*x*x;
    if(D==0) return 0.0;
    double a2 = ((x2*y - x*xy)*n + x*x*y - n*x2y - x2*x*y + x*xy)/D; // rough curvature proxy
    return a2; };
  double cv = quad(true) + quad(false);
  sc.Subgraph[1][sc.ArraySize-1]=cv;
}
