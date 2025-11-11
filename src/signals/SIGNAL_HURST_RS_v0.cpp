#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_HURST_RS_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Hurst R/S v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Hurst R/S v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(128); // Fenêtre

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


int n=In_01_N.GetInt(); if (n<8) n=128;
int s = last-n+1; if (s<0) s=0;
int N = last - s + 1;
double mean=0.0; for(int i=s;i<=last;++i) mean += sc.Close[i]; mean/=N;
double cum=0.0, maxV=-1e9, minV=1e9, var=0.0;
for(int i=s;i<=last;++i){ double d=sc.Close[i]-mean; cum += d; if (cum>maxV) maxV=cum; if (cum<minV) minV=cum; var += d*d; }
double R = maxV - minV;
double S = (N>1)? sqrt(var/(N-1)) : 0.0;
double RS = (S>0.0)? R/S : 0.0;
double Result = (RS>0.0)? log(RS)/log((double)N) : 0.0; // H approx


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }