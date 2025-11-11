// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_CROSS_CORR_LAG_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="CROSS_CORR_LAG_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(120);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<W+1) return;
  double mx=0,my=0; for(int i=idx-W+1;i<=idx;++i){ mx+=sc.Open[i]; my+=sc.Close[i-1]; } mx/=W; my/=W;
  double num=0, dx=0, dy=0; for(int i=idx-W+1;i<=idx;++i){ double a=sc.Open[i]-mx; double b=sc.Close[i-1]-my; num+=a*b; dx+=a*a; dy+=b*b; }
  sc.Subgraph[1][idx]=(dx>0&&dy>0? num/sqrt(dx*dy):0.0);
}
