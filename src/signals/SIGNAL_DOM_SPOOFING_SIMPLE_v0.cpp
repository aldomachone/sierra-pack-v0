#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_DOM_SPOOFING_SIMPLE_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "DOM Spoofing simple v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "DOM Spoofing simple v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_p95_Quantite = sc.Input[0]; In_01_p95_Quantite.Name = "01. p95 Quantite"; In_01_p95_Quantite.SetFloat(100.0); // Seuil quantité L1

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Heuristique: quantité L1 élevée sans exécution → possible spoof
s_MarketDepthEntry mdB, mdA;
double b=0.0,a=0.0;
if (sc.GetBidMarketDepthEntryAtLevel(mdB,0)) b = mdB.Quantity;
if (sc.GetAskMarketDepthEntryAtLevel(mdA,0)) a = mdA.Quantity;
double thr = In_01_p95_Quantite.GetFloat(); // proxy seuil
double Result = 0.0;
if (b>thr && fabs(sc.Close[last]-sc.Close[last-1])<=0.0) Result = 1.0;
if (a>thr && fabs(sc.Close[last]-sc.Close[last-1])<=0.0) Result = fmax(Result,1.0);


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }