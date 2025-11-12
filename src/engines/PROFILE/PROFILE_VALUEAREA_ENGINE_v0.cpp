// ============================================================================
// Pack v0 — Engines (v0) — Suppléments
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_PROFILE_VALUEAREA_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited=sc.GetPersistentInt(1);

  if(sc.SetDefaults)
  {
    sc.GraphName="PROFILE_VALUEAREA_ENGINE_v0";
    sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;

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

    sc.Input[0].Name="01. Lookback barres";
    sc.Input[0].SetInt(100); sc.Input[0].SetIntLimits(10,200000);
    sc.Input[1].Name="02. Couverture VA %";
    sc.Input[1].SetFloat(70.0f);

    sc.DrawZeros=false; return;
  }

  if(!inited || sc.IsFullRecalculation) inited=1;
  if(sc.ArraySize<=0) return;

  int lb=sc.Input[0].GetInt();
  int start=sc.ArraySize-lb; if(start<0) start=0;

  // histogramme grossier par barres: volume par close (proxy)
  const int B=64;
  double hi=-1e300, lo=1e300;
  for(int i=start;i<sc.ArraySize;++i){ if(sc.High[i]>hi) hi=sc.High[i]; if(sc.Low[i]<lo) lo=sc.Low[i]; }
  if(hi<=lo) return;
  double bins[B]={0};
  for(int i=start;i<sc.ArraySize;++i)
  {
    int b=(int)((sc.Close[i]-lo)/(hi-lo)*(B-1)); if(b<0) b=0; if(b>=B) b=B-1;
    bins[b]+=sc.Volume[i];
  }
  double tot=0; for(int i=0;i<B;++i) tot+=bins[i];
  if(tot<=0) return;
  // trouver VA qui couvre x%
  double target=sc.Input[1].GetFloat()/100.0*tot;
  // centre = bin max
  int poc=0; for(int i=1;i<B;++i) if(bins[i]>bins[poc]) poc=i;
  int l=poc, r=poc; double acc=bins[poc];
  while(acc<target && (l>0 || r<B-1)){ if(r<B-1 && (l==0 || bins[r+1]>=bins[l-1])){ ++r; acc+=bins[r]; } else { --l; acc+=bins[l]; } }
  double vaLo = lo + (hi-lo)*l/(B-1);
  double vaHi = lo + (hi-lo)*r/(B-1);
  int idx=sc.ArraySize-1;
  sc.Subgraph[1][idx]=vaLo;
  sc.Subgraph[2][idx]=vaHi;
}
