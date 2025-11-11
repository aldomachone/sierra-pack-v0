#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_TAPE_OFI_CLASSIC_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Tape OFI classic proxy v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Tape OFI classic proxy v0";
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


// OFI ~ ΔBestBidSize - ΔBestAskSize ; proxy avec volume directionnel par prix
double d = (double)sc.Close[last]- (double)sc.Close[last-1];
double Result = (d>=0)? sc.Volume[last] : - (double)sc.Volume[last];
// Normaliser
static double emaV=0.0; double a=0.2; if (sc.UpdateStartIndex==0) emaV=sc.Volume[0];
emaV = a*sc.Volume[last] + (1.0-a)*emaV;
if (emaV>0.0) Result /= emaV;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }