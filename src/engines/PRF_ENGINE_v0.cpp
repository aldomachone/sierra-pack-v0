// File: PRF_ENGINE_v0.cpp
#include "Pack_v0.h"

SCSFExport scsf_PRF_ENGINE_v0(SCStudyInterfaceRef sc)
{
  enum { IN_HLms=1, IN_DtMs, IN_HystOn, IN_HystOff, IN_RefracN, IN_CooldownMs }; // 01..06
  if(sc.SetDefaults){
    sc.AutoLoop=0; sc.UpdateAlways=1; sc.ValueFormat=26; sc.GraphRegion=0; sc.FreeDLL=0;
    for(int sg=1; sg<=16; ++sg){
      sc.Subgraph[sg].Name = SCString("ENG_PRF_")+SCString(sg);
      sc.Subgraph[sg].DrawStyle = DRAWSTYLE_IGNORE;
      sc.Subgraph[sg].DrawZeros = 0;
      sc.Subgraph[sg].DisplayAsMainPriceGraphValue = 0;
    }
    sc.Input[IN_HLms].Name     = "01. Demi-vie (ms)";           sc.Input[IN_HLms].SetInt(250);
    sc.Input[IN_DtMs].Name     = "02. Pas temps (ms)";          sc.Input[IN_DtMs].SetInt(50);
    sc.Input[IN_HystOn].Name   = "03. Hystérésis ON (|norm|)";  sc.Input[IN_HystOn].SetFloat(0.80f);
    sc.Input[IN_HystOff].Name  = "04. Hystérésis OFF";          sc.Input[IN_HystOff].SetFloat(0.40f);
    sc.Input[IN_RefracN].Name  = "05. Réfractaire N";           sc.Input[IN_RefracN].SetInt(2);
    sc.Input[IN_CooldownMs].Name="06. Cooldown (ms)";           sc.Input[IN_CooldownMs].SetInt(250);
    sc.DisplayStudyInputValues=false; return;
  }

  // State
  du::PrfState& st = *(du::PrfState*)sc.GetPersistentPointer(100);
  if(sc.Index==0 && sc.UpdateStartIndex==0 && sc.IsFullRecalculation){
    if(sc.GetPersistentPointer(100)==nullptr){
      void* mem = sc.AllocateMemory(sizeof(du::PrfState));
      sc.SetPersistentPointer(100, mem);
      du::PrfState* pst = (du::PrfState*)mem; pst->reset_all();
    }else{
      ((du::PrfState*)sc.GetPersistentPointer(100))->reset_all();
    }
  }
  du::PrfParams p{};
  p.hl_ms      = (double)std::max(1, sc.Input[IN_HLms].GetInt());
  p.dt_ms      = (double)std::max(1, sc.Input[IN_DtMs].GetInt());
  p.hOn        = (double)sc.Input[IN_HystOn].GetFloat();
  p.hOff       = (double)sc.Input[IN_HystOff].GetFloat();
  p.refracN    = std::max(1, sc.Input[IN_RefracN].GetInt());
  p.cooldownMs = std::max(0, sc.Input[IN_CooldownMs].GetInt());

  du::PrfState* pst = (du::PrfState*)sc.GetPersistentPointer(100);
  if(!pst) return;

  // Entrée simple: variation de Close en ticks
  const double ts = sc.TickSize;
  double x = 0.0;
  if(sc.Index>0 && ts>0.0){
    x = ((double)sc.Close[sc.Index] - (double)sc.Close[sc.Index-1])/ts;
  }

  const long long nowMs = (long long)(sc.BaseDateTimeIn[sc.Index].GetAsDouble()*24.0*3600.0*1000.0);
  const bool trig = du::prf_update_tick(x, *pst, p, nowMs);

  // Features → SG1..16
  double feats[16]; const int n = du::prf_features_v1(*pst, p, nowMs, feats);
  for(int i=0;i<n && i<16;++i) sc.Subgraph[i+1][sc.Index] = (float)feats[i];
  // Optionnel: positionner SG16 à +1/-1 si trigger up/down par signe de x
  if(trig) sc.Subgraph[16][sc.Index] = (float)(x>=0.0 ? +1.0 : -1.0);
}
