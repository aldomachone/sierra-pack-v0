// ============================================================================
// Pack v0 — Engines v0 (variants supplémentaires)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_SPOOFING_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited=sc.GetPersistentInt(1); int& phase=sc.GetPersistentInt(2); double& peak=sc.GetPersistentDouble(3);
  if(sc.SetDefaults){
    sc.GraphName="DOM_SPOOFING_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[1].Name="02. Δqty min"; sc.Input[1].SetInt(200); sc.Input[1].SetIntLimits(1,1000000);
    sc.Input[2].Name="03. Fenêtre détection ticks"; sc.Input[2].SetInt(10); sc.Input[2].SetIntLimits(1,100000);
    sc.DrawZeros=false; return;
  }
  if(!inited||sc.IsFullRecalculation){ inited=1; phase=0; peak=0; }
  if(sc.TickSize<=0) return;
  int N=sc.Input[0].GetInt(); int thr=sc.Input[1].GetInt(); int win=sc.Input[2].GetInt();
  s_MarketDepthEntry md{}; double sum=0; static double prev=0; static int age=0;
  for(int i=0;i<sc.GetAskMarketDepthNumberOfLevels() && i<N;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); sum+=md.Quantity; }
  double d=sum-prev; prev=sum; if(age<win) ++age;
  if(phase==0 && d>thr){ phase=1; peak=sum; age=0; }
  else if(phase==1 && (prev<peak-thr || age>=win)){ int spoof = (prev<peak-thr? 1:0); sc.Subgraph[1][sc.ArraySize-1]=spoof; phase=0; peak=0; age=0; }
}
