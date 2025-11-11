// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_ORDERFLOW_CONVOLUTION_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="ORDERFLOW_CONVOLUTION_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Niveaux OFI"; sc.Input[0].SetInt(8);
    sc.Input[1].Name="02. EMA courte %"; sc.Input[1].SetFloat(0.2f);
    sc.Input[2].Name="03. EMA longue %"; sc.Input[2].SetFloat(0.05f);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); s_MarketDepthEntry md{}; double b=0,a=0;
  for(int i=0;i<N && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<N && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity; }
  static double pb=0,pa=0; if(sc.Index==0){ pb=b; pa=a; }
  double ofi=(b-pb)-(a-pa); pb=b; pa=a;
  double aS=sc.Input[1].GetFloat(); double aL=sc.Input[2].GetFloat();
  static double eS=0,eL=0; eS = (1.0-aS)*eS + aS*ofi; eL=(1.0-aL)*eL + aL*ofi;
  sc.Subgraph[1][sc.ArraySize-1]=eS; sc.Subgraph[2][sc.ArraySize-1]=eL; sc.Subgraph[3][sc.ArraySize-1]=(eS-eL);
}
