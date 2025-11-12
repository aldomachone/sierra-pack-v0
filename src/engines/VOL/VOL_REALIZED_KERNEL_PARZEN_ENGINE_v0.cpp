// ============================================================================
// Pack v0 — Engines v0 (VOL_JUMPS_PRO_v1)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VOL_REALIZED_KERNEL_PARZEN_ENGINE_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="VOL_REALIZED_KERNEL_PARZEN_ENGINE_v1"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(180);
    sc.DrawZeros=false; return;
  }
  int i=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(i<W+1) return; int s=i-W+1; double rv=0;
  for(int k=s;k<=i;++k){ double r=sc.Close[k]-sc.Close[k-1]; double u=(double)(k-s)/(double)(W); double w=(1-fabs(2*u-1)); if(w<0) w=0; rv += w*r*r; }
  sc.Subgraph[1][i]=rv;
}
