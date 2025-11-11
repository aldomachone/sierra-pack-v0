#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_DOM_LATENCY_RACE_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "DOM Latency Race v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "DOM Latency Race v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Facteur = sc.Input[0]; In_01_Facteur.Name = "01. Facteur"; In_01_Facteur.SetFloat(1.0); // Seuil relatif volume

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Approximé: si plusieurs barres dans Δt min? Ici sans horodatage ms, on utilise UpdateAlways et marque des bursts via volume
static double emaV=0.0; double alpha=0.5;
double v = sc.Volume[last]; if (sc.UpdateStartIndex==0) emaV=v;
emaV = alpha*v + (1.0-alpha)*emaV;
double Result = (v>emaV*(1.0+In_01_Facteur.GetFloat()))? (v/emaV - 1.0) : 0.0;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }