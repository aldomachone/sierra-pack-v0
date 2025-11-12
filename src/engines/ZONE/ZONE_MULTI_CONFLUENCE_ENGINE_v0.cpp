// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #4)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ZONE_MULTI_CONFLUENCE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ZONE_MULTI_CONFLUENCE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Tolérance ATRx"; sc.Input[0].SetFloat(1.0f);
    sc.Input[1].Name="02. ATR n"; sc.Input[1].SetInt(14);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=1) return; int idx=sc.ArraySize-1;
  int N=sc.Input[1].GetInt(); int s=idx-N; if(s<1) s=1; double tr=0; int c=0; for(int k=s;k<=idx;++k){ tr+=fmax(sc.High[k]-sc.Low[k], fmax(fabs(sc.High[k]-sc.Close[k-1]), fabs(sc.Low[k]-sc.Close[k-1]))); ++c; } double atr=(c>0? tr/c:1.0);
  double tol=sc.Input[0].GetFloat()*atr;
  double ref1=(sc.High[idx]+sc.Low[idx]+sc.Close[idx])/3.0; // pivot proxy P
  double ref2=sc.Close[idx-1]; // close-1
  double ref3=(sc.High[idx]-sc.Low[idx])*0.5 + sc.Low[idx]; // mid range
  double near=0; if(fabs(sc.Close[idx]-ref1)<=tol) near+=1; if(fabs(sc.Close[idx]-ref2)<=tol) near+=1; if(fabs(sc.Close[idx]-ref3)<=tol) near+=1;
  sc.Subgraph[1][idx]=near; sc.Subgraph[2][idx]=atr;
}
