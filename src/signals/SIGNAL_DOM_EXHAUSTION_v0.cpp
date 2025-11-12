#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_DOM_EXHAUSTION_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "DOM Exhaustion v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "DOM Exhaustion v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Vol_Drop = sc.Input[0]; In_01_Vol_Drop.Name = "01. Vol Drop"; In_01_Vol_Drop.SetFloat(0.3); // Baisse volume vs EMA
SCInputRef In_02_Depth_Thr = sc.Input[1]; In_02_Depth_Thr.Name = "02. Depth Thr"; In_02_Depth_Thr.SetFloat(200.0); // Seuil profondeur L1

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Baisse de profondeur + baisse de volume récent
static double emaV=0.0; double alpha=0.2;
double v = sc.Volume[last];
if (sc.UpdateStartIndex==0) emaV = v;
emaV = alpha*v + (1.0-alpha)*emaV;
s_MarketDepthEntry mdB, mdA; double sum=0.0;
if (sc.GetBidMarketDepthEntryAtLevel(mdB,0)) sum += mdB.Quantity;
if (sc.GetAskMarketDepthEntryAtLevel(mdA,0)) sum += mdA.Quantity;
double Result = (v<emaV*(1.0-In_01_Vol_Drop.GetFloat())) && (sum<In_02_Depth_Thr.GetFloat()) ? 1.0 : 0.0;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }