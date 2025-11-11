// ============================================================================
// Pack v0 — Engines v0 (RoughVol&Jumps)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_MEDRV_REALIZED_VARIANCE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="MEDRV_REALIZED_VARIANCE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(180);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<3) return; int s=(idx-W>2? idx-W:2);
  const int MAX=8192; static double rr[MAX]; int n=0;
  for(int i=s;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; if(n<MAX) rr[n++]=r*r; }
  if(n<3) return;
  for(int i=0;i<n-1;++i){ int mi=i; for(int j=i+1;j<n;++j) if(rr[j]<rr[mi]) mi=j; double t=rr[i]; rr[i]=rr[mi]; rr[mi]=t; }
  double med = (n%2? rr[n/2] : 0.5*(rr[n/2 -1] + rr[n/2]));
  sc.Subgraph[1][idx]=med;
}
