// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PRICE_DISCOVERY_SPEED_ENGINE_v0b(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PRICE_DISCOVERY_SPEED_ENGINE_v0b"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(200);
    sc.Input[1].Name="02. Lags max"; sc.Input[1].SetInt(10);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int L=sc.Input[1].GetInt(); if(idx<3) return; int s=(idx-W>1? idx-W:1);
  const int MAX=2048; static double r[MAX]; int n=0; for(int i=s;i<=idx;++i){ if(n<MAX) r[n++]=sc.Close[i]-sc.Close[i-1]; }
  if(n<L+2) return; double m=0; for(int i=0;i<n;++i) m+=r[i]; m/=n; double v=0; for(int i=0;i<n;++i){ double d=r[i]-m; v+=d*d; }
  if(v<=0) return; double tau=0; for(int lag=1; lag<=L; ++lag){ double c=0; int cnt=0; for(int i=lag;i<n;++i){ c+=(r[i]-m)*(r[i-lag]-m); ++cnt; } c=(cnt>0? c/cnt:0.0); double rho=c/v; if(rho<=1.0/ math.e && tau==0) tau=lag; sc.Subgraph[lag][idx]=rho; } if(tau>0) sc.Subgraph[1][idx]=tau;
}
