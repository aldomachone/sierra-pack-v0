// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #4)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PROFILE_BELL_SHAPE_SCORE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="PROFILE_BELL_SHAPE_SCORE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Lookback"; sc.Input[0].SetInt(120);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<=0) return; int lb=sc.Input[0].GetInt(); int start=sc.ArraySize-lb; if(start<0) start=0;
  const int B=64; double hi=-1e300, lo=1e300;
  for(int i=start;i<sc.ArraySize;++i){ if(sc.High[i]>hi) hi=sc.High[i]; if(sc.Low[i]<lo) lo=sc.Low[i]; }
  if(hi<=lo) return; double bins[B]; for(int i=0;i<B;++i) bins[i]=0.0;
  for(int i=start;i<sc.ArraySize;++i){ int b=(int)((sc.Close[i]-lo)/(hi-lo)*(B-1)); if(b<0) b=0; if(b>=B) b=B-1; bins[b]+=sc.Volume[i]; }
  int l=0,r=B-1; while(l<r && bins[l]==0) ++l; while(r>l && bins[r]==0) --r; if(r<=l) return;
  double mid=(l+r)/2.0; double sym=0, tot=0;
  for(int i=0;i<=r-l;++i){ double a=bins[l+i], b=bins[r-i]; sym += 1.0/(1.0+fabs(a-b)); tot+=1.0; }
  double score = (tot>0? sym/tot:0.0);
  sc.Subgraph[1][sc.ArraySize-1]=score;
}
