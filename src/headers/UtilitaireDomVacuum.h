#pragma once
#include "sierrachart.h"
#include "UtilitaireDom.h"

// Vacuum v0: baisse nette des quantités near
namespace du {

inline float 	vacNearDrop		(const SCStudyInterfaceRef& sc, int nearLevels, double prevNearSum)
{
  double now=0; for (int li=0; li<nearLevels; ++li) now += du::readBidQtyAt(sc, li) + du::readAskQtyAt(sc, li);
  return (float)(prevNearSum - now); // >0 si chute
}

inline bool   	duVacuumDrop	(const double* prev,const double* now,int avail,double thrAbs,double thrPct,int /*minLifeMs*/,long long /*nowMs*/){
  double loss=0, base=0; for(int i=0;i<avail;++i){ double d=(prev?prev[i]:0)-(now?now[i]:0); if(d>0) loss+=d; base+=(prev?prev[i]:0); }
  return loss>thrAbs || (base>1e-12 && loss/base > thrPct);
}
inline double 	duVacuumScore	(double nearLoss,double farLoss){ return nearLoss*0.7 + farLoss*0.3; }

} // namespace du
