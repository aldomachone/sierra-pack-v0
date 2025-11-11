#include "Pack_v0.h"

SCSFExport scsf_DOM_ENGINE_v0(SCStudyInterfaceRef sc)
{
  enum { P_In_ModeStale=1, P_In_SpreadMax, P_In_DepthMin };
  if(sc.SetDefaults){
    sc.AutoLoop = 0;
    sc.UpdateAlways = 1;
    sc.ValueFormat = 26;
    sc.GraphRegion = 0;
    sc.FreeDLL = 0;

    // Subgraphs réservés 1..24 pour l’engine (invisibles)
    for(int sg=1; sg<=24; ++sg){
      sc.Subgraph[sg].Name = "ENG_DOM_" + SCString(sg);
      sc.Subgraph[sg].DrawStyle = DRAWSTYLE_IGNORE;
      sc.Subgraph[sg].DrawZeros = 0;
      sc.Subgraph[sg].DisplayAsMainPriceGraphValue = 0;
    }

    sc.Input[P_In_ModeStale].Name = "01. DOM figé → Mode";
    sc.Input[P_In_ModeStale].SetCustomInputStrings("Geler;Zéro;Continuer");
    sc.Input[P_In_ModeStale].SetCustomInputIndex(0);

    sc.Input[P_In_SpreadMax].Name = "02. Spread max (ticks)";
    sc.Input[P_In_SpreadMax].SetFloat(2.0f);

    sc.Input[P_In_DepthMin].Name = "03. Profondeur min";
    sc.Input[P_In_DepthMin].SetInt(10);

    sc.DisplayStudyInputValues = false;
    return;
  }

  // Gates
  const int modeStale = sc.Input[P_In_ModeStale].GetIndex();
  const double spreadMax = sc.Input[P_In_SpreadMax].GetFloat();
  const int depthMin = std::max(1, sc.Input[P_In_DepthMin].GetInt());

  const double ts  = sc.TickSize;
  const double bid = sc.Bid[sc.Index];
  const double ask = sc.Ask[sc.Index];
  if(!(ts>0.0) || !(bid>0.0) || !(ask>0.0)) return;

  const int avail = sc.GetMarketDepthNumberOfLevels();
  if(avail < depthMin) return;

  // Gate stale
  const bool stale = du::domIsStale(sc);
  if(stale){
    if(modeStale==0) return;             // Geler
    if(modeStale==1){                    // Zéro
      for(int sg=1; sg<=24; ++sg) sc.Subgraph[sg][sc.Index]=0.0f;
      return;
    }
  }

  // Gate spread
  const double sprT = (ask - bid) / ts;
  if(sprT > spreadMax) return;

  // Lecture unique par bandes Near/Mid/Far (exemple simple)
  double nearB=0, nearA=0, midB=0, midA=0, farB=0, farA=0;
  const int nNear=10, nMid=20, nFar=30;

  for(int li=0; li<nNear; ++li){
    nearB += du::readBidQtyAt(sc, li);
    nearA += du::readAskQtyAt(sc, li);
  }
  for(int li=10; li<10+nMid; ++li){
    midB  += du::readBidQtyAt(sc, li);
    midA  += du::readAskQtyAt(sc, li);
  }
  for(int li=30; li<30+nFar; ++li){
    farB  += du::readBidQtyAt(sc, li);
    farA  += du::readAskQtyAt(sc, li);
  }

  // Expose 6 SG internes (1..6) → lisibles par signaux via GetArray()
  sc.Subgraph[1][sc.Index] = (float)nearB;
  sc.Subgraph[2][sc.Index] = (float)nearA;
  sc.Subgraph[3][sc.Index] = (float)midB;
  sc.Subgraph[4][sc.Index] = (float)midA;
  sc.Subgraph[5][sc.Index] = (float)farB;
  sc.Subgraph[6][sc.Index] = (float)farA;
}
