#pragma once
#include <cmath>
#include <cfloat>
#include <climits>
#include <cstdint>
#include "sierrachart.h"

// ============================================================================
// UtilitaireACSILCore.h — Wrappers et helpers ACSIL (réorganisé, dédoublonné)
// - Inline, pas d’état global ni d’allocation dynamique.
// - Familles : Sanitize | Index/Temps | Prix/Formats | BaseData | Subgraphs
//              Inputs | Calculs array-based | MarketDepth | Time&Sales
//              Alerts/Logs | Chart helpers | FutureIntersection
//              TimeSession (fenêtres/TradingDay)
//              Optionnels: Graphics Tools | Trading | Spreadsheet
// - Les commentaires d’origine sont conservés.
// ============================================================================

// ---------------------------------------------------------------------------
// Macros utilitaires
// ---------------------------------------------------------------------------
#ifndef DU_ASSUME
  #define DU_ASSUME(x) do{ (void)sizeof(x); }while(0)
#endif
#ifndef DU_UNSUPPORTED
  #define DU_UNSUPPORTED(name) /* TODO: wrapper for: name */
#endif

namespace du {

// ============================================================================
// 1) Sanitize / math util
// ============================================================================
constexpr double kEps = 1e-12;
inline double safe(double x){ return std::isfinite(x) ? x : 0.0; }
inline double clamp(double x,double lo,double hi){ return x<lo?lo:(x>hi?hi:x); }
inline double ema_pct(double prev,double x,double alphaPct){
  const double a = clamp(alphaPct,0.0,100.0)/100.0; return (1.0-a)*prev + a*x;
} // cf. EmaPct équivalente conservée via cette version compacte. :contentReference[oaicite:1]{index=1}

// ============================================================================
// 2) Index / état des barres / temps de graphique
// ============================================================================
inline int  BarIndex(const SCStudyInterfaceRef sc){ return sc.Index; }
inline int  BarsCount(const SCStudyInterfaceRef sc){ return sc.ArraySize; }
inline bool IsFirstIndex(const SCStudyInterfaceRef sc){ return sc.Index<=0; }
inline bool IsLastIndex (const SCStudyInterfaceRef sc){ return sc.Index==sc.ArraySize-1; }
inline bool BarHasClosed (const SCStudyInterfaceRef sc){
  return sc.GetBarHasClosedStatus(sc.Index) != BHCS_BAR_HAS_NOT_CLOSED;
}

inline SCDateTime   Now (const SCStudyInterfaceRef sc){ return sc.CurrentSystemDateTime; }
inline SCDateTimeMS NowMs(const SCStudyInterfaceRef sc){ return sc.CurrentSystemDateTimeMS; }

inline const SCDateTimeArray& DateTimeArr(const SCStudyInterfaceRef sc){ return sc.BaseDateTimeIn; }
inline int DayOfWeek(const SCStudyInterfaceRef sc){ return sc.BaseDateTimeIn[sc.Index].GetDayOfWeek(); }

inline int IndexForTime(const SCStudyInterfaceRef sc, SCDateTime t) {
  int idx = sc.GetContainingIndexForSCDateTime(t);
  if (idx < 0) idx = 0;
  if (idx >= sc.ArraySize) idx = sc.ArraySize - 1;
  return idx;
} // wrapper dédoublonné. :contentReference[oaicite:2]{index=2}

inline SCDateTime ConvertFromChartTZ(const SCStudyInterfaceRef sc, const SCDateTime& tChart){
  SCDateTime out; sc.ConvertDateTimeFromChartTimeZone(tChart,out); return out;
}
inline SCDateTime ConvertToChartTZ(const SCStudyInterfaceRef sc, const SCDateTime& tUTC){
  SCDateTime out; sc.ConvertDateTimeToChartTimeZone(tUTC,out); return out;
}

// ============================================================================
// 3) Prix & formats
// ============================================================================
inline double TickSize(const SCStudyInterfaceRef sc)         { return sc.TickSize; }
inline double RoundToTick(const SCStudyInterfaceRef sc, double p) { return sc.RoundToTickSize(p, sc.TickSize); }
inline double FromTicks(const SCStudyInterfaceRef sc, int ticks)  { return ticks * sc.TickSize; }
inline int    ToTicks  (const SCStudyInterfaceRef sc, double d)   { return (int)sc.Round(d / sc.TickSize); } // :contentReference[oaicite:3]{index=3}

// ============================================================================
// 4) Accès BaseData (séries OHLCV & DateTime)
// ============================================================================
inline const SCFloatArray& Close (const SCStudyInterfaceRef sc){ return sc.Close; }
inline const SCFloatArray& Open  (const SCStudyInterfaceRef sc){ return sc.Open;  }
inline const SCFloatArray& High  (const SCStudyInterfaceRef sc){ return sc.High;  }
inline const SCFloatArray& Low   (const SCStudyInterfaceRef sc){ return sc.Low;   }
inline const SCFloatArray& Volume(const SCStudyInterfaceRef sc){ return sc.Volume; }
inline const SCDateTimeArray& DateTime(const SCStudyInterfaceRef sc){ return sc.BaseDateTimeIn; } // :contentReference[oaicite:4]{index=4}

// ============================================================================
// 5) Helpers Subgraph
// ============================================================================
inline void SgHideAll(SCSubgraphRef sg, bool drawZeros = false) {
  sg.DrawStyle = DRAWSTYLE_IGNORE;
  sg.DrawZeros = drawZeros ? 1 : 0;
  sg.DisplayNameValueInWindowsFlags = 0;
}
inline void SgMarkerTransparent(SCSubgraphRef sg) {
  sg.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE;
  sg.DrawZeros = 0;
  sg.PrimaryColor = RGB(255,255,255);
  sg.LineWidth = 1;
  sg.DisplayNameValueInWindowsFlags = 0;
}
inline void SgLastBarOnly(SCSubgraphRef sg) {
  sg.DrawStyleEx = DRAWSTYLE_EX_ONLY_AT_LAST_BAR;
} // :contentReference[oaicite:5]{index=5}

// Batch utilitaires (rendus implicites par usages existants)
inline void SG_BatchHide(SCStudyInterfaceRef sc, int sgFrom=1, int sgTo=24){
  for(int i=sgFrom;i<=sgTo;++i) SgHideAll(sc.Subgraph[i]);
}
inline void SG_BatchLastBarOnly(SCStudyInterfaceRef sc, int sgFrom=1, int sgTo=24){
  for(int i=sgFrom;i<=sgTo;++i) SgLastBarOnly(sc.Subgraph[i]);
}
inline void SG_BatchSetWidth(SCStudyInterfaceRef sc, int w, int sgFrom=1, int sgTo=24){
  for(int i=sgFrom;i<=sgTo;++i) sc.Subgraph[i].LineWidth = w;
}
inline void SG_ZeroAll(SCStudyInterfaceRef sc, int sgFrom=1, int sgTo=24){
  for(int i=sgFrom;i<=sgTo;++i) sc.Subgraph[i].Arrays[0][sc.Index] = 0.0;
} // batch helpers référencés par la config rapide. :contentReference[oaicite:6]{index=6}

// ============================================================================
// 6) Inputs (raccourcis Set courants)
// ============================================================================
inline void InYesNo (SCInputRef in, const char* nom, bool def) { in.Name = nom; in.SetYesNo(def ? 1 : 0); }
inline void InInt   (SCInputRef in, const char* nom, int def, int minV=INT_MIN, int maxV=INT_MAX) { in.Name = nom; in.SetInt(def); in.SetIntLimits(minV, maxV); }
inline void InFloat (SCInputRef in, const char* nom, double def) { in.Name = nom; in.SetFloat(def); }
inline void InMovAvg(SCInputRef in, const char* nom, unsigned defType) { in.Name = nom; in.SetMovAvgType(defType); } // :contentReference[oaicite:7]{index=7}

// ============================================================================
// 7) Calculs "array-based" (wrappers sûrs sur SCFloatArray)
// ============================================================================
inline void SMA (const SCStudyInterfaceRef sc, const SCFloatArrayRef in, SCFloatArrayRef out, int len){ sc.SimpleMovAvg(in, out, len); }
inline void EMA (const SCStudyInterfaceRef sc, const SCFloatArrayRef in, SCFloatArrayRef out, int len){ sc.ExponentialMovAvg(in, out, len); }
inline void RSI (const SCStudyInterfaceRef sc, const SCFloatArrayRef in, SCFloatArrayRef out, int len, unsigned maType){ sc.RSI(in, out, maType, len); }
inline void MACD(const SCStudyInterfaceRef sc, const SCFloatArrayRef in, SCFloatArrayRef macd, SCFloatArrayRef signal, SCFloatArrayRef histo,
                 int fastLen, int slowLen, int sigLen, unsigned maType){
  sc.MACD(in, macd, signal, histo, fastLen, slowLen, sigLen, maType);
}
inline void ZigZag(const SCStudyInterfaceRef sc, const SCFloatArrayRef in, SCFloatArrayRef out,double reversalInTicks){
  sc.ZigZag(in,out,reversalInTicks);
} // 

// ============================================================================
// 8) Market Depth (Bid/Ask)
// ============================================================================
struct DepthAt { double Price=0.0; float Qty=0.0f; int NumOrders=0; };

inline bool ReadBidAtLevel(const SCStudyInterfaceRef sc, int level, DepthAt& out) {
  s_MarketDepthEntry md{};
  const bool ok = sc.GetBidMarketDepthEntryAtLevel(md, level) != 0;
  if (!ok) return false;
  out.Price = md.Price; out.Qty = md.Quantity; out.NumOrders = md.NumOrders;
  return true;
}
inline bool ReadAskAtLevel(const SCStudyInterfaceRef sc, int level, DepthAt& out) {
  s_MarketDepthEntry md{};
  const bool ok = sc.GetAskMarketDepthEntryAtLevel(md, level) != 0;
  if (!ok) return false;
  out.Price = md.Price; out.Qty = md.Quantity; out.NumOrders = md.NumOrders;
  return true;
}
inline int DepthLevelsAvailableBid(const SCStudyInterfaceRef sc) { return sc.NumberOfBidLevels; }
inline int DepthLevelsAvailableAsk(const SCStudyInterfaceRef sc) { return sc.NumberOfAskLevels; } // :contentReference[oaicite:9]{index=9}

// ============================================================================
// 9) Time & Sales
// ============================================================================
inline void EnsureTimeAndSales(const SCStudyInterfaceRef sc, bool enable=true) {
  if(enable) sc.MaintainTimeAndSalesData = 1;
}
inline bool GetTimeAndSales(const SCStudyInterfaceRef sc, c_SCTimeAndSalesArray& out) {
  EnsureTimeAndSales(sc);
  sc.GetTimeAndSales(out);
  return out.Size() > 0;
} // :contentReference[oaicite:10]{index=10}

// ============================================================================
// 10) Alertes / Logs simples
// ============================================================================
inline void AlertText(const SCStudyInterfaceRef sc, int alertNum, const char* msg) {
  sc.SetAlert(alertNum, msg);
} // :contentReference[oaicite:11]{index=11}

inline void UI_DisableAll(SCStudyInterfaceRef sc, int sgFrom=1, int sgTo=24){
  // Masque valeurs/labels des Subgraphs utiles pour headless
  for(int i=sgFrom;i<=sgTo;++i){
    sc.Subgraph[i].DisplayNameValueInWindowsFlags = 0;
  }
}

// ============================================================================
// 11) Chart / Region helpers (non graphiques)
// ============================================================================
inline void ApplyHeadlessDefaults(SCStudyInterfaceRef sc, int sgFrom=1, int sgTo=24) {
  sc.AutoLoop = 0;
  sc.UpdateAlways = 1;
  sc.GraphRegion = 0;
  sc.ValueFormat = 26;
  sc.FreeDLL = 0;
  for (int i = sgFrom; i <= sgTo; ++i) SgHideAll(sc.Subgraph[i]);
  sc.DrawZeros = 0;
  sc.DisplayStudyName = 0;
} // version unique gardée. :contentReference[oaicite:12]{index=12}

inline void SetRegion     (SCStudyInterfaceRef sc,int region){ sc.GraphRegion = region; }
inline void SetValueFormat(SCStudyInterfaceRef sc,int vf){ sc.ValueFormat = vf; }

// ============================================================================
// 12) Ligne jusqu’à l’intersection future (outil dédié SC, sans état)
// ============================================================================
inline int AddLineUntilFutureIntersection(const SCStudyInterfaceRef sc, float price, COLORREF color, int width, unsigned style){
  s_UseTool t{}; t.Clear(); t.ChartNumber = sc.ChartNumber; t.DrawingType = DRAWING_HORIZONTALLINE;
  t.BeginValue = price; t.Color = color; t.LineWidth = width; t.LineStyle = (LineStyles)style;
  t.AddMethod = UTAM_ADD_OR_ADJUST; t.AddAsUserDrawnDrawing = 0; t.BeginDateTime = sc.BaseDateTimeIn[sc.Index];
  return sc.AddLineUntilFutureIntersection(t);
} // :contentReference[oaicite:13]{index=13}

// ============================================================================
// 13) Gabarits LastCall — purge état visible
// ============================================================================
inline void LastCall_ResetCore(SCStudyInterfaceRef /*sc*/){ /* points d’ancrage pour reset custom */ }
inline void LastCall_ZeroVisibleSG(SCStudyInterfaceRef sc, int sgFrom=1, int sgTo=24){ SG_ZeroAll(sc, sgFrom, sgTo); } // :contentReference[oaicite:14]{index=14}

// ============================================================================
// 14) TimeSession — découpes par “Trading Day”, fenêtres horaires, IB, etc.
// ============================================================================
inline int HMS_to_sec(int h,int m,int s){ return h*3600 + m*60 + s; }
inline void sec_to_HMS(int sec,int& h,int& m,int& s){ if(sec<0) sec=0; h=sec/3600; sec%=3600; m=sec/60; s=sec%60; }

inline SCDateTime   NowChart   (const SCStudyInterfaceRef sc){ return sc.CurrentSystemDateTime; }
inline SCDateTimeMS NowChartMS (const SCStudyInterfaceRef sc){ return sc.CurrentSystemDateTimeMS; }
inline SCDateTime   ToChartTZ  (const SCStudyInterfaceRef sc, const SCDateTime& utc){ SCDateTime out; sc.ConvertDateTimeToChartTimeZone(utc,out); return out; }
inline SCDateTime   FromChartTZ(const SCStudyInterfaceRef sc, const SCDateTime& t){   SCDateTime out; sc.ConvertDateTimeFromChartTimeZone(t,out); return out; }

inline int TradingDayDateAt(const SCStudyInterfaceRef sc, int index){
  return sc.GetTradingDayDate(sc.BaseDateTimeIn[index]);
}
inline int FirstIndexOfTradingDay(const SCStudyInterfaceRef sc, int tradingDayDate){
  const int n = sc.ArraySize; if(n<=0) return -1;
  int lo=0, hi=n-1, ans=-1;
  while(lo<=hi){
    int mid=(lo+hi)>>1;
    int d=sc.GetTradingDayDate(sc.BaseDateTimeIn[mid]);
    if(d<tradingDayDate) lo=mid+1; else { ans=mid; hi=mid-1; }
  }
  return ans;
} // extrait groupé TimeSession. :contentReference[oaicite:15]{index=15}

// ============================================================================
// 15) Optionnel — Graphics Tools (s_UseTool)
//    Activer via: #define DU_ENABLE_GRAPHICS_TOOLS avant l’include
// ============================================================================
#ifdef DU_ENABLE_GRAPHICS_TOOLS

inline COLORREF duRGB(unsigned r,unsigned g,unsigned b){ return RGB((BYTE)r,(BYTE)g,(BYTE)b); }
struct Pen { COLORREF color=RGB(255,255,255); int width=1; LineStyles style=LINESTYLE_SOLID; };
struct Fill{ COLORREF color=RGB(0,0,0);       int alpha=80; }; // 0..100

// Base
inline void _BaseTool(s_UseTool& t, const SCStudyInterfaceRef sc, int drawingType){
  t.Clear(); t.ChartNumber=sc.ChartNumber; t.DrawingType=drawingType;
  t.AddMethod=UTAM_ADD_OR_ADJUST; t.AddAsUserDrawnDrawing=0;
}

// Lignes
inline int Tool_HLine(SCStudyInterfaceRef sc, double price, Pen pen = {}){
  s_UseTool t; _BaseTool(t, sc, DRAWING_HORIZONTALLINE);
  t.BeginValue=(float)price; t.Color=pen.color; t.LineWidth=pen.width; t.LineStyle=pen.style;
  return sc.UseTool(t);
}
inline int Tool_Line(SCStudyInterfaceRef sc, int i1,double p1,int i2,double p2, Pen pen = {}){
  s_UseTool t; _BaseTool(t, sc, DRAWING_LINE);
  t.BeginIndex=i1; t.BeginValue=(float)p1; t.EndIndex=i2; t.EndValue=(float)p2;
  t.Color=pen.color; t.LineWidth=pen.width; t.LineStyle=pen.style; return sc.UseTool(t);
}
inline int Tool_Ray(SCStudyInterfaceRef sc, int i1,double p1,int i2,double p2, Pen pen = {}){
  s_UseTool t; _BaseTool(t, sc, DRAWING_RAY);
  t.BeginIndex=i1; t.BeginValue=(float)p1; t.EndIndex=i2; t.EndValue=(float)p2;
  t.Color=pen.color; t.LineWidth=pen.width; t.LineStyle=pen.style; return sc.UseTool(t);
}
// Texte stationnaire HUD (X/Y en % écran)
inline int Tool_StationaryText(SCStudyInterfaceRef sc, double x_percent, double y_percent,
                               const SCString& text, COLORREF color = RGB(255,255,255), int fontSize = 8){
  s_UseTool t; _BaseTool(t, sc, DRAWING_STATIONARY_TEXT);
  t.BeginValue=(float)x_percent; t.EndValue=(float)y_percent; t.Color=color; t.FontSize=fontSize; t.Text=text;
  return sc.UseTool(t);
}

// MAJ / recherche / suppression / z-order
inline bool Tool_GetByLineNumber   (SCStudyInterfaceRef sc, int lineNumber, s_UseTool& out){ out.Clear(); out.ChartNumber=sc.ChartNumber; out.LineNumber=lineNumber; return sc.GetUserDrawnDrawingByLineNumber(out)!=0; }
inline bool Tool_UpdateByLineNumber(SCStudyInterfaceRef sc, const s_UseTool& in){ s_UseTool t=in; t.AddMethod=UTAM_ADD_OR_ADJUST; return sc.UseTool(t)!=0; }
inline bool Tool_DeleteByLineNumber(SCStudyInterfaceRef sc, int lineNumber){ return sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, lineNumber)!=0; }
inline void Tool_DeleteAllFromStudy(SCStudyInterfaceRef sc){ sc.DeleteACSRelatedDrawings(sc.ChartNumber, sc.StudyGraphInstanceID); }
inline void Tool_BringToFront(SCStudyInterfaceRef sc, int lineNumber){ sc.BringToFront(sc.ChartNumber, lineNumber); }
inline void Tool_SendToBack  (SCStudyInterfaceRef sc, int lineNumber){ sc.SendToBack(sc.ChartNumber, lineNumber); }

