// ============================================================================
// Pack v0 — Engines (v0) — complémentaires
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_PRF_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited  = sc.GetPersistentInt(1);
  double& emaResp = sc.GetPersistentDouble(2);

  if (sc.SetDefaults)
  {
    sc.GraphName = "PRF_ENGINE_v0";
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

    sc.Input[0].Name = "01. Horizon barres";
    sc.Input[0].SetInt(5); sc.Input[0].SetIntLimits(1, 1000);

    sc.Input[1].Name = "02. EMA %";
    sc.Input[1].SetInt(80); sc.Input[1].SetIntLimits(1, 99);

    sc.DrawZeros = false;
    return;
  }

  if (!inited || sc.IsFullRecalculation) { inited = 1; emaResp=0; }
  if (sc.ArraySize <= 1) return;

  int H = sc.Input[0].GetInt();
  int idx = sc.ArraySize-1;
  int j = idx - H; if (j < 0) j = 0;
  double resp = sc.Close[idx] - sc.Close[j];
  double a = sc.Input[1].GetInt()/100.0;
  emaResp = a*resp + (1-a)*emaResp;
  sc.Subgraph[1][idx] = emaResp;
}
