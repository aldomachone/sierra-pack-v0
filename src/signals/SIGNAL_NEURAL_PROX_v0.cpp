#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_NEURAL_PROX_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Neural Prox v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Neural Prox v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(64); // Fenêtre z
SCInputRef In_02_w = sc.Input[1]; In_02_w.Name = "02. w"; In_02_w.SetFloat(1.0); // Poids
SCInputRef In_03_b = sc.Input[2]; In_03_b.Name = "03. b"; In_03_b.SetFloat(0.0); // Biais

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// tanh(w * zscore(price) + b)
int n=In_01_N.GetInt(); if (n<2) n=64;
double w=In_02_w.GetFloat(); double b=In_03_b.GetFloat();
int s=last-n+1; if (s<0) s=0; int N=last-s+1;
double mean=0.0; for(int i=s;i<=last;++i) mean+=sc.Close[i]; mean/=N;
double var=0.0; for(int i=s;i<=last;++i){ double d=sc.Close[i]-mean; var+=d*d; } var/=N;
double sd=(var>0.0)? sqrt(var):1.0;
double z=(sc.Close[last]-mean)/sd;
double Result = tanh(w*z + b);


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }