// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_SPREAD_MEAN_REVERT_COEFF_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="SPREAD_MEAN_REVERT_COEFF_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre obs"; sc.Input[0].SetInt(120);
    sc.DrawZeros=false; return;
  }
  if(sc.GetBidMarketDepthNumberOfLevels()<1 || sc.GetAskMarketDepthNumberOfLevels()<1) return;
  s_MarketDepthEntry md{}; sc.GetBidMarketDepthEntryAtLevel(md,0); double pb=md.Price; sc.GetAskMarketDepthEntryAtLevel(md,0); double pa=md.Price;
  double s=(sc.TickSize>0? (pa-pb)/sc.TickSize:0.0);
  const int MAX=2048; static double x[MAX]; static int n=0;
  if(n<MAX) x[n++]=s; else { for(int i=1;i<n;++i) x[i-1]=x[i]; x[n-1]=s; }
  int W=sc.Input[0].GetInt(); if(n<=W) return; double sx=0, sxx=0, sxy=0, sy=0; for(int i=n-W+1;i<n;++i){ sx+=x[i-1]; sy+=x[i]; sxx+=x[i-1]*x[i-1]; sxy+=x[i-1]*x[i]; }
  double den = (W-1)*sxx - sx*sx; double a1 = (den!=0? ((W-1)*sxy - sx*sy)/den : 0.0);
  sc.Subgraph[1][sc.ArraySize-1]=a1;
}
