// ============================================================================
// Pack v0 — Engines v0 (auto-generated)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_FVG_PERSIST_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="FVG_PERSIST_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Subgraph[5].Name = "SG05";
    sc.Subgraph[5].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[5].DrawZeros = false;
    sc.Subgraph[5].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[6].Name = "SG06";
    sc.Subgraph[6].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[6].DrawZeros = false;
    sc.Subgraph[6].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[7].Name = "SG07";
    sc.Subgraph[7].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[7].DrawZeros = false;
    sc.Subgraph[7].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[8].Name = "SG08";
    sc.Subgraph[8].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[8].DrawZeros = false;
    sc.Subgraph[8].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Min gap ticks"; sc.Input[0].SetFloat(1.0f);
    sc.Input[1].Name="02. Max âge barres"; sc.Input[1].SetInt(50); sc.Input[1].SetIntLimits(1,100000);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<3) return; int idx=sc.ArraySize-1;
  double k=sc.Input[0].GetFloat();
  double up = sc.Low[idx-1] - sc.High[idx-2];
  double dn = sc.Low[idx-2] - sc.High[idx-1];
  double upOpen = (up/sc.TickSize >= k ? 1.0 : 0.0);
  double dnOpen = ((-dn)/sc.TickSize >= k ? 1.0 : 0.0);
  // age counters simple
  static int ageUp=0, ageDn=0;
  if(upOpen>0) ageUp=0; else if(ageUp<1000000) ++ageUp;
  if(dnOpen>0) ageDn=0; else if(ageDn<1000000) ++ageDn;
  sc.Subgraph[1][idx]=upOpen; sc.Subgraph[2][idx]=dnOpen; sc.Subgraph[3][idx]=ageUp; sc.Subgraph[4][idx]=ageDn;
}
