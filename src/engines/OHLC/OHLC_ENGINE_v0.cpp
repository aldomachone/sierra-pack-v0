// ============================================================================
// Pack v0 — Engine de base (v0)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_OHLC_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if (sc.SetDefaults)
  {
    sc.GraphName = "OHLC_ENGINE_v0";
    sc.AutoLoop = 0; sc.UpdateAlways = 0; sc.GraphRegion = 0; sc.ValueFormat = 26; sc.FreeDLL = 0;
    for (int i=1;i<=8;++i){ sc.Subgraph[i].Name = "SG%02d"; sc.Subgraph[i].DrawStyle = DRAWSTYLE_IGNORE; sc.Subgraph[i].DrawZeros=false; sc.Subgraph[i].DisplayAsMainPriceGraphValue=0; }
    sc.DrawZeros=false; return;
  }
  if (sc.ArraySize<=0) return; int idx=sc.ArraySize-1;
  sc.Subgraph[1][idx]=sc.Open[idx];
  sc.Subgraph[2][idx]=sc.High[idx];
  sc.Subgraph[3][idx]=sc.Low[idx];
  sc.Subgraph[4][idx]=sc.Close[idx];
}
