// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ENTROPY_RETURNS_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ENTROPY_RETURNS_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(200);
    sc.Input[1].Name="02. Bins"; sc.Input[1].SetInt(16);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int B=sc.Input[1].GetInt(); if(idx<2||B<2) return; int s=(idx-W>1? idx-W:1);
  double mn=1e18,mx=-1e18; for(int i=s;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; if(r<mn) mn=r; if(r>mx) mx=r; }
  if(mx<=mn) return; double H=0; for(int b=0;b<B;++b){ int c=0; double lo=mn + (mx-mn)*b/B; double hi=mn + (mx-mn)*(b+1)/B; for(int i=s;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; if(r>=lo && r<hi) ++c; } double p=(double)c/(W>0? W:1); if(p>0) H -= p*log(p); }
  sc.Subgraph[1][idx]=H;
}
