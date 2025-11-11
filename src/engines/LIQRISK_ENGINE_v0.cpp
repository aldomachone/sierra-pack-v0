// File: LIQRISK_ENGINE_v0.cpp
#include "Pack_v0.h"

SCSFExport scsf_LIQRISK_ENGINE_v0(SCStudyInterfaceRef sc)
{
  enum { IN_ModeStale=1, IN_SpreadMax, IN_DepthMin, IN_NearCnt, IN_EMAPct, IN_RecovHLms }; // 01..06
  if(sc.SetDefaults){
    sc.AutoLoop=0; sc.UpdateAlways=1; sc.ValueFormat=26; sc.GraphRegion=0; sc.FreeDLL=0;
    for(int sg=1; sg<=16; ++sg){
      sc.Subgraph[sg].Name = SCString("ENG_LIQRISK_")+SCString(sg);
      sc.Subgraph[sg].DrawStyle = DRAWSTYLE_IGNORE;
      sc.Subgraph[sg].DrawZeros = 0;
      sc.Subgraph[sg].DisplayAsMainPriceGraphValue = 0;
    }
    sc.Input[IN_ModeStale].Name = "01. DOM figé → Mode";
    sc.Input[IN_ModeStale].SetCustomInputStrings("Geler;Zéro;Continuer");
    sc.Input[IN_ModeStale].SetCustomInputIndex(0);
    sc.Input[IN_SpreadMax].Name = "02. Spread max (ticks)"; sc.Input[IN_SpreadMax].SetFloat(2.0f);
    sc.Input[IN_DepthMin].Name  = "03. Profondeur min";     sc.Input[IN_DepthMin].SetInt(10);
    sc.Input[IN_NearCnt].Name   = "04. Niveaux Near";       sc.Input[IN_NearCnt].SetInt(10);
    sc.Input[IN_EMAPct].Name    = "05. EMA (%)";            sc.Input[IN_EMAPct].SetInt(50);
    sc.Input[IN_RecovHLms].Name = "06. HL recovery (ms)";   sc.Input[IN_RecovHLms].SetInt(60000);
    sc.DisplayStudyInputValues=false; return;
  }

  // Gates de base
  const int modeStale = sc.Input[IN_ModeStale].GetIndex();
  const double spreadMax = sc.Input[IN_SpreadMax].GetFloat();
  const int depthMin = std::max(1, sc.Input[IN_DepthMin].GetInt());
  const double ts  = sc.TickSize; const double bid = sc.Bid[sc.Index]; const double ask = sc.Ask[sc.Index];
  if(!(ts>0.0) || !(bid>0.0) || !(ask>0.0)) return;
  if(sc.GetMarketDepthNumberOfLevels() < depthMin) return;
  if(du::domIsStale(sc)){ if(modeStale==0) return; if(modeStale==1){ for(int sg=1; sg<=16; ++sg) sc.Subgraph[sg][sc.Index]=0.0f; return; } }
  if(((ask-bid)/ts) > spreadMax) return;

  // Params & state
  const int nearCnt = std::max(0, sc.Input[IN_NearCnt].GetInt());
  const int emaPct  = std::max(1, std::min(99, sc.Input[IN_EMAPct].GetInt()));
  const int hlRecMs = std::max(1, sc.Input[IN_RecovHLms].GetInt());

  // Appels helpers (API supposée présente dans UtilitaireLiqRisk.h)
  // lr_update_tick(sc, nearCnt, emaPct, hlRecMs, nowMs)
  // lr_features_v1(out[16])
  const long long nowMs = (long long)(sc.BaseDateTimeIn[sc.Index].GetAsDouble()*24.0*3600.0*1000.0);
  // Ces fonctions doivent exister dans ton header UtilitaireLiqRisk.h enrichi.
  // Si leur nom diffère, adapte ici.
  double out[16]={0};
  bool ok = false;
#ifdef DU_LIQRISK_HAS_ENGINE_V1
  ok = du::lr_update_tick(sc, nearCnt, emaPct, hlRecMs, nowMs);
  if(ok){ du::lr_features_v1(out); }
#else
  // Fallback neutre si header pas encore à jour
  (void)nearCnt; (void)emaPct; (void)hlRecMs; (void)nowMs;
#endif

  for(int i=0;i<16;++i) sc.Subgraph[i+1][sc.Index] = (float)out[i];
}
