// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VOLUME_IMBALANCE_PERSISTENCE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="VOLUME_IMBALANCE_PERSISTENCE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre ms"; sc.Input[0].SetInt(2000);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<3) return;
  double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd - sc.Input[0].GetInt()/86400000.0;
  double last=0; int prev=0; int runs=0, flips=0;
  for(int i=ts.Size()-1;i>=0; --i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type!=SC_TS_TRADES) continue; int sgn=(last>0? (e.Price>last? +1:(e.Price<last? -1:0)):0); last=e.Price; if(sgn==0) continue; if(prev==0) prev=sgn; else if(sgn!=prev){ ++flips; prev=sgn; } ++runs; }
  sc.Subgraph[1][sc.ArraySize-1]=(runs>0? 1.0 - (double)flips/runs : 0.0);
}
