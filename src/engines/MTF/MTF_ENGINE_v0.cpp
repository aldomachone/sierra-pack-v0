// ============================================================================
// Pack v0 — Engines (v0) — complémentaires
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_MTF_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited = sc.GetPersistentInt(1);

  if (sc.SetDefaults)
  {
    sc.GraphName = "MTF_ENGINE_v0";
    sc.AutoLoop = 0;
    sc.UpdateAlways = 0;
    sc.GraphRegion = 0;
    sc.ValueFormat = 26;
    sc.FreeDLL = 0;

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
    sc.Subgraph[7].Name = "SG07";
    sc.Subgraph[7].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[7].DrawZeros = false;
    sc.Subgraph[7].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[8].Name = "SG08";
    sc.Subgraph[8].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[8].DrawZeros = false;
    sc.Subgraph[8].DisplayAsMainPriceGraphValue = 0;

    sc.Input[0].Name = "01. Période HTF (barres)";
    sc.Input[0].SetInt(5); sc.Input[0].SetIntLimits(1, 10000);

    sc.DrawZeros = false;
    return;
  }

  if (!inited || sc.IsFullRecalculation) { inited = 1; }
  if (sc.ArraySize <= 0) return;

  int N = sc.Input[0].GetInt();
  int start = sc.ArraySize - N; if (start < 0) start = 0;
  double sum=0.0; for(int i=start;i<sc.ArraySize;++i) sum += sc.Close[i];
  double avg = (sc.ArraySize-start>0? sum / (sc.ArraySize - start) : 0.0);
  sc.Subgraph[1][sc.ArraySize-1] = avg;
}
