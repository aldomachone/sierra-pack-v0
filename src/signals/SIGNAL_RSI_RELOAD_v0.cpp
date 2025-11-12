#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_RSI_RELOAD_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "RSI Reload v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "RSI Reload v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(14); // Fenêtre RSI
SCInputRef In_02_RSI_Low = sc.Input[1]; In_02_RSI_Low.Name = "02. RSI Low"; In_02_RSI_Low.SetFloat(30.0); // Seuil bas
SCInputRef In_03_RSI_High = sc.Input[2]; In_03_RSI_High.Name = "03. RSI High"; In_03_RSI_High.SetFloat(70.0); // Seuil haut

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


int n=In_01_N.GetInt(); if (n<2) n=14;
double up=0.0, dn=0.0;
for(int i=last-n+1;i<=last;++i){ int j=(i<1)?1:i; double d=sc.Close[j]-sc.Close[j-1]; if(d>=0) up+=d; else dn-=d; }
double rs = (dn>0.0)? up/dn : 0.0;
double rsi = (dn>0.0)? 100.0 - 100.0/(1.0+rs) : 100.0;
double Result = (rsi<In_02_RSI_Low.GetFloat())? 1.0 : ( (rsi>In_03_RSI_High.GetFloat())? -1.0 : 0.0 );


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }