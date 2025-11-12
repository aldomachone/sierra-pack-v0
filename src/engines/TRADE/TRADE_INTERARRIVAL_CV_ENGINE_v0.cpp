// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats #12)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TRADE_INTERARRIVAL_CV_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TRADE_INTERARRIVAL_CV_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre ms"; sc.Input[0].SetInt(1500);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<3 || sc.ArraySize==0) return;
  int win=sc.Input[0].GetInt(); double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd-win/86400000.0;
  static double buf[4096]; int n=0; double last=-1;
  for(int i=ts.Size()-1;i>=0; --i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type!=SC_TS_TRADES) continue; if(last>=0){ double dt=(e.DateTime-last)*86400000.0; if(n<4096) buf[n++]=fabs(dt); } last=e.DateTime; }
  if(n<2) return; double m=0; for(int i=0;i<n;++i) m+=buf[i]; m/=n;
  double s2=0; for(int i=0;i<n;++i){ double d=buf[i]-m; s2+=d*d; } s2/=(n-1);
  double cv = (m>0? sqrt(s2)/m : 0.0);
  sc.Subgraph[1][sc.ArraySize-1]=cv;
}
