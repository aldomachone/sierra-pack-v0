#include "sierrachart.h"
#include "SignalsPack_v0.h"
SCDLLName("PACK_SIGNALS_V0")

SCSFExport scsf_SIGNAL_ARBITRAGE_STAT_v0(SCStudyInterfaceRef sc)
{
  SCSubgraphRef SG_Marker = sc.Subgraph[0];

  if (sc.SetDefaults)
  {
    sc.GraphName = "StatArb Divergence v0";
    sc.AutoLoop = 0;
    sc.UpdateAlways = 1;
    sc.GraphRegion = 0;
    sc.ValueFormat = 26;
    sc.FreeDLL = 0;

    spv0::InitMarker(SG_Marker, "StatArb Divergence v0");
    spv0::HideAll(sc);
    return;
  }

SCInputRef In_01_Period_EMA = sc.Input[0]; In_01_Period_EMA.Name = "01. Period EMA"; In_01_Period_EMA.SetInt(50); // Période EMA
SCInputRef In_02_Period_Std = sc.Input[1]; In_02_Period_Std.Name = "02. Period Std"; In_02_Period_Std.SetInt(100); // Période écart-type

  const int last = sc.ArraySize-1;
  if (last < 2) return;


  int n = (int)In_01_Period.GetInt();
  if (n <= 1) n = 50;
  double alpha = 2.0 / (n + 1.0);
  static double ema = 0.0;
  if (sc.UpdateStartIndex == 0) ema = sc.Close[0];
  for (int i = sc.UpdateStartIndex; i <= last; ++i)
    ema = alpha*sc.Close[i] + (1.0-alpha)*ema;

  // Std dev simple
  int m = (int)In_02_Std_Period.GetInt();
  if (m <= 1) m = 100;
  double mean = 0.0;
  for (int i = max(0,last-m+1); i <= last; ++i) mean += sc.Close[i];
  mean /= m;
  double var = 0.0;
  for (int i = max(0,last-m+1); i <= last; ++i) {{ double d=sc.Close[i]-mean; var += d*d; }}
  var /= m;
  double sd = sqrt(var);
  double Result = (sd>0.0) ? (sc.Close[last] - ema)/sd : 0.0;


  for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i)
    SG_Marker[i] = 0.0;

  double amp = fabs(Result);
  double size = amp * 10.0;
  if (size > 10.0) size = 10.0;
  SG_Marker[last] = (float)size;
}