// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #6)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_OFI_CUM_ENGINE_v0(SCStudyInterfaceRef sc)
{
  double& pb=sc.GetPersistentDouble(1); double& pa=sc.GetPersistentDouble(2); static double cum=0;
  if(sc.SetDefaults){
    sc.GraphName="OFI_CUM_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Near niveaux"; sc.Input[0].SetInt(10);
    sc.Input[1].Name="02. Reset quotidien"; sc.Input[1].SetYesNo(0);
    sc.DrawZeros=false; return;
  }
  if(sc.IsFullRecalculation || (sc.Input[1].GetYesNo() && sc.Index==0)) cum=0;
  s_MarketDepthEntry md{}; int N=sc.Input[0].GetInt(); double b=0,aQ=0;
  for(int i=0;i<sc.GetBidMarketDepthNumberOfLevels() && i<N;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); aQ+=md.Quantity; }
  if(sc.Index==0){ pb=b; pa=aQ; }
  double ofi=(b-pb)-(aQ-pa); pb=b; pa=aQ; cum+=ofi;
  sc.Subgraph[1][sc.ArraySize-1]=cum;
}
