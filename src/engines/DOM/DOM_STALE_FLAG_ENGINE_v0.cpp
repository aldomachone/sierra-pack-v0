// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_STALE_FLAG_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_STALE_FLAG_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Niveaux check"; sc.Input[0].SetInt(3);
    sc.DrawZeros=false; return;
  }
  static double last=0; bool changed=false; int L=sc.Input[0].GetInt(); s_MarketDepthEntry md{};
  static double pb0=0, pa0=0, qb0=0, qa0=0;
  if(sc.GetBidMarketDepthNumberOfLevels()>0){ sc.GetBidMarketDepthEntryAtLevel(md,0); if(md.Price!=pb0 || md.Quantity!=qb0){ changed=true; pb0=md.Price; qb0=md.Quantity; } }
  if(sc.GetAskMarketDepthNumberOfLevels()>0){ sc.GetAskMarketDepthEntryAtLevel(md,0); if(md.Price!=pa0 || md.Quantity!=qa0){ changed=true; pa0=md.Price; qa0=md.Quantity; } }
  for(int i=1;i<L;++i){
    if(i<sc.GetBidMarketDepthNumberOfLevels()){ sc.GetBidMarketDepthEntryAtLevel(md,i); if(md.Quantity!=0){} }
    if(i<sc.GetAskMarketDepthNumberOfLevels()){ sc.GetAskMarketDepthEntryAtLevel(md,i); if(md.Quantity!=0){} }
  }
  if(changed||last==0) last=sc.CurrentSystemDateTime;
  double stale_ms=(sc.CurrentSystemDateTime-last)*86400000.0;
  sc.Subgraph[1][sc.ArraySize-1]=stale_ms;
}
