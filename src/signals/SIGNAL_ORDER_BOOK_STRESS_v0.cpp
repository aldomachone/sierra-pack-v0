#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_ORDER_BOOK_STRESS_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Order Book Stress v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Order Book Stress v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(32); // Fenêtre ATR

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Proxy: ratio High-Low vs ATR
int n=In_01_N.GetInt(); if (n<2) n=32;
double tr=0.0; for(int i=last-n+1;i<=last;++i){ int j=(i<1)?1:i; tr += fabs((double)sc.High[j]- (double)sc.Low[j]); }
tr/=n;
double range = fabs((double)sc.Close[last]- (double)sc.Open[last]);
double Result = (tr>0.0)? range/tr : 0.0;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }