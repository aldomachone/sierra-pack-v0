// ============================================================================
// Pack v0 — Engines (v0) — DOM & StopRun
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_DOM_IMBAL_SWEEP_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited = sc.GetPersistentInt(1);

  if (sc.SetDefaults)
  {
    sc.GraphName = "DOM_IMBAL_SWEEP_ENGINE_v0";
    sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
    sc.UsesMarketDepthData = 1;

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
    sc.Subgraph[9].Name = "SG09";
    sc.Subgraph[9].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[9].DrawZeros = false;
    sc.Subgraph[9].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[10].Name = "SG10";
    sc.Subgraph[10].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[10].DrawZeros = false;
    sc.Subgraph[10].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[11].Name = "SG11";
    sc.Subgraph[11].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[11].DrawZeros = false;
    sc.Subgraph[11].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[12].Name = "SG12";
    sc.Subgraph[12].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[12].DrawZeros = false;
    sc.Subgraph[12].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[13].Name = "SG13";
    sc.Subgraph[13].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[13].DrawZeros = false;
    sc.Subgraph[13].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[14].Name = "SG14";
    sc.Subgraph[14].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[14].DrawZeros = false;
    sc.Subgraph[14].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[15].Name = "SG15";
    sc.Subgraph[15].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[15].DrawZeros = false;
    sc.Subgraph[15].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[16].Name = "SG16";
    sc.Subgraph[16].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[16].DrawZeros = false;
    sc.Subgraph[16].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[17].Name = "SG17";
    sc.Subgraph[17].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[17].DrawZeros = false;
    sc.Subgraph[17].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[18].Name = "SG18";
    sc.Subgraph[18].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[18].DrawZeros = false;
    sc.Subgraph[18].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[19].Name = "SG19";
    sc.Subgraph[19].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[19].DrawZeros = false;
    sc.Subgraph[19].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[20].Name = "SG20";
    sc.Subgraph[20].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[20].DrawZeros = false;
    sc.Subgraph[20].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[21].Name = "SG21";
    sc.Subgraph[21].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[21].DrawZeros = false;
    sc.Subgraph[21].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[22].Name = "SG22";
    sc.Subgraph[22].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[22].DrawZeros = false;
    sc.Subgraph[22].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[23].Name = "SG23";
    sc.Subgraph[23].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[23].DrawZeros = false;
    sc.Subgraph[23].DisplayAsMainPriceGraphValue = 0;
    sc.Subgraph[24].Name = "SG24";
    sc.Subgraph[24].DrawStyle = DRAWSTYLE_IGNORE;
    sc.Subgraph[24].DrawZeros = false;
    sc.Subgraph[24].DisplayAsMainPriceGraphValue = 0;

    sc.Input[0].Name = "01. Niveaux Near";
    sc.Input[0].SetInt(8); sc.Input[0].SetIntLimits(1,60);

    sc.Input[1].Name = "02. Niveaux Mid";
    sc.Input[1].SetInt(16); sc.Input[1].SetIntLimits(1,60);

    sc.Input[2].Name = "03. Niveaux Far";
    sc.Input[2].SetInt(32); sc.Input[2].SetIntLimits(1,60);

    sc.DrawZeros=false; return;
  }

  if (!inited || sc.IsFullRecalculation) { inited=1; }
  if (sc.TickSize<=0) return;

  int nN=sc.Input[0].GetInt(), nM=sc.Input[1].GetInt(), nF=sc.Input[2].GetInt();

  s_MarketDepthEntry md{};
  double bN=0,bM=0,bF=0,aN=0,aM=0,aF=0;
  int nb=sc.GetBidMarketDepthNumberOfLevels(), na=sc.GetAskMarketDepthNumberOfLevels();
  for(int i=0;i<nb && i<nF;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); if(i<nN) bN+=md.Quantity; else if(i<nM) bM+=md.Quantity; else bF+=md.Quantity; }
  for(int i=0;i<na && i<nF;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); if(i<nN) aN+=md.Quantity; else if(i<nM) aM+=md.Quantity; else aF+=md.Quantity; }

  auto ratio = [](double x,double y){ return (x+y>0? (x-y)/(x+y) : 0.0); };
  double rN = ratio(bN,aN);
  double rM = ratio(bM,aM);
  double rF = ratio(bF,aF);

  // Sweep score = signe(rN) * (|rN| + 0.5|rM| + 0.25|rF|)
  double sgn = (rN>0? +1.0 : (rN<0? -1.0 : 0.0));
  double sweep = sgn*(fabs(rN)+0.5*fabs(rM)+0.25*fabs(rF));

  int idx=sc.ArraySize-1; if(idx>=0){ 
    sc.Subgraph[1][idx]=rN; sc.Subgraph[2][idx]=rM; sc.Subgraph[3][idx]=rF; 
    sc.Subgraph[4][idx]=sweep;
  }
}
