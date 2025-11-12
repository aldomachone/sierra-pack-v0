// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #13)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_QUOTE_TRADE_LAG_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="QUOTE_TRADE_LAG_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";\n    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[1].DrawZeros = false;\n    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[2].Name = "SG02";\n    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[2].DrawZeros = false;\n    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[3].Name = "SG03";\n    sc.Subgraph[3].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[3].DrawZeros = false;\n    sc.Subgraph[3].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[4].Name = "SG04";\n    sc.Subgraph[4].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[4].DrawZeros = false;\n    sc.Subgraph[4].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Niveaux surveillés"; sc.Input[0].SetInt(1);
    sc.Input[1].Name="02. Fenêtre ms"; sc.Input[1].SetInt(1500);
    sc.DrawZeros=false; return;
  }
  static double lastDomTime=0;
  static double lags[512]; static int ln=0;
  // detect DOM top-of-book change time
  if(sc.GetBidMarketDepthNumberOfLevels()>0 && sc.GetAskMarketDepthNumberOfLevels()>0){
    s_MarketDepthEntry md{}; sc.GetBidMarketDepthEntryAtLevel(md,0); double pb=md.Price; double qb=md.Quantity;
    sc.GetAskMarketDepthEntryAtLevel(md,0); double pa=md.Price; double qa=md.Quantity;
    static double pb0=0,pa0=0,qb0=0,qa0=0;
    if(pb!=pb0 || pa!=pa0 || qb!=qb0 || qa!=qa0){ lastDomTime = sc.CurrentSystemDateTime; pb0=pb; pa0=pa; qb0=qb; qa0=qa; }
  }
  // scan recent trades and measure lag since lastDomTime
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<1 || lastDomTime==0) return;
  double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd - sc.Input[1].GetInt()/86400000.0;
  for(int i=ts.Size()-1;i>=0; --i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type!=SC_TS_TRADES) continue;
    double lag = (e.DateTime - lastDomTime) * 86400000.0; if(lag>=0 && ln<512){ lags[ln++]=lag; } }
  if(ln>0){
    // compute median
    int m=ln//2; #define SWAP(a,b){double t=a;a=b;b=t;}
    #definemin(a,b) ((a)<(b)?(a):(b))
    // partial selection (naive)
    for(int i=0;i<=m; ++i){ int mi=i; for(int j=i+1;j<ln;++j) if(lags[j]<lags[mi]) mi=j; SWAP(lags[i], lags[mi]); }
    double med = (ln%2? lags[m] : 0.5*(lags[m-1]+lags[m]));
    sc.Subgraph[1][sc.ArraySize-1]=med;
    ln=0;
  }
}
