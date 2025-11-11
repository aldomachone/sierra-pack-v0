#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// ============================================================================
// UtilitaireTapeBVC.h — Bulk Volume Classification & CumDelta (v1 enrichi)
// ----------------------------------------------------------------------------
// Objectif : fournir un BVC robuste pour Sierra Chart, utilisable au niveau
//  "barres" (Up/DownVolume) et "tape" (Time&Sales) avec états cumulés.
// - Tout inline, aucune allocation, aucun état global.
// - Zéro dépendance hors sierrachart.h + UtilitaireSanitize.h.
// - Conserve l'API v0 (bvcBar, bvcDelta, tbAgg/tbClassifyAgg, tbBulkVolume,
//   tbCumDeltaStep) et ajoute un noyau complet Time&Sales.
// ----------------------------------------------------------------------------
// Principes :
//  1) Quote-Test prioritaire si Bid/Ask fiables :
//     price >= ask  → BUY ; price <= bid → SELL ; sinon indéterminé.
//  2) Lee–Ready (tick rule) en repli : si price > lastPrice → BUY ;
//     si price < lastPrice → SELL ; sinon indéterminé.
//  3) Heuristique mid : si |price - mid| > 0 → signe par rapport au mid.
//  4) On ne re-classe jamais les non-trades (SC_TS_BIDASKVALUES).
//  5) Cumul Delta = Σ(+vol for BUY) − Σ(+vol for SELL).
// ----------------------------------------------------------------------------
// Également fourni :
//  - État cumulatif (BvcState) : cumDelta, cumBuy, cumSell, runs, last tick.
//  - Itération Time&Sales avec séquence locale pour traiter seulement le neuf.
//  - Fonctions features MLP : 10 features standardisées.
//  - Reset doux/dur et détection nouvelle journée via SCDateTime.
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Types de base et compat v0
// ---------------------------------------------------------------------------
enum tbAgg { TB_BUY = 1, TB_SELL = -1, TB_UNK = 0 };

inline void bvcBar(const SCStudyInterfaceRef& sc, int idx, float& buyVol, float& sellVol)
{
  buyVol  = (float)sc.UpVolume[idx];
  sellVol = (float)sc.DownVolume[idx];
}

inline float bvcDelta(const SCStudyInterfaceRef& sc, int idx)
{
  return (float)sc.UpVolume[idx] - (float)sc.DownVolume[idx];
}

inline tbAgg tbClassifyAgg(int /*tsEntry*/) { return TB_UNK; }
inline int   tbBulkVolume(tbAgg /*side*/, int sz) { return sz; }
inline int   tbCumDeltaStep(tbAgg side, int sz) { return side==TB_BUY? +sz : side==TB_SELL? -sz : 0; }

// ---------------------------------------------------------------------------
// Constantes locales
// ---------------------------------------------------------------------------
constexpr double kDayToMs = 86400000.0;
constexpr double kTiny    = 1e-12;

// ---------------------------------------------------------------------------
// Conversions SCDateTime ↔ ms
// ---------------------------------------------------------------------------
inline long long scdt_to_ms(SCDateTime t)
{
  const double d = (double)t; // Sierra: jours julien fractionnaire
  return (long long)(d * kDayToMs + 0.5);
}

// ---------------------------------------------------------------------------
// Quote test et tick rule
// ---------------------------------------------------------------------------
inline tbAgg bvc_quote_test(double price, double bid, double ask)
{
  if (bid > 0.0 && ask > 0.0 && ask >= bid) {
    if (price >= ask) return TB_BUY;
    if (price <= bid) return TB_SELL;
  }
  return TB_UNK;
}

inline tbAgg bvc_tick_rule(double price, double lastPrice)
{
  if (lastPrice <= 0.0) return TB_UNK;
  if (price > lastPrice)  return TB_BUY;
  if (price < lastPrice)  return TB_SELL;
  return TB_UNK;
}

