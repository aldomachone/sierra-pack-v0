#pragma once
#include "sierrachart.h"

namespace du {

// v0: identité. Versionner ici si l’index change plus tard.
inline int compatIdx(int oldIdx){ return oldIdx; }

inline bool getStudyArray(const SCStudyInterfaceRef& sc, int studyId, int oldIdx, SCFloatArray& out){
  return sc.GetStudyArrayUsingID(studyId, compatIdx(oldIdx), out) != 0;
}

inline bool tryGetStudyValue(const SCStudyInterfaceRef& sc, int studyId, int oldIdx, int idx, float& v){
  SCFloatArray arr;
  if(!getStudyArray(sc, studyId, oldIdx, arr)) return false;
  v = arr[idx];
  return true;
}

} // namespace du
