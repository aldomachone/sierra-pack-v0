#include "src/headers/Pack_v0.h"

// Engine Tape minimal v0 : accumulateurs Buy/Sell par barre, Delta.
// Subgraphs cachés; pas de classification avancée.

SCSFExport scsf_TAPE_ENGINE_V0(SCStudyInterfaceRef sc)
{
  if (sc.SetDefaults)
  {
    sc.AutoLoop = 1;              // v0 simple par barre
    sc.UpdateAlways = 0;
    sc.GraphRegion = 1;
    sc.ValueFormat = 26;

    sc.Subgraph[0].Name = "BuyVol";
    sc.Subgraph[1].Name = "SellVol";
    sc.Subgraph[2].Name = "Delta";
    for (int i = 0; i < 3; ++i)
    {
      sc.Subgraph[i].DrawStyle = DRAWSTYLE_IGNORE;
      sc.Subgraph[i].DrawZeros = 0;
    }

    sc.Input[0].Name = "01. Remise à zéro par nouvelle barre";
    sc.Input[0].SetYesNo(1);

    return;
  }

  if (sc.Index == 0 && sc.Input[0].GetYesNo())
  {
    sc.Subgraph[0][sc.Index] = 0;
    sc.Subgraph[1][sc.Index] = 0;
    sc.Subgraph[2][sc.Index] = 0;
  }

  // v0: utilise simplement VolumeUp/VolumeDown de la barre
  const float buy  = (float)sc.UpVolume[sc.Index];
  const float sell = (float)sc.DownVolume[sc.Index];

  sc.Subgraph[0][sc.Index] = buy;
  sc.Subgraph[1][sc.Index] = sell;
  sc.Subgraph[2][sc.Index] = buy - sell;
}
