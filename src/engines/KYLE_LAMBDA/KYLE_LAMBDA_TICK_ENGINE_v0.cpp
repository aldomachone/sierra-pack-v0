// ============================================================================
// Pack v0 — Engines v0 (lot HFT formulas #11)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_KYLE_LAMBDA_TICK_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="KYLE_LAMBDA_TICK_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre ms"; sc.Input[0].SetInt(1000);
    sc.Input[1].Name="02. Near niveaux OFI"; sc.Input[1].SetInt(10);
    sc.DrawZeros=false; return;
  }
  // OFI near-band
  int N=sc.Input[1].GetInt(); s_MarketDepthEntry md{}; double b=0,a=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<N; ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N; ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity; }
  static double pb=0, pa=0; if(sc.Index==0){ pb=b; pa=a; }
  double ofi=(b-pb)-(a-pa); pb=b; pa=a;

  // ΔP over window
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<2 || sc.ArraySize==0) return;
  int win=sc.Input[0].GetInt();
  double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd-win/86400000.0;
  double pFirst=0, pLast=0; for(int i=ts.Size()-1;i>=0; --i){ const auto& e=ts[i]; if(e.Type!=SC_TS_TRADES) continue; if(pLast==0) pLast=e.Price; if(e.DateTime<tBeg){ pFirst=e.Price; break; } pFirst=e.Price; }
  double dP=fabs(pLast - pFirst);
  double lam = (fabs(ofi)>0? dP/fabs(ofi) : 0.0);
  sc.Subgraph[1][sc.ArraySize-1]=lam;         // λ
  sc.Subgraph[2][sc.ArraySize-1]=ofi;         // OFI
  sc.Subgraph[3][sc.ArraySize-1]=dP;          // |ΔP| window
}
