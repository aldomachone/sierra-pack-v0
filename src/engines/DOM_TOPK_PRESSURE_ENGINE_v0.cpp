// ============================================================================
// Pack v0 — Engines v0 (lot complémentaire #9)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_DOM_TOPK_PRESSURE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="DOM_TOPK_PRESSURE_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.UsesMarketDepthData=1; sc.FreeDLL=0;
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
    sc.Input[0].Name="01. Scanner niveaux"; sc.Input[0].SetInt(30);
    sc.Input[1].Name="02. K"; sc.Input[1].SetInt(3);
    sc.Input[2].Name="03. Poids 1/(d+1)"; sc.Input[2].SetYesNo(1);
    sc.DrawZeros=false; return;
  }
  int L=sc.Input[0].GetInt(), K=sc.Input[1].GetInt(); int useW=sc.Input[2].GetYesNo();
  s_MarketDepthEntry md{};
  auto topk=[&](bool bid){ double best[16]; double dist[16]; int m=0; for(int i=0;i<16;++i){ best[i]=0; dist[i]=1; }
    int n = bid? sc.GetBidMarketDepthNumberOfLevels(): sc.GetAskMarketDepthNumberOfLevels();
    for(int i=0;i<n && i<L; ++i){ if(bid) sc.GetBidMarketDepthEntryAtLevel(md,i); else sc.GetAskMarketDepthEntryAtLevel(md,i);
      double v=md.Quantity; int pos=-1; for(int j=0;j<m;++j) if(v>best[j]) { pos=j; break; }
      if(pos<0 && m<K) pos=m++;
      if(pos>=0){ for(int j=min(m-1,15); j>pos; --j){ best[j]=best[j-1]; dist[j]=dist[j-1]; } best[pos]=v; dist[pos]=i+1; if(m<16 && pos==m) ++m; }
    }
    double s=0; for(int j=0;j<m; ++j) s+= useW? best[j]/(dist[j]) : best[j]; return s; };
  double press = topk(true) - topk(false);
  sc.Subgraph[1][sc.ArraySize-1]=press;
}
