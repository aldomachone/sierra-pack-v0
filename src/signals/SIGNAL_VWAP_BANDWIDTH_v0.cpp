#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_VWAP_BANDWIDTH_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "VWAP Bandwidth v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "VWAP Bandwidth v0";
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


  int n=In_01_N.GetInt(); if(n<2)n=64; int s=last-n+1; if(s<0)s=0;
  double num=0,den=0; for(int i=s;i<=last;++i){ double p=(sc.High[i]+sc.Low[i]+sc.Close[i])/3.0; double v=sc.Volume[i]; num+=p*v; den+=v; }
  double vwap=(den>0.0)? num/den:sc.Close[last];
  double bw=0.0; for(int i=s;i<=last;++i){ double p=(sc.High[i]+sc.Low[i]+sc.Close[i])/3.0; bw += fabs(p - vwap); }
  bw/= (double)(n);
  double Result = bw / (fabs(vwap)>0.0? fabs(vwap):1.0);


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }