// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PRICE_IMPACT_PATH_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PRICE_IMPACT_PATH_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre ms"; sc.Input[0].SetInt(5000);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<3 || sc.ArraySize==0) return;
  double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd - sc.Input[0].GetInt()/86400000.0;
  double v=0; double p0=0, plast=0; bool init=false;
  for(int i=ts.Size()-1;i>=0; --i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type!=SC_TS_TRADES) continue; if(!init){ p0=e.Price; plast=e.Price; init=true; } v+=e.Volume; plast=e.Price; }
  if(!init) return; double imp=fabs(plast-p0); double slope=(v>0? (imp>0? log(imp)/log(1.0+v):0.0):0.0);
  sc.Subgraph[1][sc.ArraySize-1]=slope;
}
