// ============================================================================
// Pack v0 — Engines v0 (auto-generated)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TAPE_CLUSTER_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TAPE_CLUSTER_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0; sc.MaintainTimeAndSalesData=1;
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
    sc.Subgraph[7].Name = "SG07";
    sc.Subgraph[7].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[7].DrawZeros = false;
    sc.Subgraph[7].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[8].Name = "SG08";
    sc.Subgraph[8].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[8].DrawZeros = false;
    sc.Subgraph[8].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Seuil taille"; sc.Input[0].SetInt(10); sc.Input[0].SetIntLimits(1,1000000);
    sc.Input[1].Name="02. Fenêtre ms"; sc.Input[1].SetInt(800); sc.Input[1].SetIntLimits(50,10000);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()==0 || sc.ArraySize==0) return;
  int thr=sc.Input[0].GetInt(); int win=sc.Input[1].GetInt();
  double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd-win/86400000.0; int clusters=0; int cur=0;
  for(int i=ts.Size()-1;i>=0;--i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type!=SC_TS_TRADES) continue; if(e.Volume>=thr) ++cur; else if(cur>0){ ++clusters; cur=0; } }
  if(cur>0) ++clusters;
  int idx=sc.ArraySize-1; sc.Subgraph[1][idx]=clusters;
}
