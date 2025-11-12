#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_TAPE_PACE_CHANGE_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Tape Pace Change v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Tape Pace Change v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Alpha = sc.Input[0]; In_01_Alpha.Name = "01. Alpha"; In_01_Alpha.SetFloat(0.3); // EMA du rythme

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


static double ema=0.0; double v=sc.Volume[last];
double a=In_01_Alpha.GetFloat(); if (a<=0.0||a>=1.0) a=0.3;
if (sc.UpdateStartIndex==0) ema=v;
ema = a*v + (1.0-a)*ema;
double Result = (ema>0.0)? (v-ema)/ema : 0.0;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }