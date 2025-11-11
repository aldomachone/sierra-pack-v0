// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT4: Risk&Regime, Math&Finance)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PRICE_JUMP_DETECT_BNS_PROXY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PRICE_JUMP_DETECT_BNS_PROXY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(120);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); if(idx<3) return; int s=(idx-W>1? idx-W:1);
  double rv=0,rq=0; for(int i=s;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; rv+=r*r; double ar=fabs(r); rq+=ar*ar*ar*ar; }
  double stat = (rq>0? rv/sqrt(rq):0.0);
  sc.Subgraph[1][idx]=stat;
}
