#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_GATE_COMBO_PROXY_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Gate Combo proxy v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Gate Combo proxy v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Mode = sc.Input[0]; In_01_Mode.Name = "01. Mode"; In_01_Mode.SetInt(0); // 0=AND 1=OR

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Combinaison logique de 3 sous-signaux internes simples
double a = (sc.Close[last]>sc.Close[last-1])?1.0:0.0;
double b = (sc.Volume[last]>sc.Volume[last-1])?1.0:0.0;
double c = (fabs(sc.High[last]-sc.Low[last])>fabs(sc.High[last-1]-sc.Low[last-1]))?1.0:0.0;
int mode = In_01_Mode.GetInt(); // 0=AND, 1=OR
double Result = (mode==0)? ((a>0.5&&b>0.5&&c>0.5)?1.0:0.0) : ((a>0.5||b>0.5||c>0.5)?1.0:0.0);


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }