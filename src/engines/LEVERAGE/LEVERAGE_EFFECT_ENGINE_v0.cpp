// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #14)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_LEVERAGE_EFFECT_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="LEVERAGE_EFFECT_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int s=(idx-W>2? idx-W:2);
  const int MAX=8192; static double r[MAX], dv[MAX]; int n=0;
  for(int i=s;i<=idx; ++i){ if(n<MAX){ double ri=sc.Close[i]-sc.Close[i-1]; double vi=fabs(sc.Close[i]-sc.Close[i-1]); double vim1=fabs(sc.Close[i-1]-sc.Close[i-2]); r[n]=ri; dv[n]=vi-vim1; ++n; } }
  if(n<3) return; double mr=0,mv=0; for(int i=0;i<n;++i){ mr+=r[i]; mv+=dv[i]; } mr/=n; mv/=n;
  double num=0, vr=0, vv=0; for(int i=0;i<n;++i){ double dr=r[i]-mr, dvv=dv[i]-mv; num+=dr*dvv; vr+=dr*dr; vv+=dvv*dvv; }
  double corr = (vr>0 && vv>0? num/sqrt(vr*vv):0.0);
  sc.Subgraph[1][idx]=corr;
}
