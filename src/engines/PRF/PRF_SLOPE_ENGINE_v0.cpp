// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #6)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PRF_SLOPE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PRF_SLOPE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Période"; sc.Input[0].SetInt(20);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=1) return; int idx=sc.ArraySize-1; int N=sc.Input[0].GetInt(); int s=idx-N; if(s<0) s=0;
  double x=0,y=0,xx=0,xy=0; int n=0;
  for(int i=s;i<=idx;++i){ double t=i-s; x+=t; y+=sc.Close[i]; xx+=t*t; xy+=t*sc.Close[i]; ++n; }
  double den=n*xx - x*x; double slope=(den!=0? (n*xy - x*y)/den:0.0);
  sc.Subgraph[1][idx]=slope;
}
