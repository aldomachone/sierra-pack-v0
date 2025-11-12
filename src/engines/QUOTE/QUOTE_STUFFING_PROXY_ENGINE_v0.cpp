// ============================================================================
// Pack v0 — Engines v0 (lot Extra-HFT #15)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_QUOTE_STUFFING_PROXY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="QUOTE_STUFFING_PROXY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre ms"; sc.Input[0].SetInt(800);
    sc.DrawZeros=false; return;
  }
  static int qcnt=0; int win=sc.Input[0].GetInt();
  if(sc.GetBidMarketDepthNumberOfLevels()>0 && sc.GetAskMarketDepthNumberOfLevels()>0){ s_MarketDepthEntry md{}; static double pb0=0,pa0=0,qb0=0,qa0=0; sc.GetBidMarketDepthEntryAtLevel(md,0); double pb=md.Price; double qb=md.Quantity; sc.GetAskMarketDepthEntryAtLevel(md,0); double pa=md.Price; double qa=md.Quantity; if(pb!=pb0||pa!=pa0||qb!=qb0||qa!=qa0){ ++qcnt; pb0=pb; pa0=pa; qb0=qb; qa0=qa; } }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<1) return; double tEnd=ts[ts.Size()-1].DateTime; double tBeg=tEnd - win/86400000.0; int trades=0; for(int i=ts.Size()-1;i>=0; --i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type==SC_TS_TRADES) ++trades; }
  double proxy = (trades>0? qcnt/(double)trades : qcnt);
  sc.Subgraph[1][sc.ArraySize-1]=proxy; qcnt = int(qcnt*0.85);
}