inline tbAgg bvc_mid_heuristic(double price, double bid, double ask)
{
  if (bid > 0.0 && ask > 0.0) {
    const double mid = 0.5 * (bid + ask);
    if (price > mid) return TB_BUY;
    if (price < mid) return TB_SELL;
  }
  return TB_UNK;
}

// ---------------------------------------------------------------------------
// Paramètres et état cumulatif
// ---------------------------------------------------------------------------
struct BvcParams
{
  // Priorité des tests
  bool   preferQuoteTest   = true;   // true: Quote-Test d'abord
  bool   useMidHeuristic   = true;   // exploiter la position vs mid si indéterminé

  // Réglages séquence T&S
  int    maxBatch          = 4096;   // traitement max par appel (sécurité)

  // Resets
  bool   resetOnNewDay     = true;   // reset dur à l'ouverture d'un nouveau jour
};

struct BvcState
{
  // Cumulatifs
  double cumDelta  = 0.0;
  double cumBuy    = 0.0;
  double cumSell   = 0.0;

  // Dernière observation
  double lastPrice = 0.0;
  double lastBid   = 0.0;
  double lastAsk   = 0.0;

  // Runs consécutifs
  int runBuy = 0;
  int runSell = 0;

  // Pour l'itération T&S
  int lastSeq = 0;                // index local de lecture
  SCDateTime lastTs;              // time du dernier évènement traité

  // Telemetry
  int  nTrades = 0;
  int  nBuys   = 0;
  int  nSells  = 0;

  // Reset complet
  void reset_hard()
  {
    cumDelta = cumBuy = cumSell = 0.0;
    lastPrice = lastBid = lastAsk = 0.0;
    runBuy = runSell = 0;
    lastSeq = 0; lastTs = 0.0;
    nTrades = nBuys = nSells = 0;
  }
};

// ---------------------------------------------------------------------------
// Classification d'un trade isolé à partir de price/size/bid/ask/contexte
// ---------------------------------------------------------------------------
inline tbAgg bvc_classify_trade(double price, int size, double bid, double ask, double lastPrice, bool preferQuoteTest)
{
  if (size <= 0 || price <= 0.0) return TB_UNK;

  tbAgg s = TB_UNK;
  if (preferQuoteTest) {
    s = bvc_quote_test(price, bid, ask);
    if (s == TB_UNK)
      s = bvc_tick_rule(price, lastPrice);
  } else {
    s = bvc_tick_rule(price, lastPrice);
    if (s == TB_UNK)
      s = bvc_quote_test(price, bid, ask);
  }
  if (s == TB_UNK)
    s = bvc_mid_heuristic(price, bid, ask);
  return s;
}

// Applique le trade au state
inline void bvc_apply_trade(BvcState& st, double price, int size, double bid, double ask, tbAgg side)
{
  if (size <= 0) return;
  if (side == TB_BUY) {
    st.cumBuy   += size;
    st.cumDelta += size;
    st.runBuy   += 1;
    st.runSell   = 0;
  } else if (side == TB_SELL) {
    st.cumSell  += size;
    st.cumDelta -= size;
    st.runSell  += 1;
    st.runBuy    = 0;
  } else {
    // Indéterminé: pas d'effet sur cumDelta ni runs
  }
  st.lastPrice = price > 0.0 ? price : st.lastPrice;
  st.lastBid   = bid   > 0.0 ? bid   : st.lastBid;
  st.lastAsk   = ask   > 0.0 ? ask   : st.lastAsk;
  st.nTrades  += 1;
  if (side == TB_BUY)  st.nBuys  += 1;
  if (side == TB_SELL) st.nSells += 1;
}

