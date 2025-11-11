#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_AUTOREG_AR1_RESID_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "AR(1) Residual z v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "AR(1) Residual z v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(64); // Fenêtre OLS

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


int n=In_01_N.GetInt(); if (n<10) n=64;
int s=last-n+1; if (s<1) s=1; int N=last-s+1;
// Estimation OLS AR(1): x_t = a + b x_{t-1} + e_t
double sumx=0.0,sumy=0.0,sumxx=0.0,sumxy=0.0; 
for(int i=s;i<=last;++i){ double y=sc.Close[i]; double x=sc.Close[i-1]; sumx+=x; sumy+=y; sumxx+=x*x; sumxy+=x*y; }
double denom = N*sumxx - sumx*sumx;
double b = (denom!=0.0)? (N*sumxy - sumx*sumy)/denom : 0.0;
double a = (N>0)? (sumy - b*sumx)/N : 0.0;
double yhat = a + b*sc.Close[last-1];
double e = sc.Close[last] - yhat;
// Normaliser par sd des résidus
static double m=0.0,v=0.0; static int k=0; ++k; double de=e-m; m+=de/k; v+=de*(e-m);
double sd = (k>1)? sqrt(v/(k-1)):0.0;
double Result = (sd>0.0)? e/sd : 0.0;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }