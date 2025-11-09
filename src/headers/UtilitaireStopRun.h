#pragma once
#include "sierrachart.h"
#include "UtilitaireTapeBVC.h"

// Stop-run v0: gros delta absolu sur la barre
namespace du {

inline bool srTriggerSimple(const SCStudyInterfaceRef& sc, int idx, float thrAbsDelta)
{
  return fabsf(du::bvcDelta(sc, idx)) >= thrAbsDelta;
}

} // namespace du
