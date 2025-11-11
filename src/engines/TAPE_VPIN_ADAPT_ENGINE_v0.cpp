// ============================================================================
// Pack v0 — Engines v0 (TAPE_PRO_v1)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TAPE_VPIN_ADAPT_ENGINE_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TAPE_VPIN_ADAPT_ENGINE_v1"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Seau volume"; sc.Input[0].SetFloat(500.0f);
    sc.Input[1].Name="02. Nb seaux M"; sc.Input[1].SetInt(20);
    sc.DrawZeros=false; return;
  }
  static double bucket=0,buyv=0,sellv=0,last=0; static double series[256]; static int n=0;
  double V=sc.Input[0].GetFloat(); int M=sc.Input[1].GetInt(); if(M<2) M=2; if(M>256) M=256;
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<1) return;
  const auto& e=ts[ts.Size()-1]; if(e.Type==SC_TS_TRADES){
    int sgn=(last>0? (e.Price>last? +1:(e.Price<last? -1:0)):0); last=e.Price; double v=e.Volume; if(sgn>=0) buyv+=v; if(sgn<=0) sellv+=v;
    double prev=bucket; bucket+=v; if(prev<V && bucket>=V){ double diff=fabs(buyv-sellv)/(V>0?V:1.0); if(n<M){ series[n++]=diff; } else { for(int i=1;i<n;++i) series[i-1]=series[i]; series[n-1]=diff; } bucket=bucket-V; buyv=0; sellv=0; }
  }
  if(n>0){ double s=0; for(int i=0;i<n;++i) s+=series[i]; sc.Subgraph[1][sc.ArraySize-1]=s/(double)n; }
}
