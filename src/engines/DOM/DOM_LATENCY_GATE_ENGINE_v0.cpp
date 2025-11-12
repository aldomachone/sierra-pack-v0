// ============================================================================
// Pack v0 — Engines v0 (auto-generated)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_LATENCY_GATE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_LATENCY_GATE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Lag max ms"; sc.Input[0].SetInt(150); sc.Input[0].SetIntLimits(0,5000);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; if(idx<0) return;
  double lag=0.0; // placeholder
  sc.Subgraph[1][idx] = (lag <= sc.Input[0].GetInt()? 1.0 : 0.0); sc.Subgraph[2][idx]=lag;
}
