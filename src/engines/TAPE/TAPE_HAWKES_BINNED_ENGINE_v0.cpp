// ============================================================================
// Pack v0 — Engines v0 (Tape-only)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TAPE_HAWKES_BINNED_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TAPE_HAWKES_BINNED_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Bin ms"; sc.Input[0].SetInt(200);
    sc.Input[1].Name="02. Alpha self-excite"; sc.Input[1].SetFloat(0.2f);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<1) return;
  double now=ts[ts.Size()-1].DateTime; double bin_ms=sc.Input[0].GetInt();
  static double lambda_=0; double a=sc.Input[1].GetFloat();
  int cnt=0; double t0=now - bin_ms/86400000.0;
  for(int i=ts.Size()-1;i>=0;--i){ const auto& e=ts[i]; if(e.DateTime<t0) break; if(e.Type==SC_TS_TRADES) ++cnt; }
  lambda_ = (1.0-a)*lambda_ + a*cnt;
  sc.Subgraph[1][sc.ArraySize-1]=lambda_;
}
