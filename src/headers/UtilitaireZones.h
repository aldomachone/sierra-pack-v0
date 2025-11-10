#pragma once
#include "sierrachart.h"

namespace du {

struct 			Zone 									{ float lo=0, hi=0; };
inline bool  	znContains	(const Zone& z, float p) 	{ return p>=z.lo && p<=z.hi; }
inline float 	znCenter 	(const Zone& z)          	{ return 0.5f*(z.lo+z.hi); }
inline float 	znDist   	(const Zone& z, float p) 	{
  if (p<z.lo) return z.lo - p;
  if (p>z.hi) return p - z.hi;
  return 0.0f;
}

} // namespace du
