#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_FEATURE_MOMENTUM_MULTI_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Feature Momentum Multi v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Feature Momentum Multi v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N1 = sc.Input[0]; In_01_N1.Name = "01. N1"; In_01_N1.SetInt(8); // fenêtre1
SCInputRef In_02_N2 = sc.Input[1]; In_02_N2.Name = "02. N2"; In_02_N2.SetInt(16); // fenêtre2
SCInputRef In_03_N3 = sc.Input[2]; In_03_N3.Name = "03. N3"; In_03_N3.SetInt(32); // fenêtre3

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  int n1=In_01_N1.GetInt(), n2=In_02_N2.GetInt(), n3=In_03_N3.GetInt();
  if(n1<2)n1=8; if(n2<2)n2=16; if(n3<2)n3=32;
  auto mom=[&](int n){ int i0=last-n; if(i0<0)i0=0; return sc.Close[last]-sc.Close[i0]; };
  double m1=mom(n1), m2=mom(n2), m3=mom(n3);
  double Result = 0.5*m1 + 0.3*m2 + 0.2*m3;


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }