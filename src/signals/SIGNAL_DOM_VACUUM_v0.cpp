#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_DOM_VACUUM_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "DOM Vacuum v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "DOM Vacuum v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Drop = sc.Input[0]; In_01_Drop.Name = "01. Drop"; In_01_Drop.SetFloat(0.25); // Baisse relative de profondeur L1

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Vide soudain: forte baisse de somme L1
s_MarketDepthEntry mdB, mdA;
double b=0.0,a=0.0;
if (sc.GetBidMarketDepthEntryAtLevel(mdB,0)) b = mdB.Quantity;
if (sc.GetAskMarketDepthEntryAtLevel(mdA,0)) a = mdA.Quantity;
static double prev=0.0;
double sum = b+a;
if (sc.UpdateStartIndex==0) prev=sum;
double drop = (prev>0.0)? (prev - sum)/prev : 0.0;
double Result = (drop>In_01_Drop.GetFloat()) ? drop : 0.0;
prev = sum;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }