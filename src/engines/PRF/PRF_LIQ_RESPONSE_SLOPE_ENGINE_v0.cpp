// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #7)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PRF_LIQ_RESPONSE_SLOPE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PRF_LIQ_RESPONSE_SLOPE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(30);
    sc.Input[1].Name="02. Seuil volume x"; sc.Input[1].SetFloat(1.5f);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=1) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt();
  int s=max(1, idx-W); double mv=0; int n=idx-s; for(int k=s;k<idx;++k) mv+=sc.Volume[k]; if(n>0) mv/=n;
  double x=0,y=0,xx=0,xy=0; int m=0;
  for(int i=s;i<=idx;++i){ if(mv>0 && sc.Volume[i]/mv>=sc.Input[1].GetFloat()){ double t=i-s; x+=t; y+=sc.Close[i]; xx+=t*t; xy+=t*sc.Close[i]; ++m; } }
  double den=m*xx - x*x; double slope=(den!=0? (m*xy - x*y)/den:0.0);
  sc.Subgraph[1][idx]=slope;
}
