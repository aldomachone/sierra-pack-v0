#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_DOM_ICEBERG_PROX_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "DOM Iceberg Proxy v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "DOM Iceberg Proxy v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_p90_Volume = sc.Input[0]; In_01_p90_Volume.Name = "01. p90 Volume"; In_01_p90_Volume.SetFloat(700.0); // Seuil volume
SCInputRef In_02_Max_DeltaP = sc.Input[1]; In_02_Max_DeltaP.Name = "02. Max DeltaP"; In_02_Max_DeltaP.SetFloat(0.0); // Seuil mouvement prix
SCInputRef In_03_Min_Streak = sc.Input[2]; In_03_Min_Streak.Name = "03. Min Streak"; In_03_Min_Streak.SetInt(3); // Nombre de barres consécutives

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Plusieurs barres avec volume élevé et prix inchangé → refill probable
double vol = sc.Volume[last];
double dp = fabs((double)sc.Close[last]- (double)sc.Close[last-1]);
static int streak=0;
if (vol>=In_01_p90_Volume.GetFloat() && dp<=In_02_Max_DeltaP.GetFloat()) ++streak;
else streak=0;
double Result = (streak>=In_03_Min_Streak.GetInt())? (double)streak : 0.0;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }