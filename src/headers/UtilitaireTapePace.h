#pragma once
#include "sierrachart.h"
#include "UtilitaireTempo.h"
#include "UtilitaireSanitize.h"

namespace du {

inline float	pacePerSecBar	(const SCStudyInterfaceRef& sc, int idx){
  const double vol = (double)sc.UpVolume[idx] + (double)sc.DownVolume[idx];
  const double dt  = max(1, sc.SecondsPerBar);
  return (float)(vol / dt);
}

inline int 		interArrivalMs	(long long last, long long now){ return (int)max(0LL, now-last); }

inline double tpInterArrivalMs(long long lastTs,long long ts){ return (double)(ts-lastTs); }
inline double tpPaceEMA(double prev,double dtMs,double alphaPct){ double a=alphaPct/100.0; return prev + a*(dtMs-prev); }
inline double tpRate(int ctr,double dtMs){ return dtMs>0? ctr*1000.0/dtMs : 0.0; }

} // namespace du
