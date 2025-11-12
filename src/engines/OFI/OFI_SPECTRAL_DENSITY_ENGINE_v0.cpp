// ============================================================================
// Pack v0 — Engines v0 (lot Extra-HFT #15)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_OFI_SPECTRAL_DENSITY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="OFI_SPECTRAL_DENSITY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Near niveaux"; sc.Input[0].SetInt(8);
    sc.Input[1].Name="02. Fenêtre barres"; sc.Input[1].SetInt(128);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); int W=sc.Input[1].GetInt(); if(W>256) W=256;
  s_MarketDepthEntry md{}; static double ofi[512]; static int n=0;
  double b=0,a=0; for(int i=0;i<N && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; } for(int i=0;i<N && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity; }
  static double pb=0, pa=0; if(sc.Index==0){ pb=b; pa=a; }
  double x=(b-pb)-(a-pa); pb=b; pa=a;
  if(n<512) ofi[n++]=x; else { for(int i=1;i<n;++i) ofi[i-1]=ofi[i]; ofi[n-1]=x; }
  int M=(n>W? W:n); if(M<8) return;
  for(int k=1;k<=4;++k){ double Re=0, Im=0; for(int t=0;t<M;++t){ double ang=2*3.141592653589793*k*t/M; Re += ofi[n-M+t]*cos(ang); Im += ofi[n-M+t]*sin(ang); } double P=(Re*Re+Im*Im)/M; sc.Subgraph[k][sc.ArraySize-1]=P; }
}
