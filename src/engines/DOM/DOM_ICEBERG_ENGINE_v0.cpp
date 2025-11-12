// ============================================================================
// Pack v0 — Engines v0 (variants supplémentaires)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_ICEBERG_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited=sc.GetPersistentInt(1); int& cons=sc.GetPersistentInt(2);
  if(sc.SetDefaults){
    sc.GraphName="DOM_ICEBERG_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Seuil best qty"; sc.Input[0].SetInt(50); sc.Input[0].SetIntLimits(1,1000000);
    sc.Input[1].Name="02. Min répétitions"; sc.Input[1].SetInt(3); sc.Input[1].SetIntLimits(1,1000);
    sc.DrawZeros=false; return;
  }
  if(!inited||sc.IsFullRecalculation){ inited=1; cons=0; }
  if(sc.TickSize<=0||sc.ArraySize<=1) return;
  s_MarketDepthEntry md{}; double b0=0,a0=0;
  if(sc.GetBidMarketDepthNumberOfLevels()>0){ sc.GetBidMarketDepthEntryAtLevel(md,0); b0=md.Quantity; }
  if(sc.GetAskMarketDepthNumberOfLevels()>0){ sc.GetAskMarketDepthEntryAtLevel(md,0); a0=md.Quantity; }
  static double pb0=0, pa0=0;
  int thr=sc.Input[0].GetInt();
  bool refilled = (b0>=thr && pb0>=thr && b0>=pb0) || (a0>=thr && pa0>=thr && a0>=pa0);
  pb0=b0; pa0=a0;
  if(refilled) ++cons; else cons=0;
  int ok = (cons>=sc.Input[1].GetInt()? 1:0);
  sc.Subgraph[1][sc.ArraySize-1]=ok;
}
