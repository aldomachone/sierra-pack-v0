// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #4)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_OHLC_MTF_STATE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="OHLC_MTF_STATE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Lookback court"; sc.Input[0].SetInt(20);
    sc.Input[1].Name="02. Lookback long"; sc.Input[1].SetInt(80);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=0) return; int idx=sc.ArraySize-1;
  auto swing=[&](int N){ int s=idx-N; if(s<0) s=0; double hi=-1e300, lo=1e300; for(int i=s;i<=idx;++i){ if(sc.High[i]>hi) hi=sc.High[i]; if(sc.Low[i]<lo) lo=sc.Low[i]; } return std::make_pair(lo,hi); };
  auto s1=swing(sc.Input[0].GetInt()); auto s2=swing(sc.Input[1].GetInt());
  double px=sc.Close[idx]; double stS = (px>=s1.second? +1:(px<=s1.first? -1:0)); double stL=(px>=s2.second? +1:(px<=s2.first? -1:0));
  sc.Subgraph[1][idx]=stS; sc.Subgraph[2][idx]=stL;
}
