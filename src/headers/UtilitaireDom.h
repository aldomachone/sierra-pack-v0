#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"

// === DOM helpers (v0) ===
// Lecture niveau, agrégations Near/Mid/Far, check "stale" basique.

namespace du {

  // readBidQtyAt : quantité bid au niveau (0 = L1)
  inline double readBidQtyAt(const SCStudyInterfaceRef& sc, int level)
  {
    s_MarketDepthEntry md{};
    if (sc.GetBidMarketDepthEntryAtLevel(md, level)) return md.Quantity;
    return 0.0;
  }

  // readAskQtyAt
  inline double readAskQtyAt(const SCStudyInterfaceRef& sc, int level)
  {
    s_MarketDepthEntry md{};
    if (sc.GetAskMarketDepthEntryAtLevel(md, level)) return md.Quantity;
    return 0.0;
  }

  struct DomBands
  {
    double nearBid{0}, nearAsk{0};
    double midBid{0},  midAsk{0};
    double farBid{0},  farAsk{0};
  };

  // sumBands : somme Bid/Ask sur 3 bandes
  inline DomBands sumBands(const SCStudyInterfaceRef& sc, int firstLevel, int totalLevels, int nearN, int midN)
  {
    DomBands b{};
    s_MarketDepthEntry md{};
    for (int i = 0; i < totalLevels; ++i)
    {
      const int li = firstLevel + i - 1; // firstLevel = 1 → li=0
      if (i < nearN)
      {
        if (sc.GetBidMarketDepthEntryAtLevel(md, li)) b.nearBid += md.Quantity;
        if (sc.GetAskMarketDepthEntryAtLevel(md, li)) b.nearAsk += md.Quantity;
      }
      else if (i < nearN + midN)
      {
        if (sc.GetBidMarketDepthEntryAtLevel(md, li)) b.midBid += md.Quantity;
        if (sc.GetAskMarketDepthEntryAtLevel(md, li)) b.midAsk += md.Quantity;
      }
      else
      {
        if (sc.GetBidMarketDepthEntryAtLevel(md, li)) b.farBid += md.Quantity;
        if (sc.GetAskMarketDepthEntryAtLevel(md, li)) b.farAsk += md.Quantity;
      }
    }
    return b;
  }

  // normalizePct : retourne near/mid/far en %
  inline void normalizePct(double nearTot, double midTot, double farTot, float& outNear, float& outMid, float& outFar)
  {
    const double all = nearTot + midTot + farTot;
    if (all <= 0.0) { outNear = outMid = outFar = 0.0f; return; }
    outNear = (float)(100.0 * nearTot / all);
    outMid  = (float)(100.0 * midTot  / all);
    outFar  = (float)(100.0 * farTot  / all);
  }

  // staleCheck : vrai si aucune variation minDelta sur les N premiers niveaux depuis last snapshot
  // v0 : stockage local des dernières quantités L1
  inline bool staleCheckL1(const SCStudyInterfaceRef& sc, int minDeltaQty)
  {
    s_MarketDepthEntry md{};
    static double lastBid = -1.0, lastAsk = -1.0;
    double bid=0, ask=0;
    if (sc.GetBidMarketDepthEntryAtLevel(md, 0)) bid = md.Quantity;
    if (sc.GetAskMarketDepthEntryAtLevel(md, 0)) ask = md.Quantity;

    bool fresh = false;
    if (lastBid < 0 || lastAsk < 0) fresh = true;
    else if (std::fabs(bid - lastBid) >= minDeltaQty) fresh = true;
    else if (std::fabs(ask - lastAsk) >= minDeltaQty) fresh = true;

    lastBid = bid; lastAsk = ask;
    return !fresh; // true = stale
  }

} // namespace du
