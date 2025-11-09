#include "src/headers/Pack_v0.h"

// Engine DOM minimal v0 : lecture unique du DOM par tick, 3 bandes Near/Mid/Far
// Subgraphs cachés, exposés pour lecture par signaux/MLP. Aucune logique lourde.

SCSFExport scsf_DOM_ENGINE_V0(SCStudyInterfaceRef sc)
{
  if (sc.SetDefaults)
  {
    sc.AutoLoop = 0;
    sc.UpdateAlways = 1;
    sc.GraphRegion = 1;
    sc.ValueFormat = 26;
    sc.UsesMarketDepthData = 1;

    // Subgraphs exposés (cachés)
    sc.Subgraph[0].Name = "DOM Near %";
    sc.Subgraph[1].Name = "DOM Mid %";
    sc.Subgraph[2].Name = "DOM Far %";
    for (int i = 0; i < 3; ++i)
    {
      sc.Subgraph[i].DrawStyle = DRAWSTYLE_IGNORE;
      sc.Subgraph[i].DrawZeros = 0;
    }

    // Inputs 01..N
    sc.Input[0].Name = "01. Niveaux";
    sc.Input[0].SetInt(60); sc.Input[0].SetIntLimits(1, 120);

    sc.Input[1].Name = "02. Premier niveau";
    sc.Input[1].SetInt(1);  sc.Input[1].SetIntLimits(1, 120);

    sc.Input[2].Name = "03. Répartition bandes (near/mid/far)";
    sc.Input[2].SetInt(10); // near=10, mid=30, far=reste

    sc.Input[3].Name = "04. Normaliser en % de profondeur totale";
    sc.Input[3].SetYesNo(1);

    return;
  }

  // --- Exécution ---
  const int levels = sc.Input[0].GetInt();
  const int first  = sc.Input[1].GetInt();
  const int nearN  = sc.Input[2].GetInt();
  const int midN   = 30; // fixe v0
  const int farN   = max(0, levels - nearN - midN);

  // Lecture snapshot DOM (une fois)
  s_MarketDepthEntry md{};
  double sumBidNear=0, sumAskNear=0, sumBidMid=0, sumAskMid=0, sumBidFar=0, sumAskFar=0;

  int availBid = sc.GetMarketDepthNumberOfLevels(sc.MarketDepthGraphType == DEPTH_BID);
  int availAsk = sc.GetMarketDepthNumberOfLevels(sc.MarketDepthGraphType == DEPTH_ASK);
  (void)availBid; (void)availAsk; // v0: non utilisé, mais lecture ci-dessus force MAJ interne

  for (int li = 0; li < levels; ++li)
  {
    if (li < nearN)
    {
      if (sc.GetBidMarketDepthEntryAtLevel(md, first + li - 1)) sumBidNear += md.Quantity;
      if (sc.GetAskMarketDepthEntryAtLevel(md, first + li - 1)) sumAskNear += md.Quantity;
    }
    else if (li < nearN + midN)
    {
      if (sc.GetBidMarketDepthEntryAtLevel(md, first + li - 1)) sumBidMid += md.Quantity;
      if (sc.GetAskMarketDepthEntryAtLevel(md, first + li - 1)) sumAskMid += md.Quantity;
    }
    else
    {
      if (sc.GetBidMarketDepthEntryAtLevel(md, first + li - 1)) sumBidFar += md.Quantity;
      if (sc.GetAskMarketDepthEntryAtLevel(md, first + li - 1)) sumAskFar += md.Quantity;
    }
  }

  const double nearTot = sumBidNear + sumAskNear;
  const double midTot  = sumBidMid  + sumAskMid;
  const double farTot  = sumBidFar  + sumAskFar;
  const double allTot  = nearTot + midTot + farTot;

  double vNear = nearTot;
  double vMid  = midTot;
  double vFar  = farTot;

  if (sc.Input[3].GetYesNo() && allTot > 0)
  {
    vNear = 100.0 * nearTot / allTot;
    vMid  = 100.0 * midTot  / allTot;
    vFar  = 100.0 * farTot  / allTot;
  }

  // Écrire à l'index courant (exposés pour GetArray())
  sc.Subgraph[0][sc.Index] = (float)vNear;
  sc.Subgraph[1][sc.Index] = (float)vMid;
  sc.Subgraph[2][sc.Index] = (float)vFar;
}
