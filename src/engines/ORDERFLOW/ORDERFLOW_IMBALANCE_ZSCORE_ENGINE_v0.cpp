// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ORDERFLOW_IMBALANCE_ZSCORE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ORDERFLOW_IMBALANCE_ZSCORE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Niveaux near"; sc.Input[0].SetInt(8);
    sc.Input[1].Name="02. Fenêtre Z"; sc.Input[1].SetInt(200);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); s_MarketDepthEntry md{}; double b=0,a=0;
  for(int i=0;i<N && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<N && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity; }
  double x=b-a;
  const int MAX=4096; static double buf[MAX]; static int n=0;
  if(n<MAX) buf[n++]=x; else { for(int i=1;i<MAX;++i) buf[i-1]=buf[i]; buf[MAX-1]=x; n=MAX; }
  int W=sc.Input[1].GetInt(); if(W<5) W=5; if(n<W) return;
  double m=0; for(int i=n-W;i<n;++i) m+=buf[i]; m/=W;
  double v=0; for(int i=n-W;i<n;++i){ double d=buf[i]-m; v+=d*d; } v/=W;
  double z=(v>0? (x-m)/sqrt(v):0.0);
  sc.Subgraph[1][sc.ArraySize-1]=z;
}
