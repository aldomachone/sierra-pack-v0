// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT4: Risk&Regime, Math&Finance)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_REALIZED_CVAR_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="REALIZED_CVAR_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(400);
    sc.Input[1].Name="02. Quantile q"; sc.Input[1].SetFloat(0.05f);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<2||W<10) return; int s=(idx-W>1? idx-W:1);
  const int MAX=8192; static double r[MAX]; int n=0;
  for(int i=s;i<=idx;++i){ if(n<MAX) r[n++]=sc.Close[i]-sc.Close[i-1]; }
  if(n<5) return;
  for(int i=0;i<n-1;++i){ int mi=i; for(int j=i+1;j<n;++j) if(r[j]<r[mi]) mi=j; double t=r[i]; r[i]=r[mi]; r[mi]=t; }
  double q=sc.Input[1].GetFloat(); if(q<=0) return; int k=(int)(q*(n-1)); if(k<1) k=1;
  double sum=0; for(int i=0;i<=k;++i) sum+=r[i];
  sc.Subgraph[1][idx]=sum/(k+1);
}
