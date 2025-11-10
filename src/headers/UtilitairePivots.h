#pragma once
#include "sierrachart.h"

namespace du {

// Pivots classiques (daily) à partir de H/L/C fournis
struct 			PivotsClassic 			{ float P=0, R1=0, S1=0, R2=0, S2=0, R3=0, S3=0; };

inline 			PivotsClassic pvClassic	(float H, float L, float C)
{
  PivotsClassic p{};
  const float P = (H + L + C) / 3.0f;
  p.P = P;
  p.R1 = 2*P - L; p.S1 = 2*P - H;
  p.R2 = P + (H - L); p.S2 = P - (H - L);
  p.R3 = H + 2*(P - L); p.S3 = L - 2*(H - P);
  return p;
}

inline void 	pvCompute				(int /*set*/, int /*tf*/, double O,double H,double L,double C, double* out){ if(!out) return; out[0]=(H+L+C)/3.0; }
inline int  	pvState					(double price,double level){ if(price>level) return 1; if(price<level) return -1; return 0; }

} // namespace du
