// ============================================================================
// Pack v0 — Engines v0 (CONTEXT_ZONES_v1)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_CTX_AVWAP_ZSCORE_ENGINE_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="CTX_AVWAP_ZSCORE_ENGINE_v1"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(240);
    sc.DrawZeros=false; return;
  }
  int i=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(i<W) return;
  double pv=0,vv=0; for(int k=i-W+1;k<=i;++k){ pv+=sc.Close[k]*sc.Volume[k]; vv+=sc.Volume[k]; }
  double avwap=(vv>0? pv/vv:sc.Close[i]); double m=0,v=0; for(int k=i-W+1;k<=i;++k){ m+=sc.Close[k]; } m/=W; for(int k=i-W+1;k<=i;++k){ double d=sc.Close[k]-m; v+=d*d; } v/=W;
  sc.Subgraph[1][i]=(v>0? (sc.Close[i]-avwap)/sqrt(v):0.0);
}
