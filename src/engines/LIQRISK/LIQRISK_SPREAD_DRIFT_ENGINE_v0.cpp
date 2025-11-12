// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_LIQRISK_SPREAD_DRIFT_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited=sc.GetPersistentInt(1); double& ema=sc.GetPersistentDouble(2);
  if(sc.SetDefaults){
    sc.GraphName="LIQRISK_SPREAD_DRIFT_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. EMA %"; sc.Input[0].SetInt(85);
    sc.DrawZeros=false; return;
  }
  if(!inited||sc.IsFullRecalculation){ inited=1; ema=0; }
  if(sc.ArraySize<=1 || sc.TickSize<=0) return; int idx=sc.ArraySize-1;
  double spreadTicks = (sc.Ask[idx]-sc.Bid[idx])/sc.TickSize; double a=sc.Input[0].GetInt()/100.0; ema=a*spreadTicks + (1-a)*ema;
  sc.Subgraph[1][idx]=ema;
}
