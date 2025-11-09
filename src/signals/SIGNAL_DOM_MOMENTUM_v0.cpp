#include "src/headers/Pack_v0.h"

// Signal v0: marqueurs UP/DN selon variation de Close.
// Transparent Circle Variable, ne déplace pas l'échelle.

SCSFExport scsf_SIGNAL_DOM_MOMENTUM_v0(SCStudyInterfaceRef sc)
{
  if (sc.SetDefaults)
  {
    sc.AutoLoop = 1;
    sc.UpdateAlways = 0;
    sc.GraphRegion = 0;
    sc.ValueFormat = 26;

    sc.Subgraph[0].Name = "UP";
    sc.Subgraph[0].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
    sc.Subgraph[0].PrimaryColor = RGB(0,179,0);
    sc.Subgraph[0].LineWidth = 15;
    sc.Subgraph[0].DrawZeros = 0;
    sc.Subgraph[0].DisplayAsMainPriceGraphValue = 0;

    sc.Subgraph[1].Name = "DN";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
    sc.Subgraph[1].PrimaryColor = RGB(255,0,0);
    sc.Subgraph[1].LineWidth = 15;
    sc.Subgraph[1].DrawZeros = 0;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;

    sc.Input[0].Name = "01. Taille marqueur";
    sc.Input[0].SetInt(15); sc.Input[0].SetIntLimits(1, 40);

    sc.Input[1].Name = "02. Activer";
    sc.Input[1].SetYesNo(1);

    return;
  }

  if (!sc.Input[1].GetYesNo())
    return;

  const int i = sc.Index;
  const float price = (float)sc.Close[i];

  // Reset visuels
  sc.Subgraph[0][i] = 0;
  sc.Subgraph[1][i] = 0;

  if (i > 0)
  {
    if (sc.Close[i] > sc.Close[i-1])
      sc.Subgraph[0][i] = price; // UP
    else if (sc.Close[i] < sc.Close[i-1])
      sc.Subgraph[1][i] = price; // DN
  }

  sc.Subgraph[0].LineWidth = sc.Input[0].GetInt();
  sc.Subgraph[1].LineWidth = sc.Input[0].GetInt();
}
