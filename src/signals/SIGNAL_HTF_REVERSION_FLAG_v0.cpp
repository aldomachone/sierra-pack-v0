#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_HTF_REVERSION_FLAG_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "HTF Reversion Flag v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "HTF Reversion Flag v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(240); // Fenêtre
SCInputRef In_02_Z = sc.Input[1]; In_02_Z.Name = "02. Z"; In_02_Z.SetFloat(1.5); // Seuil

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  int n=In_01_N.GetInt(); if(n<10)n=240; int s=last-n+1; if(s<0)s=0;
  double mean=0.0; for(int i=s;i<=last;++i) mean+=sc.Close[i]; mean/=(last-s+1);
  double var=0.0; for(int i=s;i<=last;++i){ double d=sc.Close[i]-mean; var+=d*d; } var/=(last-s+1);
  double sd=(var>0.0)? sqrt(var):1.0;
  double z = (sc.Close[last]-mean)/sd;
  double Result = (fabs(z)>In_02_Z.GetFloat())? -z:0.0;


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }