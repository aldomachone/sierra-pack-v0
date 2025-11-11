#include "sierrachart.h"
#include "SignalsPack_v0.h"
SCDLLName("PACK_SIGNALS_V0")

SCSFExport scsf_SIGNAL_ORDER_FLOW_MOMENTUM_v0(SCStudyInterfaceRef sc)
{
  SCSubgraphRef SG_Marker = sc.Subgraph[0];

  if (sc.SetDefaults)
  {
    sc.GraphName = "Order-Flow Momentum v0";
    sc.AutoLoop = 0;
    sc.UpdateAlways = 1;
    sc.GraphRegion = 0;
    sc.ValueFormat = 26;
    sc.FreeDLL = 0;

    spv0::InitMarker(SG_Marker, "Order-Flow Momentum v0");
    spv0::HideAll(sc);
    return;
  }

SCInputRef In_01_Nb_Ticks = sc.Input[0]; In_01_Nb_Ticks.Name = "01. Nb Ticks"; In_01_Nb_Ticks.SetInt(8); // Fenêtre momentum en ticks
SCInputRef In_02_ATR_Period = sc.Input[1]; In_02_ATR_Period.Name = "02. ATR Period"; In_02_ATR_Period.SetInt(14); // Fenêtre ATR pour normalisation

  const int last = sc.ArraySize-1;
  if (last < 2) return;


  // Proxy simple: momentum de prix normalisé par ATR court
  int lookback = (int)In_01_Nb_Ticks.GetInt(); // if int supported else float cast
  if (lookback <= 0) lookback = 8;
  int i0 = last - lookback;
  if (i0 < 0) i0 = 0;
  double roc = sc.Close[last] - sc.Close[i0];

  // ATR court
  int natr = (int)In_02_ATR_Period.GetInt();
  if (natr <= 1) natr = 14;
  double atr = 0.0;
  for (int i = max(1, last-natr+1); i <= last; ++i)
    atr += fabs((double)sc.High[i] - (double)sc.Low[i]);
  atr /= natr;
  double Result = (atr > 0.0) ? roc/atr : 0.0;


  for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i)
    SG_Marker[i] = 0.0;

  double amp = fabs(Result);
  double size = amp * 10.0;
  if (size > 10.0) size = 10.0;
  SG_Marker[last] = (float)size;
}