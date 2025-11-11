#include "sierrachart.h"
#include "SignalsPack_v0.h"
SCDLLName("PACK_SIGNALS_V0")

SCSFExport scsf_SIGNAL_CHANGE_POINT_v0(SCStudyInterfaceRef sc)
{
  SCSubgraphRef SG_Marker = sc.Subgraph[0];

  if (sc.SetDefaults)
  {
    sc.GraphName = "Change-Point CUSUM v0";
    sc.AutoLoop = 0;
    sc.UpdateAlways = 1;
    sc.GraphRegion = 0;
    sc.ValueFormat = 26;
    sc.FreeDLL = 0;

    spv0::InitMarker(SG_Marker, "Change-Point CUSUM v0");
    spv0::HideAll(sc);
    return;
  }

SCInputRef In_01_K = sc.Input[0]; In_01_K.Name = "01. K"; In_01_K.SetFloat(0.1); // Seuil drift k
SCInputRef In_02_H = sc.Input[1]; In_02_H.Name = "02. H"; In_02_H.SetInt(2); // Seuil alerte h

  const int last = sc.ArraySize-1;
  if (last < 2) return;


  double k = In_01_K.GetFloat();   // seuil drift
  double h = In_02_H.GetFloat();   // seuil alerte
  if (k <= 0.0) k = 0.1;
  if (h <= 0.0) h = 2.0;
  static double gpos = 0.0, gneg = 0.0;
  double x = (double)sc.Close[last] - (double)sc.Close[last-1];
  gpos = fmax(0.0, gpos + x - k);
  gneg = fmax(0.0, gneg - x - k);
  double Result = 0.0;
  if (gpos > h) {{ Result = 1.0; gpos = 0.0; gneg = 0.0; }}
  else if (gneg > h) {{ Result = -1.0; gpos = 0.0; gneg = 0.0; }}


  for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i)
    SG_Marker[i] = 0.0;

  double amp = fabs(Result);
  double size = amp * 10.0;
  if (size > 10.0) size = 10.0;
  SG_Marker[last] = (float)size;
}