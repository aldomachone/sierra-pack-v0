// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_SPECTRAL_ENTROPY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SPECTRAL_ENTROPY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(128);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<3) return; int s=(idx-W>1? idx-W:1); if(W>512) W=512;
  static double r[1024]; int n=0; for(int i=s;i<=idx;++i){ if(n<1024) r[n++]=sc.Close[i]-sc.Close[i-1]; }
  if(n<8) return; int M=n;
  // coarse DFT magnitudes
  double P[32]; int K=16; for(int k=0;k<K;++k){ double Re=0,Im=0; for(int t=0;t<M;++t){ double ang=2*3.141592653589793*k*t/M; Re+=r[t]*cos(ang); Im+=r[t]*sin(ang);} P[k]=(Re*Re+Im*Im); }
  double sum=0; for(int k=1;k<K;++k) sum+=P[k]; if(sum<=0) return; double H=0; for(int k=1;k<K;++k){ double p=P[k]/sum; if(p>0) H -= p*log(p); }
  sc.Subgraph[1][idx]=H;
}
