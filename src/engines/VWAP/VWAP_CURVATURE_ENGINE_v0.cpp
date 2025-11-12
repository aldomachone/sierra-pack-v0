// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #7)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VWAP_CURVATURE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="VWAP_CURVATURE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Période"; sc.Input[0].SetInt(60);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int per=sc.Input[0].GetInt(); int s=idx-per; if(s<2) s=2;
  auto vwapAt=[&](int k){ double pSum=0,vSum=0; for(int i=s;i<=k;++i){ pSum+=sc.Close[i]*sc.Volume[i]; vSum+=sc.Volume[i]; } return (vSum>0? pSum/vSum:sc.Close[k]); };
  double v0=vwapAt(idx-2), v1=vwapAt(idx-1), v2=vwapAt(idx);
  double curvature = v2 - 2*v1 + v0;
  sc.Subgraph[1][idx]=curvature;
}
