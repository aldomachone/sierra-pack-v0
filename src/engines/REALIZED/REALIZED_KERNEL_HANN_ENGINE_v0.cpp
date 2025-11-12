// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #14)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_REALIZED_KERNEL_HANN_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="REALIZED_KERNEL_HANN_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(300);
    sc.Input[1].Name="02. Lag max"; sc.Input[1].SetInt(10);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int L=sc.Input[1].GetInt(); if(L<1) L=1; if(L>50) L=50;
  int s=(idx-W>1? idx-W:1);
  const int MAX=4096; static double r[MAX]; int n=0;
  for(int i=s;i<=idx; ++i){ if(n<MAX) r[n++]=sc.Close[i]-sc.Close[i-1]; }
  if(n<2) return;
  // gamma0
  double m=0; for(int i=0;i<n;++i) m+=r[i]; m/=n;
  double g0=0; for(int i=0;i<n;++i){ double d=r[i]-m; g0+=d*d; } g0/=(double)n;
  double rk=g0;
  for(int lag=1; lag<=L; ++lag){
    double gl=0; for(int i=lag;i<n;++i){ gl += (r[i]-m)*(r[i-lag]-m); } gl/=(double)(n-lag);
    double w = 0.5 * (1.0 + cos(M_PI*lag/(L+1))); // Hann
    rk += 2.0 * w * gl;
  }
  sc.Subgraph[1][idx]=rk;
}
