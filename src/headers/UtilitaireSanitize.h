#pragma once
#include "sierrachart.h"
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <cstdint>

namespace du {
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	UtilitaireSanitize.h — Fonctions mathématiques & nettoyage
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	Constantes
constexpr double 	kEps 				=	1e-12																																			;
constexpr double 	kPi  				=	3.14159265358979323846																															;
constexpr double 	kLn2 				=	0.69314718055994530942																															;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	Clamp & sanitize scalaires
template 			<typename T>
constexpr 			T clamp					(const T& x, const T& lo, const T& hi)	noexcept					{ return (x < lo) ? lo : (x > hi ? hi : x)									;	}
inline float  		sanitize				(float v														)	{ return std::isfinite(v) ? v : 0.0f										;	}
inline double 		sanitize				(double v														)	{ return std::isfinite(v) ? v : 0.0											;	}
inline double 		duClamp					(double x, double lo, double hi									)	{ return x < lo ? lo : (x > hi ? hi : x)									;	}
inline double 		duBound01				(double x														)	{ return duClamp		(x, 0.0, 1.0)										;	}
inline bool   		duIsFinite				(double x														)	{ return std::isfinite	(x)													;	}
inline bool   		duAlmostZero			(double x, double eps = kEps									)	{ return std::fabs		(x) <= eps											;	}
inline double 		duSafeSqrt				(double x														)	{ return std::sqrt		(x < 0.0 ? 0.0 : x)									;	}
inline double 		duSafeDiv				(double num, double den, double def								)	{ return (std::isfinite(den) && std::fabs(den) > kEps) ? (num / den) : def	;	}	// Division sûre
inline double 		duZeroIfNaN				(double x														)	{ return std::isfinite(x) ? x : 0.0											;	}
inline double 		duSanitizeClamp			(double x, double lo, double hi									)	{ return std::isfinite(x) ? duClamp(x, lo, hi) : 0.0						;	}	// Sanitize + bornes

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	Tableaux & statistiques
inline void 		duSanitizeArray			(double* p, int n												)
{
  if	(!p || n <= 0)						 return															;
  for	(int i = 0; i < n; ++i)
  if	(!std::isfinite(p[i])) 				p[i] = 0.0														;
}
inline void 		duSanitizeArray			(SCFloatArrayRef a												)
{
  const int n = a.GetArraySize();
  for (int i = 0; i < n; ++i)
    if (!std::isfinite(a[i])) a[i] = 0.0;
}
inline double 		duMeanSafe				(const double* p, int n											)
{
  if (!p || n <= 0) return 0.0;
  double s = 0.0; int c = 0;
  for (int i = 0; i < n; ++i)
    if (std::isfinite(p[i])) { s += p[i]; ++c; }
  return c > 0 ? s / c : 0.0;
}
inline double 		duStd					(const double* p, int n											)
{
  if (!p || n <= 1) return 0.0;

  double m = duMeanSafe(p, n);
  double v = 0.0;
  int    c = 0;

  for (int i = 0; i < n; ++i)
  {
    if (std::isfinite(p[i]))
    {
      double d = p[i] - m;
      v += d * d;
      ++c;
    }
  }

  return c > 1 ? std::sqrt(v / (c - 1)) : 0.0;
}
inline double 		duWeightedMean			(const double* x, const double* w, int n						)
{
  if (!x || !w || n <= 0) return 0.0;
  double sw = 0.0, sx = 0.0;
  for (int i = 0; i < n; ++i)
  {
    if (std::isfinite(x[i]) && std::isfinite(w[i]))
    {
      sx += x[i] * w[i];
      sw += w[i];
    }
  }
  return (sw > kEps) ? sx / sw : 0.0;
}
inline double 		duCov					(const double* a, const double* b, int n						)
{
  if (!a || !b || n <= 1) return 0.0;
  double ma = duMeanSafe(a, n), mb = duMeanSafe(b, n), s = 0.0; int c = 0;
  for (int i = 0; i < n; ++i)
  {
    if (std::isfinite(a[i]) && std::isfinite(b[i]))
    {
      s += (a[i] - ma) * (b[i] - mb);
      ++c;
    }
  }
  return c > 1 ? s / (c - 1) : 0.0;
}
inline double 		duCorr					(const double* a, const double* b, int n						)
{
  const double sa = duStd(a, n), sb = duStd(b, n);
  return duSafeDiv(duCov(a, b, n), sa * sb, 0.0);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	Normalisation linéaire [0..1]
inline double 		duNormalize				(double x, double lo, double hi									)
{
  if (hi <= lo) return 0.0;
  return duClamp((x - lo) / (hi - lo), 0.0, 1.0);
}

// Min/Max/Range
inline double 		duMin					(const double* p, int n											)
{
  double v = DBL_MAX;
  for (int i = 0; i < n; ++i)
    if (std::isfinite(p[i]) && p[i] < v) v = p[i];
  return (v == DBL_MAX) ? 0.0 : v;
}
inline double 		duMax					(const double* p, int n											)
{
  double v = -DBL_MAX;
  for (int i = 0; i < n; ++i)
    if (std::isfinite(p[i]) && p[i] > v) v = p[i];
  return (v == -DBL_MAX) ? 0.0 : v;
}
inline double 		duRange					(const double* p, int n											)	{return duMax(p, n) - duMin(p, n)							;	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	Distances / dérivées / intégration
inline double 		duDist2					(const double* a, const double* b, int n						)
{
  if (!a || !b || n <= 0) return 0.0;
  double s = 0.0;
  for (int i = 0; i < n; ++i)
  {
    double d = a[i] - b[i];
    s += d * d;
  }
  return std::sqrt(s);
}
inline double 		duDistNorm				(const double* a, const double* b, int n						)
{
  if (!a || !b || n <= 0) return 0.0;
  double s = 0.0; int c = 0;
  for (int i = 0; i < n; ++i)
  {
    double ai = a[i], bi = b[i];
    if (std::isfinite(ai) && std::isfinite(bi))
    {
      s += std::fabs(ai - bi);
      ++c;
    }
  }
  return (c > 0) ? s / c : 0.0;
}
inline double 		duDiff					(double now, double prev, double dt								)
{
  return duSafeDiv(now - prev, dt, 0.0);
}
inline double 		duIntegrate				(double accum, double x, double dt								)	{return accum + x * dt										;	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	EMA
inline float 		emaPct					(float x, float prev, int pct									)
{
  pct = clamp(pct, 1, 99);
  const float a = (float)pct / 100.0f;
  return a * x + (1.0f - a) * prev;
}
inline float 		emaAlpha				(float x, float prev, float a									)
{
  a = clamp(a, 0.0f, 1.0f);
  return a * x + (1.0f - a) * prev;
}
inline double 		duEmaPct				(double prev, double x, double alphaPct							)
{
  const double a = duClamp(alphaPct, 0.0, 100.0) / 100.0;
  return prev + a * (x - prev);
}
inline double 		duEmaClampPct			(double prev, double x, double alphaPct, double lo, double hi	)	{return duClamp(duEmaPct(prev, x, alphaPct), lo, hi)		;	}
inline double 		duAlphaFromHalfLifeMs	(double half_ms, double dt_ms									)
{
  const double h  = (half_ms <= 0.0) ? 1.0 : half_ms;
  const double dt = (dt_ms   <  0.0) ? 0.0 : dt_ms;
  const double a  = 1.0 - std::exp(-kLn2 * dt / h);
  return duBound01(a);
}
inline float 		duEmaHalfMs				(float x, float prev, double half_ms, double dt_ms				)
{
  const float a = (float)duAlphaFromHalfLifeMs(half_ms, dt_ms);
  return emaAlpha(x, prev, a);
}
inline double 		duEmaDerivative			(double prevEma, double emaNow, double dt_ms					)	{return duSafeDiv(emaNow - prevEma, dt_ms, 0.0)				;	}
inline double 		duEmaSaturating			(double prev, double x, double alphaPct, double limit			)
{
  double e    = duEmaPct(prev, x, alphaPct);
  double diff = e - prev;
  if (std::fabs(diff) > limit)
    e = prev + duClamp(diff, -limit, limit);
  return e;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	Z-score EMA (moyenne/variance EWMA, stable numériquement)
struct 				ZState
{
  double mean = 0.0;
  double var  = 0.0;
  bool   init = false;
};
inline float 		zEmaUpdate				(ZState& st, float x, int pct									)
{
  if (!std::isfinite(x)) x = 0.0f;
  const float a = clamp(pct, 1, 99) / 100.0f;
  if (!st.init)
  {
    st.mean = x;
    st.var  = 0.0;
    st.init = true;
    return 0.0f;
  }
  const double d = (double)x - st.mean;
  st.mean += a * d;
  st.var   = (1.0 - a) * (st.var + a * d * d);
  const double sd = (st.var > DBL_EPSILON) ? std::sqrt(st.var) : 1.0;
  return (float)((x - st.mean) / sd);
}
inline double 		duZEmaUpdate			(ZState& st, double x, int pct									)
{
  if (!std::isfinite(x)) x = 0.0;
  const double a = duClamp(pct, 1, 99) / 100.0;
  if (!st.init)
  {
    st.mean = x;
    st.var  = 0.0;
    st.init = true;
    return 0.0;
  }
  const double d = x - st.mean;
  st.mean += a * d;
  st.var   = (1.0 - a) * (st.var + a * d * d);
  const double sd = (st.var > DBL_EPSILON) ? std::sqrt(st.var) : 1.0;
  return (x - st.mean) / sd;
}
inline double 		duZScore				(double x, double mean, double std, double eps					)
{
  const double s = (std < eps) ? eps : std;
  return (x - mean) / s;
}
inline double 		duZScoreEma				(double& mean, double& var, double x, double alpha				)
{
  const double d = x - mean;
  mean += alpha * d;
  var   = (1.0 - alpha) * (var + alpha * d * d);
  const double sd = (var > DBL_EPSILON) ? std::sqrt(var) : 1.0;
  return (x - mean) / sd;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	Fonctions mathématiques complémentaires
inline double 		duSafeLog				(double x														)	{ return (x > kEps) ? std::log(x) : std::log(kEps)			;	}
inline double 		duSafePow				(double x, double p												)	{ return std::pow(std::fabs(x), p) * (x < 0 ? -1.0 : 1.0)	;	}
inline double 		duExpClamp				(double x, double lo, double hi									)	{ return std::exp(duClamp(x, lo, hi))						;	}
inline double 		duZScoreAbs				(double z														)  	{ return std::fabs(z)										;	}
inline double 		duEnergy				(double x														)   { return x * x												;	}
inline double 		duEntropy01				(double p														)
{
  return (p <= 0.0 || p >= 1.0)
    ? 0.0
    : -(p * std::log2(p) + (1.0 - p) * std::log2(1.0 - p));
}
inline bool   		duIsNanInf				(double x														)	{ return !std::isfinite(x)									;	}
inline double 		duRound					(double x, int digits											)
{
  double f = std::pow(10.0, digits);
  return std::round(x * f) / f;
}
inline double 		duTrunc					(double x, int digits											)
{
  double f = std::pow(10.0, digits);
  return std::trunc(x * f) / f;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	Transformations bornées
inline double 		duSigmoid				(double x														)	{ return 1.0 / (1.0 + std::exp(-x))							;	}
inline double 		duSoftsign				(double x														)	{ return x 	 / (1.0 + std::fabs(x))							;	}
inline double 		duZto01					(double z														)	{ return 0.5 * (1.0 + std::tanh(z * 0.5))					;	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	Diagnostic optionnel
inline void 		duPrintStats			(SCStudyInterfaceRef sc,
 const double* p,
 int n,
 const char* name = "")
{
  if (!p || n <= 0) return;

  double sum = 0.0, sum2 = 0.0; 
  int c = 0;

  for (int i = 0; i < n; ++i)
  {
    if (std::isfinite(p[i]))
    {
      sum  += p[i];
      sum2 += p[i] * p[i];
      ++c;
    }
  }

  if (c > 1)
  {
    const double mean = sum / c;
    const double var  = (sum2 / c) - (mean * mean);
    const double sd   = std::sqrt(std::fabs(var));

    SCString msg;
    if (name && name[0] != '\0')
    {
      msg += name;
      msg += ": ";
    }

    SCString tmp;
    tmp.Format("mean=%.6f std=%.6f", mean, sd);
    msg += tmp;

    sc.AddMessageToLog(msg, 0);
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	Extensions optionnelles — Calculs statistiques et transformations avancées
inline double 		duMedian				(double* p, int n												)																		// Médiane / Quantile simple (tri IN-PLACE du buffer p)
{
  if (!p || n <= 0) return 0.0;
  std::sort(p, p + n);
  return (n % 2) ? p[n / 2] : 0.5 * (p[n / 2 - 1] + p[n / 2]);
}
inline double 		duQuantile				(double* p, int n, double q										)
{
  if (!p || n <= 0) return 0.0;
  q = duClamp(q, 0.0, 1.0);
  std::sort(p, p + n);
  const double pos  = q * (n - 1);
  const int    i    = (int)pos;
  const double frac = pos - i;
  return (i + 1 < n) ? p[i] + frac * (p[i + 1] - p[i]) : p[i];
}
inline double 		duEmaFMA				(double prev, double x, double a								)	{return std::fma(a, (x - prev), prev)						;	}	// Fast FMA EMA
inline float 		duFastInvSqrt			(float x														)																		// Fast inverse sqrt (approx 1/sqrt(x))
{
  union { float f; std::uint32_t u; } conv;
  conv.f = x;
  conv.u = 0x5f3759dfu - (conv.u >> 1);
  float y = conv.f;
  return y * (1.5f - 0.5f * x * y * y);
}
inline double 		duBoxCox				(double x, double lambda										)																		// Box-Cox transformation
{
  return (lambda == 0.0)
    ? std::log1p(x)
    : ((std::pow(1.0 + x, lambda) - 1.0) / lambda);
}
inline double 		duZClip					(double z, double lim											)	{ return duClamp(z, -lim, lim)								;	}	// z-score clipping et inversion
inline double 		duUnZScore				(double z, double mean, double std								)	{ return mean + z * std										;	}

struct 				RangeState																																						// Struct de range glissant (min/max EMA)
{
  double min  = 0.0		;
  double max  = 0.0		;
  bool   init = false	;
};
inline void 		duRangeUpdate			(RangeState& st, double x, double alpha							)
{
	
  if (!st.init)
  {
    st.min  = x;
    st.max  = x;
    st.init = true;
    return;
  }
  st.min = duEmaFMA(st.min, std::min(st.min, x), alpha);
  st.max = duEmaFMA(st.max, std::max(st.max, x), alpha);
}
inline double 		duRangeNormalize		(RangeState& st, double x										)	{return duNormalize(x, st.min, st.max)						;	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	Macro de compatibilité
#ifndef DU_SANITIZE_SAFE
  #define DU_SANITIZE_SAFE(x) (std::isfinite(x) ? (x) : 0.0)
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	Log conditionnel (debug)
#ifdef DU_SANITIZE_DEBUG
  #define DU_LOG(sc, msg) (sc.AddMessageToLog(msg, 0))
#else
  #define DU_LOG(sc, msg) do{}while(0)
#endif

} // namespace du