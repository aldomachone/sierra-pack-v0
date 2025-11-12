#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_REGIME_LIQUIDITY_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Regime Liquidity v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Regime Liquidity v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_DepthThr = sc.Input[0]; In_01_DepthThr.Name = "01. DepthThr"; In_01_DepthThr.SetFloat(100.0); // Seuil L1

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  s_MarketDepthEntry b,a; double q=0.0; if(sc.GetBidMarketDepthEntryAtLevel(b,0)) q+=b.Quantity; if(sc.GetAskMarketDepthEntryAtLevel(a,0)) q+=a.Quantity;
  double Result = (q<In_01_DepthThr.GetFloat())? -1.0 : 1.0; // -1 low liq, +1 normal


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }