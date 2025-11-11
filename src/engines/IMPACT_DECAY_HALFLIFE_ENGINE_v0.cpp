// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #13)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_IMPACT_DECAY_HALFLIFE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="IMPACT_DECAY_HALFLIFE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";\n    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[1].DrawZeros = false;\n    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[2].Name = "SG02";\n    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[2].DrawZeros = false;\n    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[3].Name = "SG03";\n    sc.Subgraph[3].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[3].DrawZeros = false;\n    sc.Subgraph[3].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[4].Name = "SG04";\n    sc.Subgraph[4].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[4].DrawZeros = false;\n    sc.Subgraph[4].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[5].Name = "SG05";\n    sc.Subgraph[5].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[5].DrawZeros = false;\n    sc.Subgraph[5].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[6].Name = "SG06";\n    sc.Subgraph[6].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[6].DrawZeros = false;\n    sc.Subgraph[6].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[7].Name = "SG07";\n    sc.Subgraph[7].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[7].DrawZeros = false;\n    sc.Subgraph[7].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[8].Name = "SG08";\n    sc.Subgraph[8].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[8].DrawZeros = false;\n    sc.Subgraph[8].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[9].Name = "SG09";\n    sc.Subgraph[9].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[9].DrawZeros = false;\n    sc.Subgraph[9].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[10].Name = "SG10";\n    sc.Subgraph[10].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[10].DrawZeros = false;\n    sc.Subgraph[10].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[11].Name = "SG11";\n    sc.Subgraph[11].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[11].DrawZeros = false;\n    sc.Subgraph[11].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[12].Name = "SG12";\n    sc.Subgraph[12].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[12].DrawZeros = false;\n    sc.Subgraph[12].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Near niveaux OFI"; sc.Input[0].SetInt(8);
    sc.Input[1].Name="02. Max lag bars"; sc.Input[1].SetInt(10);
    sc.DrawZeros=false; return;
  }
  // OFI near
  int N=sc.Input[0].GetInt(); s_MarketDepthEntry md{}; double b=0,a=0; 
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<N; ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N; ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity; }
  static double pb=0,pa=0; if(sc.Index==0){ pb=b; pa=a; }
  double ofi=(b-pb)-(a-pa); pb=b; pa=a;
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int K=sc.Input[1].GetInt(); if(K>10) K=10;
  static double of[2048]; static double r[2048]; static int n=0;
  // push last bar
  double ret = sc.Close[idx]-sc.Close[idx-1];
  if(n<2048){ of[n]=ofi; r[n]=ret; ++n; } else { for(int i=1;i<n;++i){ of[i-1]=of[i]; r[i-1]=r[i]; } of[n-1]=ofi; r[n-1]=ret; }
  if(n<3) return;
  // find smallest lag where corr(ret_{t+lag}, ofi_t) halves
  double mOf=0,mR=0; for(int i=0;i<n;++i){ mOf+=of[i]; mR+=r[i]; } mOf/=n; mR/=n;
  double varOf=0; for(int i=0;i<n;++i){ double d=of[i]-mOf; varOf+=d*d; }
  if(varOf<=0) return;
  double c0=0; for(int i=1;i<n; ++i) c0 += (r[i]-mR)*(of[i-1]-mOf); c0/= (n-1);
  double target = 0.5 * (c0/varOf);
  int hl=K;
  for(int lag=1; lag<=K; ++lag){
    double c=0; int cnt=0; for(int i=lag;i<n; ++i){ c += (r[i]-mR)*(of[i-lag]-mOf); ++cnt; }
    if(cnt>0) c/=cnt; double corr = c/varOf;
    sc.Subgraph[lag][idx]=corr;
    if(corr<=target && hl==K) hl=lag;
  }
  sc.Subgraph[K+1][idx]=hl; // half-life in bars
}
