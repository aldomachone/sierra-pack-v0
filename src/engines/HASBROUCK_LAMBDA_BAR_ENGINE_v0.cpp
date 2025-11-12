// ============================================================================
// Pack v0 — Engines v0 (lot HFT formulas #11)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_HASBROUCK_LAMBDA_BAR_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="HASBROUCK_LAMBDA_BAR_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(50);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<2) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int s=(idx-W>1? idx-W:1);
  // signed trade imbalance per bar from Time&Sales
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts);
  double x=0,y=0,xx=0,xy=0; int n=0;
  for(int i=s;i<=idx;++i){
    double t0=sc.BaseDateTimeIn[i-1]; double t1=sc.BaseDateTimeIn[i];
    double imb=0; double last=0;
    for(int k=ts.Size()-1;k>=0;--k){ const auto& e=ts[k]; if(e.DateTime<t0) break; if(e.DateTime>t1) continue; if(e.Type!=SC_TS_TRADES) continue; int sgn=(last>0? (e.Price>last? +1:(e.Price<last? -1:0)):0); imb += sgn*e.Volume; last=e.Price; }
    double ret = sc.Close[i]-sc.Close[i-1];
    double xi = imb; double yi = ret;
    x+=xi; y+=yi; xx+=xi*xi; xy+=xi*yi; ++n;
  }
  double den = n*xx - x*x; double lam = (den!=0? (n*xy - x*y)/den : 0.0);
  sc.Subgraph[1][idx]=lam;
}
