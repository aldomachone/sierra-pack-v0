// ============================================================================
// Pack v0 — Engines v0 (RoughVol&Jumps)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ROUGH_VOL_HURST_ABSRET_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ROUGH_VOL_HURST_ABSRET_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(300);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<W+2) return; int s=(idx-W>1? idx-W:1);
  double m=0; for(int i=s;i<=idx;++i) m+=fabs(sc.Close[i]-sc.Close[i-1]); m/= (double)(idx-s+1);
  double Y=0, Ymin=1e18, Ymax=-1e18; for(int i=s;i<=idx;++i){ Y += fabs(sc.Close[i]-sc.Close[i-1]) - m; if(Y<Ymin) Ymin=Y; if(Y>Ymax) Ymax=Y; }
  double R=Ymax-Ymin; double S=0; for(int i=s;i<=idx;++i){ double d=fabs(sc.Close[i]-sc.Close[i-1]) - m; S+=d*d; } S = math.sqrt(S/(double)(idx-s+1));
  sc.Subgraph[1][idx]=(S>0? R/S:0.0);
}
