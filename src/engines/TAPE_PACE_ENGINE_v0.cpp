// ============================================================================
// Pack v0 — Engines (v0) — Tape & Advanced
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_TAPE_PACE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited = sc.GetPersistentInt(1);
  double& emaTPS = sc.GetPersistentDouble(2); // trades per second EMA

  if (sc.SetDefaults)
  {
    sc.GraphName = "TAPE_PACE_ENGINE_v0";
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

    sc.Input[1].Name = "02. Fenêtre agrégation secondes";
    sc.Input[1].SetInt(1); sc.Input[1].SetIntLimits(1,10);

    sc.DrawZeros=false; return;
  }

  if (!inited || sc.IsFullRecalculation) { inited=1; emaTPS=0; }

  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts);
  if (ts.Size()==0) return;

  int emaPct = sc.Input[0].GetInt();
  int winSec = sc.Input[1].GetInt();
  double a = emaPct/100.0;

  double tEnd = ts[ts.Size()-1].DateTime;
  double tBeg = tEnd - winSec/86400.0;
  int trades = 0;
  for(int i=ts.Size()-1;i>=0;--i){ if(ts[i].DateTime < tBeg) break; if(ts[i].Type==SC_TS_TRADES) ++trades; }
  double tps = trades / (double)winSec;
  emaTPS = a*tps + (1-a)*emaTPS;

  int idx = sc.ArraySize-1; if(idx>=0) sc.Subgraph[1][idx]=emaTPS;
}
