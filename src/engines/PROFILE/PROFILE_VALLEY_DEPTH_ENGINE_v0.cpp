// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #7)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PROFILE_VALLEY_DEPTH_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PROFILE_VALLEY_DEPTH_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Lookback"; sc.Input[0].SetInt(150);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=0) return; int lb=sc.Input[0].GetInt(); int start=sc.ArraySize-lb; if(start<0) start=0;
  const int B=64; double hi=-1e300, lo=1e300;
  for(int i=start;i<sc.ArraySize;++i){ if(sc.High[i]>hi) hi=sc.High[i]; if(sc.Low[i]<lo) lo=sc.Low[i]; }
  if(hi<=lo) return; double bins[B]; for(int i=0;i<B;++i) bins[i]=0;
  for(int i=start;i<sc.ArraySize;++i){ int b=(int)((sc.Close[i]-lo)/(hi-lo)*(B-1)); if(b<0) b=0; if(b>=B) b=B-1; bins[b]+=sc.Volume[i]; }
  int p=0; for(int i=1;i<B;++i) if(bins[i]>bins[p]) p=i;
  int l=p, r=p; while(l>0 and bins[l-1]<=bins[l]) l-=1; while(r<B-1 and bins[r+1]<=bins[r]) r+=1;
  double valley=1e300; for(int i=l;i<=r;++i) valley = min(valley, bins[i]);
  double depth = (bins[p]>0? (bins[p]-valley)/bins[p] : 0.0);
  sc.Subgraph[1][sc.ArraySize-1]=depth;
}
