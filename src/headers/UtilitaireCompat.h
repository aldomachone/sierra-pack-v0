#pragma once
#include "sierrachart.h"

// === Compat & indexation SG (v0) ===
namespace du {

  // Compat d'index SG simple : retourne newIdx=oldIdx (v0)
  inline int compatIdx(int oldIdx) { return oldIdx; }

  // Helper lecture SG d'une autre étude avec compat
  inline bool getStudyArray(const SCStudyInterfaceRef& sc, int studyId, int oldIdx, SCFloatArray& out)
  {
    const int sg = compatIdx(oldIdx);
    return sc.GetStudyArrayUsingID(studyId, sg, out) != 0;
  }

} // namespace du
