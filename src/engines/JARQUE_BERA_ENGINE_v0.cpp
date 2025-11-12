// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_JARQUE_BERA_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="JARQUE_BERA_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int s=(idx-W>1? idx-W:1);
  const int MAX=8192; static double r[MAX]; int n=0; double m=0;
  for(int i=s;i<=idx;++i){ if(n<MAX){ r[n]=sc.Close[i]-sc.Close[i-1]; m+=r[n]; ++n; } }
  if(n<5) return; m/=n;
  double v=0; for(int i=0;i<n;++i){ double d=r[i]-m; v+=d*d; } v/=n;
  if(v<=0) return;
  double s3=0,s4=0; for(int i=0;i<n;++i){ double z=(r[i]-m)/sqrt(v); s3+=z*z*z; s4+=z*z*z*z; }
  s3/=n; s4/=n;
  double JB = n*(s3*s3/6.0 + (s4-3.0)*(s4-3.0)/24.0);
  sc.Subgraph[1][idx]=JB;
}
