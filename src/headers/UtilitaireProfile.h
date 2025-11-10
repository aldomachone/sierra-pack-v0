#pragma once
#include "sierrachart.h"

// Profile v0 très simple: POC = Close de la barre avec Volume max dans un lookback
namespace du {

struct 			ProfileOut 																						{ float poc=0; float vMax=0; };

inline 			ProfileOut vpPocSimple	(const SCStudyInterfaceRef& sc, int lookback)							{
  ProfileOut out{};
  const int i0 = max(0, sc.Index - max(1, lookback) + 1);
  int iBest = i0; float vBest = (float)sc.Volume[i0];
  for (int i=i0; i<=sc.Index; ++i)
  {
    const float v = (float)sc.Volume[i];
    if (v > vBest) { vBest = v; iBest = i; }
  }
  out.poc  = (float)sc.Close[iBest];
  out.vMax = vBest;
  return out;
}

inline int    	vpPoc					(const double* hist,int n)												{ int idx=0; double mx=-1e300; for(int i=0;i<n;++i){ if(hist[i]>mx){mx=hist[i]; idx=i;} } return idx; }
inline void   	vpVa					(const double* /*hist*/, int /*n*/, double /*pct*/, int& lo, int& hi)	{ lo=0; hi=0; }
inline int    	vpShift					(int currPOC,int prevPOC)												{ return currPOC - prevPOC; }

} // namespace du
