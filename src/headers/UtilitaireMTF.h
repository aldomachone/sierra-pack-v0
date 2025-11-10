#pragma once
#include "sierrachart.h"

// MTF v0: proxy via moyenne simple sur N barres (sans créer un vrai HTF)
namespace du {

inline float 	mtfProxySMA			(const SCStudyInterfaceRef& sc, int len)
{
  const int i0 = max(0, sc.Index - max(1,len) + 1);
  double s=0; int n=0;
  for (int i=i0; i<=sc.Index; ++i){ s += sc.Close[i]; ++n; }
  return n>0 ? (float)(s/n) : (float)sc.Close[sc.Index];
}

inline bool		mtfIsHTFBar			(SCStudyInterfaceRef)											{ return false; }
inline bool		mtfDownsampleTick	(int& /*state*/, long long /*periodMs*/, long long /*nowMs*/)	{ return true; }
inline bool		mtfAlignToHTF		(SCStudyInterfaceRef, int /*tf*/)								{ return true; }

} // namespace du
