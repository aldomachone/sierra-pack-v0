// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT4: Risk&Regime, Math&Finance)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_RETURN_ENTROPY_SIGN_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="RETURN_ENTROPY_SIGN_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
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
  int cU=0,cD=0,c0=0; for(int i=s;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; if(r>0) ++cU; else if(r<0) ++cD; else ++c0; }
  double N=(double)(cU+cD+c0); if(N<=0) return; double H=0; double pU=cU/N, pD=cD/N, p0=c0/N; 
  if(pU>0) H-=pU*log(pU); if(pD>0) H-=pD*log(pD); if(p0>0) H-=p0*log(p0);
  sc.Subgraph[1][idx]=H;
}
