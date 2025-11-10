#pragma once
#include "sierrachart.h"
#include <cstdint>

namespace du {


enum 				duSide					{ DU_BID=0, DU_ASK=1 };
enum 				duGateMode				{ DU_FREEZE=0, DU_ZERO=1, DU_CONTINUE=2 };
enum 				duCrossState			{ DU_BELOW=0, DU_CROSSUP=1, DU_ABOVE=2, DU_CROSSDOWN=3 };
struct 				duZone					{ double lo{0}, hi{0}; };
constexpr double 	DU_EPS = 1e-12;

// v0: identité. Versionner ici si l’index change plus tard.
inline int 			compatIdx				(int oldIdx)																{ return oldIdx; }

inline bool 		getStudyArray			(const SCStudyInterfaceRef& sc, int studyId, int oldIdx, SCFloatArray& out)	{
  return sc.GetStudyArrayUsingID(studyId, compatIdx(oldIdx), out) != 0;
}

inline bool 		tryGetStudyValue		(const SCStudyInterfaceRef& sc, int studyId, int oldIdx, int idx, float& v)	{
  SCFloatArray arr;
  if(!getStudyArray(sc, studyId, oldIdx, arr)) return false;
  v = arr[idx];
  return true;
}

inline bool 		duCompatIdx				(int oldIdx, int* newIdx)													{ if(newIdx)*newIdx=oldIdx; return true; }
inline int  		duCompatSg				(SCStudyInterfaceRef, int /*family*/, int sgFixed)							{ return sgFixed; }
inline int  		duCompatInput			(SCStudyInterfaceRef, const char* /*name*/, int fallbackIdx)				{ return fallbackIdx; }
inline bool 		duCompatCheckFeatSig	(int /*expected*/, int /*actual*/)											{ return true; }

} // namespace du
