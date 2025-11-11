// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #8)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_LIQRISK_SPREAD_DRIFT_SLOPE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  double& prev=sc.GetPersistentDouble(1);
  if(sc.SetDefaults){
    sc.GraphName="LIQRISK_SPREAD_DRIFT_SLOPE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Lissage n"; sc.Input[0].SetInt(5);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=0 || sc.TickSize<=0) return; int idx=sc.ArraySize-1;
  double spr=(sc.Ask[idx]-sc.Bid[idx])/sc.TickSize;
  static double ema=0; double a=1.0/max(1, sc.Input[0].GetInt()); ema=a*spr+(1-a)*ema;
  double slope = (sc.IsFullRecalculation? 0.0 : ema - prev); prev=ema;
  sc.Subgraph[1][idx]=slope; sc.Subgraph[2][idx]=ema;
}
