// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #6)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_LIQRISK_RECOVERY_HALFLIFE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& state=sc.GetPersistentInt(1); int& t0=sc.GetPersistentInt(2);
  if(sc.SetDefaults){
    sc.GraphName="LIQRISK_RECOVERY_HALFLIFE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Near niveaux"; sc.Input[0].SetInt(10);
    sc.Input[1].Name="02. Chute %"; sc.Input[1].SetFloat(0.4f);
    sc.Input[2].Name="03. Seuil recovery %"; sc.Input[2].SetFloat(0.5f);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); s_MarketDepthEntry md{}; double sum=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<N;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); sum+=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); sum+=md.Quantity; }
  static double base=0; if(sc.IsFullRecalculation) base=sum;
  if(base<=0) base=sum;
  double frac = (sum>0? sum/base:0.0);
  int idx=sc.ArraySize-1;
  if(state==0 && frac<=1.0-sc.Input[1].GetFloat()){ state=1; t0=idx; }
  if(state==1 && frac>=sc.Input[2].GetFloat()){ int half=idx-t0; sc.Subgraph[1][idx]=half; state=0; base=sum; }
}
