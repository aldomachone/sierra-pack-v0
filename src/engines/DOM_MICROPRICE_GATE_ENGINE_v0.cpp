// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT5: DOM Dynamics Advanced)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_MICROPRICE_GATE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_MICROPRICE_GATE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Seuil microbias"; sc.Input[0].SetFloat(0.2f);
    sc.DrawZeros=false; return;
  }
  if(sc.GetBidMarketDepthNumberOfLevels()<1 || sc.GetAskMarketDepthNumberOfLevels()<1) return;
  s_MarketDepthEntry b,a; sc.GetBidMarketDepthEntryAtLevel(b,0); sc.GetAskMarketDepthEntryAtLevel(a,0);
  double qb=b.Quantity, qa=a.Quantity;
  double micro = (qb+qa>0? (qa-qb)/(qa+qb):0.0);
  sc.Subgraph[1][sc.ArraySize-1]= (fabs(micro)>=sc.Input[0].GetFloat()? 1.0: 0.0);
}
