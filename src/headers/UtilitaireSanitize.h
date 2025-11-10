#pragma once
#include <cmath>
#include <cfloat>
#include "sierrachart.h"

namespace du {

template<typename T> inline T clamp	(T x, T lo, T hi)											{ return x<lo?lo:(x>hi?hi:x); 													}

inline float  	sanitize			(float  v)													{ return std::isfinite(v)?v:0.0f; 												}
inline double 	sanitize			(double v)													{ return std::isfinite(v)?v:0.0; 												}

// EMA en pourcentage [1..99]
inline float 	emaPct				(float x, float prev, int pct)								{
  pct = clamp(pct, 1, 99);
  const float a = (float)pct / 100.0f;
  return a*x + (1.0f - a)*prev;
}

// EMA avec alpha [0..1]
inline float 	emaAlpha			(float x, float prev, float a)								{
  a = clamp(a, 0.0f, 1.0f);
  return a*x + (1.0f - a)*prev;
}

// z-score EMA
struct 			ZState																		{ double mean=0, var=0; bool init=false; 										};
inline float 	zEmaUpdate			(ZState& st, float x, int pct)								{
  const float a = (float)clamp(pct,1,99) / 100.0f;
  if(!st.init){ st.mean=x; st.var=0; st.init=true; return 0.0f; }
  const double d = x - st.mean;
  st.mean += a * d;
  st.var   = (1.0 - a) * (st.var + a * d * d);
  const double sd = st.var > DBL_EPSILON ? std::sqrt(st.var) : 1.0;
  return (float)((x - st.mean) / sd);
}

inline double duClamp				(double x,double lo,double hi)								{ return x<lo?lo:(x>hi?hi:x); 													}
	
inline double duSafeDiv				(double num,double den,double def)							{ return std::isfinite(den)&&std::fabs(den)>1e-12? num/den : def; 				}
	
inline bool   duIsFinite			(double x)													{ return std::isfinite(x); 														}
	
inline double duZeroIfNaN			(double x)													{ return std::isfinite(x)?x:0.0; 												}
	
inline void   duSanitizeArray		(double* p,int n)											{ if(!p||n<=0) return; for(int i=0;i<n;++i) if(!std::isfinite(p[i])) p[i]=0.0; 	}
	
inline double duEmaPct				(double prev,double x,double alphaPct)						{ double a=duClamp(alphaPct,0,100)/100.0; return prev + a*(x-prev); 			}
	
inline double duEmaClampPct			(double prev,double x,double alphaPct,double lo,double hi)	{ return duClamp(duEmaPct(prev,x,alphaPct),lo,hi); 								}
	
inline double duZScore				(double x,double mean,double std,double eps)				{ double s=std; if(std<eps) s=eps; return (x-mean)/s; 							}

} // namespace du
