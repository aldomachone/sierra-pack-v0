#pragma once
#include "sierrachart.h"

// === Temps & cadence (v0) ===
namespace du {

/////////////////////////////////////////////////////////////////////////////////////////////// nowMs : horodatage mur réel en millisecondes
  inline long long 	nowMs(const SCStudyInterfaceRef& sc)
  {
    // SCDateTimeMS à double jours → convertir en ms
    const 			SCDateTimeMS t 	= sc.CurrentSystemDateTimeMS;
    const double 	days 			= (double)t					;
    const double 	ms 				= days * 24.0 * 60.0 * 60.0 * 1000.0;
	
    return (long long)(ms + 0.5);
  }

  // newTradingDay : vrai si changement de jour de session
  inline bool newTradingDay(const SCStudyInterfaceRef& sc, int& lastYyyymmdd)
  {
    const int ymd = sc.BaseDateTimeIn[sc.Index].GetDateYMD();
    if (ymd != lastYyyymmdd)
    {
      lastYyyymmdd = ymd;
      return true;
    }
    return false;
  }

  // jitterClampMs : si delta temps > maxJitter → retourne maxJitter
  inline int jitterClampMs(int dtMs, int maxJitterMs)
  {
    if (dtMs < 0) return 0;
    if (dtMs > maxJitterMs) return maxJitterMs;
    return dtMs;
  }

} // namespace du
