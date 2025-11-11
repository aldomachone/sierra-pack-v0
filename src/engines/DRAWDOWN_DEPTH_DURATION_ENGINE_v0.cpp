// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #13)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DRAWDOWN_DEPTH_DURATION_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DRAWDOWN_DEPTH_DURATION_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";\n    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[1].DrawZeros = false;\n    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[2].Name = "SG02";\n    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[2].DrawZeros = false;\n    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[3].Name = "SG03";\n    sc.Subgraph[3].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[3].DrawZeros = false;\n    sc.Subgraph[3].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[4].Name = "SG04";\n    sc.Subgraph[4].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[4].DrawZeros = false;\n    sc.Subgraph[4].DisplayAsMainPriceGraphValue = 0;
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<1) return; int idx=sc.ArraySize-1;
  static double peak=-1e300; static int start=0;
  if(sc.IsFullRecalculation){ peak=-1e300; start=idx; }
  if(sc.Close[idx]>peak){ peak=sc.Close[idx]; start=idx; }
  double depth = peak - sc.Close[idx];
  int dur = idx - start;
  sc.Subgraph[1][idx]=depth; sc.Subgraph[2][idx]=dur;
}
