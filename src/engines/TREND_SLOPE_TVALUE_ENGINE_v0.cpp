// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT4: Risk&Regime, Math&Finance)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TREND_SLOPE_TVALUE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TREND_SLOPE_TVALUE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
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
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(60);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<W) return;
  double sx=0,sy=0,sxx=0,sxy=0; for(int i=idx-W+1;i<=idx;++i){ double x=i; double y=sc.Close[i]; sx+=x; sy+=y; sxx+=x*x; sxy+=x*y; }
  double den=W*sxx - sx*sx; if(den==0) return; double beta=(W*sxy - sx*sy)/den;
  double alpha=(sy - beta*sx)/W; double se2=0; for(int i=idx-W+1;i<=idx;++i){ double x=i; double y=sc.Close[i]; double yhat = alpha + beta*x; double e=y-yhat; se2+=e*e; }
  se2 /= (W>2? W-2:1); double varb = (W/den)*se2; double t = (varb>0? beta/sqrt(varb):0.0);
  sc.Subgraph[1][idx]=beta; sc.Subgraph[2][idx]=t;
}
