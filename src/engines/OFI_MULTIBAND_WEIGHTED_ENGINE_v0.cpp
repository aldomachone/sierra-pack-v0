// ============================================================================
// Pack v0 — Engines v0 (lot Extra-HFT #15)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_OFI_MULTIBAND_WEIGHTED_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="OFI_MULTIBAND_WEIGHTED_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Near"; sc.Input[0].SetInt(5);
    sc.Input[1].Name="02. Mid"; sc.Input[1].SetInt(15);
    sc.Input[2].Name="03. Far"; sc.Input[2].SetInt(30);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(), M=sc.Input[1].GetInt(), F=sc.Input[2].GetInt();
  s_MarketDepthEntry md{}; auto band=[&](int L){ double b=0,a=0; for(int i=0;i<L && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity/(i+1.0); } for(int i=0;i<L && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity/(i+1.0); } return b-a; };
  static double pN=0,pM=0,pF=0; double n=band(N), m=band(M), f=band(F); if(sc.Index==0){ pN=n; pM=m; pF=f; }
  double ofiN=n-pN, ofiM=m-pM, ofiF=f-pF; pN=n; pM=m; pF=f;
  sc.Subgraph[1][sc.ArraySize-1]=ofiN; sc.Subgraph[2][sc.ArraySize-1]=ofiM; sc.Subgraph[3][sc.ArraySize-1]=ofiF;
}
