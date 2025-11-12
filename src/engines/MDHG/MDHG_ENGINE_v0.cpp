// ============================================================================
// Pack v0 — Engines (v0) — complémentaires
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_MDHG_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited     = sc.GetPersistentInt(1);
  double& emaNear = sc.GetPersistentDouble(2);
  double& emaFar  = sc.GetPersistentDouble(3);

  if (sc.SetDefaults)
  {
    sc.GraphName = "MDHG_ENGINE_v0";
    sc.AutoLoop = 0;
    sc.UpdateAlways = 1;
    sc.GraphRegion = 0;
    sc.ValueFormat = 26;
    sc.FreeDLL = 0;
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

    sc.Input[0].Name = "01. Fenêtre secondes";
    sc.Input[0].SetInt(60); sc.Input[0].SetIntLimits(1, 3600);

    sc.Input[1].Name = "02. Niveaux Near";
    sc.Input[1].SetInt(10); sc.Input[1].SetIntLimits(1, 60);

    sc.Input[2].Name = "03. Niveaux Far";
    sc.Input[2].SetInt(30); sc.Input[2].SetIntLimits(1, 60);

    sc.Input[3].Name = "04. EMA %";
    sc.Input[3].SetInt(85); sc.Input[3].SetIntLimits(1, 99);

    sc.DrawZeros = false;
    return;
  }

  if (!inited || sc.IsFullRecalculation) { inited = 1; emaNear=0; emaFar=0; }
  if (sc.TickSize <= 0) return;

  int nearN = sc.Input[1].GetInt();
  int farN  = sc.Input[2].GetInt();
  double a  = sc.Input[3].GetInt()/100.0;

  s_MarketDepthEntry md{};
  double sNear=0, sFar=0;
  int availBid = sc.GetBidMarketDepthNumberOfLevels();
  int availAsk = sc.GetAskMarketDepthNumberOfLevels();
  for(int i=0;i<availBid && i<farN;++i){ sc.GetBidMarketDepthEntryAtLevel(md,i); (i<nearN? sNear:sFar)+=md.Quantity; }
  for(int i=0;i<availAsk && i<farN;++i){ sc.GetAskMarketDepthEntryAtLevel(md,i); (i<nearN? sNear:sFar)+=md.Quantity; }

  emaNear = a*sNear + (1-a)*emaNear;
  emaFar  = a*sFar  + (1-a)*emaFar;

  int idx = sc.ArraySize-1; if (idx>=0){ sc.Subgraph[1][idx]=emaNear; sc.Subgraph[2][idx]=emaFar; }
}
