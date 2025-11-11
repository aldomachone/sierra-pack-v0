// File: DOM_ENGINE_v0.cpp
#include "Pack_v0.h"

SCSFExport scsf_DOM_ENGINE_v0(SCStudyInterfaceRef sc)
{
  enum { IN_ModeStale=1, IN_SpreadMax, IN_DepthMin,
         IN_NearCnt, IN_MidCnt, IN_FarCnt, IN_StartNear, IN_StartMid, IN_StartFar }; // 01..09
  if(sc.SetDefaults){
    sc.AutoLoop = 0; sc.UpdateAlways = 1; sc.ValueFormat = 26; sc.GraphRegion = 0; sc.FreeDLL = 0;

    for(int sg=1; sg<=24; ++sg){
      sc.Subgraph[sg].Name = SCString("ENG_DOM_")+SCString(sg);
      sc.Subgraph[sg].DrawStyle = DRAWSTYLE_IGNORE;
      sc.Subgraph[sg].DrawZeros = 0;
      sc.Subgraph[sg].DisplayAsMainPriceGraphValue = 0;
    }

    sc.Input[IN_ModeStale].Name = "01. DOM figé → Mode";
    sc.Input[IN_ModeStale].SetCustomInputStrings("Geler;Zéro;Continuer");
    sc.Input[IN_ModeStale].SetCustomInputIndex(0);

    sc.Input[IN_SpreadMax].Name = "02. Spread max (ticks)"; sc.Input[IN_SpreadMax].SetFloat(2.0f);
    sc.Input[IN_DepthMin].Name  = "03. Profondeur min";     sc.Input[IN_DepthMin].SetInt(10);

    sc.Input[IN_NearCnt].Name   = "04. Near niveaux";      sc.Input[IN_NearCnt].SetInt(10);
    sc.Input[IN_MidCnt].Name    = "05. Mid niveaux";       sc.Input[IN_MidCnt].SetInt(20);
    sc.Input[IN_FarCnt].Name    = "06. Far niveaux";       sc.Input[IN_FarCnt].SetInt(30);

    sc.Input[IN_StartNear].Name = "07. Near start";        sc.Input[IN_StartNear].SetInt(0);
    sc.Input[IN_StartMid].Name  = "08. Mid start";         sc.Input[IN_StartMid].SetInt(10);
    sc.Input[IN_StartFar].Name  = "09. Far start";         sc.Input[IN_StartFar].SetInt(30);

    sc.DisplayStudyInputValues = false;
    return;
  }

  // Gates
  const int modeStale = sc.Input[IN_ModeStale].GetIndex();
  const double spreadMax = sc.Input[IN_SpreadMax].GetFloat();
  const int depthMin = std::max(1, sc.Input[IN_DepthMin].GetInt());

  const double ts  = sc.TickSize;
  const double bid = sc.Bid[sc.Index];
  const double ask = sc.Ask[sc.Index];
  if(!(ts>0.0) || !(bid>0.0) || !(ask>0.0)) return;

  const int avail = sc.GetMarketDepthNumberOfLevels();
  if(avail < depthMin) return;

  const bool stale = du::domIsStale(sc);
  if(stale){
    if(modeStale==0) return;             // Geler
    if(modeStale==1){                    // Zéro
      for(int sg=1; sg<=24; ++sg) sc.Subgraph[sg][sc.Index]=0.0f;
      return;
    }
  }

  const double sprT = (ask - bid) / ts;
  if(sprT > spreadMax) return;

  const int nNear = std::max(0, sc.Input[IN_NearCnt].GetInt());
  const int nMid  = std::max(0, sc.Input[IN_MidCnt].GetInt());
  const int nFar  = std::max(0, sc.Input[IN_FarCnt].GetInt());

  const int sNear = std::max(0, sc.Input[IN_StartNear].GetInt());
  const int sMid  = std::max(0, sc.Input[IN_StartMid].GetInt());
  const int sFar  = std::max(0, sc.Input[IN_StartFar].GetInt());

  auto sumBand = [&](bool isBid, int start, int cnt)->double{
    if(cnt<=0) return 0.0;
    double s=0.0; const int e = start+cnt-1;
    for(int li=start; li<=e; ++li){
      s += isBid ? du::readBidQtyAt(sc, li) : du::readAskQtyAt(sc, li);
    }
    return s;
  };

  const double nearB = sumBand(true , sNear, nNear);
  const double nearA = sumBand(false, sNear, nNear);
  const double midB  = sumBand(true , sMid , nMid );
  const double midA  = sumBand(false, sMid , nMid );
  const double farB  = sumBand(true , sFar , nFar );
  const double farA  = sumBand(false, sFar , nFar );

  // SG exposition: 1..6 Near/Mid/Far Bid/Ask, 7=avail, 8=spread ticks
  sc.Subgraph[1][sc.Index] = (float)nearB;
  sc.Subgraph[2][sc.Index] = (float)nearA;
  sc.Subgraph[3][sc.Index] = (float)midB;
  sc.Subgraph[4][sc.Index] = (float)midA;
  sc.Subgraph[5][sc.Index] = (float)farB;
  sc.Subgraph[6][sc.Index] = (float)farA;
  sc.Subgraph[7][sc.Index] = (float)avail;
  sc.Subgraph[8][sc.Index] = (float)sprT;
}
