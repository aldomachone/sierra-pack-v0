// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TAIL_INDEX_HILL_RETURNS_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TAIL_INDEX_HILL_RETURNS_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(400);
    sc.Input[1].Name="02. k extrêmes"; sc.Input[1].SetInt(20);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int k=sc.Input[1].GetInt(); if(k<5) k=5;
  int s=(idx-W>1? idx-W:1);
  const int MAX=8192; static double v[MAX]; int n=0;
  for(int i=s;i<=idx;++i){ double r=fabs(sc.Close[i]-sc.Close[i-1]); if(n<MAX) v[n++]=r; }
  if(n<=k) return;
  #define SWAP(a,b) { double t=a;a=b;b=t; }
  for(int i=0;i<n-1;++i){ int mi=i; for(int j=i+1;j<n;++j) if(v[j]<v[mi]) mi=j; SWAP(v[i],v[mi]); }
  double xk=v[n-k]; double sum=0; for(int i=n-k+1;i<n;++i) sum+=log(v[i]/xk);
  double hill=(k>1? k/sum:0.0);
  sc.Subgraph[1][idx]=hill;
}
