#include "sierrachart.h"

SCDLLName("DOM_ENGINE_v0")

#include "PackUtilitaires_v0.h"

// === Toggles Pack_v0 pour CETTE DLL ===
// On veut: CORE + DOM + MLP, sans TAPE / CONTEXTE / ZONES / MTF / MEMORY_RISK
#define PACK_V0_ENABLE_CORE         1
#define PACK_V0_ENABLE_MLP          1
#define PACK_V0_ENABLE_DOM          1
#define PACK_V0_ENABLE_TAPE         0
#define PACK_V0_ENABLE_CONTEXTE     0
#define PACK_V0_ENABLE_ZONES        0
#define PACK_V0_ENABLE_MTF          0
#define PACK_V0_ENABLE_MEMORY_RISK  0

// (optionnel mais propre pour l’instant : on ignore les agrégateurs globaux)
#define PACK_V0_USE_AGGREGATORS     0

namespace {

constexpr double 	DU_EPS = 1e-9;

// Ratio imbalance (Bid/Ask) sécurisé
inline double 		DomImbalance		(double bid, double ask)
{
  const double denom = bid + ask;
  if (denom <= DU_EPS)
    return 0.0;
  double v = (bid - ask) / denom;
  if (v >  1.0) v =  1.0;
  if (v < -1.0) v = -1.0;
  return v;
}

// Share d’une bande vs total
inline double 		DepthShare			(double bandDepth, double totalDepth)
{
  if (totalDepth <= DU_EPS)
    return 0.0;
  double v = bandDepth / totalDepth;
  if (v < 0.0) v = 0.0;
  if (v > 1.0) v = 1.0;
  return v;
}

// Clamp simple entier
inline int 			ClampInt			(int v, int lo, int hi)
{
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

// Lecture volumes par bande (index de niveaux, côté Bid/Ask)
inline void 		ComputeBandVolumes	(
  SCStudyInterfaceRef sc,
  int nearCount,
  int midCount,
  int farCount,
  double& nearBid, double& midBid, double& farBid,
  double& nearAsk, double& midAsk, double& farAsk
)
{
  nearBid = midBid = farBid = 0.0;
  nearAsk = midAsk = farAsk = 0.0;

  const int totalBandLevels = nearCount + midCount + farCount;
  if (totalBandLevels <= 0)
    return;

  // BID
  const int nBid = sc.GetBidMarketDepthNumberOfLevels();
  const int maxBidLevel = nBid < totalBandLevels ? nBid : totalBandLevels;
  s_MarketDepthEntry md;

  for (int level = 0; level < maxBidLevel; ++level)
  {
    if (!sc.GetBidMarketDepthEntryAtLevel(md, level))
      break;

    const double q = (double)md.Quantity;
    if (level < nearCount)
      nearBid += q;
    else if (level < nearCount + midCount)
      midBid += q;
    else
      farBid += q;
  }

  // ASK
  const int nAsk = sc.GetAskMarketDepthNumberOfLevels();
  const int maxAskLevel = nAsk < totalBandLevels ? nAsk : totalBandLevels;

  for (int level = 0; level < maxAskLevel; ++level)
  {
    if (!sc.GetAskMarketDepthEntryAtLevel(md, level))
      break;

    const double q = (double)md.Quantity;
    if (level < nearCount)
      nearAsk += q;
    else if (level < nearCount + midCount)
      midAsk += q;
    else
      farAsk += q;
  }
}

// Lecture best bid/ask, spread en ticks et imbalance top of book
inline void 		ComputeTopOfBook	(
  SCStudyInterfaceRef sc,
  double& bestBidPrice,
  double& bestAskPrice,
  double& bestBidQty,
  double& bestAskQty,
  double& spreadTicks,
  double& tobImb
)
{
  bestBidPrice = bestAskPrice = 0.0;
  bestBidQty   = bestAskQty   = 0.0;
  spreadTicks  = 0.0;
  tobImb       = 0.0;

  s_MarketDepthEntry mdBid;
  s_MarketDepthEntry mdAsk;

  const bool haveBid = sc.GetBidMarketDepthEntryAtLevel(mdBid, 0) != 0;
  const bool haveAsk = sc.GetAskMarketDepthEntryAtLevel(mdAsk, 0) != 0;

  if (!haveBid || !haveAsk)
    return;

  bestBidPrice = mdBid.Price;
  bestAskPrice = mdAsk.Price;
  bestBidQty   = (double)mdBid.Quantity;
  bestAskQty   = (double)mdAsk.Quantity;

  if (sc.TickSize > 0.0 && bestAskPrice > bestBidPrice)
  {
    spreadTicks = (bestAskPrice - bestBidPrice) / sc.TickSize;
    if (spreadTicks < 0.0)
      spreadTicks = 0.0;
  }

  tobImb = DomImbalance(bestBidQty, bestAskQty);
}


inline double 		ClassifyDepthMode	(double nearDepth, double farDepth, double totalDepth)	// Classement “mode depth” simple : 0 = normal, 1 = Near dominant, 2 = Far dominant, 3 = depth quasi nul
{
  if (totalDepth <= DU_EPS)
    return 3.0; // vide / quasi nul

  const double nearShare = DepthShare(nearDepth, totalDepth);
  const double farShare  = DepthShare(farDepth,  totalDepth);

  if (nearShare >= 0.60)
    return 1.0; // front-loaded (Near dominant)
  if (farShare  >= 0.60)
    return 2.0; // far-loaded

  return 0.0; // “normal”
}

} // namespace
// ============================================================================
// DOM_ENGINE_V0 — Bus de features DOM (Near/Mid/Far + gates)
// - ENGINE headless : aucun Subgraph visible par défaut
// - Sorties SG1..SG24 = features DOM v0 (voir noms ci-dessous)
// - Lecture DOM niveau par niveau uniquement, pas de UseTool
// ============================================================================
SCSFExport scsf_DOM_ENGINE_V0(SCStudyInterfaceRef sc)
{
  // -------------------------------------------------------------------------
  // SetDefaults : configuration de base de l’ENGINE
  // -------------------------------------------------------------------------
  if (sc.SetDefaults)
  {
    sc.GraphName = "DOM_ENGINE_V0";
    sc.AutoLoop  = 0;
    sc.UpdateAlways = 1;
    sc.UsesMarketDepthData = 1;
    sc.ValueFormat = 26;
    sc.GraphRegion = 0;
    sc.CalculationPrecedence = LOW_PREC_LEVEL;

    // ENGINE headless : pas d’affichage dans les fenêtres de valeurs
    sc.DisplayStudyName = 0;
    sc.DisplayStudyInputValues = 0;
    sc.DisplayStudyInputValuesInStudySummary = 0;
    sc.HideStudy = 1;

    // Subgraphs = features DOM v0 (1..24)
    // Groupe A — NEAR
    sc.Subgraph[0].Name = "DOM_NearBid_Vol";       // SG1
    sc.Subgraph[1].Name = "DOM_NearAsk_Vol";       // SG2
    sc.Subgraph[2].Name = "DOM_Near_Imbalance";    // SG3
    sc.Subgraph[3].Name = "DOM_Near_DepthShare";   // SG4
    sc.Subgraph[4].Name = "DOM_NearBid_Vol_EMA";   // SG5
    sc.Subgraph[5].Name = "DOM_NearAsk_Vol_EMA";   // SG6

    // Groupe B — MID
    sc.Subgraph[6].Name  = "DOM_MidBid_Vol";       // SG7
    sc.Subgraph[7].Name  = "DOM_MidAsk_Vol";       // SG8
    sc.Subgraph[8].Name  = "DOM_Mid_Imbalance";    // SG9
    sc.Subgraph[9].Name  = "DOM_Mid_DepthShare";   // SG10

    // Groupe C — FAR
    sc.Subgraph[10].Name = "DOM_FarBid_Vol";       // SG11
    sc.Subgraph[11].Name = "DOM_FarAsk_Vol";       // SG12
    sc.Subgraph[12].Name = "DOM_Far_Imbalance";    // SG13
    sc.Subgraph[13].Name = "DOM_Far_DepthShare";   // SG14

    // Groupe D — Global / top of book
    sc.Subgraph[14].Name = "DOM_TotalDepth_Vol";       // SG15
    sc.Subgraph[15].Name = "DOM_DepthSkew_NearVsFar";  // SG16
    sc.Subgraph[16].Name = "DOM_Spread_Ticks";         // SG17
    sc.Subgraph[17].Name = "DOM_TopOfBook_Imbalance";  // SG18

    // Groupe E — Gates (features 0/1)
    sc.Subgraph[18].Name = "GATE_MicroOK";    // SG19
    sc.Subgraph[19].Name = "GATE_SpreadOK";   // SG20
    sc.Subgraph[20].Name = "GATE_StaleOK";    // SG21
    sc.Subgraph[21].Name = "GATE_SessionOK";  // SG22

    // Groupe F — Soft features / dérivées
    sc.Subgraph[22].Name = "DOM_Near_Imb_Diff";   // SG23
    sc.Subgraph[23].Name = "DOM_Depth_ModeFlag";  // SG24 (0/1/2/3)

    // Style headless : tout en IGNORE par défaut
    for (int sg = 0; sg < 24; ++sg)
    {
      sc.Subgraph[sg].DrawStyle = DRAWSTYLE_IGNORE;
      sc.Subgraph[sg].PrimaryColor = COLOR_WHITE;
      sc.Subgraph[sg].DrawZeros = 0;
      sc.Subgraph[sg].DisplayNameValueInDataLine = 0;
      sc.Subgraph[sg].DisplayNameValueInValueWindow = 0;
      sc.Subgraph[sg].DisplayNameValueInChartValuesWindow = 0;
    }

    // Inputs (01..08)
    SCInputRef In_NearLevels   = sc.Input[0];
    SCInputRef In_MidLevels    = sc.Input[1];
    SCInputRef In_FarLevels    = sc.Input[2];
    SCInputRef In_MaxSpreadTks = sc.Input[3];
    SCInputRef In_EMAPercent   = sc.Input[4];
    SCInputRef In_EnableStale  = sc.Input[5];
    SCInputRef In_StaleDelayMs = sc.Input[6];
    SCInputRef In_DebugLayout  = sc.Input[7];

    In_NearLevels.Name   = "01. Niveaux bande NEAR";
    In_NearLevels.SetInt(5);

    In_MidLevels.Name    = "02. Niveaux bande MID";
    In_MidLevels.SetInt(10);

    In_FarLevels.Name    = "03. Niveaux bande FAR";
    In_FarLevels.SetInt(20);

    In_MaxSpreadTks.Name = "04. Spread max (ticks) GATE_SpreadOK";
    In_MaxSpreadTks.SetInt(4);

    In_EMAPercent.Name   = "05. Lissage EMA (%) NEAR Vol";
    In_EMAPercent.SetInt(50); // 50% ~ EMA assez réactive

    In_EnableStale.Name  = "06. Activer GATE_StaleOK";
    In_EnableStale.SetYesNo(0);

    In_StaleDelayMs.Name = "07. Délai max (ms) avant DOM stale";
    In_StaleDelayMs.SetInt(250);

    In_DebugLayout.Name  = "08. Log layout features au démarrage";
    In_DebugLayout.SetYesNo(0);

    return;
  }

  // -------------------------------------------------------------------------
  // Initialisation persistante
  // -------------------------------------------------------------------------
  int&  p_Inited          = sc.GetPersistentInt(0);
  int&  p_LastDomChangeMs = sc.GetPersistentInt(1); // stocké en ms (approx)
  double& p_PrevNearImb   = sc.GetPersistentDouble(0);

  if (p_Inited == 0)
  {
    p_Inited = 1;
    p_LastDomChangeMs = 0;
    p_PrevNearImb = 0.0;

    // Validation pack / DOM activé
    du::pack::validate_startup(sc);
    du::pack::require_dom(sc);

    // Optionnel : log layout des features pour debug
    const SCInputRef& In_DebugLayout = sc.Input[7];
    if (In_DebugLayout.GetYesNo() != 0)
    {
      static const char* const kDomFeatureNames[24] = {
        "DOM_NearBid_Vol",
        "DOM_NearAsk_Vol",
        "DOM_Near_Imbalance",
        "DOM_Near_DepthShare",
        "DOM_NearBid_Vol_EMA",
        "DOM_NearAsk_Vol_EMA",
        "DOM_MidBid_Vol",
        "DOM_MidAsk_Vol",
        "DOM_Mid_Imbalance",
        "DOM_Mid_DepthShare",
        "DOM_FarBid_Vol",
        "DOM_FarAsk_Vol",
        "DOM_Far_Imbalance",
        "DOM_Far_DepthShare",
        "DOM_TotalDepth_Vol",
        "DOM_DepthSkew_NearVsFar",
        "DOM_Spread_Ticks",
        "DOM_TopOfBook_Imbalance",
        "GATE_MicroOK",
        "GATE_SpreadOK",
        "GATE_StaleOK",
        "GATE_SessionOK",
        "DOM_Near_Imb_Diff",
        "DOM_Depth_ModeFlag"
      };
      du::duPublishFeatureLayout(sc, "DOM_ENGINE_V0", kDomFeatureNames, 24);
    }
  }

  // -------------------------------------------------------------------------
  // Lecture des inputs
  // -------------------------------------------------------------------------
  const int maxLevelsDom = 60; // budget classique DOM (DU_LEVELS)

  int nearLevels = sc.Input[0].GetInt();
  int midLevels  = sc.Input[1].GetInt();
  int farLevels  = sc.Input[2].GetInt();

  nearLevels = ClampInt(nearLevels, 0, maxLevelsDom);
  midLevels  = ClampInt(midLevels,  0, maxLevelsDom);
  farLevels  = ClampInt(farLevels,  0, maxLevelsDom);

  const int totalLevelsCfg = nearLevels + midLevels + farLevels;
  if (totalLevelsCfg <= 0)
  {
    // Rien à agréger → tout à zéro
    for (int sg = 0; sg < 24; ++sg)
      sc.Subgraph[sg][sc.Index] = 0.0;
    return;
  }

  const int maxSpreadTicks = sc.Input[3].GetInt();
  int emaPct = sc.Input[4].GetInt();
  if (emaPct < 0)   emaPct = 0;
  if (emaPct > 100) emaPct = 100;

  const int enableStale   = sc.Input[5].GetYesNo();
  const int staleDelayMs  = sc.Input[6].GetInt() > 0 ? sc.Input[6].GetInt() : 250;

  // -------------------------------------------------------------------------
  // Lecture DOM : bandes Near/Mid/Far
  // -------------------------------------------------------------------------
  double nearBid = 0.0, midBid = 0.0, farBid = 0.0;
  double nearAsk = 0.0, midAsk = 0.0, farAsk = 0.0;

  ComputeBandVolumes(
    sc,
    nearLevels,
    midLevels,
    farLevels,
    nearBid, midBid, farBid,
    nearAsk, midAsk, farAsk
  );

  const double nearDepth = nearBid + nearAsk;
  const double midDepth  = midBid  + midAsk;
  const double farDepth  = farBid  + farAsk;
  const double totalDepth = nearDepth + midDepth + farDepth;

  // -------------------------------------------------------------------------
  // Top-of-book / spread
  // -------------------------------------------------------------------------
  double bestBidPrice = 0.0, bestAskPrice = 0.0;
  double bestBidQty = 0.0,  bestAskQty  = 0.0;
  double spreadTicks = 0.0;
  double tobImb      = 0.0;

  ComputeTopOfBook(
    sc,
    bestBidPrice,
    bestAskPrice,
    bestBidQty,
    bestAskQty,
    spreadTicks,
    tobImb
  );

  // -------------------------------------------------------------------------
  // Imbalances & depth shares
  // -------------------------------------------------------------------------
  const double nearImb 		= DomImbalance	(nearBid, nearAsk);
  const double midImb  		= DomImbalance	(midBid,  midAsk);
  const double farImb  		= DomImbalance	(farBid,  farAsk);

  const double nearShare	= DepthShare	(nearDepth, totalDepth);
  const double midShare 	= DepthShare	(midDepth,  totalDepth);
  const double farShare 	= DepthShare	(farDepth,  totalDepth);

  const double depthSkewNearVsFar = DepthShare(nearDepth - farDepth, totalDepth);

  // -------------------------------------------------------------------------
  // EMA simple sur volumes Near Bid/Ask (lissage d’affichage)
  // -------------------------------------------------------------------------
  double nearBidEma = nearBid;
  double nearAskEma = nearAsk;

  if (emaPct > 0 && emaPct < 100)
  {
    const double alpha = (double)emaPct / 100.0;

    double& prevNearBidEma = sc.GetPersistentDouble(1);
    double& prevNearAskEma = sc.GetPersistentDouble(2);

    if (prevNearBidEma == 0.0 && prevNearAskEma == 0.0 && sc.Index > 0)
    {
      // première init “douce” : on prend directement la valeur courante
      prevNearBidEma = nearBid;
      prevNearAskEma = nearAsk;
    }

    nearBidEma = alpha * nearBid + (1.0 - alpha) * prevNearBidEma;
    nearAskEma = alpha * nearAsk + (1.0 - alpha) * prevNearAskEma;

    prevNearBidEma = nearBidEma;
    prevNearAskEma = nearAskEma;
  }

  // -------------------------------------------------------------------------
  // Gates (MicroOK, SpreadOK, StaleOK, SessionOK)
  // -------------------------------------------------------------------------
  int gateMicroOK  = 0;
  int gateSpreadOK = 0;
  int gateStaleOK  = 1; // par défaut “OK” si non activé
  int gateSessionOK = 1; // placeholder v0 (pas de filtre de session ici)

  // MicroOK : DOM valide (au moins un niveau bid/ask et depth total > 0)
  if (totalDepth > 0.0 && bestBidQty > 0.0 && bestAskQty > 0.0 && bestAskPrice > bestBidPrice)
    gateMicroOK = 1;

  // SpreadOK : spread > 0 et <= maxSpreadTicks
  if (gateMicroOK && maxSpreadTicks > 0 && spreadTicks > 0.0 && spreadTicks <= (double)maxSpreadTicks)
    gateSpreadOK = 1;

  // StaleOK v0 : test très simple basé sur variation de totalDepth
  if (enableStale)
  {
    // Conversion temps système → millisecondes (~) via CurrentSystemDateTime (en jours)
    const double nowDays = sc.CurrentSystemDateTime.GetAsDouble();
    const int nowMs = (int)(nowDays * 86400000.0); // 24*60*60*1000

    // On considère le DOM “changé” si totalDepth varie (coarse mais suffisant en v0)
    static double prevTotalDepth = 0.0;
    if (totalDepth != prevTotalDepth)
    {
      prevTotalDepth = totalDepth;
      p_LastDomChangeMs = nowMs;
      gateStaleOK = 1;
    }
    else
    {
      const int diffMs = nowMs - p_LastDomChangeMs;
      gateStaleOK = (diffMs <= staleDelayMs) ? 1 : 0;
    }
  }

  // -------------------------------------------------------------------------
  // Features dérivées : Near_Imb_Diff, Depth_ModeFlag
  // -------------------------------------------------------------------------
  const double nearImbDiff = nearImb - p_PrevNearImb;
  p_PrevNearImb = nearImb;

  const double depthModeFlag = ClassifyDepthMode(nearDepth, farDepth, totalDepth);

  // -------------------------------------------------------------------------
  // Écriture des Subgraphs (SG1..SG24)
  // -------------------------------------------------------------------------
  const int idx = sc.Index;

  // Groupe A — NEAR
  sc.Subgraph[0][idx] = nearBid;        // SG1
  sc.Subgraph[1][idx] = nearAsk;        // SG2
  sc.Subgraph[2][idx] = nearImb;        // SG3
  sc.Subgraph[3][idx] = nearShare;      // SG4
  sc.Subgraph[4][idx] = nearBidEma;     // SG5
  sc.Subgraph[5][idx] = nearAskEma;     // SG6

  // Groupe B — MID
  sc.Subgraph[6][idx] = midBid;         // SG7
  sc.Subgraph[7][idx] = midAsk;         // SG8
  sc.Subgraph[8][idx] = midImb;         // SG9
  sc.Subgraph[9][idx] = midShare;       // SG10

  // Groupe C — FAR
  sc.Subgraph[10][idx] = farBid;        // SG11
  sc.Subgraph[11][idx] = farAsk;        // SG12
  sc.Subgraph[12][idx] = farImb;        // SG13
  sc.Subgraph[13][idx] = farShare;      // SG14

  // Groupe D — Global
  sc.Subgraph[14][idx] = totalDepth;         // SG15
  sc.Subgraph[15][idx] = depthSkewNearVsFar; // SG16
  sc.Subgraph[16][idx] = spreadTicks;        // SG17
  sc.Subgraph[17][idx] = tobImb;             // SG18

  // Groupe E — Gates
  sc.Subgraph[18][idx] = (double)gateMicroOK;   // SG19
  sc.Subgraph[19][idx] = (double)gateSpreadOK;  // SG20
  sc.Subgraph[20][idx] = (double)gateStaleOK;   // SG21
  sc.Subgraph[21][idx] = (double)gateSessionOK; // SG22

  // Groupe F — dérivées
  sc.Subgraph[22][idx] = nearImbDiff;     // SG23
  sc.Subgraph[23][idx] = depthModeFlag;   // SG24
}
