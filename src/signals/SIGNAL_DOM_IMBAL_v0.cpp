#include "src/headers/Pack_v0.h"

// Signal v0: simple déséquilibre en lisant l'engine DOM v0 (si présent).
// Si l'engine n'est pas présent, fallback sur variation de Close.

SCSFExport scsf_SIGNAL_DOM_IMBAL_V0(SCStudyInterfaceRef sc)
{
  if (sc.SetDefaults)
  {
    sc.AutoLoop = 1;
    sc.UpdateAlways = 0;
    sc.GraphRegion = 0;
    sc.ValueFormat = 26;

    sc.Subgraph[0].Name = "IMBAL_UP";
    sc.Subgraph[0].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
    sc.Subgraph[0].PrimaryColor = RGB(0,179,0);
    sc.Subgraph[0].LineWidth = 15;
    sc.Subgraph[0].DrawZeros = 0;
    sc.Subgraph[0].DisplayAsMainPriceGraphValue = 0;

    sc.Subgraph[1].Name = "IMBAL_DN";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
    sc.Subgraph[1].PrimaryColor = RGB(255,0,0);
    sc.Subgraph[1].LineWidth = 15;
    sc.Subgraph[1].DrawZeros = 0;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;

    sc.Input[0].Name = "01. Index étude DOM_ENGINE_V0 (si utilisé)";
    sc.Input[0].SetStudyID(0);

    sc.Input[1].Name = "02. Seuil déséquilibre (%%)";
    sc.Input[1].SetFloat(10.0f);

    sc.Input[2].Name = "03. Taille marqueur";
    sc.Input[2].SetInt(15);

    return;
  }

  const int i = sc.Index;
  sc.Subgraph[0][i] = 0;
  sc.Subgraph[1][i] = 0;

  const int domStudy = sc.Input[0].GetStudyID();
  float nearPct = 0.0f, midPct = 0.0f, farPct = 0.0f;
  bool ok = false;

  if (domStudy > 0)
  {
    // Lire les SG 1..3 de DOM_ENGINE_V0 si présent
    SCFloatArray ref1, ref2, ref3;
    if (sc.GetStudyArrayUsingID(domStudy, 0, ref1)    // Near
     && sc.GetStudyArrayUsingID(domStudy, 1, ref2)    // Mid
     && sc.GetStudyArrayUsingID(domStudy, 2, ref3))   // Far
    {
      nearPct = ref1[i];
      midPct  = ref2[i];
      farPct  = ref3[i];
      ok = true;
    }
  }

  float sig = 0.0f;
  if (ok)
  {
    // v0: signal = near - far (acheteurs si near% > far%)
    sig = (nearPct - farPct);
  }
  else
  {
    // Fallback: variation de prix
    if (i > 0) sig = (float)(sc.Close[i] - sc.Close[i-1]);
  }

  const float thr = (float)sc.Input[1].GetFloat();
  const float price = (float)sc.Close[i];

  if (sig > thr)        sc.Subgraph[0][i] = price;
  else if (sig < -thr)  sc.Subgraph[1][i] = price;

  const int size = sc.Input[2].GetInt();
  sc.Subgraph[0].LineWidth = size;
  sc.Subgraph[1].LineWidth = size;
}
