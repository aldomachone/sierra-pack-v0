#pragma once
#include "sierrachart.h"
#include "UtilitaireDom.h"
#include "UtilitaireSanitize.h"

// Mémoire de profondeur v0: EMA des quantités Near
namespace du {

struct 			MdhgState 																				{ float emaNear=0; };

inline float 	mdhgUpdate	(const SCStudyInterfaceRef& sc, MdhgState& st, int levelsNear, int emaPct)
{
  double nb=0, na=0;
  for (int li=0; li<levelsNear; ++li) { nb += du::readBidQtyAt(sc, li); na += du::readAskQtyAt(sc, li); }
  const float near = (float)(nb + na);
  st.emaNear = du::emaPct(near, st.emaNear, du::clamp(emaPct,1,99));
  return st.emaNear;
}

inline void   	mdhgUpdate	(double* /*grid*/, double /*price*/, double /*qty*/, long long /*nowMs*/)	{}
inline void   	mdhgSticky	(double* /*grid*/, long long /*halfLifeMs*/){}
inline double 	mdhgAsym	(double bidMem,double askMem)												{ return bidMem - askMem; }
inline double 	mdhgGrad	(const double* /*grid*/, double* /*slope*/)									{ return 0.0; }

} // namespace du
