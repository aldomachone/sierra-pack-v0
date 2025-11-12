#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_TAPE_INTERARRIVAL_Z_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Tape Interarrival Z v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Tape Interarrival Z v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Alpha = sc.Input[0]; In_01_Alpha.Name = "01. Alpha"; In_01_Alpha.SetFloat(0.2); // EMA lissage proxy
SCInputRef In_02_Z = sc.Input[1]; In_02_Z.Name = "02. Z"; In_02_Z.SetFloat(2.0); // Seuil

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  static double ema=0.0; double v=(double)sc.Volume[last]; if(sc.UpdateStartIndex==0) ema=v;
  double a=In_01_Alpha.GetFloat(); if(a<=0.0||a>=1.0) a=0.2;
  ema = a*v + (1.0-a)*ema;
  double z = (ema>0.0)? (v-ema)/sqrt(ema+1e-9) : 0.0;
  double Result = (fabs(z)>In_02_Z.GetFloat())? z:0.0;


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }