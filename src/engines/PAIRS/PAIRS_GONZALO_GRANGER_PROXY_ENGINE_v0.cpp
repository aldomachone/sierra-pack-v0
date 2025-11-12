// ============================================================================
// Pack v0 — Engines v0 (Pairs/Discovery)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PAIRS_GONZALO_GRANGER_PROXY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PAIRS_GONZALO_GRANGER_PROXY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
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
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<W) return;
  double sx=0,sy=0,sxx=0,sxy=0; for(int i=idx-W+1;i<=idx;++i){ double x=sc.Open[i]; double y=sc.Close[i]; sx+=x; sy+=y; sxx+=x*x; sxy+=x*y; }
  double den=W*sxx - sx*sx; double beta=(den!=0? (W*sxy - sx*sy)/den:0.0); double alpha=(sy - beta*sx)/W;
  double varx=0,vary=0; for(int i=idx-W+1;i<=idx;++i){ varx+=(sc.Open[i]-sx/W)*(sc.Open[i]-sx/W); vary+=(sc.Close[i]-sy/W)*(sc.Close[i]-sy/W); }
  double w = (varx+vary>0? vary/(varx+vary):0.5);
  sc.Subgraph[1][idx]=w;
}
