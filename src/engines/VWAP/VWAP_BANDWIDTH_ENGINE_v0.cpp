// ============================================================================
// Pack v0 — Engines v0 (compléments supplémentaires)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VWAP_BANDWIDTH_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="VWAP_BANDWIDTH_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Période"; sc.Input[0].SetInt(50); sc.Input[0].SetIntLimits(2,100000);
    sc.Input[1].Name="02. σ multiples"; sc.Input[1].SetFloat(1.0f);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=0) return;
  int per=sc.Input[0].GetInt(); int s=sc.ArraySize-per; if(s<0) s=0;
  double pSum=0,vSum=0; for(int i=s;i<sc.ArraySize;++i){ pSum+=sc.Close[i]*sc.Volume[i]; vSum+=sc.Volume[i]; }
  double vwap=(vSum>0? pSum/vSum:0.0);
  double var=0; for(int i=s;i<sc.ArraySize;++i){ double d=sc.Close[i]-vwap; var+=d*d*sc.Volume[i]; } var=(vSum>0? var/vSum:0.0);
  double sd=(var>0? sqrt(var):0.0); double k=sc.Input[1].GetFloat();
  int idx=sc.ArraySize-1; sc.Subgraph[1][idx]=fabs(sc.Close[idx]-vwap)/(sd>0? k*sd:1.0);
}
