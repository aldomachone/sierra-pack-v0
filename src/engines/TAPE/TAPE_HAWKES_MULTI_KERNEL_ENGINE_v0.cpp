// ============================================================================
// Pack v0 — Engines v0 (TAPE_PRO_v1)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TAPE_HAWKES_MULTI_KERNEL_ENGINE_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TAPE_HAWKES_MULTI_KERNEL_ENGINE_v1"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1;
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
    sc.Input[0].Name="01. Bin ms"; sc.Input[0].SetInt(200);
    sc.Input[1].Name="02. a1"; sc.Input[1].SetFloat(0.30f);
    sc.Input[2].Name="03. a2"; sc.Input[2].SetFloat(0.15f);
    sc.Input[3].Name="04. a3"; sc.Input[3].SetFloat(0.05f);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<1) return;
  double now=ts[ts.Size()-1].DateTime; double t0=now - sc.Input[0].GetInt()/86400000.0; int cnt=0;
  for(int i=ts.Size()-1;i>=0;--i){ const auto& e=ts[i]; if(e.DateTime<t0) break; if(e.Type==SC_TS_TRADES) ++cnt; }
  static double l1=0,l2=0,l3=0; l1=(1.0-sc.Input[1].GetFloat())*l1 + sc.Input[1].GetFloat()*cnt;
  l2=(1.0-sc.Input[2].GetFloat())*l2 + sc.Input[2].GetFloat()*cnt;
  l3=(1.0-sc.Input[3].GetFloat())*l3 + sc.Input[3].GetFloat()*cnt;
  int I=sc.ArraySize-1; sc.Subgraph[1][I]=l1; sc.Subgraph[2][I]=l2; sc.Subgraph[3][I]=l3; sc.Subgraph[4][I]=l1-l3;
}
