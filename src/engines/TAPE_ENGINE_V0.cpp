// File: TAPE_ENGINE_v0.cpp
#include "Pack_v0.h"

SCSFExport scsf_TAPE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  enum { IN_DtMs=1, IN_HLms }; // 01..02
  if(sc.SetDefaults){
    sc.AutoLoop = 0; sc.UpdateAlways = 1; sc.ValueFormat=26; sc.GraphRegion=0; sc.FreeDLL=0;
    for(int sg=1; sg<=16; ++sg){
      sc.Subgraph[sg].Name="ENG_TAPE_"+SCString(sg);
      sc.Subgraph[sg].DrawStyle=DRAWSTYLE_IGNORE;
      sc.Subgraph[sg].DrawZeros=0;
      sc.Subgraph[sg].DisplayAsMainPriceGraphValue=0;
    }
    sc.Input[IN_DtMs].Name="01. Pas temps (ms)"; sc.Input[IN_DtMs].SetInt(50);
    sc.Input[IN_HLms].Name="02. Demi-vie (ms)";  sc.Input[IN_HLms].SetInt(250);
    sc.DisplayStudyInputValues=false; return;
  }

  // Placeholder minimal: pas de calcul spécifique ici.
  // Les SG restent à 0 pour ne pas impacter l’échelle.
  for(int sg=1; sg<=16; ++sg) sc.Subgraph[sg][sc.Index]=0.0f;
}
