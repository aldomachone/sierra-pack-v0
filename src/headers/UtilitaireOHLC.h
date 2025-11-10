#pragma once
#include "sierrachart.h"

// OHLC v0: niveaux journaliers courants via scan de la journée
namespace du {

struct 			OHLC 																	{ float O=0, H=0, L=0, C=0; };

inline 			OHLC olDaily	(const SCStudyInterfaceRef& sc)
{
  OHLC o{};
  const int i = sc.Index;
  const int d = sc.BaseDateTimeIn[i].GetDateYMD();

  int first = i;
  for (int j=i; j>=0; --j) { if (sc.BaseDateTimeIn[j].GetDateYMD()!=d) { first=j+1; break; } first=0; }

  o.O = (float)sc.Open[first];
  o.H = (float)sc.High[first];
  o.L = (float)sc.Low[first];
  for (int j=first; j<=i; ++j)
  {
    if (sc.High[j] > o.H) o.H = (float)sc.High[j];
    if (sc.Low [j] < o.L) o.L = (float)sc.Low [j];
  }
  o.C = (float)sc.Close[i];
  return o;
}

inline void 	olLevelsTF		(int /*tf*/, double& O,double& H,double& L,double& C)	{ O=H=L=C=0.0; }
inline int  	olBreakState	(double price,double level)								{ if(price>level) return 2; if(price<level) return 0; return 1; }

} // namespace du
