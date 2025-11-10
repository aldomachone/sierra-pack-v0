#pragma once
#include "sierrachart.h"

// Momentum prix v0
namespace du {

inline float	momPrice1			(const SCStudyInterfaceRef& sc, int i)
{
  if (i<=0) return 0.0f;
  return (float)(sc.Close[i] - sc.Close[i-1]);
}

inline void   	duMomentumNear		(const double* now,const double* prev,int avail,double alphaPct,double* outScore){
  double s=0; for(int i=0;i<avail;++i){ double d=(now?now[i]:0)-(prev?prev[i]:0); s += d; } if(outScore)*outScore = s*(alphaPct/100.0);
}
inline double 	duMomentumSigned	(double bidMom,double askMom){ return bidMom - askMom; }
inline bool   	duMomentumGate		(double score,double p1,double p2,int /*warmupN*/){ return score>p1 || score>p2; }


} // namespace du
