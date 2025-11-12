#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_GARCH11_VOL_EST_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "GARCH(1,1) σ v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "GARCH(1,1) σ v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_omega = sc.Input[0]; In_01_omega.Name = "01. omega"; In_01_omega.SetFloat(1e-06); // Constante
SCInputRef In_02_alpha = sc.Input[1]; In_02_alpha.Name = "02. alpha"; In_02_alpha.SetFloat(0.05); // Poids choc
SCInputRef In_03_beta = sc.Input[2]; In_03_beta.Name = "03. beta"; In_03_beta.SetFloat(0.9); // Persistance

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


double omega=In_01_omega.GetFloat();
double alpha=In_02_alpha.GetFloat();
double beta =In_03_beta.GetFloat();
static double s2=0.0;
double r = (double)sc.Close[last]- (double)sc.Close[last-1];
if (s2<=0.0) s2 = r*r;
s2 = omega + alpha*(r*r) + beta*s2;
double Result = sqrt(fabs(s2));


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }