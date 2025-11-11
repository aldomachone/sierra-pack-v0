// File: RANGE_ENGINE_v0.cpp
#include "Pack_v0.h"

SCSFExport scsf_RANGE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  enum { IN_ModeStale=1, IN_SpreadMax, IN_DepthMin, IN_IbMinutes }; // 01..04
  if(sc.SetDefaults){
    sc.AutoLoop = 0; sc.UpdateAlways = 0; sc.ValueFormat = 26; sc.GraphRegion = 0; sc.FreeDLL = 0;
    for(int sg=1; sg<=8; ++sg){
      sc.Subgraph[sg].Name = SCString("ENG_RANGE_")+SCString(sg);
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

    sc.Input[IN_IbMinutes].Name = "04. IB minutes";
    sc.Input[IN_IbMinutes].SetInt(60);

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

  const int ibMin = std::max(1, sc.Input[IN_IbMinutes].GetInt());
  SCDateTime tStart = sc.BaseDateTimeIn[i0];
  SCDateTime tEndIB = tStart + SCDateTime(0, ibMin/1440.0);

  double ibH = -1e300, ibL = 1e300; int i=i0;
  while(i<=sc.Index && sc.BaseDateTimeIn[i] <= tEndIB){
    if(sc.High[i]>ibH) ibH=sc.High[i];
    if(sc.Low[i] <ibL) ibL=sc.Low[i];
    ++i;
  }

  double sH = -1e300, sL = 1e300;
  for(int k=i0; k<=sc.Index; ++k){
    if(sc.High[k]>sH) sH=sc.High[k];
    if(sc.Low[k] <sL) sL=sc.Low[k];
  }

  const double C = (double)sc.Close[sc.Index];
  const double posRel = (sH>sL? (C - sL) / (sH - sL) : 0.5); // 0..1

  sc.Subgraph[1][sc.Index] = (float)ibH;
  sc.Subgraph[2][sc.Index] = (float)ibL;
  sc.Subgraph[3][sc.Index] = (float)((ibH-ibL)/std::max(ts,1e-12)); // IB range ticks
  sc.Subgraph[4][sc.Index] = (float)sH;
  sc.Subgraph[5][sc.Index] = (float)sL;
  sc.Subgraph[6][sc.Index] = (float)((sH-sL)/std::max(ts,1e-12));   // Session range ticks
  sc.Subgraph[7][sc.Index] = (float)posRel;                          // position relative 0..1
}
