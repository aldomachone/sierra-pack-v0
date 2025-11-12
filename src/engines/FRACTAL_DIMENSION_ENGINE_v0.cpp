// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_FRACTAL_DIMENSION_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="FRACTAL_DIMENSION_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(256);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<3) return; int s=(idx-W>1? idx-W:1); int n=idx-s+1;
  if(n<16) return; // two scales
  int boxes1=0, boxes2=0; int step1=1, step2=4;
  double mn=1e18,mx=-1e18; for(int i=s;i<=idx;++i){ if(sc.Close[i]<mn) mn=sc.Close[i]; if(sc.Close[i]>mx) mx=sc.Close[i]; }
  if(mx<=mn) return; double h=(mx-mn)/8.0;
  for(int i=s;i<=idx;i+=step1){ int b=int((sc.Close[i]-mn)/h); if(b<0)b=0; if(b>7)b=7; boxes1 |= (1<<b); }
  for(int i=s;i<=idx;i+=step2){ int b=int((sc.Close[i]-mn)/h); if(b<0)b=0; if(b>7)b=7; boxes2 |= (1<<b); }
  int n1=0,n2=0; for(int i=0;i<8;++i){ if(boxes1&(1<<i)) ++n1; if(boxes2&(1<<i)) ++n2; }
  double D = (n1>0 && n2>0? (log((double)n2)-log((double)n1))/(log(1.0/step2)-log(1.0/step1)) : 0.0);
  sc.Subgraph[1][idx]=fabs(D);
}
