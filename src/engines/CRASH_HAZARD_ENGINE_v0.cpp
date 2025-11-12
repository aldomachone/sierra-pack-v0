// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_CRASH_HAZARD_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="CRASH_HAZARD_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[1].Name="02. Seuil σ"; sc.Input[1].SetFloat(3.0f);
    sc.DrawZeros=false; return;
  }
  int idx=sc.ArraySize-1; if(idx<5) return; int W=sc.Input[0].GetInt(); int s=(idx-W>1? idx-W:1);
  // std of returns
  double m=0; int n=0; for(int i=s;i<=idx;++i){ m+=sc.Close[i]-sc.Close[i-1]; ++n; } m/=n;
  double v=0; for(int i=s;i<=idx;++i){ double d=(sc.Close[i]-sc.Close[i-1])-m; v+=d*d; } v/=n; double sd=sqrt(v);
  int hits=0; for(int i=s;i<=idx;++i){ double d=fabs((sc.Close[i]-sc.Close[i-1])-m); if(sd>0 && d>sc.Input[1].GetFloat()*sd) ++hits; }
  sc.Subgraph[1][idx]=(double)hits/(double)(n>0?n:1);
}
