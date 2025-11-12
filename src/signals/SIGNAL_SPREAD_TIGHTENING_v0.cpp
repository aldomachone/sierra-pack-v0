#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_SPREAD_TIGHTENING_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Spread Tightening v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Spread Tightening v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Alpha = sc.Input[0]; In_01_Alpha.Name = "01. Alpha"; In_01_Alpha.SetFloat(0.3); // EMA

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  s_MarketDepthEntry b,a; double bb=sc.Close[last], aa=sc.Close[last];
  if(sc.GetBidMarketDepthEntryAtLevel(b,0)) bb=b.Price;
  if(sc.GetAskMarketDepthEntryAtLevel(a,0)) aa=a.Price;
  static double ema=0.0; double s=aa-bb; double al=In_01_Alpha.GetFloat(); if(al<=0.0||al>=1.0) al=0.3;
  if(sc.UpdateStartIndex==0) ema=s;
  ema = al*s + (1.0-al)*ema;
  double Result = (s<ema)? (ema - s)/ (ema+1e-9) : 0.0;


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }