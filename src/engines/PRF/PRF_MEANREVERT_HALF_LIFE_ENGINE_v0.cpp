// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #5)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PRF_MEANREVERT_HALF_LIFE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PRF_MEANREVERT_HALF_LIFE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre moyenne"; sc.Input[0].SetInt(50);
    sc.Input[1].Name="02. Horizon futur"; sc.Input[1].SetInt(10);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=1) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int H=sc.Input[1].GetInt();
  int s=idx-W; if(s<0) s=0; double m=0; int n=0; for(int i=s;i<=idx;++i){ m+=sc.Close[i]; ++n; } m /= (n>0? n:1);
  double d0=fabs(sc.Close[idx]-m); if(d0<=0){ sc.Subgraph[1][idx]=0; return; }
  int half=-1; for(int h=1; h<=H && idx+h<sc.ArraySize; ++h){ if(fabs(sc.Close[idx+h]-m)<=0.5*d0){ half=h; break; } }
  sc.Subgraph[1][idx]=(half>=0? half:0);
}
