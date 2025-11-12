// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TRADE_SIZE_ENTROPY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TRADE_SIZE_ENTROPY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre ms"; sc.Input[0].SetInt(3000);
    sc.Input[1].Name="02. Bins"; sc.Input[1].SetInt(16);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<3) return;
  double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd - sc.Input[0].GetInt()/86400000.0; int B=sc.Input[1].GetInt(); if(B<2) B=2;
  double mn=1e18, mx=-1e18; for(int i=ts.Size()-1;i>=0; --i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type!=SC_TS_TRADES) continue; if(e.Volume<mn) mn=e.Volume; if(e.Volume>mx) mx=e.Volume; }
  if(mx<=mn) return; double H=0;
  for(int b=0;b<B;++b){ int c=0; double lo=mn + (mx-mn)*b/B; double hi=mn + (mx-mn)*(b+1)/B; for(int i=ts.Size()-1;i>=0; --i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type!=SC_TS_TRADES) continue; if(e.Volume>=lo && e.Volume<hi) ++c; } double p=(double)c/(double)B; if(p>0) H -= p*log(p); }
  sc.Subgraph[1][sc.ArraySize-1]=H;
}
