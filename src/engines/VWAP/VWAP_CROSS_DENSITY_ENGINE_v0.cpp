// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #9)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VWAP_CROSS_DENSITY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& cnt=sc.GetPersistentInt(1);
  if(sc.SetDefaults){
    sc.GraphName="VWAP_CROSS_DENSITY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Période"; sc.Input[0].SetInt(80);
    sc.Input[1].Name="02. Demi-vie barres"; sc.Input[1].SetInt(40);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<2) return; int idx=sc.ArraySize-1; int per=sc.Input[0].GetInt(); int s=max(0, idx-per);
  double pSum=0,vSum=0; for(int i=s;i<=idx;++i){ pSum+=sc.Close[i]*sc.Volume[i]; vSum+=sc.Volume[i]; }
  double vw=(vSum>0? pSum/vSum:sc.Close[idx]);
  double prev=sc.Close[idx-1]-vw, cur=sc.Close[idx]-vw;
  double decay = pow(0.5, 1.0/max(1, sc.Input[1].GetInt()));
  cnt = int(cnt*decay);
  if(prev==0 || cur==0 || (prev<0 && cur>0) || (prev>0 && cur<0)) ++cnt;
  sc.Subgraph[1][idx]=cnt; sc.Subgraph[2][idx]=vw;
}
