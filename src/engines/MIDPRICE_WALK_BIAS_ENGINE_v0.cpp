// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_MIDPRICE_WALK_BIAS_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="MIDPRICE_WALK_BIAS_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre pas"; sc.Input[0].SetInt(200);
    sc.DrawZeros=false; return;
  }
  s_MarketDepthEntry md{}; if(sc.GetBidMarketDepthNumberOfLevels()<1 || sc.GetAskMarketDepthNumberOfLevels()<1) return;
  sc.GetBidMarketDepthEntryAtLevel(md,0); double pb=md.Price; sc.GetAskMarketDepthEntryAtLevel(md,0); double pa=md.Price; double mid=0.5*(pb+pa);
  static double last=0; if(last==0) last=mid; int up=0,dn=0; static int U=0,D=0;
  if(mid>last) ++U; else if(mid<last) ++D; last=mid;
  int W=sc.Input[0].GetInt(); static int ringU[4096], ringD[4096]; static int rp=0, rn=0;
  if(rn<4096) { ringU[rp]=U; ringD[rp]=D; ++rn; rp=(rp+1)%4096; } else { ringU[rp]=U; ringD[rp]=D; rp=(rp+1)%4096; }
  int m=min(W, rn); int du=0, dd=0; for(int i=0;i<m;++i){ int j=(rp-1-i+4096)%4096; du+= (i==0? ringU[j] : ringU[j]-ringU[(j-1+4096)%4096]); dd+= (i==0? ringD[j] : ringD[j]-ringD[(j-1+4096)%4096]); }
  double bias=(du+dd>0? (double)(du-dd)/(double)(du+dd):0.0);
  sc.Subgraph[1][sc.ArraySize-1]=bias;
}
