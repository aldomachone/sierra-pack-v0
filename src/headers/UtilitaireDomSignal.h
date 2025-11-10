#pragma once
#include <algorithm>
#include <cmath>
#include "sierrachart.h"
#include "UtilitaireSanitize.h"
#include "UtilitaireDom.h"

namespace du {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Modes de placement des marqueurs
enum 					DuSigMarkerMode
{
  DU_SIG_MARKER_CLOSE = 0,
  DU_SIG_MARKER_BEST  = 1,
  DU_SIG_MARKER_PEAK  = 2
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Etat pour normalisation score / EMA(|score|)
struct 					DuSigNormState
{
  double emaAbs;
  int    inited;

  DuSigNormState(): emaAbs(0.0), inited(0) {}
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Inputs communs 0..4
static inline void 		duSigSetupInputsCommon		(SCStudyInterfaceRef sc													)
{
  sc.Input[0].Name = "01. Fenetre niveaux DOM";
  sc.Input[0].SetInt(20);
  sc.Input[0].SetIntLimits(1, DU_LEVELS);

  sc.Input[1].Name = "02. Seuil declenchement";
  sc.Input[1].SetFloat(1.0f);

  sc.Input[2].Name = "03. Refractaire (barres)";
  sc.Input[2].SetInt(2);
  sc.Input[2].SetIntLimits(0, 1000);

  sc.Input[3].Name = "04. Mode marqueur prix";
  sc.Input[3].SetCustomInputStrings("Close;Meilleur Bid/Ask;Pic niveau");
  sc.Input[3].SetCustomInputIndex(DU_SIG_MARKER_BEST);

  sc.Input[4].Name = "05. EMA normalisation (%)";
  sc.Input[4].SetInt(25);
  sc.Input[4].SetIntLimits(1, 95);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Subgraph "score"
static inline void 		duSigSetupScoreSubgraph		(SCSubgraphRef& SG, const char* name, COLORREF col						)
{
  SG.Name   = name;
  SG.DrawStyle = DRAWSTYLE_BAR;
  SG.PrimaryColor = col;
  SG.DrawZeros  = 0;
  SG.LineWidth  = 2;
  SG.DisplayNameValueInWindowsFlags = 0;
  SG.LineLabel  = 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Subgraph "marker"
static inline void 		duSigSetupMarkerSubgraph	(SCSubgraphRef& SG, const char* name, COLORREF col						)
{
  SG.Name   = name;
  SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
  SG.PrimaryColor = col;
  SG.DrawZeros  = 0;
  SG.LineWidth  = 6;
  SG.DisplayNameValueInWindowsFlags = 0;
  SG.LineLabel  = 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Prix du marqueur
static inline double 	duSigComputeMarkerPrice		(
  SCStudyInterfaceRef sc,
  int  markerMode,
  bool isBid,
  double peakPrice,
  int  offsetTicks)
{
  const int last = sc.ArraySize - 1;
  if (last < 0)
    return 0.0;

  double p = (double)sc.Close[last];

  if (markerMode == DU_SIG_MARKER_BEST)
  {
    p = isBid ? (double)sc.Bid[last] : (double)sc.Ask[last];
  }
  else if (markerMode == DU_SIG_MARKER_PEAK)
  {
    if (peakPrice != 0.0)
      p = peakPrice;
  }

  if (offsetTicks != 0)
  {
    double tick = sc.TickSize;
    p += (double)offsetTicks * tick * (isBid ? -1.0 : 1.0);
  }

  return p;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Effacement / placement markers
static inline void 		duSigClearMarker			(SCStudyInterfaceRef sc, int subgraphIndex, int barIndex				)
{
  if (subgraphIndex < 0)
    return;
  if (barIndex < 0 || barIndex >= sc.ArraySize)
    return;

  sc.Subgraph[subgraphIndex][barIndex] = 0.0f;
}
static inline void 		duSigSetMarker				(SCStudyInterfaceRef sc, int subgraphIndex, int barIndex, double price	)
{
  if (subgraphIndex < 0)
    return;
  if (barIndex < 0 || barIndex >= sc.ArraySize)
    return;

  sc.Subgraph[subgraphIndex][barIndex] = (float)price;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Normalisation score / EMA(|score|)
static inline double 	duSigNormalizeScore			(double score, DuSigNormState& st, int emaPct							)
{
  double absx = (score >= 0.0) ? score : -score;

  int pct = emaPct;
  if (pct < 0)   pct = 0;
  if (pct > 100) pct = 100;

  double a = (double)pct / 100.0;

  if (!st.inited)
  {
    st.emaAbs = absx;
    st.inited = 1;
  }
  else
  {
    st.emaAbs = a * absx + (1.0 - a) * st.emaAbs;
  }

  if (st.emaAbs <= 0.0)
    return 0.0;

  return score / st.emaAbs;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Helper normalisation de magnitude EMA(|x|)
static inline void 		duSigUpdateNormState		(
  SCStudyInterfaceRef sc,
  int    ptrId,        // ID de persistent pointer pour DuSigNormState*
  int    emaPct,       // pourcentage EMA
  double absValue,     // magnitude >= 0 (ex: |BullSig|)
  double& magNormOut
)
{
  DuSigNormState* st = (DuSigNormState*)sc.GetPersistentPointer(ptrId);
  if (!st)
  {
    st = new DuSigNormState();
    sc.SetPersistentPointer(ptrId, st);
  }

  int pct = emaPct;
  if (pct < 0)   pct = 0;
  if (pct > 100) pct = 100;

  if (pct == 0)
  {
    magNormOut = 0.0;
    return;
  }

  double normVal = duSigNormalizeScore(absValue, *st, pct);
  if (normVal < 0.0)
    normVal = -normVal;

  magNormOut = normVal;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Réfractaire en nombre de barres
static inline bool 		duSigCanTriggerByBars		(int currentIndex, int lastTriggerIndex, int refracBars					)
{
  if (refracBars <= 0)
    return true;
  if (lastTriggerIndex < 0)
    return true;

  const int delta = currentIndex - lastTriggerIndex;
  return (delta >= refracBars);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Facteur de taille pour marqueurs multi-niveaux
static inline double 	duSigMarkerSizeFactor		(
  int  grade,
  int  multiLevelMode,
  bool boosted
)
{
  double f = 1.0;
  if (multiLevelMode != 0 && grade > 0)
    f += 0.4 * (double)(grade - 1);
  if (boosted)
    f *= 1.5;
  return f;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Moteur Pull/Stack factorisé (DOM → scores + Bull/Bear/Net)
struct 					DuPullStackOutputs
{
  double scoreBidNorm;
  double scoreAskNorm;
  double BullSig;
  double BearSig;
  double NetSig;

  DuPullStackOutputs()
    : scoreBidNorm(0.0)
    , scoreAskNorm(0.0)
    , BullSig(0.0)
    , BearSig(0.0)
    , NetSig(0.0)
  {}
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// IDs persistents (pointeurs) pour le moteur
enum
{
  DU_PSF_PTR_PREV_BID = 100,
  DU_PSF_PTR_PREV_ASK = 101,
  DU_PSF_PTR_NORM_BID = 102,
  DU_PSF_PTR_NORM_ASK = 103
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// IDs persistents (entiers) pour le moteur
enum
{
  DU_PSF_INT_INIT = 100
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Nettoyage des persistents du moteur
static inline void 		duPullStackEngineCleanup	(SCStudyInterfaceRef sc													)
{
  duSafeDeletePersistentDbl<DU_PSF_PTR_PREV_BID>(sc);
  duSafeDeletePersistentDbl<DU_PSF_PTR_PREV_ASK>(sc);

  if (DuSigNormState* stB = (DuSigNormState*)sc.GetPersistentPointer(DU_PSF_PTR_NORM_BID))
  {
    delete stB;
    sc.SetPersistentPointer(DU_PSF_PTR_NORM_BID, nullptr);
  }
  if (DuSigNormState* stA = (DuSigNormState*)sc.GetPersistentPointer(DU_PSF_PTR_NORM_ASK))
  {
    delete stA;
    sc.SetPersistentPointer(DU_PSF_PTR_NORM_ASK, nullptr);
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Coeur Pull/Stack factorisé
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// weightProfile : 0 = 1/(n+1) / 1 = exp(-n/k) / 2 = core only (n < coreDepth => poids 1, sinon 0)
static inline void 		duPullStackEngineCore		(
  SCStudyInterfaceRef sc,
  int depth,
  int emaPct,
  int weightProfile,
  int weightParam,
  DuPullStackOutputs& out,
  bool& warmup
)
{
  out    = DuPullStackOutputs();
  warmup = false;

  int maxDepth = depth;
  if (maxDepth < 1)
    maxDepth = 1;
  if (maxDepth > DU_LEVELS)
    maxDepth = DU_LEVELS;

  const int last = sc.ArraySize - 1;
  if (last < 0)
  {
    warmup = true;
    return;
  }

  int availBid = sc.GetBidMarketDepthNumberOfLevels();
  int availAsk = sc.GetAskMarketDepthNumberOfLevels();
  int avail    = (availBid < availAsk ? availBid : availAsk);

  if (avail <= 0)
  {
    warmup = true;
    return;
  }

  int effectiveDepth = maxDepth;
  if (avail < effectiveDepth)
    effectiveDepth = avail;

  int& inited = sc.GetPersistentInt(DU_PSF_INT_INIT);

  double* prevBid = duGetOrAllocPersistentDbl(sc, DU_PSF_PTR_PREV_BID, maxDepth);
  double* prevAsk = duGetOrAllocPersistentDbl(sc, DU_PSF_PTR_PREV_ASK, maxDepth);

  DuSigNormState* stBid = (DuSigNormState*)sc.GetPersistentPointer(DU_PSF_PTR_NORM_BID);
  if (!stBid)
  {
    stBid = new DuSigNormState();
    sc.SetPersistentPointer(DU_PSF_PTR_NORM_BID, stBid);
  }
  DuSigNormState* stAsk = (DuSigNormState*)sc.GetPersistentPointer(DU_PSF_PTR_NORM_ASK);
  if (!stAsk)
  {
    stAsk = new DuSigNormState();
    sc.SetPersistentPointer(DU_PSF_PTR_NORM_ASK, stAsk);
  }

  int coreDepth = maxDepth;
  if (weightProfile == 2)
  {
    if (weightParam > 0 && weightParam < coreDepth)
      coreDepth = weightParam;
  }

  double scoreBidRaw = 0.0;
  double scoreAskRaw = 0.0;

  for (int lvl = 0; lvl < effectiveDepth; ++lvl)
  {
    s_MarketDepthEntry depBid;
    s_MarketDepthEntry depAsk;

    // signature: GetBidMarketDepthEntryAtLevel(DepthEntry, LevelIndex)
    if (!sc.GetBidMarketDepthEntryAtLevel(depBid, lvl))
      depBid.Quantity = 0.0;
    if (!sc.GetAskMarketDepthEntryAtLevel(depAsk, lvl))
      depAsk.Quantity = 0.0;

    double qBid = (double)depBid.Quantity;
    double qAsk = (double)depAsk.Quantity;

    if (!inited)
    {
      prevBid[lvl] = qBid;
      prevAsk[lvl] = qAsk;
    }

    double dBid = qBid - prevBid[lvl];
    double dAsk = qAsk - prevAsk[lvl];

    double w = 1.0;
    if (weightProfile == 0) // 1/(n+1)
    {
      w = 1.0 / (double)(lvl + 1);
    }
    else if (weightProfile == 1) // exp(-n/k)
    {
      double k = (weightParam > 0) ? (double)weightParam : 5.0;
      w = std::exp(-(double)lvl / k);
    }
    else if (weightProfile == 2) // core only
    {
      if (lvl >= coreDepth)
        w = 0.0;
    }

    scoreBidRaw += w * dBid;
    scoreAskRaw += w * dAsk;

    prevBid[lvl] = qBid;
    prevAsk[lvl] = qAsk;
  }

  inited = 1;

  out.scoreBidNorm = duSigNormalizeScore(scoreBidRaw, *stBid, emaPct);
  out.scoreAskNorm = duSigNormalizeScore(scoreAskRaw, *stAsk, emaPct);

  double BullSig = 0.0;
  if (out.scoreBidNorm > 0.0) BullSig += out.scoreBidNorm;
  if (out.scoreAskNorm < 0.0) BullSig += -out.scoreAskNorm;

  double BearSig = 0.0;
  if (out.scoreAskNorm > 0.0) BearSig += out.scoreAskNorm;
  if (out.scoreBidNorm < 0.0) BearSig += -out.scoreBidNorm;

  out.BullSig = BullSig;
  out.BearSig = BearSig;
  out.NetSig  = BullSig - BearSig;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DOM stale helper
// Convention de features (voir DOM_SIGNAL_PULLSTACK_SPIKE.cpp) :
//   - SG_SpreadRegime = -1 (wide) / 0 (normal) / +1 (tight)
//   - SG_DOMStale     = 0 (DOM actif) / 1 (DOM figé >= N barres)

// Ce helper met à jour 3 persistents :
//   * persistentLastDomId : dernier index de barre où le DOM a changé
//   * persistentPrevBidId : qty bid niveau 0 précédente
//   * persistentPrevAskId : qty ask niveau 0 précédente

// Retourne true si le DOM est figé depuis >= maxBars barres.

static inline bool 		duUpdateDomStaleBars(
  SCStudyInterfaceRef sc,
  int persistentLastDomId,
  int persistentPrevBidId,
  int persistentPrevAskId,
  int maxBars,
  int lastBarIndex,
  int bidQty0,
  int askQty0)
{
  if (maxBars <= 0)
    return false;

  int& lastDomBar = sc.GetPersistentInt(persistentLastDomId);
  int& prevBidQ0  = sc.GetPersistentInt(persistentPrevBidId);
  int& prevAskQ0  = sc.GetPersistentInt(persistentPrevAskId);

  // Première barre: init
  if (lastBarIndex == 0 && lastDomBar == 0 && prevBidQ0 == 0 && prevAskQ0 == 0)
  {
    lastDomBar = lastBarIndex;
  }
  else if (bidQty0 != prevBidQ0 || askQty0 != prevAskQ0)
  {
    // DOM a changé à cette barre
    lastDomBar = lastBarIndex;
  }

  prevBidQ0 = bidQty0;
  prevAskQ0 = askQty0;

  if (lastDomBar < 0)
    return false;

  const int barsSince = lastBarIndex - lastDomBar;
  return (barsSince >= maxBars);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Clamp EMA % + warning si 0
// - pct < 0   -> 0
// - pct > 100 -> 100
// - pct == 0  -> normalisation désactivée explicitement
// Le warning est loggé une seule fois par instance de study via persistentWarnId.

static inline int 		duClampEmaPctWithWarn(
  SCStudyInterfaceRef sc,
  int pct,
  int persistentWarnId,
  const char* contextName)
{
  if (pct < 0)   pct = 0;
  if (pct > 100) pct = 100;

  if (pct == 0)
  {
    int& warned = sc.GetPersistentInt(persistentWarnId);
    if (!warned)
    {
      SCString msg;
      msg.Format("DOM_SIGNAL_PULLSTACK: EMA %% = 0 dans '%s' -> normalisation DESACTIVEE.", contextName);
      sc.AddMessageToLog(msg, 0);
      warned = 1;
    }
  }

  return pct;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers ENGINE partagés : lecture des subgraphs d'une étude moteur DOM_PULLSTACK_ENGINE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lit un subgraph de l'étude moteur référencée par InEngineStudy.
// - InEngineStudy : input avec StudyID + éventuellement SubgraphIndex
// - subgraphIndex >= 0 : index explicite, sinon on prend InEngineStudy.GetSubgraphIndex()
// - lastIndex      : index de barre courant à sécuriser
// Retourne true si le subgraph existe et contient lastIndex.
// Lecture d’un SG d’une étude ENGINE référencée par InEngineStudy.
// Tolère ENGINE sur un autre chart. Ne dépend pas de la région.
// 'lastIndex' est ignoré ici pour éviter les faux négatifs.
static inline bool 		duPsfEngineGetArray(
  SCStudyInterfaceRef sc,
  SCInputRef&         InEngineStudy,
  int                 subgraphIndex,
  SCFloatArray&       outArray,
  int                 /*lastIndex*/)
{
  const int engStudyID = InEngineStudy.GetStudyID();
  if (engStudyID == 0) return false;

  const int engChart   = InEngineStudy.GetChartNumber();
  const int sg         = (subgraphIndex >= 0) ? subgraphIndex
                                              : InEngineStudy.GetSubgraphIndex();

  sc.GetStudyArrayFromChartUsingID(engChart, engStudyID, sg, outArray);
  return outArray.GetArraySize() > 0;
}

// Index sûr même si l’historique ENGINE et celui du lecteur n’ont pas la même taille.
static inline int 		duPsfIdxCompat(int readerLast, const SCFloatArray& engArr)
{
  const int n = engArr.GetArraySize();
  return n > 0 ? (readerLast < n ? readerLast : n - 1) : -1;
}

static inline bool 		duPsfEngineReadValue(
  SCStudyInterfaceRef sc,
  SCInputRef& InEngineStudy,
  int subgraphIndex,
  int readerLast,
  double& valueOut)
{
  SCFloatArray tmp;
  if (!duPsfEngineGetArray(sc, InEngineStudy, subgraphIndex, tmp, readerLast))
  { valueOut = 0.0; return false; }

  const int ie = duPsfIdxCompat(readerLast, tmp);
  if (ie < 0) { valueOut = 0.0; return false; }

  valueOut = tmp[ie];
  return true;
}

inline bool 			duHysteresis	(int& state,double x,double onPct,double offPct){ if(state==0 && x>=onPct) state=1; else if(state==1 && x<=offPct) state=0; return state==1; }
inline bool 			duMSurN			(int& counter,bool cond,int M,int N){ if(cond){ if(++counter>=M) return true; } else { if(++counter>=N) counter=0; } return false; }
inline bool 			duCooldown		(long long& last,long long now,long long cooldownMs){ if(now-last<cooldownMs) return false; last=now; return true; }

struct 					duP2Buf			{ double dummy; };
inline void   			duP2_Init		(duP2Buf*,int){}
inline void   			duP2_Update		(duP2Buf*,double){}
inline double 			duP2_Quantile	(duP2Buf*,double){ return 0.0; }

inline int 				duNmsTopK		(const double* v,int N,int /*radius*/,double /*promPct*/,int K,int* keepMask){
  if(!keepMask) return 0; for(int i=0;i<N;++i) keepMask[i]=0; for(int i=0;i<N && i<K;++i) keepMask[i]=1; return K<N?K:N;
}
inline double 			duPctOfMax		(double x,double mx,double eps){ return mx>eps? x/mx : 0.0; }
inline double 			duSmoothStep01	(double x){ if(x<=0) return 0; if(x>=1) return 1; return x*x*(3-2*x); }

} // namespace du
