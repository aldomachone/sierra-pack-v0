#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"   // du::sanitize
#include "UtilitaireTempo.h"      // du::toMs
#include "UtilitaireDom.h"        // du::readBidQtyAt, du::readAskQtyAt, du::domIsStale

// ============================================================================
// UtilitaireDomImbalSweep.h — Imbalance + Sweep DOM (v1 enrichi, compat v0)
// ----------------------------------------------------------------------------
// • 100% inline, aucune allocation, aucun état global.
// • Compat v0 conservée :
//    - imbalNearMinusFarPct(sc, levels, nearN, midN)
//    - duImbalance(qBid,qAsk,avail,&obi,&ratio)
//    - duImbalBands(near,mid,far,&score)
//    - duSweepDetect(qDelta,avail,thrAbs,thrRel,&score)
// • Ajouts v1 :
//    - Bandes Near/Mid/Far paramétrables, OBI par bande + pondération.
//    - Sweep côté Bid/Ask avec Δ>0 cumulés, seuils absolu/relatif, life.
//    - EMA des scores, hystérésis On/Off, réfractaire N, cooldown ms.
//    - Gates: DOM stale, spread max, profondeur min.
//    - 16 features MLP standardisées.
// ----------------------------------------------------------------------------
// Conventions :
//  - qBid/qAsk = tableaux de quantités par niveau (≥0). qDelta = Δ (ex: disparitions).
//  - avail = niveaux valides [0..avail-1].
//  - Lecture DOM via du::readBidQtyAt/du::readAskQtyAt si besoin.
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Compat v0 d'origine (préservée)
// ---------------------------------------------------------------------------
inline float 	imbalNearMinusFarPct	(const SCStudyInterfaceRef& sc, int levels, int nearN, int midN)
{
  const auto b = du::sumBands(sc, 1, levels, nearN, midN);
  float n=0,m=0,f=0; du::normalizePct(b.nearBid+b.nearAsk, b.midBid+b.midAsk, b.farBid+b.farAsk, n,m,f);
  return n - f;
}

inline void 	duImbalance				(const double* qBid,const double* qAsk,int avail,double* obi,double* ratio)
{
  double sb=0, sa=0; for(int i=0;i<avail;++i){ sb+=qBid?qBid[i]:0.0; sa+=qAsk?qAsk[i]:0.0; }
  if(obi)   *obi   = sb - sa;
  if(ratio) *ratio = (sa>1e-12? sb/sa : 0.0);
}

inline double 	duImbalBands			(double near,double mid,double far,double* score){
	double s = 0.60*near + 0.30*mid + 0.10*far; if(score) *score = s; return s; }

inline bool 	duSweepDetect			(const double* qDelta,int avail,double thrAbs,double thrRel,double* score){
  double s=0, base=0; for(int i=0;i<avail;++i){ const double d = qDelta? qDelta[i] : 0.0; if(d>0) s += d; base += std::fabs(d); }
  if(score) *score=s; return (s >= thrAbs) || (base>1e-12 && s/base >= thrRel);
}

// ---------------------------------------------------------------------------
// v1 — enrichi
// ---------------------------------------------------------------------------
struct 			IsBands { int startNear=0, cntNear=10; int startMid=10, cntMid=20; int startFar=30, cntFar=30; };

struct 			IsParams
{
  // Bandes et poids
  IsBands bands{}; double wNear=0.60, wMid=0.30, wFar=0.10;
  // EMA + hystérésis
  double alphaPct=70.0, hOn=0.9, hOff=0.5; int refractoryN=2; int cooldownMs=250;
  // Sweep
  double swThrAbs=50.0, swThrRel=0.20; double swMinLifeMs=120.0; // Δ cumulé sur Δ>0
  // Gates
  int requireFresh=1; int minAvail=10; double spreadMaxTks=4.0;
};

enum 			IsPhase:int { IS_IDLE=0, IS_ARMED=1, IS_TRIGGERED=2, IS_COOLDOWN=3 };

struct 			IsState
{
  // OBI par bande
  double obiNear=0, obiMid=0, obiFar=0, obiW=0, obiWEma=0; // Bid-Ask
  double baseNear=0, baseMid=0, baseFar=0;                  // Bid+Ask

  // Sweep scores
  double swBid=0, swAsk=0, swTot=0; // cumuls Δ>0

