// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #9)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ZONE_MULTI_REF_CONFLUENCE2_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ZONE_MULTI_REF_CONFLUENCE2_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. ATR n"; sc.Input[0].SetInt(14);
    sc.Input[1].Name="02. Tolérance ATRx"; sc.Input[1].SetFloat(0.6f);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<2) return; int idx=sc.ArraySize-1;
  int N=sc.Input[0].GetInt(); int s=max(1, idx-N); double tr=0; int c=0; for(int k=s;k<=idx;++k){ tr+=fmax(sc.High[k]-sc.Low[k], fmax(fabs(sc.High[k]-sc.Close[k-1]), fabs(sc.Low[k]-sc.Close[k-1]))); ++c; } double atr=(c>0? tr/c:1.0);
  double tol=sc.Input[1].GetFloat()*atr; double px=sc.Close[idx];
  double P=(sc.High[idx-1]+sc.Low[idx-1]+sc.Close[idx-1])/3.0;
  double C1=sc.Close[idx-1]; double MR=(sc.High[idx]+sc.Low[idx])*0.5;
  int score=0; if(fabs(px-P)<=tol) ++score; if(fabs(px-C1)<=tol) ++score; if(fabs(px-MR)<=tol) ++score;
  sc.Subgraph[1][idx]=score; sc.Subgraph[2][idx]=atr;
}