// Couleurs prêtes à l’emploi
inline Pen  PenRed  (int w=1, LineStyles s=LINESTYLE_SOLID){ return Pen{RGB(255,0,0), w, s}; }
inline Pen  PenGreen(int w=1, LineStyles s=LINESTYLE_SOLID){ return Pen{RGB(0,179,0), w, s}; }
inline Pen  PenBlue (int w=1, LineStyles s=LINESTYLE_SOLID){ return Pen{RGB(64,160,255), w, s}; }
inline Fill FillGray(int a=80){ return Fill{RGB(64,64,64), a}; } // 

#endif // DU_ENABLE_GRAPHICS_TOOLS

// ============================================================================
// 16) Optionnel — Trading (wrappers simples)
//    Activer via: #define DU_ENABLE_TRADING avant l’include
// ============================================================================
#ifdef DU_ENABLE_TRADING
enum class Side { Buy, Sell };

inline bool Order_Cancel(SCStudyInterfaceRef sc, unsigned int internalID){
  s_SCTradeOrder ord{}; if(!sc.GetOrderByOrderID(internalID, ord)) return false;
  return sc.CancelOrder(ord) != 0;
}
inline bool Position_Get(SCStudyInterfaceRef sc, s_SCPositionData& pos){
  return sc.GetTradePosition(pos) != 0;
}
inline void Trading_FlattenAndCancel_All(SCStudyInterfaceRef sc){
  sc.FlattenAndCancelAllOrders();
} // :contentReference[oaicite:17]{index=17}
#endif // DU_ENABLE_TRADING

// ============================================================================
// 17) Optionnel — Spreadsheet (wrappers minimaux)
//    Activer via: #define DU_ENABLE_SPREADSHEET avant l’include
// ============================================================================
#ifdef DU_ENABLE_SPREADSHEET
inline bool Sheet_GetHandle(SCStudyInterfaceRef sc, int sheetNum, int& outHandle){
  outHandle = sc.GetSpreadsheetSheetHandle(sheetNum);
  return outHandle != 0;
}
inline bool Sheet_SetCell(SCStudyInterfaceRef sc, int sheetHandle, int row, int col, double v){
  return sc.SetSpreadsheetCell(sheetHandle, row, col, v) != 0;
}
inline bool Sheet_GetCell(SCStudyInterfaceRef sc, int sheetHandle, int row, int col, double& v){
  return sc.GetSpreadsheetCell(sheetHandle, row, col, v) != 0;
} // :contentReference[oaicite:18]{index=18}
#endif // DU_ENABLE_SPREADSHEET

} // namespace du
