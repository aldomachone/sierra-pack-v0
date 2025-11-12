#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_DOM_CROSS_GAP_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "DOM Cross Gap v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "DOM Cross Gap v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Min_Ticks = sc.Input[0]; In_01_Min_Ticks.Name = "01. Min Ticks"; In_01_Min_Ticks.SetInt(2); // Seuil de gap
SCInputRef In_02_TickSize = sc.Input[1]; In_02_TickSize.Name = "02. TickSize"; In_02_TickSize.SetFloat(1.0); // Taille tick

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  double dp = fabs((double)sc.Close[last] - (double)sc.Close[last-1]);
  double tick = In_02_TickSize.GetFloat(); if (tick<=0.0) tick=1.0;
  double Result = (dp >= In_01_Min_Ticks.GetInt()*tick)? dp/(In_01_Min_Ticks.GetInt()*tick) : 0.0;


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }