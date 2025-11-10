#pragma once
#include "sierrachart.h"

namespace du {

struct 				RangeOut 																		{ float lo=0, hi=0, pos=0; };
struct				rgIBInfo																		{ double hi{0}, lo{0}; };
// Range & position relative sur lookback barres
inline 				RangeOut rgRangePos	(const SCStudyInterfaceRef& sc, int lookback)
{
  RangeOut r{};
  const int i0 = max(0, sc.Index - max(1,lookback) + 1);
  r.lo = (float)sc.Low[i0]; r.hi = (float)sc.High[i0];
  for (int i=i0; i<=sc.Index; ++i) { if (sc.Low[i] < r.lo) r.lo=(float)sc.Low[i]; if (sc.High[i]>r.hi) r.hi=(float)sc.High[i]; }
  const float p = (float)sc.Close[sc.Index];
  const float w = max(1e-6f, r.hi - r.lo);
  r.pos = (p - r.lo) / w; // [0..1]
  return r;
}

inline 				rgIBInfo rgIB		(long long /*sessionStart*/, const double* /*firstHourHL*/)	{ return {}; }
inline double   	rgSessionRange		(double hi,double lo)										{ return hi-lo; }
inline double   	rgPosInRange		(double price,double hi,double lo)							{ if(hi<=lo) return 0.0; return (price-lo)/((hi-lo)); }

} // namespace du
