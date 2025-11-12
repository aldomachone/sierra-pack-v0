// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TAPE_RUNS_MARKOV_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TAPE_RUNS_MARKOV_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
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
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<2 || sc.ArraySize==0) return;
  int win=sc.Input[0].GetInt(); double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd-win/86400000.0;
  int uu=0, ud=0, du=0, dd=0; double last=ts[0].Price; int lastDir=0;
  for(int i=1;i<ts.Size();++i){ const auto& e=ts[i]; if(e.DateTime<tBeg) continue; if(e.Type!=SC_TS_TRADES) continue; int dir=(e.Price>last? +1:(e.Price<last? -1:0)); if(lastDir==+1 && dir==+1) ++uu; if(lastDir==+1 && dir==-1) ++ud; if(lastDir==-1 && dir==+1) ++du; if(lastDir==-1 && dir==-1) ++dd; if(dir!=0) lastDir=dir; last=e.Price; }
  double pContUp = (uu+ud>0? (double)uu/(uu+ud):0.0);
  double pContDn = (dd+du>0? (double)dd/(dd+du):0.0);
  sc.Subgraph[1][sc.ArraySize-1]=pContUp; sc.Subgraph[2][sc.ArraySize-1]=pContDn;
}
