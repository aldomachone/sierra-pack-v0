#pragma once
#include "sierrachart.h"

// VWAP v0 cumulatif par session (utilise High/Low/Close approximation typique)
namespace du {

struct VwapState { double cumPV=0, cumV=0; int lastDay=0; };

inline float vwTypical(const SCStudyInterfaceRef& sc, int i)
{
  return (float)((sc.High[i] + sc.Low[i] + sc.Close[i]) / 3.0);
}

inline float vwVwap(SCStudyInterfaceRef sc, VwapState& st)
{
  const int i = sc.Index;
  const int day = sc.BaseDateTimeIn[i].GetDateYMD();
  if (i==0 || day != st.lastDay) { st.cumPV=0; st.cumV=0; st.lastDay=day; }

  const double v  = max(0.0, (double)sc.Volume[i]);
  const double tp = (double)vwTypical(sc,i);
  st.cumPV += tp * v;
  st.cumV  += v;
  return (float)(st.cumV>0 ? st.cumPV/st.cumV : tp);
}

} // namespace du
