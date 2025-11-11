// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ROLL_SPREAD_ESTIMATOR_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ROLL_SPREAD_ESTIMATOR_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(200);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; if(idx<3) return; int W=sc.Input[0].GetInt(); int s=(idx-W>2? idx-W:2);
  double num=0; int n=0; for(int i=s;i<=idx-1;++i){ double r1=sc.Close[i]-sc.Close[i-1]; double r2=sc.Close[i+1]-sc.Close[i]; num += r1*r2; ++n; }
  double cov = (n>0? num/n:0.0); double spread = (cov<0? 2.0*sqrt(fabs(cov)) : 0.0);
  sc.Subgraph[1][idx]=spread;
}
