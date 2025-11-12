// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #2)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_TAPE_PRINTSIZE_P95_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="TAPE_PRINTSIZE_P95_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Fenêtre ms"; sc.Input[0].SetInt(1000);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()==0 || sc.ArraySize==0) return;
  int win=sc.Input[0].GetInt(); double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd-win/86400000.0;
  const int MAX=512; double arr[MAX]; int n=0;
  for(int i=ts.Size()-1;i>=0 && n<MAX;--i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type==SC_TS_TRADES) arr[n++]=e.Volume; }
  if(n==0) return; int k=(int)(0.95*n); if(k<0) k=0; if(k>=n) k=n-1;
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
  for(int i=0;i<=k;++i){ int best=i; for(int j=i+1;j<n;++j) if(arr[j]<arr[best]) best=j; double tmp=arr[i]; arr[i]=arr[best]; arr[best]=tmp; }
  #pragma GCC diagnostic pop
  sc.Subgraph[1][sc.ArraySize-1]=arr[k];
}
