// File: OHLC_ENGINE_v0.cpp
#include "Pack_v0.h"

SCSFExport scsf_OHLC_ENGINE_v0(SCStudyInterfaceRef sc)
{
  enum { IN_ModeStale=1, IN_SpreadMax, IN_DepthMin }; // 01..03
  if(sc.SetDefaults){
    sc.AutoLoop = 0; sc.UpdateAlways = 0; sc.ValueFormat = 26; sc.GraphRegion = 0; sc.FreeDLL = 0;
    for(int sg=1; sg<=8; ++sg){
      sc.Subgraph[sg].Name = SCString("ENG_OHLC_")+SCString(sg);
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

  int i0 = sc.Index; const int curDay = sc.GetTradingDayDate(sc.BaseDateTimeIn[sc.Index]);
  while(i0>0 && sc.GetTradingDayDate(sc.BaseDateTimeIn[i0-1]) == curDay) --i0;

  const double O = (double)sc.Open[i0];
  double H = (double)sc.High[i0];
  double L = (double)sc.Low[i0];
  for(int i=i0+1; i<=sc.Index; ++i){
    if(sc.High[i] > H) H = sc.High[i];
    if(sc.Low[i]  < L) L = sc.Low[i];
  }
  const double C = (double)sc.Close[sc.Index];
  const double state = (C>H? 1.0 : (C<L? -1.0 : 0.0)); // break état simple

  sc.Subgraph[1][sc.Index] = (float)O;
  sc.Subgraph[2][sc.Index] = (float)H;
  sc.Subgraph[3][sc.Index] = (float)L;
  sc.Subgraph[4][sc.Index] = (float)C;
  sc.Subgraph[5][sc.Index] = (float)state; // -1,0,+1
}
