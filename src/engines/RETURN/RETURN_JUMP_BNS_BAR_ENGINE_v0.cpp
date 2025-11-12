// ============================================================================
// Pack v0 — Engines v0 (lot Extra-HFT #15)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_RETURN_JUMP_BNS_BAR_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="RETURN_JUMP_BNS_BAR_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre bars"; sc.Input[0].SetInt(60);
    sc.Input[1].Name="02. Seuil Z"; sc.Input[1].SetFloat(2.5f);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1; int W=sc.Input[0].GetInt(); int s=(idx-W>1? idx-W:1); double Zt=0;
  double RV=0,BP=0; for(int i=s;i<=idx; ++i){ double r=sc.Close[i]-sc.Close[i-1]; RV+=r*r; if(i>1) BP += fabs(sc.Close[i]-sc.Close[i-1])*fabs(sc.Close[i-1]-sc.Close[i-2]); }
  double mu1 = sqrt(2.0/3.141592653589793); double JV = RV - (mu1*mu1)*BP; double varJV = (RV>0? RV/W : 0.0);
  Zt = (varJV>0? JV/sqrt(varJV):0.0);
  sc.Subgraph[1][idx] = (fabs(Zt)>=sc.Input[1].GetFloat()? 1.0:0.0);
}
