// ============================================================================
// Pack v0 — Engines v0 (variants supplémentaires)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ZONE_CONFLUENCE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ZONE_CONFLUENCE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Subgraph[7].Name = "SG07";
    sc.Subgraph[7].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[7].DrawZeros = false;
    sc.Subgraph[7].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[8].Name = "SG08";
    sc.Subgraph[8].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[8].DrawZeros = false;
    sc.Subgraph[8].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Période VWAP"; sc.Input[0].SetInt(50); sc.Input[0].SetIntLimits(2,100000);
    sc.Input[1].Name="02. ATR n"; sc.Input[1].SetInt(14); sc.Input[1].SetIntLimits(1,10000);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=1) return; int idx=sc.ArraySize-1; int per=sc.Input[0].GetInt();
  int s=idx-per; if(s<0) s=0; double pSum=0,vSum=0; for(int i=s;i<=idx;++i){ pSum+=sc.Close[i]*sc.Volume[i]; vSum+=sc.Volume[i]; }
  double vwap=(vSum>0? pSum/vSum:0.0);
  double H=sc.High[idx], L=sc.Low[idx], C=sc.Close[idx]; double P=(H+L+C)/3.0;
  int N=sc.Input[1].GetInt(); int ss=idx-N; if(ss<1) ss=1; double trSum=0; for(int k=ss;k<=idx;++k){ double tr=fmax(sc.High[k]-sc.Low[k], fmax(fabs(sc.High[k]-sc.Close[k-1]), fabs(sc.Low[k]-sc.Close[k-1]))); trSum+=tr; } double atr=(idx-ss+1>0? trSum/(idx-ss+1):0.0);
  if(atr<=0) atr=1.0;
  double score = 2.0/(1.0+fabs(C-vwap)/atr) + 2.0/(1.0+fabs(C-P)/atr);
  sc.Subgraph[1][idx]=score; sc.Subgraph[2][idx]=vwap; sc.Subgraph[3][idx]=P; sc.Subgraph[4][idx]=atr;
}
