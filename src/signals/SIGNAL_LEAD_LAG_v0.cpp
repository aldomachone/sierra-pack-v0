#include "sierrachart.h"
#include "SignalsPack_v0.h"
SCDLLName("PACK_SIGNALS_V0")

SCSFExport scsf_SIGNAL_LEAD_LAG_v0(SCStudyInterfaceRef sc)
{
  SCSubgraphRef SG_Marker = sc.Subgraph[0];

  if (sc.SetDefaults)
  {
    sc.GraphName = "Lead-Lag Proxy v0";
    sc.AutoLoop = 0;
    sc.UpdateAlways = 1;
    sc.GraphRegion = 0;
    sc.ValueFormat = 26;
    sc.FreeDLL = 0;

    spv0::InitMarker(SG_Marker, "Lead-Lag Proxy v0");
    spv0::HideAll(sc);
    return;
  }

SCInputRef In_01_Fenetre = sc.Input[0]; In_01_Fenetre.Name = "01. Fenetre"; In_01_Fenetre.SetInt(64); // Fenêtre corrélation lag-1

  const int last = sc.ArraySize-1;
  if (last < 2) return;


  int w = (int)In_01_Fenetre.GetInt();
  if (w <= 2) w = 64;
  int start = max(1, last-w+1);
  double num=0.0, d1=0.0, d2=0.0;
  for (int i=start+1; i<=last; ++i) {{
    double r0 = sc.Close[i]-sc.Close[i-1];
    double r1 = sc.Close[i-1]-sc.Close[i-2];
    num += r0*r1;
    d1 += r0*r0;
    d2 += r1*r1;
  }}
  double denom = sqrt(d1*d2);
  double Result = (denom>0.0)? num/denom : 0.0; // approx corr lag-1


  for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i)
    SG_Marker[i] = 0.0;

  double amp = fabs(Result);
  double size = amp * 10.0;
  if (size > 10.0) size = 10.0;
  SG_Marker[last] = (float)size;
}