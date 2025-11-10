#pragma once
#include <cmath>
#include "sierrachart.h"

namespace du {

// ms depuis epoch (horloge système)
inline long long 	nowMs				(const SCStudyInterfaceRef& sc)								{
  const SCDateTimeMS t = sc.CurrentSystemDateTimeMS;
  const double ms = (double)t * 24.0 * 3600.0 * 1000.0;
  return (long long)(ms + 0.5);
}

// ms de la barre idx (horloge chart)
inline long long 	barTimeMs			(const SCStudyInterfaceRef& sc, int idx)					{
  const double ms = (double)sc.BaseDateTimeIn[idx] * 24.0 * 3600.0 * 1000.0;
  return (long long)(ms + 0.5);
}

inline long long 	duNowNs				(SCStudyInterfaceRef)										{ return 0; }

// vrai si nouvelle journée de trading
inline bool 		newTradingDay		(const SCStudyInterfaceRef& sc, int& lastYmd)				{
  const int ymd = sc.BaseDateTimeIn[sc.Index].GetDateYMD();
  if (ymd != lastYmd){ lastYmd = ymd; return true; }
  return false;
}

inline bool 		duIsNewTradingDay	(SCStudyInterfaceRef sc, int LastDate)						{ return sc.BaseDateTimeIn[sc.ArraySize-1].GetDate() != LastDate; }

// clamp jitter (latence) en ms
inline int 			jitterClampMs		(int dtMs, int maxJitterMs)									{
  if (dtMs < 0) return 0;
  if (dtMs > maxJitterMs) return maxJitterMs;
  return dtMs;
}

inline long long 	duJitterClamp		(long long prevMs, long long nowMs, long long maxJumpMs)	{
  if(nowMs - prevMs > maxJumpMs) return prevMs + maxJumpMs;
  if(prevMs - nowMs > maxJumpMs) return prevMs - maxJumpMs;
  return nowMs;
}

// secondes équivalentes à N barres
inline double 		secFromBars			(const SCStudyInterfaceRef& sc, int bars)					{
  return (double)bars * (double)max(1, sc.SecondsPerBar);
}

inline double 		duClockSkewEstimate	(SCStudyInterfaceRef, long long)							{ return 0.0; }

inline long long 	duSessionElapsedMs	(SCStudyInterfaceRef)										{ return 0; }

inline bool 		duDebounceMs		(int& state, long long nowMs, long long minGapMs)			{
  static long long last=0; if(nowMs - last < minGapMs) return false; last=nowMs; state++; return true;
}

} // namespace du

