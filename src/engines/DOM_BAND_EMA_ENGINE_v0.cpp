// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_BAND_EMA_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_BAND_EMA_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Near niveaux"; sc.Input[0].SetInt(8);
    sc.Input[1].Name="02. Mid niveaux"; sc.Input[1].SetInt(16);
    sc.Input[2].Name="03. Far niveaux"; sc.Input[2].SetInt(32);
    sc.Input[3].Name="04. EMA %"; sc.Input[3].SetInt(85);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(), M=sc.Input[1].GetInt(), F=sc.Input[2].GetInt(); double a=sc.Input[3].GetInt()/100.0;
  s_MarketDepthEntry md{}; double bN=0,aN=0,bM=0,aM=0,bF=0,aF=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<F;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); if(i<N) bN+=md.Quantity; else if(i<M) bM+=md.Quantity; else bF+=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<F;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); if(i<N) aN+=md.Quantity; else if(i<M) aM+=md.Quantity; else aF+=md.Quantity; }
  static double eN=0,eM=0,eF=0;
  double n=bN+aN, m=bM+aM, f=bF+aF;
  eN=a*n+(1-a)*eN; eM=a*m+(1-a)*eM; eF=a*f+(1-a)*eF;
  int idx=sc.ArraySize-1; sc.Subgraph[1][idx]=eN; sc.Subgraph[2][idx]=eM; sc.Subgraph[3][idx]=eF;
}
