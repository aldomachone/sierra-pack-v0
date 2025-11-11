// ============================================================================
// Pack v0 — Engines v0 (ExtraHFT3)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_MIDQUOTE_CHANGE_RATE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="MIDQUOTE_CHANGE_RATE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(50);
    sc.DrawZeros=false; return;
  }
  if(sc.GetBidMarketDepthNumberOfLevels()<1 || sc.GetAskMarketDepthNumberOfLevels()<1) return;
  s_MarketDepthEntry md{}; sc.GetBidMarketDepthEntryAtLevel(md,0); double pb=md.Price; sc.GetAskMarketDepthEntryAtLevel(md,0); double pa=md.Price;
  double mid=0.5*(pb+pa); static double last=0; static int changes=0;
  if(last==0) last=mid; if(mid!=last){ ++changes; last=mid; }
  int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); static int ring[2048]; static int rp=0,rn=0;
  if(rn<2048){ ring[rp]=changes; rp=(rp+1)%2048; ++rn; } else { ring[rp]=changes; rp=(rp+1)%2048; }
  int m=min(W,rn); int delta=0; for(int i=0;i<m;++i){ int j=(rp-1-i+2048)%2048; int j2=(j-1+2048)%2048; delta += (i==0? ring[j] : ring[j]-ring[j2]); }
  sc.Subgraph[1][idx]= (double)delta/(double)(m>0? m:1);
}
