// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_COINTEGRATION_SCORE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="COINTEGRATION_SCORE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    // Note: proxy using Open vs Close of same instrument for demo
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(100);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<W||W<10) return;
  double sx=0,sy=0,sxx=0,sxy=0,syy=0; for(int i=idx-W+1;i<=idx;++i){ double x=sc.Open[i]; double y=sc.Close[i]; sx+=x; sy+=y; sxx+=x*x; sxy+=x*y; syy+=y*y; }
  double den=W*sxx - sx*sx; double beta=(den!=0? (W*sxy - sx*sy)/den:0.0); double alpha=(sy - beta*sx)/W;
  double res2=0; for(int i=idx-W+1;i<=idx;++i){ double u = sc.Close[i] - (alpha + beta*sc.Open[i]); res2+=u*u; }
  double mse = res2/W;
  sc.Subgraph[1][idx]=(mse>0? 1.0/mse:0.0);
}
