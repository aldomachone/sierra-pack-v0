#pragma once
#include "sierrachart.h"
#include "UtilitaireCompat.h"

namespace du {

struct 			ReloadZone 														{ float price=0; float size=0; int ttl=0; }; // ttl en barres

inline void 	rzDecay				(ReloadZone& z)        						{ if (z.ttl>0) --z.ttl; }
inline bool 	rzAlive				(const ReloadZone& z)  						{ return z.ttl>0; }
inline float 	rzDistance			(const ReloadZone& z, float price) 			{ return fabsf(price - z.price); }

inline 			duZone rzFromSwing	(	double hi,double lo,int /*depthTicks*/)	{ return duZone{lo,hi}; }
inline int    	rzState				(double price,const duZone& z)				{ if(price<z.lo) return -1; if(price>z.hi) return 1; return 0; }

} // namespace du
