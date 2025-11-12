#pragma once
#include "sierrachart.h"

// ============================================================================
// UtilitaireEngineContract.h — Contrat SG / Features v1 (Pack_v0)
// - Stabilise les index SG par famille d'ENGINE et les noms de features v1.
// - Aucun état global. Inline only. Pas de sc.StudyDescription.
// - A inclure par tous les ENGINE headless et par les SIGNALS lecteurs.
// ============================================================================
namespace du {

// Mapping SG figé (v1)
enum { DU_SG_MAX = 24 };
struct DU_SgRange { int first, last; };

// Familles standards (réservé pack v1)
static inline DU_SgRange DU_SG_DOM()     { return {1,24}; }
static inline DU_SgRange DU_SG_TAPE()    { return {1,16}; }
static inline DU_SgRange DU_SG_CONTEXT() { return {1, 8}; }
static inline DU_SgRange DU_SG_ZONES()   { return {1, 8}; }

// Helper simple de vérification (runtime léger)
inline void duCheckSgRange(const SCStudyInterfaceRef sc, DU_SgRange r){
  int n = sc.NumberOfArrays; (void)n; // Sierra ne fournit pas le nombre de SG exposé directement
  // Point de contrôle symbolique: clamp bornes pour éviter débordement utilisateur.
  if(r.first < 1 || r.last < r.first) {
    sc.AddMessageToLog("DU: SG range invalide (first/last).", 1);
  }
}

// Publication des noms de features (facultatif, pour debug CSV #LAYOUT)
inline void duPublishFeatureLayout(const SCStudyInterfaceRef sc, const char* family, const char* const* names, int count){
  SCString msg; msg.Format("DU:LAYOUT:%s:", family);
  for(int i=0;i<count;++i){
    msg += names[i];
    if(i+1<count) msg += ",";
  }
  sc.AddMessageToLog(msg, 0);
}

} // namespace du
