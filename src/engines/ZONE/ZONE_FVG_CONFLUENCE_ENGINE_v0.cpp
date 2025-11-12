// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #10)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ZONE_FVG_CONFLUENCE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ZONE_FVG_CONFLUENCE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Période VWAP"; sc.Input[0].SetInt(60);
    sc.Input[1].Name="02. ATR n"; sc.Input[1].SetInt(14);
    sc.Input[2].Name="03. Tolérance ATRx"; sc.Input[2].SetFloat(0.5f);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1;
  double f=0; if(sc.Low[idx] > sc.High[idx-2]) f= (sc.Low[idx]-sc.High[idx-2]); else if(sc.Low[idx-2] > sc.High[idx]) f= (sc.Low[idx-2]-sc.High[idx]);
  int per=sc.Input[0].GetInt(); int s=(idx-per>0? idx-per:0); double pSum=0,vSum=0; for(int i=s;i<=idx;++i){ pSum+=sc.Close[i]*sc.Volume[i]; vSum+=sc.Volume[i]; } double vwap=(vSum>0? pSum/vSum:sc.Close[idx]);
  int N=sc.Input[1].GetInt(); int sa=(idx-N>1? idx-N:1); double tr=0; int c=0; for(int k=sa;k<=idx;++k){ tr+=fmax(sc.High[k]-sc.Low[k], fmax(fabs(sc.High[k]-sc.Close[k-1]), fabs(sc.Low[k]-sc.Close[k-1]))); ++c; } double atr=(c>0? tr/c:1.0);
  double tol=sc.Input[2].GetFloat()*atr; double mr=(sc.High[idx]+sc.Low[idx])*0.5; double P=(sc.High[idx-1]+sc.Low[idx-1]+sc.Close[idx-1])/3.0;
  int score=0; if(f>0){ if(fabs(vwap-mr)<=tol) ++score; if(fabs(vwap-P)<=tol) ++score; if(fabs(mr-P)<=tol) ++score; }
  sc.Subgraph[1][idx]=score; sc.Subgraph[2][idx]=f; sc.Subgraph[3][idx]=vwap;
}
