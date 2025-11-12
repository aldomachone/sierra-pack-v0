// ============================================================================
// Pack v0 — Engines v0 (lot Extra-HFT #15)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_SQUARE_ROOT_IMPACT_FIT_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SQUARE_ROOT_IMPACT_FIT_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Seau volume"; sc.Input[0].SetInt(2000);
    sc.Input[1].Name="02. Fenêtre seaux"; sc.Input[1].SetInt(30);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<3 || sc.ArraySize==0) return;
  int V=sc.Input[0].GetInt(); int W=sc.Input[1].GetInt(); if(W>256) W=256;
  static double X[256], Y[256]; int n=0; double lastP=0, buck=0, vBuy=0, vSell=0, pStart=0;
  for(int i=0;i<ts.Size(); ++i){ const auto& e=ts[i]; if(e.Type!=SC_TS_TRADES) continue; if(lastP==0){ lastP=e.Price; pStart=e.Price; }
    int sgn=(e.Price>lastP? +1:(e.Price<lastP? -1:0)); lastP=e.Price; if(sgn>=0) vBuy+=e.Volume; else vSell+=e.Volume; buck += e.Volume;
    if(buck>=V && n<256){ double vTot=vBuy+vSell; double impact=fabs(lastP - pStart); X[n]=sqrt(vTot>1? vTot:1); Y[n]=impact; ++n; buck=0; vBuy=0; vSell=0; pStart=lastP; } }
  if(n<3) return; double x=0,y=0,xx=0,xy=0; for(int i=0;i<n;++i){ x+=X[i]; y+=Y[i]; xx+=X[i]*X[i]; xy+=X[i]*Y[i]; }
  double den=n*xx-x*x; double b=(den!=0? (n*xy-x*y)/den : 0.0);
  sc.Subgraph[1][sc.ArraySize-1]=b;
}
