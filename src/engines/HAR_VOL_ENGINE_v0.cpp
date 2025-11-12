// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_HAR_VOL_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="HAR_VOL_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre courte"; sc.Input[0].SetInt(5);
    sc.Input[1].Name="02. Fenêtre moyenne"; sc.Input[1].SetInt(20);
    sc.Input[2].Name="03. Fenêtre longue"; sc.Input[2].SetInt(60);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; if(idx<2) return;
  auto rv=[&](int W){ double s=0; int i0=(idx-W>1? idx-W:1); for(int i=i0;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; s+=r*r; } return s/(double)W; };
  double s=rv(sc.Input[0].GetInt()); double m=rv(sc.Input[1].GetInt()); double l=rv(sc.Input[2].GetInt());
  sc.Subgraph[1][idx]=s; sc.Subgraph[2][idx]=m; sc.Subgraph[3][idx]=l;
}
