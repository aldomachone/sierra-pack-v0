#include "src/headers/Pack_v0.h"

// Signal v0: "sweep" basique avec Up/DownVolume par barre.
// Marqueur si UpVolume ou DownVolume dépasse le seuil.

SCSFExport scsf_SIGNAL_TAPE_SWEEP_v0(SCStudyInterfaceRef sc)
{
  if (sc.SetDefaults)
  {
    sc.AutoLoop = 1;
    sc.UpdateAlways = 0;
    sc.GraphRegion = 0;
    sc.ValueFormat = 26;

    sc.Subgraph[0].Name = "SWEEP_UP";
    sc.Subgraph[0].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
    sc.Subgraph[0].PrimaryColor = RGB(0,179,0);
    sc.Subgraph[0].LineWidth = 15;
    sc.Subgraph[0].DrawZeros = 0;
    sc.Subgraph[0].DisplayAsMainPriceGraphValue = 0;

    sc.Subgraph[1].Name = "SWEEP_DN";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
    sc.Subgraph[1].PrimaryColor = RGB(255,0,0);
    sc.Subgraph[1].LineWidth = 15;
    sc.Subgraph[1].DrawZeros = 0;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;

    sc.Input[0].Name = "01. Seuil UpVolume";
    sc.Input[0].SetFloat(100.0f);

    sc.Input[1].Name = "02. Seuil DownVolume";
    sc.Input[1].SetFloat(100.0f);

    sc.Input[2].Name = "03. Taille marqueur";
    sc.Input[2].SetInt(15);

    return;
  }

  const int i = sc.Index;
  sc.Subgraph[0][i] = 0;
  sc.Subgraph[1][i] = 0;

  const float uv = (float)sc.UpVolume[i];
  const float dv = (float)sc.DownVolume[i];
  const float p  = (float)sc.Close[i];

  if (uv >= sc.Input[0].GetFloat()) sc.Subgraph[0][i] = p;
  if (dv >= sc.Input[1].GetFloat()) sc.Subgraph[1][i] = p;

  const int size = sc.Input[2].GetInt();
  sc.Subgraph[0].LineWidth = size;
  sc.Subgraph[1].LineWidth = size;
}
