#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_DOM_ROTATION_RATE_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "DOM Rotation Rate v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "DOM Rotation Rate v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Alpha = sc.Input[0]; In_01_Alpha.Name = "01. Alpha"; In_01_Alpha.SetFloat(0.3); // EMA

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  s_MarketDepthEntry b,a; double pb=sc.Close[last-1], pc=sc.Close[last];
  double rot = fabs(pc - pb);
  static double ema=0.0; if (sc.UpdateStartIndex==0) ema=rot;
  double apha = In_01_Alpha.GetFloat(); if (apha<=0.0||apha>=1.0) apha=0.3;
  ema = apha*rot + (1.0-apha)*ema;
  double Result = (ema>0.0)? rot/ema - 1.0 : 0.0;


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }