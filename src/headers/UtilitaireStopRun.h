#pragma once
#include "sierrachart.h"
#include "UtilitaireTapeBVC.h"

// Stop-run v0: gros delta absolu sur la barre
namespace du {

inline bool 	srTriggerSimple	(const SCStudyInterfaceRef& sc, int idx, float thrAbsDelta)
{
  return fabsf(du::bvcDelta(sc, idx)) >= thrAbsDelta;
}

inline bool   	duStopRunGate	(int /*micropriceState*/, bool /*paceGate*/){ return true; }
inline double 	duStopRunScore	(double ofi,double sweep,double vacuum,double imbalance){ return 0.4*ofi + 0.3*sweep + 0.2*vacuum + 0.1*imbalance; }

} // namespace du
