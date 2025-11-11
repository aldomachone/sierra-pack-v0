// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats #12)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_CORWIN_SCHULTZ_SPREAD_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="CORWIN_SCHULTZ_SPREAD_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre k"; sc.Input[0].SetInt(2);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int k=sc.Input[0].GetInt(); if(k<1) k=1;
  int s=(idx-2*k>0? idx-2*k:0);
  double beta=0, gamma=0;
  for(int i=s+k;i<=idx; ++i){
    double H1=sc.High[i]; double L1=sc.Low[i]; double H2=sc.High[i-k]; double L2=sc.Low[i-k];
    if(L1<=0||L2<=0) continue;
    beta += pow(log(H1/L1),2) + pow(log(H2/L2),2);
    gamma += pow(log(max(H1,H2)/min(L1,L2)),2);
  }
  int n=idx-(s+k)+1; if(n<=0) return; beta/=n; gamma/=n;
  double alpha = (sqrt(max(0.0,2*beta)) - sqrt(max(0.0,beta))) / (3 - 2*sqrt(max(0.0,beta)));
  alpha = max(0.0, min(alpha, 1.0));
  double S = 2*(exp(alpha)-1)/(1+exp(alpha));
  sc.Subgraph[1][idx]=S;
}
