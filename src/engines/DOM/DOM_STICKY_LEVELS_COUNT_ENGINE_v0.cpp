// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT5: DOM Dynamics Advanced)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_STICKY_LEVELS_COUNT_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_STICKY_LEVELS_COUNT_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Seuil durée ticks"; sc.Input[0].SetInt(20);
    sc.DrawZeros=false; return;
  }
  static int ageB=0, ageA=0; s_MarketDepthEntry b,a; if(sc.GetBidMarketDepthNumberOfLevels()>0){ sc.GetBidMarketDepthEntryAtLevel(b,0); } if(sc.GetAskMarketDepthNumberOfLevels()>0){ sc.GetAskMarketDepthEntryAtLevel(a,0); }
  static double qb=0, qa=0; if(b.Quantity!=qb){ qb=b.Quantity; ageB=0; } else ++ageB; if(a.Quantity!=qa){ qa=a.Quantity; ageA=0; } else ++ageA;
  int thr=sc.Input[0].GetInt(); int sticky=(ageB>=thr)+(ageA>=thr);
  sc.Subgraph[1][sc.ArraySize-1]=sticky;
}
