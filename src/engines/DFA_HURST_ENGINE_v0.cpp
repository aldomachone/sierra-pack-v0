// ============================================================================
// Pack v0 — Engines v0 (lot Extra-HFT #15)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DFA_HURST_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DFA_HURST_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";
    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[1].DrawZeros = false;
    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[2].Name = "SG02";
    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[2].DrawZeros = false;
    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[3].Name = "SG03";
    sc.Subgraph[3].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[3].DrawZeros = false;
    sc.Subgraph[3].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[4].Name = "SG04";
    sc.Subgraph[4].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[4].DrawZeros = false;
    sc.Subgraph[4].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(300);
    sc.Input[1].Name="02. Segment m"; sc.Input[1].SetInt(20);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<5) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int m=sc.Input[1].GetInt();
  int s=(idx-W>0? idx-W:0); int n=idx-s+1; if(n<=m*2) return;
  static double X[8192]; int N=0; double mean=0; for(int i=s;i<=idx;++i){ mean+=sc.Close[i]; } mean/=n;
  double cum=0; for(int i=s;i<=idx && N<8192;++i){ cum += sc.Close[i]-mean; X[N++]=cum; }
  int segs = N/m; if(segs<2) return; double F=0; int cnt=0;
  for(int k=0;k<segs; ++k){ int st=k*m; int en=st+m-1; double x=0,y=0,xx=0,xy=0; int c=0;
    for(int t=st;t<=en; ++t){ double tt=t-st; x+=tt; y+=X[t]; xx+=tt*tt; xy+=tt*X[t]; ++c; }
    double den=c*xx-x*x; double a=(den!=0? (c*xy-x*y)/den:0.0); double b=(y-a*x)/c;
    double err=0; for(int t=st;t<=en; ++t){ double tt=t-st; double fit=a*tt+b; double e=X[t]-fit; err+=e*e; }
    F += err/c; ++cnt;
  }
  double Fm = (cnt>0? sqrt(F/cnt):0.0);
  double H = (Fm>0 && m>1? log(Fm)/log((double)m) : 0.0);
  sc.Subgraph[1][idx]=H;
}
