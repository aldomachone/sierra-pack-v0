// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #7)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_RANGE_REGIME_SHIFT_ENGINE_v0(SCStudyInterfaceRef sc)
{
  double& prev=sc.GetPersistentDouble(1);
  if(sc.SetDefaults){
    sc.GraphName="RANGE_REGIME_SHIFT_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Lookback"; sc.Input[0].SetInt(60);
    sc.Input[1].Name="02. ATR n"; sc.Input[1].SetInt(14);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=1) return; int idx=sc.ArraySize-1; int L=sc.Input[0].GetInt(); int s=idx-L; if(s<0) s=0;
  double hi=-1e300, lo=1e300; for(int i=s;i<=idx;++i){ if(sc.High[i]>hi) hi=sc.High[i]; if(sc.Low[i]<lo) lo=sc.Low[i]; }
  int N=sc.Input[1].GetInt(); int sa=idx-N; if(sa<1) sa=1; double tr=0; int c=0; for(int k=sa;k<=idx;++k){ tr+=fmax(sc.High[k]-sc.Low[k], fmax(fabs(sc.High[k]-sc.Close[k-1]), fabs(sc.Low[k]-sc.Close[k-1]))); ++c; } double atr=(c>0? tr/c:1.0);
  double regime = (atr>0? (hi-lo)/atr:0.0); if(sc.IsFullRecalculation) prev=regime;
  sc.Subgraph[1][idx]=regime - prev; prev=regime;
}
