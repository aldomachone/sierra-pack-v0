#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_SPECTRAL_ENTROPY_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Spectral Entropy v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Spectral Entropy v0";
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


  int n=In_01_N.GetInt(); if(n<8)n=64; int s=last-n+1; if(s<1)s=1;
  // Proxy: histogramme d'amplitude sur 4 bandes de différences
  double e1=0,e2=0,e3=0,e4=0;
  for(int i=s;i<=last;++i){ double d=sc.Close[i]-sc.Close[i-1]; double a=fabs(d);
    if(a<0.25) e1+=a; else if(a<0.5) e2+=a; else if(a<1.0) e3+=a; else e4+=a; }
  double E=e1+e2+e3+e4; double H=0.0;
  if(E>0.0){ double p1=e1/E, p2=e2/E, p3=e3/E, p4=e4/E;
    auto L=[](double p){ return (p<=0.0)?0.0: -p*log(p)/log(2.0); };
    H = L(p1)+L(p2)+L(p3)+L(p4); }
  double Result = H/2.0; // normaliser 0..1 approx


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }