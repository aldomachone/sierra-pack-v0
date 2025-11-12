// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VOL_OF_VOL_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="VOL_OF_VOL_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre vol"; sc.Input[0].SetInt(30);
    sc.Input[1].Name="02. Fenêtre VoV"; sc.Input[1].SetInt(30);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; if(idx<3) return;
  auto rv=[&](int W){ int s=(idx-W>1? idx-W:1); double S=0; for(int i=s;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; S+=r*r; } return S/(double)W; };
  const int MAX=1024; static double v[MAX]; static int n=0;
  double vol=rv(sc.Input[0].GetInt()); if(n<MAX) v[n++]=vol; else { for(int i=1;i<n;++i) v[i-1]=v[i]; v[n-1]=vol; }
  int W=sc.Input[1].GetInt(); if(n<W+1) return; double m=0; for(int i=n-W;i<n;++i) m+=v[i]; m/=W; double vv=0; for(int i=n-W;i<n;++i){ double d=v[i]-m; vv+=d*d; } vv/=W;
  sc.Subgraph[1][idx]=vv;
}
