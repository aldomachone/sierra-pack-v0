// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #13)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TSRV_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TSRV_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";\n    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[1].DrawZeros = false;\n    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[2].Name = "SG02";\n    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[2].DrawZeros = false;\n    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[3].Name = "SG03";\n    sc.Subgraph[3].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[3].DrawZeros = false;\n    sc.Subgraph[3].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[4].Name = "SG04";\n    sc.Subgraph[4].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[4].DrawZeros = false;\n    sc.Subgraph[4].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(200);
    sc.Input[1].Name="02. Pas large m"; sc.Input[1].SetInt(5);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int m=sc.Input[1].GetInt(); if(m<2) m=2;
  int s=(idx-W>m? idx-W: m);
  // RV fine
  double RV=0; for(int i=s;i<=idx; ++i){ double r=sc.Close[i]-sc.Close[i-1]; RV += r*r; }
  // RV subsampled every m
  double RVm=0; for(int i=s;i<=idx; i+=m){ if(i-m<0) continue; double r=sc.Close[i]-sc.Close[i-m]; RVm += r*r; }
  double k = (double)m;
  double TSRV = RVm/k - (1.0 - 1.0/k) * RV;
  sc.Subgraph[1][idx]=TSRV;
}
