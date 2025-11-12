// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #14)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ZHANG_NOISE_VARIANCE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ZHANG_NOISE_VARIANCE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[1].Name="02. Pas large m"; sc.Input[1].SetInt(5);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int m=sc.Input[1].GetInt(); if(m<2) m=2;
  int s=(idx-W>m? idx-W: m);
  double RV=0; for(int i=s;i<=idx; ++i){ double r=sc.Close[i]-sc.Close[i-1]; RV += r*r; }
  double RVm=0; for(int i=s;i<=idx; i+=m){ if(i-m<0) continue; double r=sc.Close[i]-sc.Close[i-m]; RVm += r*r; }
  double k = (double)m;
  double eta2 = (RV - RVm/k) / (2.0*(1.0 - 1.0/k)); // Zhang (proxy)
  sc.Subgraph[1][idx]=eta2;
}
