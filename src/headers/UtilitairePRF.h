#pragma once
#include "sierrachart.h"

// Réponse / demi-vie v0
namespace du {

inline float 		prfAlphaFromHalfLifeSec	(float halfLifeSec, float dtSec)
{
  halfLifeSec = max(0.1f, halfLifeSec);
  dtSec       = max(0.01f, dtSec);
  const float a = 1.0f - expf(-dtSec * 0.69314718f / halfLifeSec);
  return a;
}

inline float 		prfEmaHL				(float x, float prev, float halfLifeSec, float dtSec)
{
  const float a = prfAlphaFromHalfLifeSec(halfLifeSec, dtSec);
  return a*x + (1.0f - a)*prev;
}

inline double 		prfResponse				(long long dtMs,long long halfLifeMs)					{ if(halfLifeMs<=0) return 0.0; return 1.0 - std::exp(-std::log(2.0)*dtMs/halfLifeMs); }
inline double 		prfSlope				(const double* /*series*/, int /*n*/)					{ return 0.0; }
inline long long 	prfHalfLife				(double targetPct,long long dtMs)						{ if(targetPct<=0||targetPct>=1) return 0; return (long long)(-dtMs*std::log(1-targetPct)/std::log(2.0)); }

} // namespace du
