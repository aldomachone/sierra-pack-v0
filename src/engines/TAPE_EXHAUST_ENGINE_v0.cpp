// ============================================================================
// Pack v0 — Engines (v0) — Tape & Advanced
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_TAPE_EXHAUST_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited = sc.GetPersistentInt(1);
  double& exhaust = sc.GetPersistentDouble(2);
  double& emaIA = sc.GetPersistentDouble(3);
  double& emaSz = sc.GetPersistentDouble(4);

  if (sc.SetDefaults)
  {
    sc.GraphName = "TAPE_EXHAUST_ENGINE_v0";
    sc.AutoLoop = 0; sc.UpdateAlways = 1; sc.GraphRegion = 0; sc.ValueFormat = 26; sc.FreeDLL = 0;
    sc.MaintainTimeAndSalesData = 1;

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

    sc.Input[0].Name = "01. EMA %";
    sc.Input[0].SetInt(85); sc.Input[0].SetIntLimits(1,99);

    sc.DrawZeros=false; return;
  }

  if (!inited || sc.IsFullRecalculation) { inited=1; exhaust=0; emaIA=0; emaSz=0; }

  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts);
  if (ts.Size()==0) return;

  double a = sc.Input[0].GetInt()/100.0;
  static double lastT=0;
  for(int i = max(0, ts.Size()-2000); i<ts.Size(); ++i)
  {
    const auto& e = ts[i];
    if (e.Type != SC_TS_TRADES) continue;
    if (lastT>0) { double ia = (e.DateTime-lastT)*86400000.0; emaIA = a*ia + (1-a)*emaIA; }
    emaSz = a*e.Volume + (1-a)*emaSz;
    lastT = e.DateTime;
  }
  // Exhaustion when inter-arrival grows while size shrinks
  exhaust = (emaIA>0 ? (emaSz/emaIA) : 0.0);
  int idx=sc.ArraySize-1; if(idx>=0) sc.Subgraph[1][idx]=exhaust;
}
