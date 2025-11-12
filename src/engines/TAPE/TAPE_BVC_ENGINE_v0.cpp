// ============================================================================
// Pack v0 — Engines (v0) — Tape & Advanced
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_TAPE_BVC_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited = sc.GetPersistentInt(1);
  double& buyEMA = sc.GetPersistentDouble(2);
  double& sellEMA = sc.GetPersistentDouble(3);

  if (sc.SetDefaults)
  {
    sc.GraphName = "TAPE_BVC_ENGINE_v0";
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
    sc.Subgraph[9].Name = "SG09";
    sc.Subgraph[9].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[9].DrawZeros = false;
    sc.Subgraph[9].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[10].Name = "SG10";
    sc.Subgraph[10].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[10].DrawZeros = false;
    sc.Subgraph[10].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[11].Name = "SG11";
    sc.Subgraph[11].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[11].DrawZeros = false;
    sc.Subgraph[11].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[12].Name = "SG12";
    sc.Subgraph[12].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[12].DrawZeros = false;
    sc.Subgraph[12].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[13].Name = "SG13";
    sc.Subgraph[13].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[13].DrawZeros = false;
    sc.Subgraph[13].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[14].Name = "SG14";
    sc.Subgraph[14].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[14].DrawZeros = false;
    sc.Subgraph[14].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[15].Name = "SG15";
    sc.Subgraph[15].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[15].DrawZeros = false;
    sc.Subgraph[15].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[16].Name = "SG16";
    sc.Subgraph[16].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[16].DrawZeros = false;
    sc.Subgraph[16].DisplayAsMainPriceGraphValue = 0;

    sc.Input[0].Name = "01. EMA %";
    sc.Input[0].SetInt(85); sc.Input[0].SetIntLimits(1,99);

    sc.DrawZeros=false; return;
  }

  if (!inited || sc.IsFullRecalculation) { inited=1; buyEMA=0; sellEMA=0; }

  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts);
  if (ts.Size()==0) return;

  double a = sc.Input[0].GetInt()/100.0;
  static double lastPx = 0.0;
  for(int i = max(0, ts.Size()-2000); i<ts.Size(); ++i)
  {
    const auto& e = ts[i];
    if (e.Type != SC_TS_TRADES) continue;
    int dir = 0;
    if (lastPx>0) dir = (e.Price > lastPx ? +1 : (e.Price < lastPx ? -1 : 0));
    lastPx = e.Price;
    if (dir>=0) buyEMA = a*e.Volume + (1-a)*buyEMA;
    if (dir<=0) sellEMA = a*e.Volume + (1-a)*sellEMA;
  }
  int idx = sc.ArraySize-1; if(idx>=0){ sc.Subgraph[1][idx]=buyEMA; sc.Subgraph[2][idx]=sellEMA; sc.Subgraph[3][idx]=buyEMA-sellEMA; }
}
