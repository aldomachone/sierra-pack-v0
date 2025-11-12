#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_PIVOT_TOUCH_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Pivot Touch v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Pivot Touch v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(96); // Fenêtre pour Pivots
SCInputRef In_02_TickSize = sc.Input[1]; In_02_TickSize.Name = "02. TickSize"; In_02_TickSize.SetFloat(1.0); // Taille tick
SCInputRef In_03_MaxTicks = sc.Input[2]; In_03_MaxTicks.Name = "03. MaxTicks"; In_03_MaxTicks.SetInt(3); // Tolérance

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


int n=In_01_N.GetInt(); if (n<2) n=96;
int s=last-n; if (s<0) s=0;
double H=-1e9,L=1e9,C=sc.Close[s];
for(int i=s;i<last;++i){ if (sc.High[i]>H) H=sc.High[i]; if (sc.Low[i]<L) L=sc.Low[i]; C=sc.Close[i]; }
double P = (H+L+C)/3.0;
double S1 = 2*P - H;
double R1 = 2*P - L;
double d = fmin(fabs(sc.Close[last]-S1), fabs(sc.Close[last]-R1));
double tick = In_02_TickSize.GetFloat(); if (tick<=0.0) tick=1.0;
double Result = (d<=In_03_MaxTicks.GetInt()*tick)? 1.0 : 0.0;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }