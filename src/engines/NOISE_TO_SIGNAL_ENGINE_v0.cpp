// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_NOISE_TO_SIGNAL_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="NOISE_TO_SIGNAL_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(300);
    sc.Input[1].Name="02. Pas large m"; sc.Input[1].SetInt(4);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int m=sc.Input[1].GetInt(); if(m<2)m=2;
  int s=(idx-W>m? idx-W: m);
  double RV=0,RVm=0; for(int i=s;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; RV+=r*r; }
  for(int i=s;i<=idx;i+=m){ if(i-m<0) continue; double r=sc.Close[i]-sc.Close[i-m]; RVm+=r*r; }
  double ns = (RVm/m>0? (RV - RVm/m) / (RVm/m) : 0.0);
  sc.Subgraph[1][idx]=ns;
}
