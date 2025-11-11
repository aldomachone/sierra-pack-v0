// ============================================================================
// Pack v0 — Engines v0 (RoughVol&Jumps)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_BIPOWER_VARIATION_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="BIPOWER_VARIATION_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
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
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<2) return; int s=(idx-W>2? idx-W:2);
  double mu = (math.pi/2.0);
  double bpv=0; for(int i=s;i<=idx;++i){ double r= sc.Close[i]-sc.Close[i-1]; double r1= sc.Close[i-1]-sc.Close[i-2]; bpv += abs(r)*abs(r1); }
  sc.Subgraph[1][idx]= mu*bpv/(double)(W>0? W:1);
}
