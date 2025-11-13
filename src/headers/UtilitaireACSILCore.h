#pragma once
#include "sierrachart.h"
// ============================================================================
// UtilitaireACSILCore.h — Wrappers ACSIL génériques pour Sierra Chart
//
// Rôle : fournir une couche d'abstraction propre pour manipuler :
//   - Index, temps de barres, statut de clôture
//   - Prix / formats / ticks
//   - BaseData, Subgraphs, Inputs
//   - Tableaux (SCFloatArray, SCSubgraphRef, etc.)
//   - MarketDepth et Time&Sales (accès de base)
//   - Paramètres "headless" standard pour les engines
// ============================================================================
#ifndef DU_UNSUPPORTED
  #define DU_UNSUPPORTED(name) /* TODO: wrapper for: name */
#endif

namespace du {
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 Index / état des barres / temps de graphique
inline int 			BarIndex					(const SCStudyInterfaceRef sc										)	{return sc.Index																;	}	 // Index courant dans le chart
inline int 			ChartArraySize				(const SCStudyInterfaceRef sc										)	{return sc.ArraySize															;	}	 // Taille du tableau principal du chart (BaseData/Subgraphs)
inline int 			ArraySize					(const SCStudyInterfaceRef sc										)	{return ChartArraySize(sc)														;	}	 // Alias rétro-compatible
inline bool 		IsLastIndex					(const SCStudyInterfaceRef sc										)	{return sc.Index == sc.ArraySize - 1											;	}
inline bool 		IsFirstIndex				(const SCStudyInterfaceRef sc										)	{return sc.Index == 0															;	}
inline int 			GetBarHasClosedStatus		(const SCStudyInterfaceRef sc										)	{return sc.GetBarHasClosedStatus(sc.Index)										;	}	 // Statut de clôture de la barre
inline bool 		IsBarClosed					(const SCStudyInterfaceRef sc										)	{return GetBarHasClosedStatus(sc) == BHCS_BAR_HAS_CLOSED						;	}
inline bool 		IsBarFinal					(const SCStudyInterfaceRef sc										)	{return GetBarHasClosedStatus(sc) != BHCS_BAR_NOT_CLOSED						;	}
inline SCDateTime 	BarStart					(const SCStudyInterfaceRef sc										)	{return sc.BaseDateTimeIn[sc.Index]												;	}	 // Date/heure de barres
inline SCDateTime 	BarEnd						(const SCStudyInterfaceRef sc										)	{return sc.BaseDateTimeIn[sc.Index] + sc.SecondsPerBar							;	}
inline SCDateTime 	BarDateTime					(const SCStudyInterfaceRef sc										)	{return sc.BaseDateTimeIn[sc.Index]												;	}
inline int 			BarOffsetFromLastVisible	(const SCStudyInterfaceRef sc, int index							)	{return sc.IndexOfLastVisibleBar - index										;	}	 // Offset par rapport à la dernière barre visible (0 = dernière visible)
inline int 			BarOffsetFromLastVisible	(const SCStudyInterfaceRef sc										)	{return BarOffsetFromLastVisible(sc, sc.Index)									;	}	 // Variante pratique pour la barre courante
inline int 			BarNumber					(const SCStudyInterfaceRef sc										)	{return BarOffsetFromLastVisible(sc)											;	}	 // Alias rétro-compatible : historiquement "BarNumber" = offset depuis la dernière barre visible (0 = bar la plus récente visible).
inline int 			LastVisibleIndex			(const SCStudyInterfaceRef sc										)	{return sc.IndexOfLastVisibleBar												;	}
inline int 			BarCount					(const SCStudyInterfaceRef sc										)	{return sc.ArraySize															;	}
inline SCDateTime 	Now							(const SCStudyInterfaceRef sc										)	{return sc.CurrentSystemDateTime												;	}	 // Temps système / session
inline int 			SessionStartTime			(const SCStudyInterfaceRef sc										)	{return sc.StartTime															;	}
inline int 			SessionEndTime				(const SCStudyInterfaceRef sc										)	{return sc.EndTime																;	}
inline int 			ChartTimeZone				(const SCStudyInterfaceRef sc										)	{return sc.TimeZone																;	}
inline int 			SessionDayOfWeek			(const SCStudyInterfaceRef sc										)	{return sc.StartDayOfWeek														;	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 Conversion de fuseaux horaires (chart <-> système)
inline SCDateTime 	ConvertFromChartTZ			(const SCStudyInterfaceRef sc, const SCDateTime& tChart				)	{SCDateTime out; sc.ConvertDateTimeFromChartTimeZone(tChart	, out); return out	;	}
inline SCDateTime 	ConvertToChartTZ			(const SCStudyInterfaceRef sc, const SCDateTime& tUTC				)	{SCDateTime out; sc.ConvertDateTimeToChartTimeZone	(tUTC	, out); return out	;	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 Prix & formats
inline double 		TickSize					(const SCStudyInterfaceRef sc										)	{return sc.TickSize																;	}
inline double 		RoundToTick					(const SCStudyInterfaceRef sc, double price							)	{return sc.RoundToTickSize(price, sc.TickSize)									;	}
inline double 		FromTicks					(const SCStudyInterfaceRef sc, int 	  ticks							)	{return ticks * sc.TickSize														;	}
inline int 			ToTicks						(const SCStudyInterfaceRef sc, double value							)	{return (int)sc.Round(value / sc.TickSize)										;	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 BaseData & Subgraphs
inline const		SCFloatArray& BaseOpen 		(const SCStudyInterfaceRef sc										)	{ return sc.Open																;	}
inline const		SCFloatArray& BaseHigh 		(const SCStudyInterfaceRef sc										)	{ return sc.High																;	}
inline const		SCFloatArray& BaseLow  		(const SCStudyInterfaceRef sc										)	{ return sc.Low																	;	}
inline const		SCFloatArray& BaseClose		(const SCStudyInterfaceRef sc										)	{ return sc.Close																;	}
inline const		SCFloatArray& BaseVolume	(const SCStudyInterfaceRef sc										)	{ return sc.Volume																;	}
inline const		SCFloatArray& BaseNumber	(const SCStudyInterfaceRef sc										)	{ return sc.NumberOfTrades														;	}
inline 				SCSubgraphRef SG			(SCSubgraphRef sg													)	{ return sg																		;	}	 // Accès direct à un Subgraph
inline void 		ClearSubgraph				(SCSubgraphRef sg													)												 											 // Helpers de configuration Subgraph
{
  sg				.Name 																							=	""																				;
  sg				.DrawStyle 																						=	DRAWSTYLE_IGNORE																;
  sg				.PrimaryColor 																					=	RGB(0, 0, 0)																	;
  sg				.LineWidth																						=	1																				;
  sg				.DrawZeros																						=	0																				;
  sg				.DisplayNameValueInWindowsFlags 																=	0																				;
}
inline void 		HideSubgraph				(SCSubgraphRef sg													)
{
  sg				.DrawStyle 																						=	DRAWSTYLE_IGNORE																;
  sg				.DrawZeros 																						=	0																				;
  sg				.DisplayNameValueInWindowsFlags																	=	0																				;
}
inline void 		SetSubgraphAsMarker			(SCSubgraphRef sg													)																							 // Marker standard (cercle transparent) pour signaux
{
  sg				.DrawStyle 																						=	DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE										;
  sg				.PrimaryColor 																					=	RGB(255, 0, 0)																	;
  sg				.SecondaryColorUsed 																			=	0																				;
  sg				.LineWidth 																						=	2																				;
  sg				.DrawZeros 																						=	0																				;
  sg				.DisplayNameValueInWindowsFlags																=	0																					;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 Inputs
inline 				SCInputRef IN				(SCInputRef in														)	{return in																		;	}
inline void 		InitYesNo					(SCInputRef in, const char* name, int   def = 0						)	{in.Name = name; in.SetYesNo		(def != 0)									;	}
inline void 		InitInt						(SCInputRef in, const char* name, int   def, int   minv, int maxv	)	{in.Name = name; in.SetInt			(def); in.SetIntLimits	(minv, maxv)		;	}
inline void 		InitFloat					(SCInputRef in, const char* name, float def, float minv, float maxv	)	{in.Name = name; in.SetFloat		(def); in.SetFloatLimits(minv, maxv)		;	}
inline void 		InitColor					(SCInputRef in, const char* name, COLORREF def						)	{in.Name = name; in.SetColor		(def)										;	}
inline void 		InitString					(SCInputRef in, const char* name, const char* def					)	{in.Name = name; in.SetString		(def)										;	}
inline void 		InitChart					(SCInputRef in, const char* name, int defChartNum = 1				)	{in.Name = name; in.SetChartNumber	(defChartNum)								;	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 SCFloatArray / séries
inline int 			SeriesSize					(const SCFloatArrayRef arr											)	{return arr.GetArraySize()														;	} 	 // Taille d'une série float (SCFloatArray)
inline int 			ArraySize					(const SCFloatArrayRef arr											)	{return SeriesSize(arr)															;	}	 // Alias rétro-compatible
inline void 		ClearArray					(SCFloatArrayRef arr												)																							 // Mise à zéro d'une série
{
  const int n = arr.GetArraySize();
  for (int i = 0; i < n; ++i)
    arr[i] = 0.0f;
}
inline void 		CopyArray					(SCFloatArrayRef dest, const SCFloatArrayRef src					)																							 // Copie d'une série dans une autre (resize automatique)
{
  const int n = src.GetArraySize();
  dest.ResizeIfNecessary(n);
  for (int i = 0; i < n; ++i)
    dest[i] = src[i];
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// MarketDepth générique
inline int 			GetBidDepthLevels			(const SCStudyInterfaceRef sc										)
{
  s_MarketDepthEntry e;
  int count = 0;
  while (sc.GetBidMarketDepthEntryAtLevel(e, count))
    ++count;
  return count;
}
inline int 			GetAskDepthLevels			(const SCStudyInterfaceRef sc										)
{
  s_MarketDepthEntry e;
  int count = 0;
  while (sc.GetAskMarketDepthEntryAtLevel(e, count))
    ++count;
  return count;
}
inline bool 		GetBidEntry					(const SCStudyInterfaceRef sc, int level, s_MarketDepthEntry& e		)	{return sc.GetBidMarketDepthEntryAtLevel(e, level) != 0							;	}
inline bool 		GetAskEntry					(const SCStudyInterfaceRef sc, int level, s_MarketDepthEntry& e		)	{return sc.GetAskMarketDepthEntryAtLevel(e, level) != 0							;	}
inline double 		GetBidPriceAtLevel			(const SCStudyInterfaceRef sc, int level							)
{
  s_MarketDepthEntry e;
  if (!GetBidEntry(sc, level, e))
    return 0.0;
  return e.Price;
}
inline double 		GetAskPriceAtLevel			(const SCStudyInterfaceRef sc, int level							)
{
  s_MarketDepthEntry e;
  if (!GetAskEntry(sc, level, e))
    return 0.0;
  return e.Price;
}
inline int 			GetBidSizeAtLevel			(const SCStudyInterfaceRef sc, int level							)
{
  s_MarketDepthEntry e				;
  if (!GetBidEntry	(sc, level, e)	)
		return 0					;
		return e.Quantity			;
}
inline int 			GetAskSizeAtLevel			(const SCStudyInterfaceRef sc, int level							)
{
  s_MarketDepthEntry e				;
  if (!GetAskEntry	(sc, level, e)	)
		return 0					;
		return e.Quantity			;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Time & Sales (accès minimal)
inline bool 		HasTimeAndSales				(const SCStudyInterfaceRef sc										)	{return sc.MaintainTimeAndSalesData != 0										;	}
inline int 			GetTimeAndSales				(const SCStudyInterfaceRef sc, s_TimeAndSalesArray& out				)	{sc.GetTimeAndSales(out);return out.Size()										;	}	 // Retourne le nombre d'entrées dans out
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Paramètres "headless" standard pour engines
inline void			ApplyHeadlessDefaults		(SCStudyInterfaceRef sc)
{
  sc.GraphRegion						= 0		;
  sc.ValueFormat						= 26	;     // format 26 explicite
  sc.AutoLoop							= 0		;
  sc.UpdateAlways						= 1		;
  sc.FreeDLL							= 0		;

  // Aucun SG visible / aucune valeur affichée par défaut
  for (int i = 0; i < SC_MAX_NUM_STUDY_SUBGRAPHS; ++i)
  {
    SCSubgraphRef sg					= sc.Subgraph[i]	;
    sg.DrawStyle						= DRAWSTYLE_IGNORE	;
    sg.DrawZeros						= 0					;
    sg.DisplayNameValueInWindowsFlags	= 0					;
  }

  sc.DisplayStudyName 					= 0		;
  sc.DisplayStudyInputValues 			= 0		;
  sc.DisplayStudyValues 				= 0		;
}
inline void			DisableStudyWindows			(SCStudyInterfaceRef sc)
{
  sc.DisplayStudyName 					= 0		;
  sc.DisplayStudyInputValues			= 0		;
  sc.DisplayStudyValues 				= 0		;
}
inline void			DisableSubgraphWindows		(SCStudyInterfaceRef sc)												{for (int i = 0; i < SC_MAX_NUM_STUDY_SUBGRAPHS; ++i)
  {
    SCSubgraphRef sg 					= sc.Subgraph[i]	;
    sg.DisplayNameValueInWindowsFlags 	= 0					;
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Alertes / logging
inline void			AlertMessage				(SCStudyInterfaceRef sc, const char* msg							)	{sc.AddAlert(msg)																;	}
inline void			LogMessage					(SCStudyInterfaceRef sc, const char* msg, int show = 0				)	{sc.AddMessageToLog(msg, show != 0)												;	}

}