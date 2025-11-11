// ============================================================================
// Pack v0 — Engines v0 (lot Extra-HFT #15)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_MICROPRICE_DRIFT_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="MICROPRICE_DRIFT_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
  s_MarketDepthEntry md{}; sc.GetBidMarketDepthEntryAtLevel(md,0); double qb=md.Quantity; double pb=md.Price; sc.GetAskMarketDepthEntryAtLevel(md,0); double qa=md.Quantity; double pa=md.Price;
  double mp = (qa>0||qb>0? (pa*qb + pb*qa)/(qa+qb) : 0.5*(pa+pb));
  int W=sc.Input[0].GetInt(); int i=sc.ArraySize-1; int s=(i-W>0? i-W:0); double m=0; int n=0; for(int j=s;j<=i;++j){ m+=sc.Close[j]; ++n; } m/= (n>0? n:1);
  sc.Subgraph[1][i]=mp - m;
}
