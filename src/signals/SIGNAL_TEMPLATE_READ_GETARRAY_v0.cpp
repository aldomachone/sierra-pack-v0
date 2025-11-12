#include "sierrachart.h"
#include "Pack_v0.h"
#include "UtilitaireEngineContract.h"

// SIGNAL_TEMPLATE_READ_GETARRAY_v0
// Lit un ENGINE côté DOM/TAPE/CONTEXT via GetArray et place des marqueurs transparents.

SCSFExport scsf_SIGNAL_TEMPLATE_READ_GETARRAY_v0(SCStudyInterfaceRef sc)
{
  if(sc.SetDefaults){
    du::ApplyHeadlessDefaults(sc, 1, 4);
    sc.GraphRegion = 0;
    du::SgMarkerTransparent(sc.Subgraph[1]); sc.Subgraph[1].Name = "SignalUp";
    du::SgMarkerTransparent(sc.Subgraph[2]); sc.Subgraph[2].Name = "SignalDn";
    du::SgMarkerTransparent(sc.Subgraph[3]); sc.Subgraph[3].Name = "Score";
    du::SgMarkerTransparent(sc.Subgraph[4]); sc.Subgraph[4].Name = "Gate";
    // Inputs minimalistes
    sc.Input[0].Name = "01. Chart source"; sc.Input[0].SetInt(1);
    sc.Input[1].Name = "02. Study ID source"; sc.Input[1].SetInt(1);
    sc.Input[2].Name = "03. Subgraph score"; sc.Input[2].SetInt(1);
    sc.Input[3].Name = "04. Seuil score"; sc.Input[3].SetFloat(0.0);
    return;
  }

  int chartNum = sc.Input[0].GetInt();
  int studyId  = sc.Input[1].GetInt();
  int sgScore  = sc.Input[2].GetInt();
  double thr   = sc.Input[3].GetFloat();

  SCFloatArray arr;
  if(!sc.GetStudyArrayFromChart(chartNum, studyId, sgScore, arr)){
    // Pas de source -> rien
    sc.Subgraph[1][sc.Index] = 0.0f;
    sc.Subgraph[2][sc.Index] = 0.0f;
    sc.Subgraph[3][sc.Index] = 0.0f;
    sc.Subgraph[4][sc.Index] = 0.0f;
    return;
  }

  float x = (sc.Index < arr.GetArraySize()) ? arr[sc.Index] : 0.0f;
  sc.Subgraph[3][sc.Index] = x;
  int gate = std::isfinite(x) ? 1 : 0;
  sc.Subgraph[4][sc.Index] = (float)gate;

  if(gate && x > thr){
    sc.Subgraph[1][sc.Index] = x;
    sc.Subgraph[2][sc.Index] = 0.0f;
  }else if(gate && x < -thr){
    sc.Subgraph[1][sc.Index] = 0.0f;
    sc.Subgraph[2][sc.Index] = -x;
  }else{
    sc.Subgraph[1][sc.Index] = 0.0f;
    sc.Subgraph[2][sc.Index] = 0.0f;
  }
}
