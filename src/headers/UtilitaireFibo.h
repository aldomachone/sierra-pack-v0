#pragma once
#include "sierrachart.h"

namespace du {

struct FiboSet { float lv236=0, lv382=0, lv500=0, lv618=0, lv786=0; };

// Retracements entre swingHi et swingLo (up si hi>lo)
inline FiboSet fbRetrace(float swingHi, float swingLo)
{
  FiboSet f{};
  const bool up = swingHi >= swingLo;
  const float a = up ? swingHi : swingLo;
  const float b = up ? swingLo : swingHi;
  const float d = a - b;
  f.lv236 = a - 0.236f*d;
  f.lv382 = a - 0.382f*d;
  f.lv500 = a - 0.500f*d;
  f.lv618 = a - 0.618f*d;
  f.lv786 = a - 0.786f*d;
  return f;
}

} // namespace du
