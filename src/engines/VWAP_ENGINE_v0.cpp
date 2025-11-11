// File: VWAP_ENGINE_v0.cpp
#include "Pack_v0.h"

SCSFExport scsf_VWAP_ENGINE_v0(SCStudyInterfaceRef sc)
{
  enum { IN_ModeStale=1, IN_SpreadMax, IN_DepthMin, IN_SessionResetMin }; // 01..04
  if(sc.SetDefaults){
    sc.AutoLoop = 0; sc.UpdateAlways = 1; sc.ValueFormat = 26; sc.GraphRegion = 0; sc.FreeDLL = 0;
    for(int sg=1; sg<=8; ++sg){
      sc.Subgraph[sg].Name = SCString("ENG_VWAP_")+SCString(sg);
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

    sc.Input[IN_SessionResetMin].Name = "04. Reset session à (minute)";
    sc.Input[IN_SessionResetMin].SetInt(0); // 0 = début session broker

    sc.DisplayStudyInputValues = false;
    return;
  }

  const int modeStale = sc.Input[IN_ModeStale].GetIndex();
  const double spreadMax = sc.Input[IN_SpreadMax].GetFloat();
  const int depthMin = std::max(1, sc.Input[IN_DepthMin].GetInt());

  const double ts = sc.TickSize;
  const double bid = sc.Bid[sc.Index];
  const double ask = sc.Ask[sc.Index];
  if(!(ts>0.0) || !(bid>0.0) || !(ask>0.0)) return;

  const int avail = sc.GetMarketDepthNumberOfLevels();
  if(avail < depthMin) return;

  if(du::domIsStale(sc)){
    if(modeStale==0) return;
    if(modeStale==1){ for(int sg=1; sg<=8; ++sg) sc.Subgraph[sg][sc.Index]=0.0f; return; }
  }
  const double sprT = (ask - bid)/ts; if(sprT > spreadMax) return;

  int& lastDay = sc.GetPersistentInt(1);
  int& inited  = sc.GetPersistentInt(2);
  double& sumPV = sc.GetPersistentDouble(10);
  double& sumV  = sc.GetPersistentDouble(11);

  const int day = sc.GetTradingDayDate(sc.BaseDateTimeIn[sc.Index]);
  const int resetMin = sc.Input[IN_SessionResetMin].GetInt();
  const int curMin   = (int)(sc.BaseDateTimeIn[sc.Index].GetMinute());
  if(!inited || day != lastDay || (resetMin>0 && curMin == resetMin)){
    sumPV = 0.0; sumV = 0.0; inited = 1; lastDay = day;
  }

  const double P = ((double)sc.High[sc.Index] + (double)sc.Low[sc.Index] + (double)sc.Close[sc.Index]) / 3.0;
  const double V = (double)sc.Volume[sc.Index];
  if(V > 0.0){ sumPV += P*V; sumV += V; }
  const double vwap = (sumV>0.0? sumPV/sumV : (double)sc.Close[sc.Index]);

  sc.Subgraph[1][sc.Index] = (float)vwap;                                // VWAP
  sc.Subgraph[2][sc.Index] = (float)(((double)sc.Close[sc.Index]-vwap)/ts); // dist ticks
  sc.Subgraph[3][sc.Index] = (float)sprT;                                 // spread ticks
  sc.Subgraph[4][sc.Index] = (float)sumV;                                 // cum volume
}
