#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_POLY_TREND_RESID_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Poly Trend Residual v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Poly Trend Residual v0";
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


  int n=In_01_N.GetInt(); if(n<4)n=64; int s=last-n+1; if(s<1)s=1; int N=s? (last-s+1):0;
  // régression linéaire simple pour proxy poly
  double sx=0, sy=0, sxx=0, sxy=0; int idx=0;
  for(int i=s;i<=last;++i){ ++idx; double x=(double)idx; double y=sc.Close[i]; sx+=x; sy+=y; sxx+=x*x; sxy+=x*y; }
  double den = idx*sxx - sx*sx; double b = (den!=0.0)? (idx*sxy - sx*sy)/den : 0.0; double a = (idx>0)? (sy - b*sx)/idx : 0.0;
  double yhat = a + b*(double)idx;
  double resid = sc.Close[last] - yhat;
  static double m=0.0, v=0.0; static int k=0; ++k; double d=resid-m; m+=d/k; v+=d*(resid-m);
  double sd=(k>1)? sqrt(v/(k-1)):1.0;
  double Result = resid/sd;


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }