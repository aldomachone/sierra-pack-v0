// ============================================================================
// Pack v0 — Engines (v0) — Tape & Advanced
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"


SCSFExport scsf_CONFLUENCE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  int& inited = sc.GetPersistentInt(1);

  if (sc.SetDefaults)
  {
    sc.GraphName = "CONFLUENCE_ENGINE_v0";
    sc.AutoLoop = 0; sc.UpdateAlways = 0; sc.GraphRegion = 0; sc.ValueFormat = 26; sc.FreeDLL = 0;

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

    sc.Input[0].Name = "01. Poids VWAP";
    sc.Input[0].SetFloat(1.0f);
    sc.Input[1].Name = "02. Poids Pivot";
    sc.Input[1].SetFloat(1.0f);
    sc.Input[2].Name = "03. Poids Zone";
    sc.Input[2].SetFloat(1.0f);
    sc.Input[3].Name = "04. Fenêtre ATR";
    sc.Input[3].SetInt(14); sc.Input[3].SetIntLimits(1, 10000);

    sc.DrawZeros=false; return;
  }

  if (!inited || sc.IsFullRecalculation) { inited=1; }
  if (sc.ArraySize <= 1) return;
  int idx = sc.ArraySize-1;

  // Placeholders: VWAP ≈ moyenne Close, Pivot ≈ (H+L+C)/3 sur dernière barre, Zone ≈ (H+L)/2 sur lookback court
  double vwap = 0.0; int win=20; int start = sc.ArraySize - win; if (start<0) start=0;
  for(int i=start;i<sc.ArraySize;++i) vwap += sc.Close[i];
  vwap = (sc.ArraySize-start>0? vwap/(sc.ArraySize-start) : 0.0);

  double P = (sc.High[idx]+sc.Low[idx]+sc.Close[idx])/3.0;

  int lb=50; int s2 = sc.ArraySize - lb; if (s2<0) s2=0;
  double hi=-1e300, lo=1e300; for(int i=s2;i<sc.ArraySize;++i){ if(sc.High[i]>hi) hi=sc.High[i]; if(sc.Low[i]<lo) lo=sc.Low[i]; }
  double zone = 0.5*(hi+lo);

  // ATR pour normaliser distances
  int N = sc.Input[3].GetInt();
  int s3 = sc.ArraySize - N; if (s3 < 1) s3 = 1;
  double trSum=0.0; for(int k=s3;k<=idx;++k){ double tr=fmax(sc.High[k]-sc.Low[k], fmax(fabs(sc.High[k]-sc.Close[k-1]), fabs(sc.Low[k]-sc.Close[k-1]))); trSum+=tr; }
  double atr=(idx-s3+1>0? trSum/(idx-s3+1):0.0);
  if (atr<=0) return;

  double wV=sc.Input[0].GetFloat(), wP=sc.Input[1].GetFloat(), wZ=sc.Input[2].GetFloat();
  double px = sc.Close[idx];
  double dV = fabs(px - vwap)/atr;
  double dP = fabs(px - P)/atr;
  double dZ = fabs(px - zone)/atr;

  double score = wV*(1.0/(1.0+dV)) + wP*(1.0/(1.0+dP)) + wZ*(1.0/(1.0+dZ));
  sc.Subgraph[1][idx] = score;
  sc.Subgraph[2][idx] = vwap;
  sc.Subgraph[3][idx] = P;
  sc.Subgraph[4][idx] = zone;
}
