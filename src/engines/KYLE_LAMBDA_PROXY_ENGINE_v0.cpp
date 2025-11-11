// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_KYLE_LAMBDA_PROXY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="KYLE_LAMBDA_PROXY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(120);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; if(idx<2) return; int W=sc.Input[0].GetInt(); int s=(idx-W>1? idx-W:1);
  // signed returns * signed volume proxy using bar volume
  double sx=0, sy=0, sxx=0, sxy=0; int n=0;
  for(int i=s;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; double sv=(r>0? +1:(r<0? -1:0)) * (double)sc.Volume[i]; sx+=sv; sy+=r; sxx+=sv*sv; sxy+=sv*r; ++n; }
  double den = n*sxx - sx*sx; double lam = (den!=0? (n*sxy - sx*sy)/den : 0.0);
  sc.Subgraph[1][idx]=lam;
}
