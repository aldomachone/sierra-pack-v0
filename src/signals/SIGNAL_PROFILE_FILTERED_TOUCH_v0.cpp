#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_PROFILE_FILTERED_TOUCH_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Profile Filtered Touch v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Profile Filtered Touch v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(128); // Fenêtre
SCInputRef In_02_MaxTicks = sc.Input[1]; In_02_MaxTicks.Name = "02. MaxTicks"; In_02_MaxTicks.SetInt(3); // Tolérance

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  int n=In_01_N.GetInt(); if(n<2)n=128; int s=last-n+1; if(s<1)s=1;
  double H=-1e9,L=1e9,C=sc.Close[s]; for(int i=s;i<=last;++i){ if(sc.High[i]>H)H=sc.High[i]; if(sc.Low[i]<L)L=sc.Low[i]; C=sc.Close[i]; }
  double P=(H+L+C)/3.0;
  double d=fabs(sc.Close[last]-P); double tick=1.0;
  double Result = (d<=In_02_MaxTicks.GetInt()*tick)? 1.0:0.0;


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }