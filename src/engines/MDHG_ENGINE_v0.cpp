// File: MDHG_ENGINE_v0.cpp
#include "Pack_v0.h"

SCSFExport scsf_MDHG_ENGINE_v0(SCStudyInterfaceRef sc)
{
  enum { IN_ModeStale=1, IN_SpreadMax, IN_DepthMin,
         IN_HLms, IN_EMAPct, IN_ZWin,
         IN_NearStart, IN_NearCnt, IN_MidStart, IN_MidCnt, IN_FarStart, IN_FarCnt }; // 01..12
  if(sc.SetDefaults){
    sc.AutoLoop=0; sc.UpdateAlways=1; sc.ValueFormat=26; sc.GraphRegion=0; sc.FreeDLL=0;
    for(int sg=1; sg<=16; ++sg){
      sc.Subgraph[sg].Name = SCString("ENG_MDHG_")+SCString(sg);
      sc.Subgraph[sg].DrawStyle = DRAWSTYLE_IGNORE;
      sc.Subgraph[sg].DrawZeros = 0;
      sc.Subgraph[sg].DisplayAsMainPriceGraphValue = 0;
    }
    sc.Input[IN_ModeStale].Name = "01. DOM figé → Mode";
    sc.Input[IN_ModeStale].SetCustomInputStrings("Geler;Zéro;Continuer");
    sc.Input[IN_ModeStale].SetCustomInputIndex(0);
    sc.Input[IN_SpreadMax].Name = "02. Spread max (ticks)"; sc.Input[IN_SpreadMax].SetFloat(2.0f);
    sc.Input[IN_DepthMin].Name  = "03. Profondeur min";     sc.Input[IN_DepthMin].SetInt(10);

    sc.Input[IN_HLms].Name      = "04. Demi-vie mémoire (ms)"; sc.Input[IN_HLms].SetInt(60000);
    sc.Input[IN_EMAPct].Name    = "05. EMA lissage (%)";       sc.Input[IN_EMAPct].SetInt(50);
    sc.Input[IN_ZWin].Name      = "06. Fenêtre z-score (N)";   sc.Input[IN_ZWin].SetInt(32);

    sc.Input[IN_NearStart].Name = "07. Near start"; sc.Input[IN_NearStart].SetInt(0);
    sc.Input[IN_NearCnt].Name   = "08. Near niveaux"; sc.Input[IN_NearCnt].SetInt(10);
    sc.Input[IN_MidStart].Name  = "09. Mid start"; sc.Input[IN_MidStart].SetInt(10);
    sc.Input[IN_MidCnt].Name    = "10. Mid niveaux"; sc.Input[IN_MidCnt].SetInt(20);
    sc.Input[IN_FarStart].Name  = "11. Far start"; sc.Input[IN_FarStart].SetInt(30);
    sc.Input[IN_FarCnt].Name    = "12. Far niveaux"; sc.Input[IN_FarCnt].SetInt(30);

    sc.DisplayStudyInputValues=false; return;
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

  if(du::domIsStale(sc)){
    if(modeStale==0) return;
    if(modeStale==1){ for(int sg=1; sg<=16; ++sg) sc.Subgraph[sg][sc.Index]=0.0f; return; }
  }
  const double sprT = (ask - bid) / ts;
  if(sprT > spreadMax) return;

  // Params
  du::MdhgParams p{};
  p.hlMs   = (double)std::max(1, sc.Input[IN_HLms].GetInt());
  p.emaPct = std::max(1, std::min(99, sc.Input[IN_EMAPct].GetInt()));
  p.zWin   = std::max(0, sc.Input[IN_ZWin].GetInt());
  p.bands.startNear = std::max(0, sc.Input[IN_NearStart].GetInt());
  p.bands.cntNear   = std::max(0, sc.Input[IN_NearCnt].GetInt());
  p.bands.startMid  = std::max(0, sc.Input[IN_MidStart].GetInt());
  p.bands.cntMid    = std::max(0, sc.Input[IN_MidCnt].GetInt());
  p.bands.startFar  = std::max(0, sc.Input[IN_FarStart].GetInt());
  p.bands.cntFar    = std::max(0, sc.Input[IN_FarCnt].GetInt());

  // State persistants
  du::MdhgStateV1& st = *(du::MdhgStateV1*)sc.GetPersistentPointer(100);
  if(sc.Index==0 && sc.UpdateStartIndex==0 && sc.IsFullRecalculation){
    if(sc.GetPersistentPointer(100)==nullptr){
      void* mem = sc.AllocateMemory(sizeof(du::MdhgStateV1));
      sc.SetPersistentPointer(100, mem);
      du::MdhgStateV1* pst = (du::MdhgStateV1*)mem; pst->reset_all();
    }else{
      ((du::MdhgStateV1*)sc.GetPersistentPointer(100))->reset_all();
    }
  }

  // Update
  du::MdhgStateV1* pst = (du::MdhgStateV1*)sc.GetPersistentPointer(100);
  if(!pst) return;
  const double dtMs = 50.0; // pas nominal
  const long long nowMs = (long long)(sc.BaseDateTimeIn[sc.Index].GetAsDouble()*24.0*3600.0*1000.0);
  du::mdhg_update_tick(sc, *pst, p, nowMs, dtMs);

  // Features → SG1..16
  double feats[16]; const int n = du::mdhg_features_v1(*pst, feats);
  for(int i=0;i<n && i<16;++i) sc.Subgraph[i+1][sc.Index] = (float)feats[i];
}
