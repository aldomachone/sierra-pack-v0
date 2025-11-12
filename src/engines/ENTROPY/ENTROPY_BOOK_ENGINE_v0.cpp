// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #13)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ENTROPY_BOOK_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ENTROPY_BOOK_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";\n    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[1].DrawZeros = false;\n    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[2].Name = "SG02";\n    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[2].DrawZeros = false;\n    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[3].Name = "SG03";\n    sc.Subgraph[3].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[3].DrawZeros = false;\n    sc.Subgraph[3].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[4].Name = "SG04";\n    sc.Subgraph[4].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[4].DrawZeros = false;\n    sc.Subgraph[4].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Niveaux near"; sc.Input[0].SetInt(8);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); s_MarketDepthEntry md{}; double sum=0; static double v[128]; int n=0;
  for(int i=0;i<N; ++i){
    double qb=0, qa=0;
    if(i<sc.GetBidMarketDepthNumberOfLevels()){ sc.GetBidMarketDepthEntryAtLevel(md,i); qb=md.Quantity; }
    if(i<sc.GetAskMarketDepthNumberOfLevels()){ sc.GetAskMarketDepthEntryAtLevel(md,i); qa=md.Quantity; }
    v[n++]=qb; v[n++]=qa; sum += qb+qa;
  }
  if(sum<=0){ sc.Subgraph[1][sc.ArraySize-1]=0; return; }
  double H=0; for(int i=0;i<n; ++i){ double p=v[i]/sum; if(p>0) H -= p*log(p); }
  sc.Subgraph[1][sc.ArraySize-1]=H;
}
