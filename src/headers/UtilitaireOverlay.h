#pragma once
#include "sierrachart.h"

// =============================================================================
// UtilitaireOverlay.h — Aides d'overlay (v2 enrichi, compat v0, drop‑in)
// -----------------------------------------------------------------------------
// • Objectif : simplifier l'affichage de marqueurs/labels sans impacter l'échelle.
// • Conventions Pack v0 :
//    - Marqueurs = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE
//    - Taille variable, couleur up/dn, affichage « dernière barre uniquement »
//    - Aucune allocation globale, aucun état statique caché
// • Toutes les fonctions sont inline. Aucune dépendance hors sierrachart.h.
// =============================================================================

namespace du {

// Couleurs par défaut Pack v0 (neutres, modifiables côté appelant)
inline COLORREF DU_COL_UP()   { return RGB(0,255,0); }   // vert
inline COLORREF DU_COL_DN()   { return RGB(255,0,0); }   // rouge
inline COLORREF DU_COL_TXT()  { return RGB(255,255,255);} // blanc

// -----------------------------------------------------------------------------
// Base: configuration d'un subgraph « marker » prêt pour l'overlay
// -----------------------------------------------------------------------------
inline void ovMarkerSetup(SCSubgraphRef sg, const char* name, int sizePx,
                          COLORREF colMain = DU_COL_UP(), COLORREF colAlt = DU_COL_DN())
{
  sg.Name = name ? name : "Marker";
  sg.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
  sg.LineWidth = sizePx > 0 ? sizePx : 6;           // taille par défaut 6 px
  sg.DrawZeros = 0;                                  // 0 ⇒ ne déplace pas l'échelle
  sg.DisplayAsMainPriceGraphValue = 0;               // aucun effet valeur principale
  sg.PrimaryColor   = colMain;                       // up
  sg.SecondaryColor = colAlt;                        // dn (optionnel selon vos usages)
  sg.DisplayNameValueInWindowsFlags = 0;
  sg.LineLabel = 0;                                  // pas de label de ligne implicite
}

// Variante historique (compat v0) conservée
inline void markerTransparent(SCSubgraphRef sg, int size)
{ ovMarkerSetup(sg, sg.Name.GetChars(), size, DU_COL_UP(), DU_COL_DN()); }

// -----------------------------------------------------------------------------
// Politique d'impact à l'échelle pour l'ensemble des SG de l'étude
// -----------------------------------------------------------------------------
inline void ovDisableScaleImpactAll(SCStudyInterfaceRef sc)
{
  for (int i=0; i<SC_SUBGRAPH_MAX; ++i){
    sc.Subgraph[i].DrawZeros = 0;
    sc.Subgraph[i].DisplayAsMainPriceGraphValue = 0;
  }
}

// -----------------------------------------------------------------------------
// Last‑bar‑only: affiche le marqueur uniquement sur la dernière barre
//  • Utilise un persistent pour effacer la précédente position
// -----------------------------------------------------------------------------
inline void ovSetMarkerLastBarOnly(SCStudyInterfaceRef sc, SCSubgraphRef sg,
                                   double price, int persistentLastBarId, float sizePx = 0.0f)
{
  const int last = sc.ArraySize - 1; if (last < 0) return;
  int& prev = sc.GetPersistentInt(persistentLastBarId);
  if (prev >= 0 && prev < sc.ArraySize) sg[prev] = 0.0f; // clear ancien
  if (sizePx > 0.0f) sg.LineWidth = sizePx;
  sg[last] = (float)price;                               // place le marqueur
  sg.DataColor[last] = DU_COL_TXT();                     // couleur ponctuelle si désiré
  prev = last;
}

// Efface le marqueur de la barre persistante mémorisée
inline void ovClearLastMarker(SCStudyInterfaceRef sc, SCSubgraphRef sg, int persistentLastBarId)
{
  int& prev = sc.GetPersistentInt(persistentLastBarId);
  if (prev >= 0 && prev < sc.ArraySize) sg[prev] = 0.0f;
  prev = -1;
}

// -----------------------------------------------------------------------------
// Placement au meilleur Bid/Ask ou au Close, avec offset en ticks
// -----------------------------------------------------------------------------
inline double ovPickPrice(SCStudyInterfaceRef sc, int mode /*0=Close,1=BestBid,2=BestAsk*/, int offsetTicks = 0)
{
  const int i = sc.ArraySize - 1; if (i < 0) return 0.0;
  double p = (double)sc.Close[i];
  if (mode == 1) p = (double)sc.Bid[i];
  else if (mode == 2) p = (double)sc.Ask[i];
  if (offsetTicks != 0) p += (double)offsetTicks * sc.TickSize * (mode==1 ? -1.0 : +1.0);
  return p;
}

// Place un marqueur « up » ou « dn » selon le signe d'un score
inline void ovMarkBySign(SCStudyInterfaceRef sc, SCSubgraphRef sg, double score,
                         int priceMode /*0/1/2*/, int persistentLastBarId,
                         int offsetTicks = 0, float sizePx = 0.0f)
{
  const double price = ovPickPrice(sc, priceMode, offsetTicks);
  if (score == 0.0) { ovClearLastMarker(sc, sg, persistentLastBarId); return; }
  // taille optionnelle proportionnelle
  if (sizePx <= 0.0f) {
    const float base = 6.0f; const float add = (float)std::min(12.0, std::fabs(score));
    sg.LineWidth = base + add; 
  } else sg.LineWidth = sizePx;
  // colorisation ponctuelle si besoin (facultatif)
  const int last = sc.ArraySize - 1; if (last >= 0){ sg.DataColor[last] = (score > 0.0 ? DU_COL_UP() : DU_COL_DN()); }
  ovSetMarkerLastBarOnly(sc, sg, price, persistentLastBarId);
}

// -----------------------------------------------------------------------------
// Labels « dernière barre » uniquement sur un SG texte/valeur
// -----------------------------------------------------------------------------
inline void lastBarOnlyNameValue(SCSubgraphRef sg, bool on)
{
  sg.DrawStyle = on ? DRAWSTYLE_SUBGRAPH_NAME_AND_VALUE_LABELS_ONLY : DRAWSTYLE_IGNORE;
  sg.DrawZeros = 0;
}

// -----------------------------------------------------------------------------
// Dessiner « sous » le prix pour éviter l'occlusion
// -----------------------------------------------------------------------------
inline void ovDrawUnderMain(SCStudyInterfaceRef sc, bool yes)
{ sc.DrawStudyUnderneathMainPriceGraph = yes ? 1 : 0; }

// -----------------------------------------------------------------------------
// Raccourcis « legacy » pour compatibilité ascendante v0
// -----------------------------------------------------------------------------
inline void ovSetMarker(SCStudyInterfaceRef sc, SCSubgraphRef sg, int idx, double y, float size)
{ sg[idx] = (float)y; sg.DataColor[idx] = DU_COL_TXT(); sg.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE; sg.LineWidth = size; }

inline void ovLastBarOnly(SCStudyInterfaceRef, SCSubgraphRef, bool /*visible*/){ /* obsolète: remplacé par ovSetMarkerLastBarOnly */ }
inline void ovZeroDoesNotMoveScale(SCStudyInterfaceRef, SCSubgraphRef){ /* obsolète: géré via DrawZeros=0 */ }
inline void ovRightAlignLabels(SCStudyInterfaceRef, const char* /*sgName*/, bool /*showValue*/){ /* placeholder: alignement UI côté plateforme */ }

} // namespace du
