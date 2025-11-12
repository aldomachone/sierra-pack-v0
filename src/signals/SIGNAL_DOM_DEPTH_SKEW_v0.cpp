#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_DOM_DEPTH_SKEW_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "DOM Depth Skew v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "DOM Depth Skew v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

// Aucun input

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  s_MarketDepthEntry b,a; double qb=0.0, qa=0.0;
  if (sc.GetBidMarketDepthEntryAtLevel(b,0)) qb=b.Quantity;
  if (sc.GetAskMarketDepthEntryAtLevel(a,0)) qa=a.Quantity;
  double sum = qb+qa;
  double Result = (sum>0.0)? fabs(qb-qa)/sum : 0.0;


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }