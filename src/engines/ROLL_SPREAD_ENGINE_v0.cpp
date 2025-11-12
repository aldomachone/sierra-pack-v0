// ============================================================================
// Pack v0 — Engines v0 (lot HFT formulas #11)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ROLL_SPREAD_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ROLL_SPREAD_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(60);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int s=(idx-W>2? idx-W:2);
  double m1=0,m2=0, c=0; int n=0;
  for(int i=s;i<=idx;++i){ double dp = sc.Close[i]-sc.Close[i-1]; double dpm1 = sc.Close[i-1]-sc.Close[i-2]; m1+=dp; m2+=dpm1; ++n; }
  if(n==0) return; m1/=n; m2/=n;
  for(int i=s;i<=idx;++i){ double dp = sc.Close[i]-sc.Close[i-1]; double dpm1 = sc.Close[i-1]-sc.Close[i-2]; c += (dp-m1)*(dpm1-m2); }
  double cov = (n>1? c/(n-1):0.0);
  double spread = (cov<0? 2.0*sqrt(-cov) : 0.0);
  sc.Subgraph[1][idx]=spread;
}
