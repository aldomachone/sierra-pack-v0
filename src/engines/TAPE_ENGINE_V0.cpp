// ============================================================================
// Pack v0 — Engine de base (v0)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TAPE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited = sc.GetPersistentInt(1); double& emaTPS = sc.GetPersistentDouble(2);
  if (sc.SetDefaults)
  {
    sc.GraphName = "TAPE_ENGINE_v0";
    sc.AutoLoop = 0; sc.UpdateAlways = 1; sc.GraphRegion = 0; sc.ValueFormat = 26; sc.FreeDLL = 0;
    sc.MaintainTimeAndSalesData = 1;
    for (int i=1;i<=16;++i){ sc.Subgraph[i].Name = "SG%02d"; sc.Subgraph[i].DrawStyle = DRAWSTYLE_IGNORE; sc.Subgraph[i].DrawZeros=false; sc.Subgraph[i].DisplayAsMainPriceGraphValue=0; }
    sc.Input[0].Name="01. EMA % TPS"; sc.Input[0].SetInt(85); sc.Input[0].SetIntLimits(1,99);
    sc.DrawZeros=false; return;
  }
  if (!inited || sc.IsFullRecalculation) { inited=1; emaTPS=0; }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()==0 || sc.ArraySize==0) return;
  double a=sc.Input[0].GetInt()/100.0;
  double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd - 1/86400.0; // 1s
  int trades=0; for(int i=ts.Size()-1;i>=0;--i){ if(ts[i].DateTime<tBeg) break; if(ts[i].Type==SC_TS_TRADES) ++trades; }
  emaTPS = a*trades + (1-a)*emaTPS;
  sc.Subgraph[1][sc.ArraySize-1]=emaTPS;
}
