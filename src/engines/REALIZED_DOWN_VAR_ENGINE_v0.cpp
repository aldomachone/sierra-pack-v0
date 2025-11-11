// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT4: Risk&Regime, Math&Finance)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_REALIZED_DOWN_VAR_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="REALIZED_DOWN_VAR_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26;
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
  int idx=sc.ArraySize-1; if(idx<2) return; int W=sc.Input[0].GetInt(); int s=(idx-W>1? idx-W:1);
  double m=0; int n=0; for(int i=s;i<=idx;++i){ m+=sc.Close[i]-sc.Close[i-1]; ++n; } m/= (n>0? n:1);
  double dv=0; for(int i=s;i<=idx;++i){ double r=(sc.Close[i]-sc.Close[i-1])-m; if(r<0) dv+=r*r; }
  sc.Subgraph[1][idx]=dv/(double)(n>0?n:1);
}
