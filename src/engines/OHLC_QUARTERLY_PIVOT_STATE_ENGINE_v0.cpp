// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #9)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_OHLC_QUARTERLY_PIVOT_STATE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="OHLC_QUARTERLY_PIVOT_STATE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Subgraph[5].Name = "SG05";
    sc.Subgraph[5].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[5].DrawZeros = false;
    sc.Subgraph[5].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[6].Name = "SG06";
    sc.Subgraph[6].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[6].DrawZeros = false;
    sc.Subgraph[6].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Lookback barres"; sc.Input[0].SetInt(90);
    sc.Input[1].Name="02. Tolérance ATRx"; sc.Input[1].SetFloat(0.5f);
    sc.Input[2].Name="03. ATR n"; sc.Input[2].SetInt(14);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=1) return; int idx=sc.ArraySize-1; int L=sc.Input[0].GetInt(); int s=max(0, idx-L);
  double H=-1e300, Lw=1e300, C=0; for(int i=s;i<=idx;++i){ if(sc.High[i]>H) H=sc.High[i]; if(sc.Low[i]<Lw) Lw=sc.Low[i]; C=sc.Close[i]; }
  double P=(H+Lw+C)/3.0, R1=2*P-Lw, S1=2*P-H;
  int N=sc.Input[2].GetInt(); int sa=max(1, idx-N); double tr=0; int c=0; for(int k=sa;k<=idx;++k){ tr+=fmax(sc.High[k]-sc.Low[k], fmax(fabs(sc.High[k]-sc.Close[k-1]), fabs(sc.Low[k]-sc.Close[k-1]))); ++c; } double atr=(c>0? tr/c:1.0);
  double tol=sc.Input[1].GetFloat()*atr; double px=sc.Close[idx];
  double state=(fabs(px-P)<=tol? 1.0:0.0) + (fabs(px-R1)<=tol? 2.0:0.0) + (fabs(px-S1)<=tol? -2.0:0.0);
  sc.Subgraph[1][idx]=state; sc.Subgraph[2][idx]=P; sc.Subgraph[3][idx]=R1; sc.Subgraph[4][idx]=S1;
}
