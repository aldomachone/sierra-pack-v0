#pragma once
#include "sierrachart.h"

// Bulk Volume Classification v0 (proxy barre): utilise Up/DownVolume
namespace du {

enum 			tbAgg			{ TB_BUY=1, TB_SELL=-1, TB_UNK=0 };

inline void 	bvcBar			(const SCStudyInterfaceRef& sc, int idx, float& buyVol, float& sellVol)
{
  buyVol  = (float)sc.UpVolume[idx];
  sellVol = (float)sc.DownVolume[idx];
}

// Delta = buy - sell
inline float 	bvcDelta		(const SCStudyInterfaceRef& sc, int idx)
{
  return (float)sc.UpVolume[idx] - (float)sc.DownVolume[idx];
}

inline tbAgg 	tbClassifyAgg	(int /*tsEntry*/)			{ return TB_UNK; }
inline int   	tbBulkVolume	(tbAgg /*side*/, int sz)	{ return sz; }
inline int   	tbCumDeltaStep	(tbAgg side, int sz)		{ return (side==TB_BUY? +sz : side==TB_SELL? -sz : 0); }

} // namespace du
