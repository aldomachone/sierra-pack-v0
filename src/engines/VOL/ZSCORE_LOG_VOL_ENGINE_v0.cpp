// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT4: Risk&Regime, Math&Finance)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ZSCORE_LOG_VOL_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ZSCORE_LOG_VOL_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
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
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<2) return; int s=(idx-W>1? idx-W:1);
  double lv[4096]; int n=0; for(int i=s;i<=idx && n<4096;++i){ double r=sc.Close[i]-sc.Close[i-1]; lv[n++]=log(r*r+1e-12); }
  if(n<5) return; double m=0; for(int i=0;i<n;++i) m+=lv[i]; m/=n; double v=0; for(int i=0;i<n;++i){ double d=lv[i]-m; v+=d*d; } v/=n;
  double z=(v>0? (lv[n-1]-m)/sqrt(v):0.0);
  sc.Subgraph[1][idx]=z;
}
