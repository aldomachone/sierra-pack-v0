// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats #12)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_VARIANCE_RATIO_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="VARIANCE_RATIO_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Lag q (barres)"; sc.Input[0].SetInt(4);
    sc.Input[1].Name="02. Fenêtre bars"; sc.Input[1].SetInt(200);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int q=sc.Input[0].GetInt(); int W=sc.Input[1].GetInt();
  if(q<2) q=2; int s=(idx-W>q? idx-W: q);
  // 1-step variance
  double m1=0; int n1=0; for(int i=s;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; m1+=r; ++n1; } if(n1<2) return; m1/=n1;
  double v1=0; for(int i=s;i<=idx;++i){ double r=sc.Close[i]-sc.Close[i-1]; double d=r-m1; v1+=d*d; }
  v1/=(n1-1);
  // q-step variance
  double mq=0; int nq=0; for(int i=s;i<=idx; ++i){ if(i-q<0) continue; double rq=sc.Close[i]-sc.Close[i-q]; mq+=rq; ++nq; }
  if(nq<2) return; mq/=nq;
  double vq=0; for(int i=s;i<=idx; ++i){ if(i-q<0) continue; double rq=sc.Close[i]-sc.Close[i-q]; double d=rq-mq; vq+=d*d; }
  vq/=(nq-1);
  double VR = (q>0 && v1>0? vq/(q*v1) : 0.0);
  sc.Subgraph[1][idx]=VR;
}
