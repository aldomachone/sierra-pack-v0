// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #14)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PRICE_RESPONSE_OFI_TAU_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PRICE_RESPONSE_OFI_TAU_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Subgraph[7].Name = "SG07";
    sc.Subgraph[7].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[7].DrawZeros = false;
    sc.Subgraph[7].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[8].Name = "SG08";
    sc.Subgraph[8].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[8].DrawZeros = false;
    sc.Subgraph[8].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Niveaux near OFI"; sc.Input[0].SetInt(8);
    sc.Input[1].Name="02. Max tau bars"; sc.Input[1].SetInt(5);
    sc.Input[2].Name="03. Fenêtre barres"; sc.Input[2].SetInt(200);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); int K=sc.Input[1].GetInt(); if(K>8) K=8; int W=sc.Input[2].GetInt();
  // compute OFI_t
  s_MarketDepthEntry md{}; double b=0,a=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<N; ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N; ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity; }
  static double pb=0,pa=0; if(sc.Index==0){ pb=b; pa=a; }
  double ofi=(b-pb)-(a-pa); pb=b; pa=a;
  // push series
  const int MAX=4096; static double OFI[MAX]; static double R[MAX]; static int n=0;
  int idx=sc.ArraySize-1;
  double r = (idx>0? sc.Close[idx]-sc.Close[idx-1]:0.0);
  if(n<MAX){ OFI[n]=ofi; R[n]=r; ++n; } else { for(int i=1;i<n;++i){ OFI[i-1]=OFI[i]; R[i-1]=R[i]; } OFI[n-1]=ofi; R[n-1]=r; }
  // compute cov for lags 1..K over last W samples
  int s = (n-W>0? n-W:0);
  double mOf=0,mR=0; int M=n-s; if(M<3) return; for(int i=s;i<n;++i){ mOf+=OFI[i]; mR+=R[i]; } mOf/=M; mR/=M;
  for(int tau=1; tau<=K; ++tau){ double c=0; int cnt=0; for(int i=s; i<n-tau; ++i){ c += (R[i+tau]-mR)*(OFI[i]-mOf); ++cnt; } c = (cnt>0? c/cnt:0.0); sc.Subgraph[tau][idx]=c; }
}
