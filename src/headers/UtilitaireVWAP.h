#pragma once
#include "sierrachart.h"

namespace du {

struct 			VwapState 																		{ double cumPV=0, cumV=0; int lastDay=0; };

inline float 	typical			(const SCStudyInterfaceRef& sc, int i)							{
  return (float)((sc.High[i] + sc.Low[i] + sc.Close[i]) / 3.0);
}

inline float 	vwap			(SCStudyInterfaceRef sc, VwapState& st)							{
  const int i = sc.Index, day = sc.BaseDateTimeIn[i].GetDateYMD();
  if(i==0 || day!=st.lastDay){ st.cumPV=0; st.cumV=0; st.lastDay=day; }
  const double v  = max(0.0, (double)sc.Volume[i]);
  const double tp = (double)typical(sc,i);
  st.cumPV += tp * v;
  st.cumV  += v;
  return (float)(st.cumV>0 ? st.cumPV/st.cumV : tp);
}

inline double 	vwVwap			(double sumPQ,double sumQ)										{ return sumQ>1e-12? sumPQ/sumQ : 0.0; }
inline void   	vwBands			(double vwap,double stdev,double k,double& upper,double& lower)	{ upper=vwap+k*stdev; lower=vwap-k*stdev; }
inline int    	vwCrossState	(double price,double vwap)										{ return price<vwap?0:(price>vwap?2:1); }

} // namespace du
