#pragma once
#include "sierrachart.h"
#include "UtilitaireDom.h"

// Risque de liquidité v0: spread relatif / profondeur near
namespace du {

inline float 	lrRiskScore			(const SCStudyInterfaceRef& sc, int nearLevels)
{
  const float bid = (float)sc.Bid[sc.Index];
  const float ask = (float)sc.Ask[sc.Index];
  const float spread = max(0.0f, ask - bid);

  double depth=0;
  for (int li=0; li<nearLevels; ++li) depth += du::readBidQtyAt(sc, li) + du::readAskQtyAt(sc, li);
  if (depth <= 0) return 0.0f;

  return 1000.0f * spread / (float)depth; // échelle arbitraire
}

inline int    	lrNearIndex			(double price,const double* zones,int n){ int idx=0; double dist=1e300; for(int i=0;i<n;++i)	{ double d=std::fabs(price-zones[i]); if(d<dist){dist=d; idx=i;} } return idx; }
inline bool   	lrCollapseFlag		(double depthNear,double depthFar,double thr)													{ return depthNear+depthFar<thr; }
inline double 	lrRecoveryHalfLife	(const double* /*depthSeries*/, int /*n*/)														{ return 0.0; }

} // namespace du
