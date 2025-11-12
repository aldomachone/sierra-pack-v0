#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_FEATURE_KURT_SKEW_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Feature Kurtosis-Skew v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Feature Kurtosis-Skew v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(64); // Fenêtre

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  int n=In_01_N.GetInt(); if(n<4)n=64; int s=last-n+1; if(s<1)s=1; int N=last-s+1;
  double mean=0, m2=0, m3=0, m4=0;
  for(int i=s;i<=last;++i){ double d=sc.Close[i]; mean+=d; } mean/=N;
  for(int i=s;i<=last;++i){ double x=sc.Close[i]-mean; double x2=x*x; m2+=x2; m3+=x2*x; m4+=x2*x2; }
  m2/=N; m3/=N; m4/=N;
  double skew=(m2>0.0)? m3/(pow(m2,1.5)):0.0; double kurt=(m2>0.0)? m4/(m2*m2):0.0;
  double Result = 0.5*skew + 0.5*(kurt-3.0);


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }