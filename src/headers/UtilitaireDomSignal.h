#pragma once
#include <algorithm>
#include <cmath>
#include "sierrachart.h"
#include "UtilitaireSanitize.h"
#include "UtilitaireDom.h"
#include "UtilitaireTempo.h"  // du::toMs

namespace du {

// =====================================================================================
//                           ENUMS / ETATS EXISTANTS (compat v0)
// =====================================================================================

// Modes de placement des marqueurs
enum DuSigMarkerMode { DU_SIG_MARKER_CLOSE=0, DU_SIG_MARKER_BEST=1, DU_SIG_MARKER_PEAK=2 };

// Etat pour normalisation score / EMA(|score|)
struct DuSigNormState { double emaAbs; int inited; DuSigNormState(): emaAbs(0.0), inited(0) {} };

// =====================================================================================
//                           INPUTS & SUBGRAPHS HELPERS
// =====================================================================================

// Inputs communs 0..4
static inline void duSigSetupInputsCommon(SCStudyInterfaceRef sc)
{
  sc.Input[0].Name = "01. Fenetre niveaux DOM";      sc.Input[0].SetInt(20); sc.Input[0].SetIntLimits(1, DU_LEVELS);
  sc.Input[1].Name = "02. Seuil declenchement";      sc.Input[1].SetFloat(1.0f);
  sc.Input[2].Name = "03. Refractaire (barres)";     sc.Input[2].SetInt(2);  sc.Input[2].SetIntLimits(0, 1000);
  sc.Input[3].Name = "04. Mode marqueur prix";       sc.Input[3].SetCustomInputStrings("Close;Meilleur Bid/Ask;Pic niveau"); sc.Input[3].SetCustomInputIndex(DU_SIG_MARKER_BEST);
  sc.Input[4].Name = "05. EMA normalisation (%)";    sc.Input[4].SetInt(25); sc.Input[4].SetIntLimits(0, 95);
}

// Subgraph score (barre ignorée par échelle si DrawZeros=0 et DisplayAsMainPriceGraphValue=0)
static inline void duSigSetupScoreSubgraph(SCSubgraphRef& SG, const char* name, COLORREF col)
{
  SG.Name = name; SG.DrawStyle = DRAWSTYLE_BAR; SG.PrimaryColor = col; SG.DrawZeros = 0; SG.LineWidth = 2; SG.DisplayNameValueInWindowsFlags = 0; SG.LineLabel = 0; SG.DisplayAsMainPriceGraphValue = 0;
}

// Subgraph marker (Transparent Circle Variable). Valeurs ≡ prix. Taille réglée côté étude si désiré.
static inline void duSigSetupMarkerSubgraph(SCSubgraphRef& SG, const char* name, COLORREF col)
{
  SG.Name = name; SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; SG.PrimaryColor = col; SG.DrawZeros = 0; SG.LineWidth = 6; SG.DisplayNameValueInWindowsFlags = 0; SG.LineLabel = 0; SG.DisplayAsMainPriceGraphValue = 0;
}

// Désactive l’impact visuel sur l’échelle pour tous les SG visibles
static inline void duSigDisableScaleImpact(SCStudyInterfaceRef sc)
{
  for (int i=0; i<SC_SUBGRAPH_MAX; ++i) { sc.Subgraph[i].DisplayAsMainPriceGraphValue = 0; sc.Subgraph[i].DrawZeros = 0; }
}

// =====================================================================================
//                           MARQUEURS — PRIX / PLACE / CLEAR
// =====================================================================================

static inline double duSigComputeMarkerPrice(
  SCStudyInterfaceRef sc, int markerMode, bool isBid,
  double peakPrice, int offsetTicks)
{
  const int last = sc.ArraySize - 1; if (last < 0) return 0.0;
  double p = (double)sc.Close[last];
  if (markerMode == DU_SIG_MARKER_BEST)      p = isBid ? (double)sc.Bid[last] : (double)sc.Ask[last];
  else if (markerMode == DU_SIG_MARKER_PEAK) p = (peakPrice != 0.0 ? peakPrice : p);
  if (offsetTicks != 0) { const double ts = sc.TickSize; p += (double)offsetTicks * ts * (isBid ? -1.0 : +1.0); }
  return p;
}

static inline void duSigClearMarker(SCStudyInterfaceRef sc, int sg, int bar)
{ if (sg>=0 && bar>=0 && bar<sc.ArraySize) sc.Subgraph[sg][bar] = 0.0f; }

static inline void duSigSetMarker(SCStudyInterfaceRef sc, int sg, int bar, double price)
{ if (sg>=0 && bar>=0 && bar<sc.ArraySize) sc.Subgraph[sg][bar] = (float)price; }

// Affiche le marqueur uniquement sur la dernière barre. Utilise un persistent pour effacer l’ancienne.
static inline void duSigMarkerLastBarOnly(SCStudyInterfaceRef sc, int sg, double price, int persistentLastBarId)
{
  const int last = sc.ArraySize - 1; if (last < 0) return; int& prev = sc.GetPersistentInt(persistentLastBarId);
  if (prev >= 0 && prev < sc.ArraySize) duSigClearMarker(sc, sg, prev); duSigSetMarker(sc, sg, last, price); prev = last;
}

// Facteur de taille markers multi-niveaux
static inline double duSigMarkerSizeFactor(int grade, int multiLevelMode, bool boosted)
{ double f=1.0; if (multiLevelMode != 0 && grade>0) f += 0.4 * (double)(grade - 1); if (boosted) f *= 1.5; return f; }

// =====================================================================================
//                           NORMALISATION |x| EMA & CLAMPS
// =====================================================================================

static inline double duSigNormalizeScore(double score, DuSigNormState& st, int emaPct)
{
  double absx = score >= 0.0 ? score : -score; int pct = emaPct; if (pct < 0) pct = 0; if (pct > 100) pct = 100; const double a = (double)pct / 100.0;
  if (!st.inited) { st.emaAbs = absx; st.inited = 1; } else { st.emaAbs = a * absx + (1.0 - a) * st.emaAbs; }
  if (st.emaAbs <= 0.0) return 0.0; return score / st.emaAbs;
}

static inline void duSigUpdateNormState(SCStudyInterfaceRef sc, int ptrId, int emaPct, double absValue, double& magNormOut)
{
  DuSigNormState* st = (DuSigNormState*)sc.GetPersistentPointer(ptrId); if (!st) { st = new DuSigNormState(); sc.SetPersistentPointer(ptrId, st); }
  int pct = emaPct; if (pct < 0) pct = 0; if (pct > 100) pct = 100; if (pct == 0) { magNormOut = 0.0; return; }
  double normVal = duSigNormalizeScore(absValue, *st, pct); if (normVal < 0.0) normVal = -normVal; magNormOut = normVal;
}

static inline int duClampEmaPctWithWarn(SCStudyInterfaceRef sc, int pct, int persistentWarnId, const char* ctx)
{
  if (pct < 0) pct = 0; if (pct > 100) pct = 100; if (pct == 0) { int& w = sc.GetPersistentInt(persistentWarnId); if (!w) { SCString m; m.Format("%s: EMA%%=0 → normalisation OFF.", ctx); sc.AddMessageToLog(m, 0); w = 1; } } return pct;
}

// =====================================================================================
//                           REFRACTAIRE / COOLDOWN / STALE DOM
// =====================================================================================

static inline bool duSigCanTriggerByBars(int cur, int lastTrig, int refracBars)
{ if (refracBars <= 0) return true; if (lastTrig < 0) return true; return (cur - lastTrig) >= refracBars; }

static inline bool duCooldownMs(long long& lastTs, long long nowMs, long long cooldownMs)
{ if (cooldownMs <= 0) { lastTs = nowMs; return true; } if (nowMs - lastTs < cooldownMs) return false; lastTs = nowMs; return true; }

// DOM stale (bar‑based) — retourne true si figé depuis >= maxBars
static inline bool duUpdateDomStaleBars(
  SCStudyInterfaceRef sc,
  int persistentLastDomId, int persistentPrevBidId, int persistentPrevAskId,
  int maxBars, int lastBarIndex, int bidQty0, int askQty0)
{
  if (maxBars <= 0) return false; int& lastDomBar = sc.GetPersistentInt(persistentLastDomId); int& prevBidQ0 = sc.GetPersistentInt(persistentPrevBidId); int& prevAskQ0 = sc.GetPersistentInt(persistentPrevAskId);
  if (lastBarIndex == 0 && lastDomBar == 0 && prevBidQ0 == 0 && prevAskQ0 == 0) { lastDomBar = lastBarIndex; }
  else if (bidQty0 != prevBidQ0 || askQty0 != prevAskQ0) { lastDomBar = lastBarIndex; }
  prevBidQ0 = bidQty0; prevAskQ0 = askQty0; if (lastDomBar < 0) return false; return (lastBarIndex - lastDomBar) >= maxBars;
}

// DOM stale (temps) — retourne true si Δt>=ms depuis la dernière variation qty niveau 0
static inline bool duUpdateDomStaleMs(
  SCStudyInterfaceRef sc, int persistentLastChangeMsId,
  int bidQty0, int askQty0, long long nowMs, int staleMs)
{
  int& prevBidQ0 = sc.GetPersistentInt(persistentLastChangeMsId+1);
  int& prevAskQ0 = sc.GetPersistentInt(persistentLastChangeMsId+2);
  long long& tLast = *(long long*)sc.GetPersistentPointer(persistentLastChangeMsId);
  if (!sc.GetPersistentPointer(persistentLastChangeMsId)) { long long* p = new long long(0); sc.SetPersistentPointer(persistentLastChangeMsId, p); tLast = nowMs; prevBidQ0 = bidQty0; prevAskQ0 = askQty0; return false; }
  if (bidQty0 != prevBidQ0 || askQty0 != prevAskQ0) { tLast = nowMs; prevBidQ0 = bidQty0; prevAskQ0 = askQty0; }
  return (nowMs - tLast) >= staleMs;
}

// =====================================================================================
//                           PULL/STACK ENGINE CORE (compat v0)
// =====================================================================================

struct DuPullStackOutputs { double scoreBidNorm, scoreAskNorm, BullSig, BearSig, NetSig; DuPullStackOutputs(): scoreBidNorm(0.0), scoreAskNorm(0.0), BullSig(0.0), BearSig(0.0), NetSig(0.0) {} };

enum { DU_PSF_PTR_PREV_BID=100, DU_PSF_PTR_PREV_ASK=101, DU_PSF_PTR_NORM_BID=102, DU_PSF_PTR_NORM_ASK=103 };

enum { DU_PSF_INT_INIT=100 };

static inline void duPullStackEngineCleanup(SCStudyInterfaceRef sc)
{
  duSafeDeletePersistentDbl<DU_PSF_PTR_PREV_BID>(sc); duSafeDeletePersistentDbl<DU_PSF_PTR_PREV_ASK>(sc);
  if (DuSigNormState* stB = (DuSigNormState*)sc.GetPersistentPointer(DU_PSF_PTR_NORM_BID)) { delete stB; sc.SetPersistentPointer(DU_PSF_PTR_NORM_BID, nullptr); }
  if (DuSigNormState* stA = (DuSigNormState*)sc.GetPersistentPointer(DU_PSF_PTR_NORM_ASK)) { delete stA; sc.SetPersistentPointer(DU_PSF_PTR_NORM_ASK, nullptr); }
}

// weightProfile: 0=1/(n+1), 1=exp(-n/k), 2=core only (n<coreDepth)
static inline void duPullStackEngineCore(
  SCStudyInterfaceRef sc, int depth, int emaPct, int weightProfile, int weightParam,
  DuPullStackOutputs& out, bool& warmup)
{
  out = DuPullStackOutputs(); warmup = false; int maxDepth = depth; if (maxDepth < 1) maxDepth = 1; if (maxDepth > DU_LEVELS) maxDepth = DU_LEVELS; const int last = sc.ArraySize - 1; if (last < 0) { warmup = true; return; }
  int availBid = sc.GetBidMarketDepthNumberOfLevels(); int availAsk = sc.GetAskMarketDepthNumberOfLevels(); int avail = (availBid < availAsk ? availBid : availAsk); if (avail <= 0) { warmup = true; return; }
  int effectiveDepth = std::min(maxDepth, avail); int& inited = sc.GetPersistentInt(DU_PSF_INT_INIT);
  double* prevBid = duGetOrAllocPersistentDbl(sc, DU_PSF_PTR_PREV_BID, maxDepth); double* prevAsk = duGetOrAllocPersistentDbl(sc, DU_PSF_PTR_PREV_ASK, maxDepth);
  DuSigNormState* stBid = (DuSigNormState*)sc.GetPersistentPointer(DU_PSF_PTR_NORM_BID); if (!stBid){ stBid=new DuSigNormState(); sc.SetPersistentPointer(DU_PSF_PTR_NORM_BID, stBid);} DuSigNormState* stAsk = (DuSigNormState*)sc.GetPersistentPointer(DU_PSF_PTR_NORM_ASK); if (!stAsk){ stAsk=new DuSigNormState(); sc.SetPersistentPointer(DU_PSF_PTR_NORM_ASK, stAsk);} int coreDepth = (weightProfile==2 && weightParam>0 && weightParam<maxDepth)? weightParam : maxDepth;
  double scoreBidRaw=0.0, scoreAskRaw=0.0;
  for (int lvl=0; lvl<effectiveDepth; ++lvl)
  {
    s_MarketDepthEntry b{}, a{}; if (!sc.GetBidMarketDepthEntryAtLevel(b, lvl)) b.Quantity=0.0; if (!sc.GetAskMarketDepthEntryAtLevel(a, lvl)) a.Quantity=0.0; const double qB=(double)b.Quantity, qA=(double)a.Quantity; if (!inited) { prevBid[lvl]=qB; prevAsk[lvl]=qA; }
    const double dB=qB - prevBid[lvl]; const double dA=qA - prevAsk[lvl]; double w=1.0;
    if (weightProfile==0) w = 1.0 / (double)(lvl+1);
    else if (weightProfile==1) { const double k = (weightParam>0? (double)weightParam : 5.0); w = std::exp(-(double)lvl / k); }
    else if (weightProfile==2) { if (lvl>=coreDepth) w = 0.0; }
    scoreBidRaw += w * dB; scoreAskRaw += w * dA; prevBid[lvl]=qB; prevAsk[lvl]=qA;
  }
  inited = 1; out.scoreBidNorm = duSigNormalizeScore(scoreBidRaw, *stBid, emaPct); out.scoreAskNorm = duSigNormalizeScore(scoreAskRaw, *stAsk, emaPct);
  double BullSig=0.0, BearSig=0.0; if (out.scoreBidNorm>0.0) BullSig += out.scoreBidNorm; if (out.scoreAskNorm<0.0) BullSig += -out.scoreAskNorm; if (out.scoreAskNorm>0.0) BearSig += out.scoreAskNorm; if (out.scoreBidNorm<0.0) BearSig += -out.scoreBidNorm; out.BullSig=BullSig; out.BearSig=BearSig; out.NetSig=BullSig - BearSig;
}

// =====================================================================================
//                           ENGINE READ HELPERS (étude ↔ ENGINE)
// =====================================================================================

// Lecture d’un SG d’une étude ENGINE référencée par InEngineStudy. Tolère chart différent.
static inline bool duPsfEngineGetArray(
  SCStudyInterfaceRef sc, SCInputRef& InEngineStudy,
  int subgraphIndex, SCFloatArray& outArray, int /*lastIndex*/)
{
  const int engStudyID = InEngineStudy.GetStudyID(); if (engStudyID == 0) return false; const int engChart = InEngineStudy.GetChartNumber(); const int sg = (subgraphIndex>=0)? subgraphIndex : InEngineStudy.GetSubgraphIndex(); sc.GetStudyArrayFromChartUsingID(engChart, engStudyID, sg, outArray); return outArray.GetArraySize() > 0;
}

// Index compatible si tailles différentes
static inline int duPsfIdxCompat(int readerLast, const SCFloatArray& engArr)
{ const int n = engArr.GetArraySize(); return n>0 ? (readerLast < n ? readerLast : n-1) : -1; }

static inline bool duPsfEngineReadValue(
  SCStudyInterfaceRef sc, SCInputRef& InEngineStudy, int subgraphIndex,
  int readerLast, double& valueOut)
{
  SCFloatArray tmp; if (!duPsfEngineGetArray(sc, InEngineStudy, subgraphIndex, tmp, readerLast)) { valueOut=0.0; return false; }
  const int ie = duPsfIdxCompat(readerLast, tmp); if (ie < 0) { valueOut=0.0; return false; } valueOut = tmp[ie]; return true;
}

// =====================================================================================
//                           HYSTERESIS / M‑SUR‑N / COOLDOWN
// =====================================================================================

inline bool duHysteresis(int& state,double x,double onPct,double offPct){ if(state==0 && x>=onPct) state=1; else if(state==1 && x<=offPct) state=0; return state==1; }
inline bool duMSurN(int& counter,bool cond,int M,int N){ if(cond){ if(++counter>=M) return true; } else { if(++counter>=N) counter=0; } return false; }
inline bool duCooldown(long long& last,long long now,long long cooldownMs){ if(now-last<cooldownMs) return false; last=now; return true; }

// =====================================================================================
//                           P² PLACEHOLDERS / UTILS
// =====================================================================================

struct duP2Buf { double dummy; }; inline void duP2_Init(duP2Buf*,int){} inline void duP2_Update(duP2Buf*,double){} inline double duP2_Quantile(duP2Buf*,double){ return 0.0; }

inline int duNmsTopK(const double* v,int N,int /*radius*/,double /*promPct*/,int K,int* keepMask){ if(!keepMask) return 0; for(int i=0;i<N;++i) keepMask[i]=0; int kept=0; for(int i=0;i<N && kept<K; ++i){ keepMask[i]=1; ++kept; } return kept; }
inline double duPctOfMax(double x,double mx,double eps){ return mx>eps? x/mx : 0.0; }
inline double duSmoothStep01(double x){ if(x<=0) return 0; if(x>=1) return 1; return x*x*(3-2*x); }

// =====================================================================================
//                           FEATURES MLP (16) — PULL/STACK STANDARD
// =====================================================================================

// 0 scoreBidNorm, 1 scoreAskNorm, 2 BullSig, 3 BearSig, 4 NetSig,
// 5 emaAbsBid, 6 emaAbsAsk, 7 magNormBid, 8 magNormAsk,
// 9 lastTrigBars, 10 canTrigger, 11 domStaleBars, 12 domStaleMs,
// 13 depthAvail, 14 spreadTicks, 15 ts
static inline int duSig_features_ps_v1(
  SCStudyInterfaceRef sc,
  const DuPullStackOutputs& o,
  const DuSigNormState& stBid,
  const DuSigNormState& stAsk,
  int lastTriggerIndex,
  int refracBars,
  int domStaleBars,
  int domStaleMsFlag)
{
  double out[16]{}; const int last = sc.ArraySize - 1; const double ts = sc.TickSize;
  out[0]=du::sanitize(o.scoreBidNorm); out[1]=du::sanitize(o.scoreAskNorm); out[2]=du::sanitize(o.BullSig); out[3]=du::sanitize(o.BearSig); out[4]=du::sanitize(o.NetSig);
  out[5]=du::sanitize(stBid.emaAbs);  out[6]=du::sanitize(stAsk.emaAbs);
  // magNorm approximatifs = |score|/emaAbs
  const double mB = (stBid.emaAbs>0? std::fabs(o.scoreBidNorm)*stBid.emaAbs : 0.0);
  const double mA = (stAsk.emaAbs>0? std::fabs(o.scoreAskNorm)*stAsk.emaAbs : 0.0);
  out[7]=du::sanitize(mB); out[8]=du::sanitize(mA);
  const int lastTrigBars = (lastTriggerIndex>=0? (last - lastTriggerIndex) : last+1);
  out[9]=(double)lastTrigBars; out[10]=(double)duSigCanTriggerByBars(last, lastTriggerIndex, refracBars);
  out[11]=(double)domStaleBars; out[12]=(double)domStaleMsFlag;
  const int depthAvail = std::min(sc.GetBidMarketDepthNumberOfLevels(), sc.GetAskMarketDepthNumberOfLevels());
  out[13]=(double)depthAvail;
  const double bid = sc.Bid[last], ask=sc.Ask[last]; out[14] = (ts>0.0 && bid>0.0 && ask>0.0? (ask-bid)/ts : 0.0);
  out[15]=du::sanitize(ts);
  // Ecriture dans SG optionnels à la charge de l’étude appelante. On renvoie 16.
  return 16;
}

} // namespace du
