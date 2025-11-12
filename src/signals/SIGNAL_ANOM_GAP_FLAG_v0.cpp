#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_ANOM_GAP_FLAG_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Anomaly Gap Flag v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Anomaly Gap Flag v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Ticks = sc.Input[0]; In_01_Ticks.Name = "01. Ticks"; In_01_Ticks.SetInt(3); // Seuil ticks
SCInputRef In_02_TickSize = sc.Input[1]; In_02_TickSize.Name = "02. TickSize"; In_02_TickSize.SetFloat(1.0); // Tick

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  double dp=fabs((double)sc.Open[last]- (double)sc.Close[last-1]); double t=In_02_TickSize.GetFloat(); if(t<=0.0)t=1.0;
  double Result = (dp>= In_01_Ticks.GetInt()*t)? 1.0:0.0;


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }