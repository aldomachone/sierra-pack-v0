// ============================================================================
// Pack v0 — Engines (v0) — Compléments 3
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_MDHG_STICKY_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited=sc.GetPersistentInt(1);
  double& stick=sc.GetPersistentDouble(2);

  if(sc.SetDefaults){
    sc.GraphName="MDHG_STICKY_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0; sc.UsesMarketDepthData=1;
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
    sc.Input[0].Name="01. Near niveaux"; sc.Input[0].SetInt(10); sc.Input[0].SetIntLimits(1,60);
    sc.Input[1].Name="02. Seuil ratio sticky %"; sc.Input[1].SetFloat(80.0f);
    sc.DrawZeros=false; return;
  }

  if(!inited||sc.IsFullRecalculation){ inited=1; stick=0; }
  if(sc.TickSize<=0) return;

  int N=sc.Input[0].GetInt();
  s_MarketDepthEntry md{}; double b=0,aQ=0;
  int nb=sc.GetBidMarketDepthNumberOfLevels(), na=sc.GetAskMarketDepthNumberOfLevels();
  for(int i=0;i<nb && i<N;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); b+=md.Quantity; }
  for(int i=0;i<na && i<N;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); aQ+=md.Quantity; }
  static double bPrev=0,aPrev=0;
  double stable = 0.0;
  double denom = (b+aQ>0? (b+aQ):1.0);
  if (fabs(b-bPrev)/denom < 0.05 && fabs(aQ-aPrev)/denom < 0.05) stable = 1.0;
  bPrev=b; aPrev=aQ;

  double thr=sc.Input[1].GetFloat()/100.0;
  stick = (stable>=thr? 1.0 : stable); // proxy
  sc.Subgraph[1][sc.ArraySize-1]=stick;
}
