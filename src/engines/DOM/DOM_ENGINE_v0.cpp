// ============================================================================
// Pack v0 — Engine de base (v0)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited = sc.GetPersistentInt(1);
  if (sc.SetDefaults)
  {
    sc.GraphName = "DOM_ENGINE_v0";
    sc.AutoLoop = 0; sc.UpdateAlways = 1; sc.GraphRegion = 0; sc.ValueFormat = 26; sc.FreeDLL = 0;
    sc.UsesMarketDepthData = 1;
    for (int i=1;i<=24;++i){ sc.Subgraph[i].Name = "SG%02d"; sc.Subgraph[i].DrawStyle = DRAWSTYLE_IGNORE; sc.Subgraph[i].DrawZeros=false; sc.Subgraph[i].DisplayAsMainPriceGraphValue=0; }
    sc.Input[0].Name="01. Niveaux Near"; sc.Input[0].SetInt(10); sc.Input[0].SetIntLimits(1,60);
    sc.Input[1].Name="02. Niveaux Far";  sc.Input[1].SetInt(30); sc.Input[1].SetIntLimits(1,60);
    sc.DrawZeros=false; return;
  }
  if (!inited || sc.IsFullRecalculation) inited=1;
  if (sc.TickSize<=0 || sc.ArraySize<=0) return;
  // Placeholder headless exposant deux métriques simples
  s_MarketDepthEntry md{}; int nN=sc.Input[0].GetInt(); int nF=sc.Input[1].GetInt();
  double b=0,a=0; int nb=sc.GetBidMarketDepthNumberOfLevels(), na=sc.GetAskMarketDepthNumberOfLevels();
  for(int i=0;i<nb && i<nN;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<na && i<nN;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity; }
  int idx=sc.ArraySize-1; sc.Subgraph[1][idx]=b; sc.Subgraph[2][idx]=-a;
}
