#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_VARIANCE_RATIO_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "Variance Ratio v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Variance Ratio v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_q = sc.Input[0]; In_01_q.Name = "01. q"; In_01_q.SetInt(4); // Agrégation
SCInputRef In_02_N = sc.Input[1]; In_02_N.Name = "02. N"; In_02_N.SetInt(64); // Fenêtre totale

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


int q=In_01_q.GetInt(); if (q<2) q=4;
int n=In_02_N.GetInt(); if (n<q+1) n=q+1;
int s=last-n+1; if (s<q) s=q;
double num=0.0,den=0.0;
for(int i=s;i<=last;++i){ double r=sc.Close[i]-sc.Close[i-1]; den += r*r; }
for(int i=s;i<=last;i+=q){ double rq=sc.Close[i]-sc.Close[i-q]; num += rq*rq; }
double VR = (q*den>0.0)? num/(q*den) : 0.0;
double Result = VR - 1.0;


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }