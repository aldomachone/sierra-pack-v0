// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_PERSIST_IMBAL_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& state=sc.GetPersistentInt(1);
  if(sc.SetDefaults){
    sc.GraphName="DOM_PERSIST_IMBAL_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Near niveaux"; sc.Input[0].SetInt(12);
    sc.Input[1].Name="02. Seuil ON %"; sc.Input[1].SetFloat(0.2f);
    sc.Input[2].Name="03. Seuil OFF %"; sc.Input[2].SetFloat(0.1f);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); s_MarketDepthEntry md{}; double b=0,a=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<N;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity; }
  double imb=(b+a>0? (b-a)/(b+a):0.0);
  double on=sc.Input[1].GetFloat(), off=sc.Input[2].GetFloat();
  if(state==0 && fabs(imb)>=on) state = (imb>0? +1:-1);
  else if(state!=0 && fabs(imb)<=off) state = 0;
  sc.Subgraph[1][sc.ArraySize-1]=state;
}
