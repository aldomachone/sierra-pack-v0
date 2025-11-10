#pragma once
#include "sierrachart.h"
#include "UtilitaireDom.h"

namespace du {

// Imbalance near-mid-far v0
inline float 	imbalNearMinusFarPct	(const SCStudyInterfaceRef& sc, int levels, int nearN, int midN)
{
  const auto b = du::sumBands(sc, 1, levels, nearN, midN);
  float n=0,m=0,f=0; du::normalizePct(b.nearBid+b.nearAsk, b.midBid+b.midAsk, b.farBid+b.farAsk, n,m,f);
  return n - f;
}


inline void 	duImbalance				(const double* qBid,const double* qAsk,int avail,double* obi,double* ratio)	{
  double sb=0, sa=0; for(int i=0;i<avail;++i){ sb+=qBid?qBid[i]:0; sa+=qAsk?qAsk[i]:0; }
  if(obi) *obi = sb - sa; if(ratio) *ratio = (sa>1e-12? sb/sa : 0.0);
}
inline double 	duImbalBands			(double near,double mid,double far,double* score)							{ double s = near*0.6 + mid*0.3 + far*0.1; if(score)*score=s; return s; }
inline bool 	duSweepDetect			(const double* qDelta,int avail,double thrAbs,double thrRel,double* score)	{
  double s=0; for(int i=0;i<avail;++i) if(qDelta && qDelta[i]>thrAbs) s+=qDelta[i]; if(score)*score=s; return s>thrRel;
}

} // namespace du
