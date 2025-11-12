// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #5)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VWAP_BANDS_STATE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="VWAP_BANDS_STATE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Période"; sc.Input[0].SetInt(50);
    sc.Input[1].Name="02. σ multiples"; sc.Input[1].SetFloat(1.0f);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=0) return; int idx=sc.ArraySize-1; int per=sc.Input[0].GetInt(); int s=idx-per; if(s<0) s=0;
  double pSum=0,vSum=0; for(int i=s;i<=idx;++i){ pSum+=sc.Close[i]*sc.Volume[i]; vSum+=sc.Volume[i]; }
  double vwap=(vSum>0? pSum/vSum:0.0);
  double var=0; for(int i=s;i<=idx;++i){ double d=sc.Close[i]-vwap; var+=d*d*sc.Volume[i]; } var=(vSum>0? var/vSum:0.0);
  double sd=(var>0? sqrt(var):1.0); double k=sc.Input[1].GetFloat();
  double up=vwap+k*sd, dn=vwap-k*sd, px=sc.Close[idx];
  double state = (px>up? +2.0 : (px>vwap? +1.0 : (px<dn? -2.0 : (px<vwap? -1.0 : 0.0))));
  sc.Subgraph[1][idx]=state; sc.Subgraph[2][idx]=vwap; sc.Subgraph[3][idx]=up; sc.Subgraph[4][idx]=dn;
}
