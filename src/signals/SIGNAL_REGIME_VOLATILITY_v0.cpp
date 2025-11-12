#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_REGIME_VOLATILITY_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Regime Volatility v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Regime Volatility v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Nshort = sc.Input[0]; In_01_Nshort.Name = "01. Nshort"; In_01_Nshort.SetInt(16); // Courte
SCInputRef In_02_Nlong = sc.Input[1]; In_02_Nlong.Name = "02. Nlong"; In_02_Nlong.SetInt(64); // Longue

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  int ns=In_01_Nshort.GetInt(); int nl=In_02_Nlong.GetInt(); if(ns<2)ns=16; if(nl<ns+1)nl=64;
  int ss=last-ns+1; if(ss<1)ss=1; int sl=last-nl+1; if(sl<1)sl=1;
  double vs=0.0, ms=0.0; for(int i=ss;i<=last;++i){ double r=sc.Close[i]-sc.Close[i-1]; ms+=r; } ms/=ns;
  for(int i=ss;i<=last;++i){ double r=sc.Close[i]-sc.Close[i-1]-ms; vs+=r*r; } vs/=ns;
  double vl=0.0, ml=0.0; for(int i=sl;i<=last;++i){ double r=sc.Close[i]-sc.Close[i-1]; ml+=r; } ml/=nl;
  for(int i=sl;i<=last;++i){ double r=sc.Close[i]-sc.Close[i-1]-ml; vl+=r*r; } vl/=nl;
  double Result = (vl>0.0)? (vs/vl - 1.0) : 0.0;


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }