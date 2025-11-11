// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_HASBROUCK_LAMBDA_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="HASBROUCK_LAMBDA_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[1].Name="02. Fenêtre bars"; sc.Input[1].SetInt(120);
    sc.DrawZeros=false; return;
  }
  int N=sc.Input[0].GetInt(); s_MarketDepthEntry md{}; double b=0,a=0;
  for(int i=0;i<N && i<sc.GetBidMarketDepthNumberOfLevels(); ++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<N && i<sc.GetAskMarketDepthNumberOfLevels(); ++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); a+=md.Quantity; }
  static double pb=0,pa=0; if(sc.Index==0){ pb=b; pa=a; }
  double ofi=(b-pb)-(a-pa); pb=b; pa=a;
  int idx=sc.ArraySize-1; if(idx<2) return; int W=sc.Input[1].GetInt(); int s=(idx-W>1? idx-W:1);
  // regress ret ~ lambda * ofi (OLS slope)
  const int MAX=4096; static double X[MAX], Y[MAX]; static int n=0;
  double r = sc.Close[idx]-sc.Close[idx-1];
  if(n<MAX){ X[n]=ofi; Y[n]=r; ++n; } else { for(int i=1;i<n;++i){ X[i-1]=X[i]; Y[i-1]=Y[i]; } X[n-1]=ofi; Y[n-1]=r; }
  int m=(n>W? W:n); if(m<10) return; double sx=0,sy=0,sxx=0,sxy=0; for(int i=n-m;i<n;++i){ sx+=X[i]; sy+=Y[i]; sxx+=X[i]*X[i]; sxy+=X[i]*Y[i]; }
  double den = m*sxx - sx*sx; double lambda = (den!=0? (m*sxy - sx*sy)/den : 0.0);
  sc.Subgraph[1][idx]=lambda;
}
