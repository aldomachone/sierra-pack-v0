// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #5)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_BESTLEVEL_FLIP_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& flips=sc.GetPersistentInt(1); double& pb=sc.GetPersistentDouble(2); double& pa=sc.GetPersistentDouble(3);
  if(sc.SetDefaults){
    sc.GraphName="DOM_BESTLEVEL_FLIP_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.DrawZeros=false; return;
  }
  double b=sc.Bid[sc.ArraySize-1], a=sc.Ask[sc.ArraySize-1];
  if(sc.IsFullRecalculation){ flips=0; pb=b; pa=a; }
  if(b!=pb || a!=pa){ ++flips; pb=b; pa=a; }
  sc.Subgraph[1][sc.ArraySize-1]=flips;
}
