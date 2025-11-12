// ============================================================================
// Pack v0 — Engines v0 (Tape-only)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TAPE_VPIN_VOLUME_BUCKET_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TAPE_VPIN_VOLUME_BUCKET_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Seau volume"; sc.Input[0].SetInt(500);
    sc.Input[1].Name="02. Nb seaux M"; sc.Input[1].SetInt(20);
    sc.DrawZeros=false; return;
  }
  static double bucket=0; static double buyv=0, sellv=0;
  static double series[256]; static int n=0;
  int V=sc.Input[0].GetInt(); int M=sc.Input[1].GetInt(); if(M<2) M=2; if(M>256) M=256;
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<1) return;
  static double last=0;
  const auto& e=ts[ts.Size()-1];
  if(e.Type==SC_TS_TRADES){
    int sign = (last>0? (e.Price>last? +1:(e.Price<last? -1:0)):0);
    last=e.Price;
    double v=e.Volume;
    if(sign>=0) buyv+=v; if(sign<=0) sellv+=v;
    double bucket_prev=bucket;
    bucket += v;
    if(bucket_prev < V && bucket >= V){
      double over = bucket - V;
      double used = v - over;
      double diff=fabs(buyv - sellv) / (double)V;
      if(n<M){ series[n++]=diff; } else { 
        for(int i=1;i<n;++i) series[i-1]=series[i]; 
        if(n==0) n=1; 
        series[n-1]=diff; 
      }
      bucket=over; // carry remainder
      // proportionally reduce buyv/sellv is complex; keep as is for proxy
      buyv=0; sellv=0;
    }
  }
  if(n>0){
    double s=0; for(int i=0;i<n;++i) s+=series[i];
    sc.Subgraph[1][sc.ArraySize-1]= s/(double)n;
  }
}
