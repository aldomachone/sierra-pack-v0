// ============================================================================
// Pack v0 — Engines (v0) — complémentaires
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_PIVOT_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited  = sc.GetPersistentInt(1);
  if (sc.SetDefaults)
  {
    sc.GraphName = "PIVOT_ENGINE_v0";
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

    sc.Input[0].Name = "01. Période principale";
    sc.Input[0].SetInt(20); sc.Input[0].SetIntLimits(1, 100000);

    sc.Input[1].Name = "02. Formule (0=Classic,1=Fibo,2=Camarilla,3=Woodie,4=DeMark)";
    sc.Input[1].SetInt(0); sc.Input[1].SetIntLimits(0,4);
    sc.Input[2].Name = "03. Période (0=J,1=T,2=A)";
    sc.Input[2].SetInt(0); sc.Input[2].SetIntLimits(0,2);

    sc.DrawZeros = false;
    return;
  }
  if (!inited || sc.IsFullRecalculation) { inited = 1; }
  if (sc.ArraySize <= 0) return;

  const int per = sc.Input[0].GetInt();

  int idx = sc.ArraySize-1;
  double H = sc.High[idx], L = sc.Low[idx], C = sc.Close[idx];
  double P = (H+L+C)/3.0;
  sc.Subgraph[1][idx] = P;

}
