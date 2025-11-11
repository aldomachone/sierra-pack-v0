// ============================================================================
// Pack v0 — Engines v0 (STATE_FILTERS_v1)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_STATE_KALMAN_MICROPRICE_ENGINE_v1(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="STATE_KALMAN_MICROPRICE_ENGINE_v1"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1;
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
    sc.Input[0].Name="01. Q bruit état"; sc.Input[0].SetFloat(0.01f);
    sc.Input[1].Name="02. R bruit obs";  sc.Input[1].SetFloat(0.10f);
    sc.DrawZeros=false; return;
  }
  if(sc.GetBidMarketDepthNumberOfLevels()<1 || sc.GetAskMarketDepthNumberOfLevels()<1) return;
  s_MarketDepthEntry b,a; sc.GetBidMarketDepthEntryAtLevel(b,0); sc.GetAskMarketDepthEntryAtLevel(a,0);
  double qb=b.Quantity, qa=a.Quantity; double micro=(qb+qa>0? (qb*a.Price + qa*b.Price)/(qb+qa): 0.5*(a.Price+b.Price));
  static double x=0,P=1; double Q=sc.Input[0].GetFloat(), R=sc.Input[1].GetFloat();
  double x_pred=x, P_pred=P+Q; double K=P_pred/(P_pred+R); x=x_pred + K*(micro-x_pred); P=(1.0-K)*P_pred;
  sc.Subgraph[1][sc.ArraySize-1]=x; sc.Subgraph[2][sc.ArraySize-1]=K;
}
