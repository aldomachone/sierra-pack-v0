// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #4)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PRF_SHOCK_RESPONSE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PRF_SHOCK_RESPONSE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Seuil volume bar"; sc.Input[0].SetFloat(2.0f);
    sc.Input[1].Name="02. Horizon barres"; sc.Input[1].SetInt(5);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=1) return; int idx=sc.ArraySize-1; int H=sc.Input[1].GetInt();
  double v=sc.Volume[idx]; double mv=0; int n=min(idx,50); for(int k=idx-n;k<idx;++k) mv+=sc.Volume[k]; if(n>0) mv/=n;
  int future=min(sc.ArraySize-1, idx+H); double resp = sc.Close[future]-sc.Close[idx];
  sc.Subgraph[1][idx] = (mv>0 && v/mv>=sc.Input[0].GetFloat()? resp: 0.0);
}
