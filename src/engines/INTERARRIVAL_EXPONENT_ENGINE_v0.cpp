// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_INTERARRIVAL_EXPONENT_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="INTERARRIVAL_EXPONENT_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre ms"; sc.Input[0].SetInt(3000);
    sc.Input[1].Name="02. k ordres"; sc.Input[1].SetInt(20);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<5 || sc.ArraySize==0) return;
  int win=sc.Input[0].GetInt(); int k=sc.Input[1].GetInt(); if(k<5) k=5; if(k>128) k=128;
  double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd - win/86400000.0; static double gaps[2048]; int n=0;
  double last=-1; for(int i=ts.Size()-1;i>=0; --i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type!=SC_TS_TRADES) continue; if(last>0){ double g=(last-e.DateTime)*86400000.0; if(n<2048) gaps[n++]=g; } last=e.DateTime; }
  if(n<=k) return; 
  #define SWAP(a,b) { double t=a;a=b;b=t; }
  for(int i=0;i<n-1;++i){ int mi=i; for(int j=i+1;j<n;++j) if(gaps[j]<gaps[mi]) mi=j; SWAP(gaps[i],gaps[mi]); }
  double xk=gaps[n-k]; double sum=0; for(int i=n-k+1;i<n;++i) sum+=log(gaps[i]/xk);
  double hill=(k>1? k/sum:0.0);
  sc.Subgraph[1][sc.ArraySize-1]=hill;
}