  // Normalisation et EMA
  double norm=0, normEma=0; // obiW/base

  // Hystérésis / phase
  int consecOver=0, consecUnder=0; int phase=IS_IDLE; long long tTrig=0, tFirstSw=0, tLast=0;

  // Util
  inline void reset_soft(){ obiNear=obiMid=obiFar=obiW=obiWEma=0; baseNear=baseMid=baseFar=0; swBid=swAsk=swTot=0; norm=normEma=0; consecOver=consecUnder=0; }
  inline void reset_all(){ reset_soft(); phase=IS_IDLE; tTrig=tFirstSw=tLast=0; }
};

// Sommes bornées sur [start..end]
inline double 	is_sum(const double* a,int avail,int start,int cnt){ if(!a||cnt<=0) return 0.0; if(start<0) start=0; const int e=start+cnt-1; double s=0; for(int i=start;i<=e && i<avail;++i) s+=a[i]; return s; }
inline double 	is_sum_pos(const double* a,int avail,int start,int cnt){ if(!a||cnt<=0) return 0.0; if(start<0) start=0; const int e=start+cnt-1; double s=0; for(int i=start;i<=e && i<avail;++i){ const double x=a[i]; if(x>0) s+=x; } return s; }

inline double 	is_ema(double prev,double x,double aPct){ const double a=aPct>0.0? aPct/100.0 : 0.0; return prev + a*(x - prev); }

// Lecture directe DOM, puis calcul OBI par bandes
inline void 	is_read_from_sc(const SCStudyInterfaceRef& sc, const IsParams& p, int avail,
                            double& obiNear,double& obiMid,double& obiFar,
                            double& baseNear,double& baseMid,double& baseFar)
{
  // Remplissage via helpers du::readBidQtyAt/du::readAskQtyAt
  auto band=[&](int s,int c){ double sb=0,sa=0; for(int li=s; li<s+c && li<avail; ++li){ sb+=du::readBidQtyAt(sc,li); sa+=du::readAskQtyAt(sc,li); } return std::pair<double,double>{sb,sa}; };
  const auto n = band(p.bands.startNear, p.bands.cntNear);
  const auto m = band(p.bands.startMid , p.bands.cntMid );
  const auto f = band(p.bands.startFar , p.bands.cntFar );
  obiNear = n.first - n.second; obiMid = m.first - m.second; obiFar = f.first - f.second;
  baseNear= n.first + n.second; baseMid= m.first + m.second; baseFar= f.first + f.second;
}

// Mise à jour à partir d’array NOW/PREV pour sweep + imbalance
// qB/A_now, qB/A_prev = quantités par niveau; dB/dA = Δ disparitions (>0) si fournis.
inline void 	is_update_arrays(IsState& st, const IsParams& p,
                            const double* qB_now,const double* qA_now,
                            const double* qB_prev,const double* qA_prev,
                            const double* dB,const double* dA,
                            int avail, long long nowMs)
{
  st.tLast = nowMs;
  // OBI par bandes
  st.obiNear = (is_sum(qB_now,avail,p.bands.startNear,p.bands.cntNear) - is_sum(qA_now,avail,p.bands.startNear,p.bands.cntNear));
  st.obiMid  = (is_sum(qB_now,avail,p.bands.startMid ,p.bands.cntMid ) - is_sum(qA_now,avail,p.bands.startMid ,p.bands.cntMid ));
  st.obiFar  = (is_sum(qB_now,avail,p.bands.startFar ,p.bands.cntFar ) - is_sum(qA_now,avail,p.bands.startFar ,p.bands.cntFar ));

  st.baseNear=  is_sum(qB_now,avail,p.bands.startNear,p.bands.cntNear) + is_sum(qA_now,avail,p.bands.startNear,p.bands.cntNear);
  st.baseMid =  is_sum(qB_now,avail,p.bands.startMid ,p.bands.cntMid ) + is_sum(qA_now,avail,p.bands.startMid ,p.bands.cntMid );
  st.baseFar =  is_sum(qB_now,avail,p.bands.startFar ,p.bands.cntFar ) + is_sum(qA_now,avail,p.bands.startFar ,p.bands.cntFar );

  // Pondération
  const double obiW = p.wNear*st.obiNear + p.wMid*st.obiMid + p.wFar*st.obiFar;
  st.obiW    = du::sanitize(obiW);
  st.obiWEma = is_ema(st.obiWEma, st.obiW, p.alphaPct);

  // Normalisation par base globale
  const double base = std::max(1e-9, st.baseNear + st.baseMid + st.baseFar);
  st.norm    = du::sanitize(st.obiW / base);
  st.normEma = is_ema(st.normEma, st.norm, p.alphaPct);

  // Sweep côté Bid/Ask via Δ>0 sur bandes
  const double swB = is_sum_pos(dB, avail, p.bands.startNear, p.bands.cntNear)
                   + is_sum_pos(dB, avail, p.bands.startMid , p.bands.cntMid )
                   + is_sum_pos(dB, avail, p.bands.startFar , p.bands.cntFar );
  const double swA = is_sum_pos(dA, avail, p.bands.startNear, p.bands.cntNear)
                   + is_sum_pos(dA, avail, p.bands.startMid , p.bands.cntMid )
                   + is_sum_pos(dA, avail, p.bands.startFar , p.bands.cntFar );
  st.swBid = du::sanitize(swB); st.swAsk = du::sanitize(swA); st.swTot = du::sanitize(swB + swA);
}

