#include "sierrachart.h"
#include "Pack_v0.h"
#include "UtilitaireEngineContract.h"

SCSFExport scsf_DOM_ENGINE_V2(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    du::ApplyHeadlessDefaults(sc, 1, 24);
    sc.GraphRegion = 0;
    // Subgraphs exposés (24 slots v1)
    for(int sg=1; sg<=24; ++sg){
      du::SgHideAll(sc.Subgraph[sg]);
      sc.Subgraph[sg].Name.Format("f%02d", sg);
    }
    sc.Subgraph[1].Name = "f01_near_imbalance";    // exemple
    sc.Subgraph[2].Name = "f02_mid_imbalance";     // exemple
    sc.Subgraph[3].Name = "f03_far_imbalance";     // exemple
    sc.Subgraph[4].Name = "f04_near_total";        // exemple
    sc.Subgraph[5].Name = "f05_mid_total";         // exemple
    sc.Subgraph[6].Name = "f06_far_total";         // exemple
    return;
  }

  // Gate DOM requis
  if(sc.UsesMarketDepthData == 0) sc.UsesMarketDepthData = 1;

  // Lecture unique DOM au tick
  int Nbid = sc.NumberOfBidLevels;
  int Nask = sc.NumberOfAskLevels;
  if(Nbid<=0 || Nask<=0) {
    // Zéro si DOM indisponible
    for(int sg=1; sg<=24; ++sg) sc.Subgraph[sg][sc.Index] = 0.0f;
    return;
  }

  // Agrégation par bandes (Near=1..5, Mid=6..15, Far=16..30, clamp aux niveaux dispo)
  auto sum_qty = [&](bool isBid, int li, int lj)->double{
    double s = 0.0;
    for(int l=li; l<=lj; ++l){
      s += (float)([&](){
        s_MarketDepthEntry e{};
        if(isBid){
          if(!sc.GetBidMarketDepthEntryAtLevel(e, l)) return 0.0f;
        }else{
          if(!sc.GetAskMarketDepthEntryAtLevel(e, l)) return 0.0f;
        }
        return e.Quantity;
      }());
    }
    return s;
  };
  int nb = Nbid, na = Nask;
  int n1 = 1,  n2 = (nb>5?5:nb),   n3 = (nb>15?15:nb);
  int a1 = 1,  a2 = (na>5?5:na),   a3 = (na>15?15:na);

  double b_near = sum_qty(true,  n1, n2);
  double b_mid  = sum_qty(true,  n2+1, n3);
  double a_near = sum_qty(false, a1, a2);
  double a_mid  = sum_qty(false, a2+1, a3);

  // Features exemples v1
  double f01 = (b_near - a_near);                 // near imbalance
  double f02 = (b_mid  - a_mid );                 // mid imbalance
  double f03 = (double)Nbid - (double)Nask;       // depth asymmetry simple
  double f04 = (b_near + a_near);                 // near total
  double f05 = (b_mid  + a_mid );                 // mid total
  double f06 = (b_near + b_mid + a_near + a_mid); // near+mid total

  sc.Subgraph[1][sc.Index] = (float)f01;
  sc.Subgraph[2][sc.Index] = (float)f02;
  sc.Subgraph[3][sc.Index] = (float)f03;
  sc.Subgraph[4][sc.Index] = (float)f04;
  sc.Subgraph[5][sc.Index] = (float)f05;
  sc.Subgraph[6][sc.Index] = (float)f06;

  // Optionnel: publier layout une fois
  static int logged=0;
  if(!logged){
    const char* names[] = {
      "f01_near_imbalance","f02_mid_imbalance","f03_depth_asym",
      "f04_near_total","f05_mid_total","f06_near_mid_total"
    };
    du::duPublishFeatureLayout(sc, "DOM_V2", names, (int)(sizeof(names)/sizeof(names[0])));
    logged=1;
  }
}
