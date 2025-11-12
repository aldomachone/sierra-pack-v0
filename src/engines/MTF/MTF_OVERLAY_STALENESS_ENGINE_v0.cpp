// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #10)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_MTF_OVERLAY_STALENESS_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& age=sc.GetPersistentInt(1);
  if(sc.SetDefaults){
    sc.GraphName="MTF_OVERLAY_STALENESS_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Age max bars"; sc.Input[0].SetInt(5);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; if(sc.IsFullRecalculation) age=0;
  ++age; if(age>sc.Input[0].GetInt()) age=sc.Input[0].GetInt();
  sc.Subgraph[1][idx]=age;
}
