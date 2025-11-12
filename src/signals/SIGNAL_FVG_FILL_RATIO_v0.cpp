#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_FVG_FILL_RATIO_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "FVG Fill Ratio v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "FVG Fill Ratio v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

// Aucun input

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Détecte un gap haussier/baissier récent et mesure le comblement
double upGap = (sc.Low[last]   > sc.High[last-1])? (sc.Low[last]-sc.High[last-1]) : 0.0;
double dnGap = (sc.High[last]  < sc.Low[last-1]) ? (sc.Low[last-1]-sc.High[last]) : 0.0;
double gap = fmax(upGap, dnGap);
static double refGap=0.0;
if (gap>0.0) refGap = gap;
double fill = (refGap>0.0)? (refGap - gap)/refGap : 0.0;
if (fill<0.0) fill=0.0; if (fill>1.0) fill=1.0;
double Result = fill; // 0..1


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }