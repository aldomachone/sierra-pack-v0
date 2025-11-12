// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #8)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TAPE_SIZE_DECILES_RATE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TAPE_SIZE_DECILES_RATE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre ms"; sc.Input[0].SetInt(1200);
    sc.Input[1].Name="02. Décile cible"; sc.Input[1].SetInt(90);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<2 || sc.ArraySize==0) return;
  int win=sc.Input[0].GetInt(); int q=sc.Input[1].GetInt();
  double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd-win/86400000.0;
  // collect window sizes
  static double buf[4096]; int n=0; int hits=0;
  for(int i=ts.Size()-1;i>=0;--i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type!=SC_TS_TRADES) continue; if(n<4096) buf[n++]=e.Volume; }
  if(n==0) return;
  // crude percentile
    // approx percentile by nth element after partial sort
  int kth = (int)(q*(n-1)/100.0); for(int i=0;i<=kth;++i){ int maxj=i; for(int j=i+1;j<n;++j) if(buf[j]>buf[maxj]) maxj=j; double tmp=buf[i]; buf[i]=buf[maxj]; buf[maxj]=tmp; } double p=buf[kth];
  for(int i=ts.Size()-1;i>=0;--i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type!=SC_TS_TRADES) continue; if(e.Volume>=p) ++hits; }
  sc.Subgraph[1][sc.ArraySize-1]= (double)hits / (double)win;
}
