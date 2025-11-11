// ============================================================================
// Pack v0 — Engines v0 (lot HFT+Stats+Math #13)
// ============================================================================
#include "sierrachart.h"
#include "Pack_v0.h"

SCSFExport scsf_HILL_TAIL_INDEX_ENGINE_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    sc.GraphName="HILL_TAIL_INDEX_ENGINE_v0"; sc.AutoLoop=0; sc.UpdateAlways=1; sc.GraphRegion=0; sc.ValueFormat=26; sc.MaintainTimeAndSalesData=1; sc.FreeDLL=0;
    sc.Subgraph[1].Name = "SG01";\n    sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[1].DrawZeros = false;\n    sc.Subgraph[1].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[2].Name = "SG02";\n    sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[2].DrawZeros = false;\n    sc.Subgraph[2].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[3].Name = "SG03";\n    sc.Subgraph[3].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[3].DrawZeros = false;\n    sc.Subgraph[3].DisplayAsMainPriceGraphValue = 0;\n    sc.Subgraph[4].Name = "SG04";\n    sc.Subgraph[4].DrawStyle = DRAWSTYLE_IGNORE;\n    sc.Subgraph[4].DrawZeros = false;\n    sc.Subgraph[4].DisplayAsMainPriceGraphValue = 0;
    sc.Input[0].Name="01. Fenêtre ms"; sc.Input[0].SetInt(3000);
    sc.Input[1].Name="02. k ordres"; sc.Input[1].SetInt(20);
    sc.DrawZeros=false; return;
  }
  c_SCTimeAndSalesArray ts; sc.GetTimeAndSales(ts); if(ts.Size()<10 || sc.ArraySize==0) return;
  int win=sc.Input[0].GetInt(); int k=sc.Input[1].GetInt(); if(k<5) k=5; if(k>128) k=128;
  double tEnd=ts[ts.Size()-1].DateTime, tBeg=tEnd-win/86400000.0;
  static double buf[2048]; int n=0;
  for(int i=ts.Size()-1;i>=0; --i){ const auto& e=ts[i]; if(e.DateTime<tBeg) break; if(e.Type!=SC_TS_TRADES) continue; if(n<2048) buf[n++]=fabs((double)e.Volume)+1e-9; }
  if(n<=k) return;
  #define SWAP(a,b) { double tmp=a; a=b; b=tmp; }
  #define PARENT(i) ((i-1)>>1)
  #define LEFT(i)   (((i)<<1)+1)
  #define RIGHT(i)  (((i)<<1)+2)
  #define HEAPIFY(A,sz,i) { int l=LEFT(i), r=RIGHT(i), m=i; if(l<sz && A[l]>A[m]) m=l; if(r<sz && A[r]>A[m]) m=r; if(m!=i){ SWAP(A[i],A[m]); HEAPIFY(A,sz,m); } }
  #define BUILD_MAX_HEAP(A,sz) { for(int i=(sz>>1)-1;i>=0;--i) HEAPIFY(A,sz,i); }
  #define HEAPSORT(A,sz) { BUILD_MAX_HEAP(A,sz); for(int i=sz-1;i>0;--i){ SWAP(A[0],A[i]); HEAPIFY(A,i,0);} }
  HEAPSORT(buf,n);
  double xk = buf[n-k]; double sum=0; for(int i=n-k+1;i<n;++i) sum += log(buf[i]/xk);
  double hill = (k>1? k / sum : 0.0);
  sc.Subgraph[1][sc.ArraySize-1]=hill;
}
