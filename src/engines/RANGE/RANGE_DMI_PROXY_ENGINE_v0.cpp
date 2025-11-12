// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #10)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_RANGE_DMI_PROXY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  double& hiPrev=sc.GetPersistentDouble(1); double& loPrev=sc.GetPersistentDouble(2);
  if(sc.SetDefaults){
    sc.GraphName="RANGE_DMI_PROXY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=0; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Période"; sc.Input[0].SetInt(14);
    sc.DrawZeros=false; return;
  }
  if(sc.ArraySize<2) return; int idx=sc.ArraySize-1;
  if(sc.IsFullRecalculation){ hiPrev=sc.High[idx-1]; loPrev=sc.Low[idx-1]; }
  double up= sc.High[idx]-hiPrev; double dn= loPrev - sc.Low[idx];
  if(up<0) up=0; if(dn<0) dn=0;
  hiPrev=sc.High[idx]; loPrev=sc.Low[idx];
  sc.Subgraph[1][idx]=up; sc.Subgraph[2][idx]=dn;
}
