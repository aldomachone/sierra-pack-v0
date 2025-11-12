#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0_NEW")

  SCSFExport scsf_SIGNAL_CONFLUENCE_VWAP_PIVOT_RSI_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];
    if (sc.SetDefaults)
    {
      sc.GraphName = "Confluence VWAP+Pivot+RSI v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "Confluence VWAP+Pivot+RSI v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_N = sc.Input[0]; In_01_N.Name = "01. N"; In_01_N.SetInt(96); // Fenêtre
SCInputRef In_02_RSI_N = sc.Input[1]; In_02_RSI_N.Name = "02. RSI_N"; In_02_RSI_N.SetInt(14); // RSI
SCInputRef In_03_TickSize = sc.Input[2]; In_03_TickSize.Name = "03. TickSize"; In_03_TickSize.SetFloat(1.0); // Tick

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


  int n=In_01_N.GetInt(); int rn=In_02_RSI_N.GetInt();
  int s=last-n+1; if(s<1)s=1;
  double num=0,den=0; for(int i=s;i<=last;++i){ double p=(sc.High[i]+sc.Low[i]+sc.Close[i])/3.0; double v=sc.Volume[i]; num+=p*v; den+=v; }
  double vwap=(den>0.0)? num/den:sc.Close[last];
  double H=-1e9,L=1e9,C=sc.Close[s];
  for(int i=s;i<=last;++i){ if(sc.High[i]>H)H=sc.High[i]; if(sc.Low[i]<L)L=sc.Low[i]; C=sc.Close[i]; }
  double P=(H+L+C)/3.0;
  double up=0.0, dn=0.0; for(int i=last-rn+1;i<=last;++i){ int j=(i<1)?1:i; double d=sc.Close[j]-sc.Close[j-1]; if(d>=0)up+=d; else dn-=d; }
  double rs=(dn>0.0)? up/dn:0.0; double rsi=(dn>0.0)? 100.0 - 100.0/(1.0+rs) : 100.0;
  double dv=fabs(sc.Close[last]-vwap), dp=fabs(sc.Close[last]-P);
  double rsiScore = (rsi<30.0||rsi>70.0)? 1.0:0.0;
  double Result = 1.0 / (1.0 + dv + dp) * (1.0 + rsiScore);


    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }