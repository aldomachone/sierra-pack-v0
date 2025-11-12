#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_CHANGE_POINT_CUSUM_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Change-Point CUSUM v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Change-Point CUSUM v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_K = sc.Input[0]; In_01_K.Name = "01. K"; In_01_K.SetFloat(0.1); // Drift
SCInputRef In_02_H = sc.Input[1]; In_02_H.Name = "02. H"; In_02_H.SetFloat(2.0); // Seuil

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


double k = In_01_K.GetFloat(), h = In_02_H.GetFloat();
if (k<=0.0) k=0.1; if (h<=0.0) h=2.0;
static double gpos=0.0, gneg=0.0;
double x = (double)sc.Close[last]- (double)sc.Close[last-1];
gpos = fmax(0.0, gpos + x - k);
gneg = fmax(0.0, gneg - x - k);
double Result = 0.0;
if (gpos>h) {{ Result = 1.0; gpos=0.0; gneg=0.0; }}
else if (gneg>h) {{ Result = -1.0; gpos=0.0; gneg=0.0; }}


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }