// ============================================================================
// Pack v0 — Engines v0 (auto-generated)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_PULLSTACK_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited=sc.GetPersistentInt(1); double& ePull=sc.GetPersistentDouble(2); double& eStack=sc.GetPersistentDouble(3);
  if(sc.SetDefaults){
    sc.GraphName="DOM_PULLSTACK_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0; sc.UsesMarketDepthData=1;
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
    sc.Input[0].Name="01. Near niveaux"; sc.Input[0].SetInt(8); sc.Input[0].SetIntLimits(1,60);
    sc.Input[1].Name="02. Mid niveaux"; sc.Input[1].SetInt(16); sc.Input[1].SetIntLimits(1,60);
    sc.Input[2].Name="03. Far niveaux"; sc.Input[2].SetInt(32); sc.Input[2].SetIntLimits(1,60);
    sc.Input[3].Name="04. EMA %"; sc.Input[3].SetInt(85); sc.Input[3].SetIntLimits(1,99);
    sc.DrawZeros=false; return;
  }
  if(!inited || sc.IsFullRecalculation){ inited=1; ePull=0; eStack=0; }
  if(sc.TickSize<=0) return;
  int nN=sc.Input[0].GetInt(), nM=sc.Input[1].GetInt(), nF=sc.Input[2].GetInt(); double a=sc.Input[3].GetInt()/100.0;
  static double prevBid=0, prevAsk=0;
  s_MarketDepthEntry md{}; double b=0,aQ=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<nF;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<nF;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); aQ+=md.Quantity; }
  double dB=b-prevBid, dA=aQ-prevAsk; prevBid=b; prevAsk=aQ;
  double pull = (dB<0? -dB:0) + (dA<0? -dA:0);
  double stack= (dB>0?  dB:0) + (dA>0?  dA:0);
  ePull = a*pull + (1-a)*ePull; eStack = a*stack + (1-a)*eStack;
  int i=sc.ArraySize-1; if(i>=0){ sc.Subgraph[1][i]=ePull; sc.Subgraph[2][i]=eStack; sc.Subgraph[3][i]=eStack - ePull; }
}
