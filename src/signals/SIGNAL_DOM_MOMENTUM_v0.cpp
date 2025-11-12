#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_DOM_MOMENTUM_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "DOM Momentum v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "DOM Momentum v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(8); // Fenêtre momentum
SCInputRef In_02_ATR = sc.Input[1]; In_02_ATR.Name = "02. ATR"; In_02_ATR.SetInt(14); // Fenêtre ATR

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Momentum de prix normalisé par ATR court
int n = In_01_N.GetInt(); if (n<2) n=8;
int i0 = last-n; if (i0<0) i0=0;
double roc = sc.Close[last]-sc.Close[i0];
double atr=0.0; int m=In_02_ATR.GetInt(); if (m<2) m=14;
for (int i = (last-m+1>1?last-m+1:1); i<=last; ++i) atr += fabs((double)sc.High[i]- (double)sc.Low[i]);
atr/=m; double Result = (atr>0.0)? roc/atr : 0.0;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }