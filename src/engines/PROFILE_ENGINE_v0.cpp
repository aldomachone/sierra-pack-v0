// File: PROFILE_ENGINE_v0.cpp
#include "Pack_v0.h"

SCSFExport scsf_PROFILE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  enum { IN_ModeStale=1, IN_SpreadMax, IN_DepthMin, IN_BarsLookback }; // 01..04
  if(sc.SetDefaults){
    sc.AutoLoop = 0; sc.UpdateAlways = 0; sc.ValueFormat = 26; sc.GraphRegion = 0; sc.FreeDLL = 0;
    for(int sg=1; sg<=8; ++sg){
      sc.Subgraph[sg].Name = SCString("ENG_PROFILE_")+SCString(sg);
      sc.Subgraph[sg].DrawStyle = DRAWSTYLE_IGNORE;
      sc.Subgraph[sg].DrawZeros = 0;
      sc.Subgraph[sg].DisplayAsMainPriceGraphValue = 0;
    }
    sc.Input[IN_ModeStale].Name = "01. DOM figé → Mode";
    sc.Input[IN_ModeStale].SetCustomInputStrings("Geler;Zéro;Continuer");
    sc.Input[IN_ModeStale].SetCustomInputIndex(0);

    sc.Input[IN_SpreadMax].Name = "02. Spread max (ticks)";
    sc.Input[IN_SpreadMax].SetFloat(2.0f);

    sc.Input[IN_DepthMin].Name = "03. Profondeur min";
    sc.Input[IN_DepthMin].SetInt(10);

    sc.Input[IN_BarsLookback].Name = "04. Barres lookback (profil)";
    sc.Input[IN_BarsLookback].SetInt(240);

    sc.DisplayStudyInputValues = false;
    return;
  }

  const int modeStale = sc.Input[IN_ModeStale].GetIndex();
  const double spreadMax = sc.Input[IN_SpreadMax].GetFloat();
  const int depthMin = std::max(1, sc.Input[IN_DepthMin].GetInt());
  const double ts = sc.TickSize; const double bid = sc.Bid[sc.Index]; const double ask = sc.Ask[sc.Index];

  if(ts>0.0 && bid>0.0 && ask>0.0){
    if(sc.GetMarketDepthNumberOfLevels() < depthMin) return;
    if(du::domIsStale(sc)){
      if(modeStale==0) return;
      if(modeStale==1){ for(int sg=1; sg<=8; ++sg) sc.Subgraph[sg][sc.Index]=0.0f; return; }
    }
    if(((ask-bid)/ts) > spreadMax) return;
  }

  const int N = std::max(1, sc.Input[IN_BarsLookback].GetInt());
  int start = sc.Index - (N-1); if(start < 0) start = 0;
  double cumVol=0.0, hi=-1e300, lo=1e300; int pocIdx = start; double pocVol=-1.0;
  for(int i=start; i<=sc.Index; ++i){
    const double v = (double)sc.Volume[i];
    cumVol += v;
    if(sc.High[i] > hi) hi = sc.High[i];
    if(sc.Low[i]  < lo) lo = sc.Low[i];
    if(v > pocVol){ pocVol=v; pocIdx=i; }
  }
  const double pocPrice = (double)sc.Close[pocIdx];

  sc.Subgraph[1][sc.Index] = (float)cumVol;                                // cum vol
  sc.Subgraph[2][sc.Index] = (float)((hi - lo) / std::max(ts,1e-12));      // range ticks
  sc.Subgraph[3][sc.Index] = (float)pocPrice;                               // POC approx
  sc.Subgraph[4][sc.Index] = (float)pocVol;                                 // vol @ POC
}
