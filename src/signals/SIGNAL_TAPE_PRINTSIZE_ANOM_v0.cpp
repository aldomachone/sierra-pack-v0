#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_TAPE_PRINTSIZE_ANOM_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Tape PrintSize Anomaly v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Tape PrintSize Anomaly v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Z = sc.Input[0]; In_01_Z.Name = "01. Z"; In_01_Z.SetFloat(3.0); // Seuil z-score

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


static double mean=0.0, m2=0.0; static int n=0;
double x = (double)sc.Volume[last]; // proxy taille trade moyen
++n; double delta=x-mean; mean += delta/n; m2 += delta*(x-mean);
double var = (n>1)? m2/(n-1):0.0; double sd = (var>0.0)? sqrt(var):0.0;
double z = (sd>0.0)? (x-mean)/sd : 0.0;
double Result = (z>In_01_Z.GetFloat())? z:0.0;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }