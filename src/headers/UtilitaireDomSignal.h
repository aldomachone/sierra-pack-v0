#pragma once
#include <algorithm>
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// === Aides signaux (v0) ===
// Hystérésis, M-sur-N, P² simple, small NMS/TopK placeholders.

namespace du {

  // hysteresis : entrée x, états ON/OFF (%)
  struct HystState { float y=0.0f; bool on=false; };
  inline bool hysteresis(HystState& st, float x, float onPct, float offPct)
  {
    onPct  = (float)clamp(onPct , 1.0f, 99.0f);
    offPct = (float)clamp(offPct, 0.0f, onPct - 0.1f);
    if (!st.on && x >= onPct)  st.on = true;
    if ( st.on && x <= offPct) st.on = false;
    st.y = x;
    return st.on;
  }

  // mSurN : valide si au moins M échantillons "true" parmi N derniers
  template<int N = 3>
  inline bool mSurN(bool sample, int M, unsigned& bitmask)
  {
    static_assert(N <= (int)(8 * sizeof(unsigned)), "N trop grand");
    bitmask = ((bitmask << 1) | (sample ? 1u : 0u)) & ((1u << N) - 1u);
    int cnt = 0; unsigned tmp = bitmask;
    while (tmp) { cnt += (tmp & 1u); tmp >>= 1u; }
    return cnt >= M;
  }

  // p2Update : mise à jour d'un seuil simple par quantile approx (EMA du max local v0)
  inline float p2Update(float x, float prev, int emaPct)
  {
    return emaPct((float)clamp(emaPct, 1, 99), x, prev); // réutilise du::emaPct via ADL
  }

  // nmsTopK v0 placeholder : renvoie x si > thr, sinon 0
  inline float nmsTopK(float x, float thr) { return x > thr ? x : 0.0f; }

} // namespace du
