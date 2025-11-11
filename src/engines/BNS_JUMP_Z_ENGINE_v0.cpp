// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #13)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_BNS_JUMP_Z_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="BNS_JUMP_Z_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";\n    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[1].DrawZeros = false;\n    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[2].Name = "SG02";\n    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[2].DrawZeros = false;\n    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[3].Name = "SG03";\n    sc.Subgraph[3].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[3].DrawZeros = false;\n    sc.Subgraph[3].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[4].Name = "SG04";\n    sc.Subgraph[4].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[4].DrawZeros = false;\n    sc.Subgraph[4].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[5].Name = "SG05";\n    sc.Subgraph[5].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[5].DrawZeros = false;\n    sc.Subgraph[5].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[6].Name = "SG06";\n    sc.Subgraph[6].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[6].DrawZeros = false;\n    sc.Subgraph[6].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(120);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int s=(idx-W>1? idx-W:1);
  double RV=0, BP=0, Q=0;
  for(int i=s;i<=idx; ++i){ double r=sc.Close[i]-sc.Close[i-1]; RV += r*r; Q += r*r*r*r; if(i>1){ double a=fabs(sc.Close[i]-sc.Close[i-1]); double b=fabs(sc.Close[i-1]-sc.Close[i-2]); BP += a*b; } }
  double mu1 = sqrt(2.0/M_PI);
  double JV = RV - (mu1*mu1)*BP;
  double varJV = ( (M_PI/2.0)*Q ) / (double)(idx-s+1); // crude proxy
  double Z = (varJV>0? JV / sqrt(varJV) : 0.0);
  sc.Subgraph[1][idx]=Z; sc.Subgraph[2][idx]=RV; sc.Subgraph[3][idx]=BP;
}
