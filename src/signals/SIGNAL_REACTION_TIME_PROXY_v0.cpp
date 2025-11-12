#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_REACTION_TIME_PROXY_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Reaction Time proxy v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Reaction Time proxy v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(16); // Fenêtre max lag

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Temps de réaction proxy: différence entre direction volume et prix
int n=In_01_N.GetInt(); if (n<2) n=16;
int s=last-n+1; if (s<1) s=1;
int lag=0;
for(int L=1; L<=n && last-L>=1; ++L){ 
  double r = sc.Close[last]-sc.Close[last-L];
  double v = 0.0; for(int i=s;i<=last;++i){ v += (double)sc.Volume[i]; }
  if ((r>=0 && v>=0) || (r<=0 && v<=0)) {{ lag=L; break; }}
}
double Result = (double)lag/(double)n; // 0..1


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }