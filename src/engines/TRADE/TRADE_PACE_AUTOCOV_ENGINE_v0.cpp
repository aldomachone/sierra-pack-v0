// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TRADE_PACE_AUTOCOV_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TRADE_PACE_AUTOCOV_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre ms"; sc.Input[0].SetInt(2000);
    sc.Input[1].Name="02. Lag bins"; sc.Input[1].SetInt(1);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<3) return;
  int win=sc.Input[0].GetInt(); int lag=sc.Input[1].GetInt();
  double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd-win/86400000.0; static double t[4096]; int n=0;
  for(int i=ts.Size()-1;i>=0; --i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type==SC_TS_TRADES){ if(n<4096) t[n++]=e.DateTime; } }
  if(n<=lag+1) return; double pace[4096]; for(int i=1;i<n;++i) pace[i-1]= (t[i]-t[i-1])*86400000.0; int m=n-1;
  double mean=0; for(int i=0;i<m;++i) mean+=pace[i]; mean/=m;
  double num=0, den=0; for(int i=lag;i<m;++i){ num += (pace[i]-mean)*(pace[i-lag]-mean); den += (pace[i]-mean)*(pace[i]-mean); }
  sc.Subgraph[1][sc.ArraySize-1]=(den>0? num/den:0.0);
}
