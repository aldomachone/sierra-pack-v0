#include "Pack_v0.h"

SCSFExport scsf_TAPE_ENGINE_v0(SCStudyInterfaceRef sc)
{
  enum { P_In_DtMs=1, P_In_HLms };
  if(sc.SetDefaults){
    sc.AutoLoop = 0; sc.UpdateAlways = 1; sc.ValueFormat=26; sc.GraphRegion=0; sc.FreeDLL=0;
    for(int sg=1; sg<=16; ++sg){ sc.Subgraph[sg].Name="ENG_TAPE_"+SCString(sg); sc.Subgraph[sg].DrawStyle=DRAWSTYLE_IGNORE; sc.Subgraph[sg].DrawZeros=0; }
    sc.Input[P_In_DtMs].Name="01. Pas temps (ms)"; sc.Input[P_In_DtMs].SetInt(50);
    sc.Input[P_In_HLms].Name="02. Demi-vie (ms)";  sc.Input[P_In_HLms].SetInt(250);
    sc.DisplayStudyInputValues=false; return;
  }

  const double dtMs = (double)std::max(1, sc.Input[P_In_DtMs].GetInt());
  const double hlMs = (double)std::max(1, sc.Input[P_In_HLms].GetInt());

  // Exemple: Pace features → SG1..SG16
  // Appelle tes helpers Tape Pace/BVC/Sweep/Exhaust et écris dans les SG
  // Ici placeholder: zéro (aucun impact d’échelle)
  for(int sg=1; sg<=16; ++sg) sc.Subgraph[sg][sc.Index]=0.0f;
}
