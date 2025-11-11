// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_RETURN_AUTOCORR_PARTIAL_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="RETURN_AUTOCORR_PARTIAL_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
  int idx=sc.ArraySize-1; if(idx<5) return; int W=sc.Input[0].GetInt(); int s=(idx-W>1? idx-W:1);
  const int MAX=1024; static double r[MAX]; int n=0; for(int i=s;i<=idx;++i){ if(n<MAX) r[n++]=sc.Close[i]-sc.Close[i-1]; }
  if(n<6) return;
  auto pacf1=[&](){ double m=0; for(int i=0;i<n;++i)m+=r[i]; m/=n; double v=0; for(int i=0;i<n;++i){ double d=r[i]-m; v+=d*d; } double c=0; for(int i=1;i<n;++i) c+=(r[i]-m)*(r[i-1]-m); return (v>0? c/v:0.0); };
  double p1=pacf1();
  sc.Subgraph[1][idx]=p1;
}
