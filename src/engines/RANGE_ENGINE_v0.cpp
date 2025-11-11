// ============================================================================
// Pack v0 — Engine de base (v0)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_RANGE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if (sc.SetDefaults)
  {
    sc.GraphName = "RANGE_ENGINE_v0";
    sc.AutoLoop = 0; sc.UpdateAlways = 0; sc.GraphRegion = 0; sc.ValueFormat = 26; sc.FreeDLL = 0;
    for (int i=1;i<=8;++i){ sc.Subgraph[i].Name = "SG%02d"; sc.Subgraph[i].DrawStyle = DRAWSTYLE_IGNORE; sc.Subgraph[i].DrawZeros=false; sc.Subgraph[i].DisplayAsMainPriceGraphValue=0; }
    sc.Input[0].Name="01. Lookback barres"; sc.Input[0].SetInt(20); sc.Input[0].SetIntLimits(1,100000);
    sc.DrawZeros=false; return;
  }
  if (sc.ArraySize<=0) return;
  int lb=sc.Input[0].GetInt(); int start=sc.ArraySize-lb; if(start<0) start=0;
  double hi=-1e300, lo=1e300; for(int i=start;i<sc.ArraySize;++i){ if(sc.High[i]>hi) hi=sc.High[i]; if(sc.Low[i]<lo) lo=sc.Low[i]; }
  int idx=sc.ArraySize-1; sc.Subgraph[1][idx]=hi; sc.Subgraph[2][idx]=lo; sc.Subgraph[3][idx]=hi-lo;
}
