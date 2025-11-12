#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_KALMAN_DRIFT_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Kalman Drift v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Kalman Drift v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Q = sc.Input[0]; In_01_Q.Name = "01. Q"; In_01_Q.SetFloat(1e-05); // Process noise
SCInputRef In_02_R = sc.Input[1]; In_02_R.Name = "02. R"; In_02_R.SetFloat(0.001); // Measure noise

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  static double x=0.0,P=1.0; double q=In_01_Q.GetFloat(), r=In_02_R.GetFloat();
  if(sc.UpdateStartIndex==0){ x=sc.Close[0]; P=1.0; }
  double x_pred=x; double P_pred=P+q;
  double z=sc.Close[last]; double K=P_pred/(P_pred+r);
  x = x_pred + K*(z-x_pred); P=(1.0-K)*P_pred;
  double Result = x - sc.Close[last];


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }