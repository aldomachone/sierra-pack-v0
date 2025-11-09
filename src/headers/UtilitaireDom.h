#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// === DOM helpers (v0) ===
// Lecture niveau, agrégations Near/Mid/Far, check "stale" basique.

namespace du {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Constantes
#ifndef DU_LEVELS
  #define DU_LEVELS 60
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Couleurs défaut (tranches)
#ifndef DU_BID_T1
  #define DU_BID_T1 RGB(180,230,255)
#endif
#ifndef DU_BID_T2
  #define DU_BID_T2 RGB(120,200,255)
#endif
#ifndef DU_BID_T3
  #define DU_BID_T3 RGB( 55,115,200)
#endif
#ifndef DU_ASK_T1
  #define DU_ASK_T1 RGB(255,180,255)
#endif
#ifndef DU_ASK_T2
  #define DU_ASK_T2 RGB(255,128,255)
#endif
#ifndef DU_ASK_T3
  #define DU_ASK_T3 RGB(180, 60,180)
#endif
#ifndef DU_COL_GRAY_MED
  #define DU_COL_GRAY_MED RGB(128,128,128)
#endif
#ifndef DU_EPS_GAP
  #define DU_EPS_GAP 1.0
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// LineLabel flags (fallback sûrs)
#ifndef LL_DISPLAY_NAME
  #define LL_DISPLAY_NAME 0x0001
#endif
#ifndef LL_DISPLAY_VALUE
  #define LL_DISPLAY_VALUE 0x0002
#endif
#ifndef LL_NAME_ALIGN_RIGHT
  #define LL_NAME_ALIGN_RIGHT 0x0008
#endif
#ifndef LL_VALUE_ALIGN_RIGHT
  #define LL_VALUE_ALIGN_RIGHT 0x0020
#endif
#ifdef LL_VALUE_ALIGN_FAR_RIGHT
  #define DU_VALUE_ALIGN_FAR_LL  LL_VALUE_ALIGN_FAR_RIGHT
#else
  #define DU_VALUE_ALIGN_FAR_LL  LL_VALUE_ALIGN_RIGHT
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Modes d’échantillonnage
#define DU_SMP_PRIX_CUMULE     0
#define DU_SMP_PRIX_NON_CUMULE 1
#define DU_SMP_PAR_NIVEAU      2
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Helpers génériques
static inline int  		duClamp									(int v, int lo, int hi																		)	{ return v<lo?lo:(v>hi?hi:v)					; }
static inline void 		duCopy									(double* dst, const double* src, int n														)	{ for (int i=0;i<n;i++) dst[i] = src[i]			; }
static inline void 		duZero									(double* dst, int n																			)	{ for (int i=0;i<n;i++) dst[i] = 0.0			; }
static inline int  		duLabelFlags_NameRight_ValueFarRight	(																							)	{
return 															(LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_RIGHT | DU_VALUE_ALIGN_FAR_LL			)	;
}
static inline bool 		duGetLastIndex							(SCStudyInterfaceRef sc, int& last															)	{ last = sc.ArraySize - 1;	return last >= 0	; }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Z-order tranches (1–10 devant)
static inline int 		duZSlot60_Tranches						(int i																						)	{
  if 															(i < 0			) i = 0																															; 
  else if 														(i >= DU_LEVELS	) i = DU_LEVELS-1																												;
  if      														(i < 10			) return 50 + i																													;   // 1–10 au-dessus
  else if 														(i < 20			) return 40 + (i-10)																											;   // 11–20
  else if 														(i < 30			) return 30 + (i-20)																											;   // 21–30
  else             	 	return      							(i - 30			)																																;   // >30 derrière
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Lecture DOM (quantités & prix)
static inline int 		duReadBidQtyAtLevel						(SCStudyInterfaceRef sc, int level0															)	{
  if 															(level0 < 0) return 0																																	;
  s_MarketDepthEntry 	md																																		{													}	;
  return 				sc.GetBidMarketDepthEntryAtLevel		(md, level0) ? 	(int)md.Quantity : 0																													;
}
static inline int 		duReadAskQtyAtLevel						(SCStudyInterfaceRef sc, int level0															)	{
  if 															(level0 < 0) return 0																																	;
  s_MarketDepthEntry 	md																																		{													}	;
  return 				sc.GetAskMarketDepthEntryAtLevel		(md, level0) ? (int)md.Quantity : 0																														;
}
static inline double 	duReadBidPriceAtLevel					(SCStudyInterfaceRef sc, int level0															)	{
  if 															(level0 < 0) return 0.0																																	;
  s_MarketDepthEntry 	md																																		{													}	;
  return 				sc.GetBidMarketDepthEntryAtLevel		(md, level0) ? md.Price : 0.0																															;
}
static inline double 	duReadAskPriceAtLevel					(SCStudyInterfaceRef sc, int level0															)	{
  if 															(level0 < 0) return 0.0																																	;
  s_MarketDepthEntry 	md																																		{													}	;
  return 				sc.GetAskMarketDepthEntryAtLevel		(md, level0) ? md.Price : 0.0																															;
}
static inline int 		duReadBidBandRaw						(SCStudyInterfaceRef sc, int firstLevel1, int count, double* rawOut							)	{
  int 					f = firstLevel1																															;
  if 					(f < 1) f 	= 1																															;
  const int 			start0 		= f - 1																														;
  const int 			n      		= duClamp					(count,1,DU_LEVELS)																				;
  for					(int i=0;i<n;i++) 						rawOut[i] = (double)duReadBidQtyAtLevel(sc, start0+i);
  return 				n																																		;
}
static inline int 		duReadAskBandRaw						(SCStudyInterfaceRef sc, int firstLevel1, int count, double* rawOut							)	{
  int 					f 			= firstLevel1																												;
  if 					(f < 1) f 	= 1																															;
  const int 			start0 		= f - 1																														;
  const int 			n      		= duClamp					(count,1,DU_LEVELS);
  for					(int i=0;i<n;i++) 						rawOut[i] = (double)duReadAskQtyAtLevel(sc, start0+i);
  return 				n																																		;
}
static inline void 		duRequireDOM							(SCStudyInterfaceRef sc																		)	{ sc.UsesMarketDepthData = 1					; }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Disponibilités DOM
static inline int 		duBidAvail								(SCStudyInterfaceRef sc																		)	{ return sc.GetBidMarketDepthNumberOfLevels()	; }
static inline int 		duAskAvail								(SCStudyInterfaceRef sc																		)	{ return sc.GetAskMarketDepthNumberOfLevels()	; }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Transformations & marquage des trous
static inline void 		duApplySamplingMode						(const double* raw, int n, int mode, double sign , double* out								)	{
  if															(mode == DU_SMP_PRIX_CUMULE){
    double 				acc = 0.0;
    for(int i=0;i<n;i++){ acc += raw[i]; out[i] = sign*acc; }
  }else{
    for(int i=0;i<n;i++) out[i] = sign*raw[i];
  }
}
static inline void 		duApplyGapMark							(const double* raw, int n, int mode, int markGaps, double sign, double* inout				)	{
  if(!markGaps) return;
  if(mode == DU_SMP_PRIX_CUMULE) return;
  for(int i=0;i<n;i++) if(raw[i] <= 0.0) inout[i] = sign * DU_EPS_GAP;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Bandes prêtes (affichage / calcul)
static inline int 		duReadBidBandModeGaps					(SCStudyInterfaceRef sc, int firstLevel1, int count, int mode, int markGaps, double* out	)	{
  double raw[DU_LEVELS];
  const int n = duReadBidBandRaw(sc, firstLevel1, count, raw);
  duApplySamplingMode(raw, n, mode, +1.0, out);
  duApplyGapMark     (raw, n, mode, markGaps, +1.0, out);
  return n;
}
static inline int 		duReadAskBandModeGaps					(SCStudyInterfaceRef sc, int firstLevel1, int count, int mode, int markGaps, double* out	)	{
  double raw[DU_LEVELS];
  const int n = duReadAskBandRaw(sc, firstLevel1, count, raw);
  duApplySamplingMode(raw, n, mode, -1.0, out);
  duApplyGapMark     (raw, n, mode, markGaps, -1.0, out);
  return n;
}
static inline int 		duReadBidBandModeCalc					(SCStudyInterfaceRef sc, int firstLevel1, int count, int mode, double* out					)	{
  double raw[DU_LEVELS];
  const int n = duReadBidBandRaw(sc, firstLevel1, count, raw);
  duApplySamplingMode(raw, n, mode, +1.0, out);
  return n;
}
static inline int 		duReadAskBandModeCalc					(SCStudyInterfaceRef sc, int firstLevel1, int count, int mode, double* out					)	{
  double raw[DU_LEVELS];
  const int n = duReadAskBandRaw(sc, firstLevel1, count, raw);
  duApplySamplingMode(raw, n, mode, -1.0, out);
  return n;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Init SG / écriture / labels / couleurs
static inline void 		duDisableValuesAndDataLine				(SCSubgraphRef SG																			)	{
  SG.DisplayNameValueInWindowsFlags = 0;
  SG.DisplayNameValueInDataLine     = 0;
  SG.IncludeInStudySummary          = 0;
  SG.IncludeInSpreadsheet           = 0;
}
static inline void 		duInit60_SubgraphsBid_Z					(SCStudyInterfaceRef sc																		)	{
  const int flags = duLabelFlags_NameRight_ValueFarRight();
  for(int i=0;i<DU_LEVELS;i++){
    const int z = duZSlot60_Tranches(i);
    SCSubgraphRef SG = sc.Subgraph[z];
    SCString nm; nm.Format("B%d", i+1);
    SG.Name      = nm;
    SG.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_TO_ZERO;
    SG.DrawZeros = 0;
    SG.LineLabel = flags;
    SG.PrimaryColor = DU_COL_GRAY_MED;
    duDisableValuesAndDataLine(SG);
  }
}
static inline void 		duInit60_SubgraphsAsk_Z					(SCStudyInterfaceRef sc																		)	{
  const int flags = duLabelFlags_NameRight_ValueFarRight();
  for(int i=0;i<DU_LEVELS;i++){
    const int z = duZSlot60_Tranches(i);
    SCSubgraphRef SG = sc.Subgraph[z];
    SCString nm; nm.Format("A%d", i+1);
    SG.Name      = nm;
    SG.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_TO_ZERO;
    SG.DrawZeros = 0;
    SG.LineLabel = flags;
    SG.PrimaryColor = DU_COL_GRAY_MED;
    duDisableValuesAndDataLine(SG);
  }
}
static inline void 		duWriteBandToBar_Z						(SCStudyInterfaceRef sc, int barIndex, const double* vals, int n							)	{
  const int m = duClamp(n,1,DU_LEVELS);
  for(int i=0;i<m;i++){
    const int z = duZSlot60_Tranches(i);
    sc.Subgraph[z][barIndex] = vals[i];
  }
}
static inline void 		duSetLabelsVisibleLastBarOnly_Z			(SCStudyInterfaceRef sc, int isLastBar														)	{
  const int flags = isLastBar ? duLabelFlags_NameRight_ValueFarRight() : 0;
  for(int i=0;i<DU_LEVELS;i++){
    const int z = duZSlot60_Tranches(i);
    sc.Subgraph[z].LineLabel = flags;
  }
}
static inline void 		duApplyTrancheColors60_Z				(SCStudyInterfaceRef sc, int c1, int c2, int c3, int crest									)	{
  for(int i=0;i<DU_LEVELS;i++){
    const int 			z = duZSlot60_Tranches(i);
    if      (i < 10) sc.Subgraph[z].PrimaryColor = c1	;
    else if (i < 20) sc.Subgraph[z].PrimaryColor = c2	;
    else if (i < 30) sc.Subgraph[z].PrimaryColor = c3	;
    else             sc.Subgraph[z].PrimaryColor = crest;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Styles utilitaires (2 styles supportés)
static inline void 		duApplyStyleAndWidth60_Z				(SCStudyInterfaceRef sc, int drawstyle, int linewidth										)	{
  for (int i=0;i<DU_LEVELS;i++){
    const int z = duZSlot60_Tranches(i);
    SCSubgraphRef SG = sc.Subgraph[z];
    SG.DrawStyle = drawstyle;
    if (drawstyle == DRAWSTYLE_TRANSPARENT_CUSTOM_VALUE_AT_Y && linewidth > 0)
      SG.LineWidth = linewidth;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// UI (décochages)
static inline void 		duUnsetStudyGUI							(SCStudyInterfaceRef sc																		)	{
						sc.DisplayStudyName        = 0	;
						sc.DisplayStudyInputValues = 0	;
						sc.IncludeInStudySummary   = 0	;
						sc.IncludeInSpreadsheet    = 0	;
}
static inline void 		duDrawUnderMain							(SCStudyInterfaceRef sc, int enable = 1														)	{
  sc.DrawStudyUnderneathMainPriceGraph = enable ? 1 : 0;
}
static inline void 		duUI_DisableGlobalNameValue				(SCStudyInterfaceRef sc																		)	{
  sc.SetChartStudyDisplaySubgraphsNameAndValue					(sc.ChartNumber, sc.StudyGraphInstanceID, 0													)	;
}
static inline void 		duUI_DisableInputValues					(SCStudyInterfaceRef sc																		)	{
  sc.SetChartStudyDisplayStudyInputValues						(sc.ChartNumber, sc.StudyGraphInstanceID, 0													)	;
}
static inline void 		duUI_DisableAlwaysShowLabels			(SCStudyInterfaceRef sc																		)	{
  sc.SetStudyAlwaysShowNameValueLabelsWhenEnabled				(sc.ChartNumber, sc.StudyGraphInstanceID, 0													)	;
}
static inline void 		duUI_DisableAll							(SCStudyInterfaceRef sc																		)	{
  duUnsetStudyGUI				(sc)	;
  duUI_DisableGlobalNameValue	(sc)	;
  duUI_DisableInputValues		(sc)	;
  duUI_DisableAlwaysShowLabels	(sc)	;
}
static inline void 		duForceStudyUI							(SCStudyInterfaceRef sc																		)	{ duUI_DisableAll(sc)							; }
static inline void 		duDisablePerSubgraph					(SCStudyInterfaceRef sc																		)	{
  for (int i = 0; i < DU_LEVELS; ++i){
    const int 			z = duZSlot60_Tranches(i);
    SCSubgraphRef 		SG 	= sc.Subgraph[z];
    duDisableValuesAndDataLine(SG);
    SG.LineLabel 		= duLabelFlags_NameRight_ValueFarRight();
  }
}
static inline void 		duUI_InitOnceOff						(SCStudyInterfaceRef sc, int persistentId = 497												)	{
  int& done = sc.GetPersistentInt(persistentId);
  if (!done){
    duUI_DisableAll(sc);
    done = 1;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Réglages communs par défaut (factorisés)
static inline void 		duApplyCommonDOMDefaults				(SCStudyInterfaceRef sc																		)	{
  sc.AutoLoop            = 0			;
  sc.UpdateAlways        = 1			;
  sc.ValueFormat         = 26			;
  sc.UsesMarketDepthData = 1			;
  sc.ScaleRangeType      = SCALE_AUTO	;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Temps / gating “DOM figé”
static inline double 	duNowMs									(SCStudyInterfaceRef sc																		)	{
  return sc.CurrentSystemDateTime.GetAsDouble() * 86400000.0;
}
static inline bool 		duBandChangedRawMinDelta				(const double* nowRaw, const double* prevRaw, int n, int minDeltaQty						)	{
  if (minDeltaQty < 1) minDeltaQty = 1;
  for (int i = 0; i < n; ++i){
    const int a = (int)nowRaw[i];
    const int b = (int)prevRaw[i];
    const int d = a >= b ? (a - b) : (b - a);
    if (d >= minDeltaQty) return true;
  }
  return false;
}
static inline bool 		duStaleUpdate							(const double* nowRaw, double* prevRaw, int n,
                                 double& lastChangeMs, double nowMs, int minDeltaQty){
  if (nowMs < lastChangeMs) lastChangeMs = nowMs;
  const bool changed = duBandChangedRawMinDelta(nowRaw, prevRaw, n, minDeltaQty);
  if (changed){
    lastChangeMs = nowMs;
    for (int i=0;i<n;i++) prevRaw[i] = nowRaw[i];
  }
  return changed;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Persistents helpers
template<int PtrId>
static inline void 		duSafeDeletePersistentDbl				(SCStudyInterfaceRef sc																		)	{
  if (double* p = (double*)sc.GetPersistentPointer(PtrId)){
    delete[] p;
    sc.SetPersistentPointer(PtrId, nullptr);
  }
}
static inline double* 	duGetOrAllocPersistentDbl				(SCStudyInterfaceRef sc, int ptrId, int size = DU_LEVELS									)	{
  if (ptrId <= 0) return nullptr;
  double* ptr = (double*)sc.GetPersistentPointer(ptrId);
  if (!ptr){
    if (size < 1) size = DU_LEVELS;
    ptr = new double[size]();
    sc.SetPersistentPointer(ptrId, ptr);
  }
  return ptr;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Logging utils (one-shot)
enum 																																							{
  DU_LOG_LVLS     = 1<<0,
  DU_LOG_FIRSTL1  = 1<<1,
  DU_LOG_DQ       = 1<<2,
  DU_LOG_STALEMS  = 1<<3,
  DU_LOG_STYLE    = 1<<4,
  DU_LOG_LINEW    = 1<<5,
  DU_LOG_TICKSIZE = 1<<6,
  DU_LOG_BPV      = 1<<7
};
static inline void 		duLogOnce								(SCStudyInterfaceRef sc, int& mask, int bit, const char* msg								)	{
  if ((mask & bit) == 0){
    sc.AddMessageToLog(msg, 1);
    mask |= bit;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Cœur générique DOM LIMITS 60
struct 					duDomLimitsIds																															{
  int pidPrev, pidCurr, pidCalc, pidGatePrev, pidPrevPx;
  int iidLastBar, iidInited, iidColInitBase, iidLabelsOn, iidLogMask;
  int didLastChangeMs;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Setup factorisé des Inputs + UI + Subgraphs
static inline void 		duSetupDOMInputs						(SCStudyInterfaceRef sc, bool isBid															)	{
  duApplyCommonDOMDefaults	(sc)	;
  duDrawUnderMain			(sc, 1)	;

  sc.Input		[0].Name  										=	"01. Nombre de niveaux (1..60)"								;
  sc.Input		[0].SetInt(60)																									;
	
  sc.Input		[1].Name  										=	"02. Premier niveau (1 = meilleur)"							;
  sc.Input		[1].SetInt(1);
	
  sc.Input		[2].Name  										=	"03. Mode d’échantillonnage (Affichage)"					;
  sc.Input		[2].SetCustomInputStrings								("Cumul qty/valeur;Non-cumul qty/valeur;Par niveau")	;
  sc.Input		[2].SetCustomInputIndex(DU_SMP_PRIX_NON_CUMULE)																	;
	
  sc.Input		[3].Name  										=	"04. Marquer explicitement les trous (Oui/Non)"				;
  sc.Input		[3].SetYesNo(1)																									;
	
  sc.Input		[4].Name  										=	"05. Mode calcul (Signaux/MLP)"								;
  sc.Input		[4].SetCustomInputStrings								("Cumul qty/valeur;Non-cumul qty/valeur;Par niveau")	;
  sc.Input		[4].SetCustomInputIndex(DU_SMP_PRIX_CUMULE)																		;

  if (isBid) {
    sc.Input	[5].Name 										=	"06. Couleur tranche 1–10 	(Bid)"	; sc.Input[5].SetColor(DU_BID_T1)		;
    sc.Input	[6].Name 										=	"07. Couleur tranche 11–20	(Bid)"	; sc.Input[6].SetColor(DU_BID_T2)		;
    sc.Input	[7].Name 										=	"08. Couleur tranche 21–30 	(Bid)"	; sc.Input[7].SetColor(DU_BID_T3)		;
    sc.Input	[8].Name 										=	"09. Couleur reste   >30 	(Bid)"	; sc.Input[8].SetColor(DU_COL_GRAY_MED)	;
  } else {	
    sc.Input	[5].Name 										=	"06. Couleur tranche 1–10 	(Ask)"	; sc.Input[5].SetColor(DU_ASK_T1)		;
    sc.Input	[6].Name 										=	"07. Couleur tranche 11–20 	(Ask)"	; sc.Input[6].SetColor(DU_ASK_T2)		;
    sc.Input	[7].Name 										=	"08. Couleur tranche 21–30 	(Ask)"	; sc.Input[7].SetColor(DU_ASK_T3)		;
    sc.Input	[8].Name 										=	"09. Couleur reste   >30 	(Ask)"	; sc.Input[8].SetColor(DU_COL_GRAY_MED)	;
  }

  sc.Input		[9].Name   										=	 "10. Seuil DOM figé (ms)"						;
  sc.Input		[9].SetInt(1500)																						;
	
  sc.Input		[10].Name  										=	 "11. Mode DOM figé"							;
  sc.Input		[10].SetCustomInputStrings							("Geler;Mettre à zéro;Continuer")				;
  sc.Input		[10].SetCustomInputIndex(0)																			;
	
  sc.Input		[11].Name  										=	 "12. Δmin DOM figé (qty)"						;
  sc.Input		[11].SetInt(1)																						;
	
  sc.Input		[12].Name  										=	 "13. Style"									;
  sc.Input		[12].SetCustomInputStrings							("Transparent Fill→0;Transparent Custom @Y")	;
  sc.Input		[12].SetCustomInputIndex(0)																			;
	
  sc.Input		[13].Name  										=	 "14. Épaisseur si 'Custom @Y'"					;
  sc.Input		[13].SetInt(6)																						;
  sc.Input		[13].SetIntLimits(0,16)																				;
	
  sc.Input		[14].Name  										=	 "15. Afficher en valeur monétaire (Oui/Non)"	;
  sc.Input		[14].SetYesNo(0)																						;
	
  sc.Input		[15].Name  										=	 "16. Convertir en devise via BPV (Oui/Non)"	;
  sc.Input		[15].SetYesNo(1)																						;

  if (isBid) duInit60_SubgraphsBid_Z(sc);
  else       duInit60_SubgraphsAsk_Z(sc);

  duUI_DisableAll(sc);
  duDisablePerSubgraph(sc);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Cœur générique
 static inline void 	duDomLimits60Core						(SCStudyInterfaceRef sc, bool isBid, const duDomLimitsIds& id								)	{
  const int 			cur = sc.ArraySize - 1;
  if (cur < 0) return		;
  duUI_InitOnceOff(sc)		;

  const int 			in_nLvls_raw   	= sc.Input[0].GetInt	()			;
  const int 			in_firstL1_raw 	= sc.Input[1].GetInt	()			;
  const int 			in_modeAff_raw 	= sc.Input[2].GetIndex	()			;
  const int 			in_markGaps    	= sc.Input[3].GetYesNo	() ? 1 : 0	;
  const int 			in_modeCalc    	= sc.Input[4].GetIndex	()			;

  const int 			c1 				= sc.Input[5].GetColor	()			;
  const int 			c2 				= sc.Input[6].GetColor	()			;
  const int 			c3 				= sc.Input[7].GetColor	()			;
  const int 			cR 				= sc.Input[8].GetColor	()			;
	
  const int 			staleMs   		= sc.Input[9].GetInt	()			;
  const int 			staleMode 		= sc.Input[10].GetIndex	()			;
  const int 			in_dq_raw 		= sc.Input[11].GetInt	()			;
	
  const int 			in_style_raw	= sc.Input[12].GetIndex	()			;
  const int 			in_lw_raw   	= sc.Input[13].GetInt	()			;
  const int 			useMoney    	= sc.Input[14].GetYesNo	() ? 1 : 0	;
  const int 			useBPV      	= sc.Input[15].GetYesNo	() ? 1 : 0	;

  int& 					logmask 		= sc.GetPersistentInt(id.iidLogMask);

  int 					nLvls 			= duClamp(in_nLvls_raw, 1, DU_LEVELS);
  if (nLvls != in_nLvls_raw)
    duLogOnce	(sc, logmask, DU_LOG_LVLS		, "01. Nombre de niveaux clampé à 1–60.");

  int firstL1 = in_firstL1_raw;
  if (firstL1 < 1){
      firstL1 = 1;
      duLogOnce	(sc, logmask, DU_LOG_FIRSTL1	, "02. Premier niveau < 1 : clampé à 1.");
  }

  int modeAff  = duClamp(in_modeAff_raw, 0, 2);
  int modeCalc = duClamp(in_modeCalc,    0, 2);

  int dqIn = in_dq_raw;
  if (dqIn < 1) {
    dqIn = 1;
    duLogOnce(sc, logmask, DU_LOG_DQ			, "12. Δmin DOM figé < 1 : clampé à 1.");
  } else if (dqIn > 1000000) {
    dqIn = 1000000;
    duLogOnce(sc, logmask, DU_LOG_DQ			, "12. Δmin DOM figé > 1 000 000 : clampé à 1 000 000.");
  }

  int styleIdx  = duClamp(in_style_raw, 0, 1);
  if (styleIdx != in_style_raw)
    duLogOnce(sc, logmask, DU_LOG_STYLE			, "13. Style invalide : clampé (0..1).");

  int lineW = in_lw_raw;
  if (lineW < 0) {
    lineW = 0;
    duLogOnce(sc, logmask, DU_LOG_LINEW			, "14. Épaisseur < 0 : clampé à 0.");
  } else if (lineW > 16) {
    lineW = 16;
    duLogOnce(sc, logmask, DU_LOG_LINEW			, "14. Épaisseur > 16 : clampé à 16.");
  }

  if (staleMs <= 0)
    duLogOnce(sc, logmask, DU_LOG_STALEMS		, "10. Seuil DOM figé (ms) ≤ 0 : risque de comportement inattendu.");

  if (useMoney && sc.TickSize <= 0.0)
    duLogOnce(sc, logmask, DU_LOG_TICKSIZE		, "TickSize ≤ 0 : monétaire imprécis.");
  if (useMoney && useBPV && sc.CurrencyValuePerTick <= 0.0)
    duLogOnce(sc, logmask, DU_LOG_BPV			, "CurrencyValuePerTick = 0 : conversion BPV indisponible (bpv=1).");

  const int   	markGaps	= in_markGaps			;
  const bool  	needPx  	= (useMoney != 0)		;

  const double	sign 		= isBid ? +1.0 : -1.0	;
  const double	bpv  		= (useMoney && useBPV && sc.TickSize > 0.0 && sc.CurrencyValuePerTick > 0.0)
                    ? (sc.CurrencyValuePerTick / sc.TickSize)
                    : 1.0;

  double* 				prev        	= duGetOrAllocPersistentDbl	(sc, id.pidPrev			);
  double* 				curr        	= duGetOrAllocPersistentDbl	(sc, id.pidCurr			);
  double* 				calc        	= duGetOrAllocPersistentDbl	(sc, id.pidCalc			);
  double* 				gatePrevRaw 	= duGetOrAllocPersistentDbl	(sc, id.pidGatePrev		);
  double* 				prevPx      	= duGetOrAllocPersistentDbl	(sc, id.pidPrevPx		);

  int&    				lastBar      	= sc.GetPersistentInt		(id.iidLastBar			);
  int&    				inited       	= sc.GetPersistentInt		(id.iidInited			);
  int&    				labelsOn     	= sc.GetPersistentInt		(id.iidLabelsOn			);
  double& 				lastChangeMs 	= sc.GetPersistentDouble	(id.didLastChangeMs	);

  int& 					colInit 		= sc.GetPersistentInt		(id.iidColInitBase + 0	);
  int& 					pC1     		= sc.GetPersistentInt		(id.iidColInitBase + 1	);
  int& 					pC2     		= sc.GetPersistentInt		(id.iidColInitBase + 2	);
  int& 					pC3     		= sc.GetPersistentInt		(id.iidColInitBase + 3	);
  int& 					pCR     		= sc.GetPersistentInt		(id.iidColInitBase + 4	);
  int& 					pSty    		= sc.GetPersistentInt		(id.iidColInitBase + 5	);
  int& 					pW      		= sc.GetPersistentInt		(id.iidColInitBase + 6	);

  if(!colInit || c1!=pC1 || c2!=pC2 || c3!=pC3 || cR!=pCR){
    duApplyTrancheColors60_Z(sc, c1, c2, c3, cR);
    pC1=c1; pC2=c2; pC3=c3; pCR=cR; colInit=1	;
  }

  const int ds = (styleIdx==1) ? DRAWSTYLE_TRANSPARENT_CUSTOM_VALUE_AT_Y
                               : DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_TO_ZERO	;
  const int lw = (ds == DRAWSTYLE_TRANSPARENT_CUSTOM_VALUE_AT_Y) ? lineW : 0	;
  if(!colInit || pSty!=styleIdx || pW!=lw){
    duApplyStyleAndWidth60_Z(sc, ds, lw);
    pSty = styleIdx; pW = lw;
  }

  const int nRead = isBid
    ? duReadBidBandModeGaps(sc, firstL1, nLvls, modeAff, markGaps, curr)
    : duReadAskBandModeGaps(sc, firstL1, nLvls, modeAff, markGaps, curr);

  double currPx[DU_LEVELS];
  duZero(currPx, DU_LEVELS);
  if (needPx){
    const int start0 = firstL1 - 1;
    const int avail  = isBid ? duBidAvail(sc) : duAskAvail(sc);
    for(int i=0;i<nLvls;i++){
      const int li = start0 + i;
      if(li < avail){
        currPx[i] = isBid ? duReadBidPriceAtLevel(sc, li) : duReadAskPriceAtLevel(sc, li);
      }
    }
  }

  if(!markGaps){
    const int avail  = isBid ? duBidAvail(sc) : duAskAvail(sc);
    const int start0 = firstL1 - 1;
    for(int i=0;i<nLvls;i++){
      const int li = start0 + i;
      if(li >= avail){
        curr[i]   = prev[i];
        if (needPx)
          currPx[i] = prevPx[i];
      }
    }
  }

  const double nowMs = duNowMs(sc);

  if (!inited){
    lastBar = cur;
    duCopy(gatePrevRaw, curr, nLvls);
    lastChangeMs = nowMs;

    if (needPx){
      for(int i=0;i<nLvls;i++) prevPx[i] = currPx[i];
    }

    double render[DU_LEVELS]; duCopy(render, curr, nLvls);
    if (useMoney){
      if (modeAff == DU_SMP_PRIX_CUMULE){
        double rawInit[DU_LEVELS];
        if (isBid) duReadBidBandRaw(sc, firstL1, nLvls, rawInit);
        else       duReadAskBandRaw(sc, firstL1, nLvls, rawInit);
        double acc = 0.0;
        for(int i=0;i<nLvls;i++){
          const double px = (currPx[i]!=0.0)?currPx[i]:prevPx[i];
          const double q  = rawInit[i];
          if (q > 0.0) acc += q * (px!=0.0?px:1.0) * bpv;
          render[i] = sign * acc;
        }
      } else {
        for(int i=0;i<nLvls;i++){
          const double px = (currPx[i]!=0.0)?currPx[i]:prevPx[i];
          render[i] *= (px!=0.0?px:1.0) * bpv;
        }
      }
    }
    duWriteBandToBar_Z(sc, cur, render, nLvls);

    duCopy(prev, curr, nLvls);
    if (isBid) duReadBidBandModeCalc(sc, firstL1, nLvls, modeCalc, calc)	;
    else       duReadAskBandModeCalc(sc, firstL1, nLvls, modeCalc, calc)	;

    duSetLabelsVisibleLastBarOnly_Z(sc, 1)	;
    labelsOn = 1							;
    inited   = 1							;
    return									;
  }

  if (nRead > 0){
    (void)duStaleUpdate	(curr, gatePrevRaw, nRead, lastChangeMs, nowMs, dqIn)		;
  }
  const bool isStale 				=	((nowMs - lastChangeMs) > (double)staleMs)	;

  if (cur != lastBar){
    duWriteBandToBar_Z					(sc, lastBar, prev, nLvls)					;
    duSetLabelsVisibleLastBarOnly_Z		(sc, 0)										;
    labelsOn 						=	0											;
    lastBar  						=	cur											;
  }

  double out[DU_LEVELS]; duCopy(out, curr, nLvls);
  double const* pxForOut = currPx;
  if (isStale && staleMode == 0) { duCopy(out, prev, nLvls); pxForOut = prevPx; }
  else if (isStale && staleMode == 1) { duZero(out, nLvls); pxForOut = currPx; }

  const bool changedOut = duBandChangedRawMinDelta(out, prev, nLvls, dqIn);

  if	(changedOut || !labelsOn)	{double render[DU_LEVELS];

  if (!useMoney){duCopy(render, out, nLvls);
    } else {
      if (modeAff == DU_SMP_PRIX_CUMULE){
        double rawForMoney[DU_LEVELS];
        if (isStale && staleMode == 0)	{duCopy(rawForMoney, gatePrevRaw, nLvls);
        } else {
          if (isBid) duReadBidBandRaw	(sc, firstL1, nLvls, rawForMoney);
          else       duReadAskBandRaw	(sc, firstL1, nLvls, rawForMoney);

          if(!markGaps){
            const int avail  = isBid ? duBidAvail(sc) : duAskAvail(sc);
            const int start0 = firstL1 - 1;
            for(int i=0;i<nLvls;i++){
              const int li = start0 + i;
              if(li >= avail) rawForMoney[i] = gatePrevRaw[i];
            }
          }
        }

        double acc = 0.0;
        for(int i=0;i<nLvls;i++){
			const double px  = (pxForOut[i]!=0.0)?pxForOut[i]:prevPx[i];
			const double qty = rawForMoney[i];
          if (qty > 0.0) acc += qty * (px!=0.0?px:1.0) * bpv;
          render[i] = sign * acc;
        }
      } else {
        for(int i=0;i<nLvls;i++){
          const double px = (pxForOut[i]!=0.0)?pxForOut[i]:prevPx[i];
          render[i] = out[i] * (px!=0.0?px:1.0) * bpv;
        }
      }
    }

    duWriteBandToBar_Z(sc, cur, render, nLvls);
    if(!labelsOn){
      duSetLabelsVisibleLastBarOnly_Z(sc, 1);
      labelsOn = 1;
    }
    duCopy(prev, out, nLvls);

    if (needPx){
      for(int i=0;i<nLvls;i++)
        if(pxForOut[i]!=0.0) prevPx[i] = pxForOut[i];
    }
  }

  if (changedOut && !(isStale && staleMode == 0)){
    if (isBid) duReadBidBandModeCalc(sc, firstL1, nLvls, modeCalc, calc);
    else       duReadAskBandModeCalc(sc, firstL1, nLvls, modeCalc, calc);
  }
}

} // namespace du
