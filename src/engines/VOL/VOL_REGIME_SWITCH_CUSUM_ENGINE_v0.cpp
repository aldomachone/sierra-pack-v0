// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT4: Risk&Regime, Math&Finance)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VOL_REGIME_SWITCH_CUSUM_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="VOL_REGIME_SWITCH_CUSUM_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. k CUSUM"; sc.Input[0].SetFloat(0.0f);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; if(idx<2) return;
  double r=sc.Close[idx]-sc.Close[idx-1]; double lv=log(r*r+1e-12);
  static double g=0; double k=sc.Input[0].GetFloat();
  g = (g + lv - k); if(g<0) g=0;
  sc.Subgraph[1][idx]=g;
}
