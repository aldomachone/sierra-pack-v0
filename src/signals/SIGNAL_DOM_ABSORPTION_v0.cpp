#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_DOM_ABSORPTION_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "DOM Absorption v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "DOM Absorption v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_p95_Volume = sc.Input[0]; In_01_p95_Volume.Name = "01. p95 Volume"; In_01_p95_Volume.SetFloat(1000.0); // Seuil volume
SCInputRef In_02_Max_DeltaP = sc.Input[1]; In_02_Max_DeltaP.Name = "02. Max DeltaP"; In_02_Max_DeltaP.SetFloat(0.0); // Seuil mouvement prix

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Absorption approximée: grand volume sur la barre avec faible ΔP
double vol = sc.Volume[last];
double dp = fabs((double)sc.Close[last]- (double)sc.Close[last-1]);
double thrV = In_01_p95_Volume.GetFloat();
double thrP = In_02_Max_DeltaP.GetFloat();
double Result = (vol>=thrV && dp<=thrP) ? (vol/thrV) : 0.0;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }