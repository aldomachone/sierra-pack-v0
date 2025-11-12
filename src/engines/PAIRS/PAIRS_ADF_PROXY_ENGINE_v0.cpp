// ============================================================================
// Pack v0 — Engines v0 (Pairs/Discovery)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PAIRS_ADF_PROXY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PAIRS_ADF_PROXY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(200);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<W+2) return;
  double sx=0,sy=0,sxx=0,sxy=0; for(int i=idx-W+1;i<=idx;++i){ double x=sc.Open[i]; double y=sc.Close[i]; sx+=x; sy+=y; sxx+=x*x; sxy+=x*y; }
  double den=W*sxx - sx*sx; if(den==0) return; double beta=(W*sxy - sx*sy)/den; double alpha=(sy - beta*sx)/W;
  const int MAX=4096; static double u[MAX]; int n=0; for(int i=idx-W+1;i<=idx;++i){ u[n++]=sc.Close[i] - (alpha + beta*sc.Open[i]); }
  double su=0,suu=0,sdu=0, sumu=0; for(int i=1;i<n;++i){ su+=u[i-1]; suu+=u[i-1]*u[i-1]; sdu+=u[i]*u[i-1]; sumu+=u[i]; }
  double den2=(suu - su*su/(n-1)); double rho=(den2!=0? (sdu - su*(sumu/(n-1)))/den2 : 0.0);
  sc.Subgraph[1][idx]=rho;
}
