// ============================================================================
// Pack v0 — Engines (v0) — Tape & Advanced
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_OFI_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited = sc.GetPersistentInt(1);

  if (sc.SetDefaults)
  {
    sc.GraphName = "OFI_ENGINE_v0";
    sc.AutoLoop = 0; sc.UpdateAlways = 0; sc.GraphRegion = 0; sc.ValueFormat = 26; sc.FreeDLL = 0;

    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[3].Name = "SG03";
    sc.Subgraph[3].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[3].DrawZeros = false;
    sc.Subgraph[3].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[4].Name = "SG04";
    sc.Subgraph[4].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[4].DrawZeros = false;
    sc.Subgraph[4].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[5].Name = "SG05";
    sc.Subgraph[5].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[5].DrawZeros = false;
    sc.Subgraph[5].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[6].Name = "SG06";
    sc.Subgraph[6].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[6].DrawZeros = false;
    sc.Subgraph[6].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[7].Name = "SG07";
    sc.Subgraph[7].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[7].DrawZeros = false;
    sc.Subgraph[7].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[8].Name = "SG08";
    sc.Subgraph[8].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[8].DrawZeros = false;
    sc.Subgraph[8].DisplayAsMainPriceGraphValue = 0;

    sc.Input[0].Name = "01. Horizon barres";
    sc.Input[0].SetInt(1); sc.Input[0].SetIntLimits(1, 1000);

    sc.DrawZeros=false; return;
  }

  if (!inited || sc.IsFullRecalculation) { inited=1; }
  if (sc.ArraySize <= 1) return;

  int H = sc.Input[0].GetInt();
  int i = sc.ArraySize-1;
  int j = i - H; if (j < 0) j = 0;

  // Approx OFI: delta BidVol - delta AskVol sur la fenêtre
  double dBV=0.0, dAV=0.0;
  dBV = sc.BidVolume[i] - sc.BidVolume[j];
  dAV = sc.AskVolume[i] - sc.AskVolume[j];
  sc.Subgraph[1][i] = dBV - dAV;       // OFI net
  sc.Subgraph[2][i] = dBV;             // BidVol delta
  sc.Subgraph[3][i] = -dAV;            // AskVol delta (négatif pour symétrie)
}