// ---------------------------------------------------------------------------
// Lecture Time&Sales depuis Sierra Chart et mise à jour de l'état
// ---------------------------------------------------------------------------
inline int bvc_process_time_and_sales(SCStudyInterfaceRef sc, const BvcParams& p, BvcState& st)
{
  c_SCTimeAndSalesArray tsArray;
  sc.GetTimeAndSales(tsArray);
  const int total = tsArray.Size();
  if (total <= 0) return 0;

  // Reset à la nouvelle journée si demandé
  if (p.resetOnNewDay && st.lastTs != 0.0) {
    if ((int)st.lastTs.GetDate() != (int)tsArray[total-1].DateTime.GetDate()) {
      st.reset_hard();
    }
  }

  int processed = 0;
  int i = st.lastSeq;
  if (i < 0) i = 0;
  if (i > total) i = total; // cas pathologique: shrink

  for (; i < total && processed < p.maxBatch; ++i) {
    const s_TimeAndSales& e = tsArray[i];

    if (e.Type == SC_TS_BIDASKVALUES) {
      // Mettre à jour les quotes connues
      if (e.Bid > 0.0) st.lastBid = e.Bid;
      if (e.Ask > 0.0) st.lastAsk = e.Ask;
      st.lastTs = e.DateTime;
      continue;
    }

    if (e.Type == SC_TS_TRADES || e.Type == SC_TS_BIDTRADE || e.Type == SC_TS_ASKTRADE) {
      const double price = e.Price;
      const int    size  = (int)e.Volume;
      if (size <= 0 || price <= 0.0) { st.lastTs = e.DateTime; continue; }

      // Quote context à l'instant du trade si disponible
      double bid = (e.Bid > 0.0 ? e.Bid : st.lastBid);
      double ask = (e.Ask > 0.0 ? e.Ask : st.lastAsk);

      tbAgg side = TB_UNK;
      // Exploiter les marquages Sierra si présents
      if (e.Type == SC_TS_BIDTRADE) side = TB_SELL;
      else if (e.Type == SC_TS_ASKTRADE) side = TB_BUY;

      if (side == TB_UNK) {
        side = bvc_classify_trade(price, size, bid, ask, st.lastPrice, p.preferQuoteTest);
      }

      bvc_apply_trade(st, price, size, bid, ask, side);
      st.lastTs = e.DateTime;
      processed++;
    }
  }

  st.lastSeq = i; // point de reprise
  return processed;
}

// ---------------------------------------------------------------------------
// Features MLP (v1) — écrit 10 features dans out[]
//  0: cum_delta
//  1: cum_buy
//  2: cum_sell
//  3: run_buy
//  4: run_sell
//  5: buy_ratio   = cum_buy / (cum_buy + cum_sell)
//  6: sell_ratio  = cum_sell / (cum_buy + cum_sell)
//  7: last_price
//  8: last_bid
//  9: last_ask
// Retourne le nombre de features écrites.
// ---------------------------------------------------------------------------
inline int bvc_features_v1(const BvcState& st, double* out)
{
  if (!out) return 0;
  const double tot = st.cumBuy + st.cumSell;
  const double buyR = (tot > kTiny) ? (st.cumBuy / tot) : 0.0;
  const double selR = (tot > kTiny) ? (st.cumSell / tot) : 0.0;

  out[0] = du::sanitize(st.cumDelta);
  out[1] = du::sanitize(st.cumBuy);
  out[2] = du::sanitize(st.cumSell);
  out[3] = (double)st.runBuy;
  out[4] = (double)st.runSell;
  out[5] = du::sanitize(buyR);
  out[6] = du::sanitize(selR);
  out[7] = du::sanitize(st.lastPrice);
  out[8] = du::sanitize(st.lastBid);
  out[9] = du::sanitize(st.lastAsk);
  return 10;
}

// ---------------------------------------------------------------------------
// Utilitaires divers
// ---------------------------------------------------------------------------
inline void bvc_soft_reset(BvcState& st)
{
  st.cumDelta = st.cumBuy = st.cumSell = 0.0;
  st.runBuy = st.runSell = 0;
  st.nTrades = st.nBuys = st.nSells = 0;
}

inline void bvc_reset_if_full_recalc(SCStudyInterfaceRef sc, BvcState& st)
{
  if (sc.IsFullRecalculation)
    st.reset_hard();
}

} // namespace du
