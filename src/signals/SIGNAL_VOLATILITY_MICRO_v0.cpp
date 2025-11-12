#include "sierrachart.h"
#include "SignalsPack_v0.h"
SCDLLName("PACK_SIGNALS_V0")

SCSFExport scsf_SIGNAL_VOLATILITY_MICRO_v0(SCStudyInterfaceRef sc)
{
  SCSubgraphRef SG_Marker = sc.Subgraph[0];

  if (sc.SetDefaults)
  {
    sc.GraphName = "Volatilité Micro v0";
    sc.AutoLoop = 0;
    sc.UpdateAlways = 1;
    sc.GraphRegion = 0;
    sc.ValueFormat = 26;
    sc.FreeDLL = 0;

    spv0::InitMarker(SG_Marker, "Volatilité Micro v0");
    spv0::HideAll(sc);
    return;
  }

SCInputRef In_01_Fenetre = sc.Input[0]; In_01_Fenetre.Name = "01. Fenetre"; In_01_Fenetre.SetInt(32); // Fenêtre volatilité réalisée

  const int last = sc.ArraySize-1;
  if (last < 2) return;


  int w = (int)In_01_Fenetre.GetInt();
  if (w <= 1) w = 32;
  int start = max(1, last-w+1);
  double rv=0.0;
  for (int i=start; i<=last; ++i) {{ double r=sc.Close[i]-sc.Close[i-1]; rv += r*r; }}
  rv /= (double)(w);
  double Result = sqrt(rv);


  for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i)
    SG_Marker[i] = 0.0;

  double amp = fabs(Result);
  double size = amp * 10.0;
  if (size > 10.0) size = 10.0;
  SG_Marker[last] = (float)size;
}