// Gates simples sur sc
inline bool 	is_gates_ok(const SCStudyInterfaceRef& sc, const IsParams& p)
{
  if (p.requireFresh && du::domIsStale(sc)) return false;
  const int avail = sc.GetMarketDepthNumberOfLevels(); if (avail < p.minAvail) return false;
  const double bid=sc.Bid[sc.Index], ask=sc.Ask[sc.Index], ts=sc.TickSize; if (ts>0.0 && bid>0.0 && ask>0.0){ const double spr=(ask-bid)/ts; if (spr>p.spreadMaxTks) return false; }
  return true;
}

// Étape hystérésis + réfractaire + cooldown. Retourne true si événement.
inline bool 	is_hysteresis_step(IsState& st, const IsParams& p, long long nowMs, bool sweepOK)
{
  if (st.phase == IS_COOLDOWN) {
    if (nowMs - st.tTrig >= p.cooldownMs) st.phase = IS_IDLE; else return false;
  }

  const bool over = (st.normEma >= p.hOn) && sweepOK;
  const bool under= (st.normEma <= p.hOff) || !sweepOK;

  if (over)  { st.consecOver++; st.consecUnder=0; if (st.phase==IS_IDLE){ st.phase=IS_ARMED; if(st.tFirstSw==0) st.tFirstSw=nowMs; } }
  if (under) { st.consecUnder++; st.consecOver=0; if (st.phase==IS_ARMED){ st.phase=IS_IDLE; st.tFirstSw=0; } }

  const bool refractoryOK = (st.phase==IS_ARMED) && (st.consecOver >= p.refractoryN) && (st.tFirstSw>0) && ((nowMs - st.tFirstSw) >= p.swMinLifeMs);
  if (refractoryOK){ st.phase=IS_TRIGGERED; st.tTrig=nowMs; st.phase=IS_COOLDOWN; return true; }
  return false;
}

// ---------------------------------------------------------------------------
// Features MLP v1 — 16 features
// 0 obiNear, 1 obiMid, 2 obiFar, 3 obiW, 4 obiWEma, 5 baseNear, 6 baseMid,
// 7 baseFar, 8 norm, 9 normEma, 10 swBid, 11 swAsk, 12 swTot,
// 13 consecOver, 14 consecUnder, 15 phase
// ---------------------------------------------------------------------------
inline int 		is_features_v1(const IsState& st, long long /*nowMs*/, double* out)
{
  if(!out) return 0;
  out[0]=du::sanitize(st.obiNear); out[1]=du::sanitize(st.obiMid); out[2]=du::sanitize(st.obiFar);
  out[3]=du::sanitize(st.obiW);    out[4]=du::sanitize(st.obiWEma);
  out[5]=du::sanitize(st.baseNear);out[6]=du::sanitize(st.baseMid);out[7]=du::sanitize(st.baseFar);
  out[8]=du::sanitize(st.norm);    out[9]=du::sanitize(st.normEma);
  out[10]=du::sanitize(st.swBid);  out[11]=du::sanitize(st.swAsk); out[12]=du::sanitize(st.swTot);
  out[13]=(double)st.consecOver;   out[14]=(double)st.consecUnder; out[15]=(double)st.phase; return 16;
}

} // namespace du
