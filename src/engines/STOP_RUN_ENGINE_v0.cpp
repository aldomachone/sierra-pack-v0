// ============================================================================
// Pack v0 — Engines (v0) — DOM & StopRun
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_STOP_RUN_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited = sc.GetPersistentInt(1);
  double& emaCad = sc.GetPersistentDouble(2);
  double& emaSz  = sc.GetPersistentDouble(3);

  if (sc.SetDefaults)
  {
    sc.GraphName = "STOP_RUN_ENGINE_v0";
    sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.FreeDLL=0;
    sc.MaintainTimeAndSalesData = 1;

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

    sc.Input[0].Name = "01. Fenêtre ms";
    sc.Input[0].SetInt(400); sc.Input[0].SetIntLimits(50, 5000);

    sc.Input[1].Name = "02. EMA %";
    sc.Input[1].SetInt(85); sc.Input[1].SetIntLimits(1,99);

    sc.DrawZeros=false; return;
  }

  if (!inited || sc.IsFullRecalculation) { inited=1; emaCad=0; emaSz=0; }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts);
  if (ts.Size()==0 || sc.ArraySize==0) return;

  int winMs = sc.Input[0].GetInt();
  double a  = sc.Input[1].GetInt()/100.0;

  double tEnd = ts[ts.Size()-1].DateTime;
  double tBeg = tEnd - winMs/86400000.0;

  int n=0; double sumSz=0; double lastP=0; int up=0,dn=0;
  for(int i=ts.Size()-1;i>=0;--i)
  {
    const auto& e=ts[i];
    if (e.DateTime < tBeg) break;
    if (e.Type!=SC_TS_TRADES) continue;
    ++n; sumSz += e.Volume;
    if (lastP>0) { if(e.Price>lastP) ++up; else if(e.Price<lastP) ++dn; }
    lastP=e.Price;
  }
  double cad = (winMs>0? n*1000.0/winMs : 0.0);
  emaCad = a*cad + (1-a)*emaCad;
  double avgSz = (n>0? sumSz/n : 0.0);
  emaSz  = a*avgSz + (1-a)*emaSz;

  // Score stop-run = direction * cadence * taille
  double dir = (up>dn? +1.0 : (dn>up? -1.0 : 0.0));
  double score = dir * emaCad * (emaSz>0? emaSz:0.0);

  int idx=sc.ArraySize-1; if(idx>=0) { sc.Subgraph[1][idx]=score; sc.Subgraph[2][idx]=emaCad; sc.Subgraph[3][idx]=emaSz; }
}
