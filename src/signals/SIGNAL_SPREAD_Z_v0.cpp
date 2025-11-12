#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_SPREAD_Z_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Spread Z-Score v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Spread Z-Score v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Alpha = sc.Input[0]; In_01_Alpha.Name = "01. Alpha"; In_01_Alpha.SetFloat(0.2); // EMA

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  s_MarketDepthEntry b,a; double bb=sc.Close[last], aa=sc.Close[last];
  if(sc.GetBidMarketDepthEntryAtLevel(b,0)) bb=b.Price;
  if(sc.GetAskMarketDepthEntryAtLevel(a,0)) aa=a.Price;
  static double m=0.0, s2=0.0; static int k=0; double x=aa-bb; ++k; double d=x-m; m+=d/k; s2+=d*(x-m);
  double sd=(k>1)? sqrt(s2/(k-1)):0.0;
  double Result = (sd>0.0)? (x-m)/sd : 0.0;


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }