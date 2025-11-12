#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_KALMAN_INNOV_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Kalman Innovation v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Kalman Innovation v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Q = sc.Input[0]; In_01_Q.Name = "01. Q"; In_01_Q.SetFloat(1e-05); // Bruit de processus
SCInputRef In_02_R = sc.Input[1]; In_02_R.Name = "02. R"; In_02_R.SetFloat(0.001); // Bruit de mesure

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Kalman 1D: état = prix, innovation = observation - estimate
double q = In_01_Q.GetFloat(); if (q<=0.0) q=1e-5;
double r = In_02_R.GetFloat(); if (r<=0.0) r=1e-3;
static double x=0.0, P=1.0;
if (sc.UpdateStartIndex==0){ x=sc.Close[0]; P=1.0; }
// Predict
double x_pred = x; double P_pred = P + q;
// Update
double z = sc.Close[last];
double K = P_pred / (P_pred + r);
x = x_pred + K*(z - x_pred);
P = (1.0 - K)*P_pred;
double innovation = z - x;
double Result = innovation / (sqrt(P + 1e-12));


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }