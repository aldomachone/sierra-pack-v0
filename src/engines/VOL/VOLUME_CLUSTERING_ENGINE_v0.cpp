// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VOLUME_CLUSTERING_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="VOLUME_CLUSTERING_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Lag"; sc.Input[0].SetInt(1);
    sc.Input[1].Name="02. Fenêtre bars"; sc.Input[1].SetInt(200);
    sc.DrawZeros=false; return;
  }
  int L=sc.Input[0].GetInt(); int idx=sc.ArraySize-1; int W=sc.Input[1].GetInt(); if(idx-L<0) return; int s=(idx-W>0? idx-W:0);
  double m=0; int n=0; for(int i=s;i<=idx;++i){ m+=sc.Volume[i]; ++n; } m/= (n>0? n:1);
  double num=0, den=0; for(int i=s+L;i<=idx;++i){ double a=sc.Volume[i]-m; double b=sc.Volume[i-L]-m; num+=a*b; den+=a*a; }
  sc.Subgraph[1][idx]=(den>0? num/den : 0.0);
}
