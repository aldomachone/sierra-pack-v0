#pragma once
#include "sierrachart.h"
#include "UtilitaireZones.h"

namespace du {

// Score simple de confluence = somme des inverses de distances clampées
inline float 	cfConfluenceScore	(const Zone* zs, int count, float price, float minDist)
{
  float s=0.0f; const float md=max(1e-6f, minDist);
  for (int i=0;i<count;++i) s += 1.0f / max(md, znDist(zs[i], price));
  return s;
}

inline void   	cfNearestZone		(double /*price*/, const double* /*zones*/, int /*n*/, int& idx, double& dist)	{ idx=-1; dist=0; }
inline double	cfScoreWeighted		(const double* items,const double* w,int n)										{ double s=0; for(int i=0;i<n;++i) s+= items[i]*(w?w[i]:1.0); return s; }
} // namespace du
