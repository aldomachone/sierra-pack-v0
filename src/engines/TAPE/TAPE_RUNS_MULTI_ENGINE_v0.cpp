// ============================================================================
// Pack v0 — Engines v0 (Tape-only)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TAPE_RUNS_MULTI_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TAPE_RUNS_MULTI_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1;
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
    sc.Input[0].Name="01. Fenêtre ms court"; sc.Input[0].SetInt(1000);
    sc.Input[1].Name="02. Fenêtre ms long"; sc.Input[1].SetInt(3000);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<2) return;
  auto runs=[&](int win_ms){ double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd - win_ms/86400000.0; double last=0; int flips=0, total=0, prev=0;
    for(int i=ts.Size()-1;i>=0;--i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type!=SC_TS_TRADES) continue; int sgn=(last>0? (e.Price>last? +1:(e.Price<last? -1:0)):0); last=e.Price; if(sgn==0) continue; if(prev==0) prev=sgn; else if(sgn!=prev){ ++flips; prev=sgn; } ++total; }
    return (total>0? 1.0 - (double)flips/total: 0.0);
  };
  sc.Subgraph[1][sc.ArraySize-1]=runs(sc.Input[0].GetInt());
  sc.Subgraph[2][sc.ArraySize-1]=runs(sc.Input[1].GetInt());
  sc.Subgraph[3][sc.ArraySize-1]=sc.Subgraph[1][sc.ArraySize-1]-sc.Subgraph[2][sc.ArraySize-1];
}
