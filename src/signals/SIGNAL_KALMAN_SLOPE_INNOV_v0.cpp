#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_KALMAN_SLOPE_INNOV_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Kalman Slope Innov v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Kalman Slope Innov v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(32); // Fenêtre EMA

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Kalman 1D sur slope d'une EMA
int n=In_01_N.GetInt(); if (n<2) n=32;
double a=2.0/(n+1.0);
static double ema=0.0; if (sc.UpdateStartIndex==0) ema=sc.Close[0];
ema = a*sc.Close[last] + (1.0-a)*ema;
static double x=0.0,P=1.0; double q=1e-5,r=1e-3;
double z = ema - sc.Close[last-1];
double x_pred = x; double P_pred=P+q;
double K = P_pred/(P_pred+r);
x = x_pred + K*(z-x_pred); P=(1.0-K)*P_pred;
double Result = (z - x)/sqrt(P+1e-12);


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }