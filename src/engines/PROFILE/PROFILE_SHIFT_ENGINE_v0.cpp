// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PROFILE_SHIFT_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PROFILE_SHIFT_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Lookback"; sc.Input[0].SetInt(100);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=0) return; int idx=sc.ArraySize-1; int lb=sc.Input[0].GetInt(); int s=idx-lb; if(s<0) s=0;
  double best=-1, poc=sc.Close[s]; for(int i=s;i<=idx;++i){ if(sc.Volume[i]>best){ best=sc.Volume[i]; poc=sc.Close[i]; } }
  static double prev=0; double shift=poc - prev; prev=poc;
  sc.Subgraph[1][idx]=shift;
}
