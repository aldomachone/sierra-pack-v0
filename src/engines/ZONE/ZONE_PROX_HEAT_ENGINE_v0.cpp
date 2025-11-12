// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #7)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ZONE_PROX_HEAT_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& cnt=sc.GetPersistentInt(1);
  if(sc.SetDefaults){
    sc.GraphName="ZONE_PROX_HEAT_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Largeur ticks"; sc.Input[0].SetFloat(4.0f);
    sc.Input[1].Name="02. Demi-vie barres"; sc.Input[1].SetInt(50);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=0 || sc.TickSize<=0) return; int idx=sc.ArraySize-1;
  double ref=sc.Close[idx-1]; double w=sc.Input[0].GetFloat()*sc.TickSize;
  double decay = pow(0.5, 1.0/max(1, sc.Input[1].GetInt()));
  cnt = int(cnt*decay);
  if(fabs(sc.Close[idx]-ref)<=w) cnt += 1;
  sc.Subgraph[1][idx]=cnt;
}
