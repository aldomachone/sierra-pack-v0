// ============================================================================
// Pack v0 — Engines v0 (auto-generated)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_LIQRISK_KILLSWITCH_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="LIQRISK_KILLSWITCH_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Max spread ticks"; sc.Input[0].SetInt(3); sc.Input[0].SetIntLimits(0,1000);
    sc.Input[1].Name="02. Max lag ms"; sc.Input[1].SetInt(250); sc.Input[1].SetIntLimits(0,5000);
    sc.Input[2].Name="03. Min range ATR"; sc.Input[2].SetFloat(0.5f);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=1) return; int idx=sc.ArraySize-1;
  double spreadTicks = (sc.Ask[idx]-sc.Bid[idx])/sc.TickSize;
  double lag=0.0; // placeholder
  // ATR proxy
  int N=14; int s=idx-N; if(s<1) s=1; double trSum=0; for(int k=s;k<=idx;++k){ double tr=fmax(sc.High[k]-sc.Low[k], fmax(fabs(sc.High[k]-sc.Close[k-1]), fabs(sc.Low[k]-sc.Close[k-1]))); trSum+=tr; } double atr=(idx-s+1>0? trSum/(idx-s+1):0.0);
  double rng = sc.High[idx]-sc.Low[idx];
  int ok = (spreadTicks<=sc.Input[0].GetInt() and lag<=sc.Input[1].GetInt() and (atr>0? rng/atr:0.0) >= sc.Input[2].GetFloat()) ? 1:0;
  sc.Subgraph[1][idx]= (ok? 0.0: 1.0); // Kill=1 si KO
  sc.Subgraph[2][idx]= (ok? 0.0: (spreadTicks>sc.Input[0].GetInt()? 1.0 : ( (atr>0? rng/atr:0.0) < sc.Input[2].GetFloat()? 2.0 : 3.0 )));
}
