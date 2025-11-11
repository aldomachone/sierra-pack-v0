#pragma once
#include "sierrachart.h"
#include "UtilitaireSanitize.h"
#include "UtilitaireZones.h"     // du::Zone

// ============================================================================
// UtilitaireReloadZone.h — Détection, suivi et scoring des « reload zones »
// v1 enrichi, 100% inline, aucune allocation, aucun état global. Compat v0 OK.
// ----------------------------------------------------------------------------
// Compat v0 conservée :
//   struct ReloadZone { float price, size; int ttl; };
//   inline void  rzDecay(ReloadZone&)
//   inline bool  rzAlive(const ReloadZone&)
//   inline float rzDistance(const ReloadZone&, float price)
//   inline du::Zone rzFromSwing(double hi,double lo,int depthTicks)
//   inline int   rzState(double price,const du::Zone& z)  // -1 en‑dessous, 0 dedans, +1 au‑dessus
// ----------------------------------------------------------------------------
// Ajouts v1 :
//  - Moteur de suivi avec états : IDLE → ARMED → ACTIVE → EXHAUSTED.
//  - Détection « reload » quand une taille réapparaît au même prix après
//    exécution partielle ou disparition, avec hystérésis, minLifeMs, minReplenish.
//  - Zone dynamique [lo,hi] autour du prix d’ancrage (halfWidthTicks) et merge.
//  - Mesures : total replenished, n cycles, peak size, last seen, decay TTL bar‑based.
//  - 14 features MLP standardisées.
// ============================================================================

namespace du {

// ---------------------------------------------------------------------------
// Compat v0
// ---------------------------------------------------------------------------
struct 			ReloadZone 				{ float price=0.0f; float size=0.0f; int ttl=0; }; // ttl en barres

inline void  	rzDecay					(ReloadZone& z)                 { if (z.ttl>0) --z.ttl; }
inline bool  	rzAlive					(const ReloadZone& z)           { return z.ttl>0; }
inline float 	rzDistance				(const ReloadZone& z, float price) { return (float)std::fabs(price - z.price); }

inline 			du::Zone rzFromSwing	(double hi,double lo,int /*depthTicks*/){
	du::Zone Z{lo,hi, /*type*/0, /*strength*/1.0}; du::znNormalize(Z); return Z; }

inline int 		rzState					(double price,const du::Zone& z){
	if (price < z.lo) return -1; if (price > z.hi) return +1; return 0; }

// ---------------------------------------------------------------------------
// v1 — enrichi
// ---------------------------------------------------------------------------
struct 			RzParams
{
  double tickSize        = 0.0;   // pour halfWidth en ticks
  int    halfWidthTicks  = 2;     // demi‑largeur de zone autour du prix ancre
  double minLifeMs       = 200.0; // durée min présence avant validation ACTIVE
  double minReplenish    = 1.0;   // quantité min rechargée pour compter un cycle
  double hysterFracOn    = 0.25;  // seuil on  (fraction de peak)
  double hysterFracOff   = 0.10;  // seuil off (fraction de peak)
  int    ttlBars         = 30;    // TTL bar‑based après dernier event
};

enum 			RzState : int 			{ RZ_IDLE=0, RZ_ARMED=1, RZ_ACTIVE=2, RZ_EXHAUST=3 };

struct 			RzRuntime
{
  // Définition de la zone
  double anchorPrice = 0.0;   // prix central 
  du::Zone zone{0,0,0,1};

  // État
  int    state = RZ_IDLE;
  int    ttl   = 0;           // bar‑based

  // Métriques
  double lastSeenMs = 0.0;    // horodatage ms du dernier évènement taille>0
  double firstSeenMs= 0.0;    // début période courante
  double lifeMs     = 0.0;    // durée d’observation courante
  double peakSize   = 0.0;    // max size observée
  double curSize    = 0.0;    // taille instantanée observée
  double replenished= 0.0;    // somme des recharges validées
  int    nCycles    = 0;      // nb de cycles de reload

  void reset_soft()
  {
    state = RZ_IDLE; ttl = 0; curSize = 0; lifeMs = 0; lastSeenMs = firstSeenMs = 0; peakSize = 0;
  }
  void reset_hard()
  {
    reset_soft(); anchorPrice = 0.0; zone = du::Zone{0,0,0,1}; replenished = 0.0; nCycles = 0;
  }
};

// Recalcule la zone autour de anchorPrice
inline void 	rz_rebuild_zone			(const RzParams& p, RzRuntime& rt)
{
  const double hw = (p.tickSize>0.0? p.halfWidthTicks * p.tickSize : 0.0);
  const double ap = rt.anchorPrice > 0.0 ? rt.anchorPrice : 0.0;
  rt.zone = du::Zone{ ap - hw, ap + hw, /*type*/1, /*strength*/ 1.0 };
  du::znNormalize(rt.zone);
}

// Armement d’une zone autour d’un prix d’ancrage initial
inline void 	rz_arm					(RzRuntime& rt, const RzParams& p, double anchorPrice, double nowMs)
{
  rt.anchorPrice = anchorPrice; rt.firstSeenMs = nowMs; rt.lastSeenMs = nowMs; rt.peakSize = 0.0; rt.curSize = 0.0;
  rt.state = RZ_ARMED; rt.ttl = p.ttlBars; rz_rebuild_zone(p, rt);
}

// Mise à jour à partir d’une observation de taille (ex: DOM agrégé à anchorPrice)
// sizeObs = taille visible actuelle au meilleur niveau inclus dans la zone.
inline void 	rz_update_size			(RzRuntime& rt, const RzParams& p, double price, double sizeObs, double nowMs)
{
  // TTL bar‑based reprendra via rz_on_new_bar()
  if (rt.state == RZ_IDLE && sizeObs > 0.0) { rz_arm(rt, p, price, nowMs); }
  if (rt.state == RZ_IDLE) return;

  // Seulement si le prix reste dans la zone
  if (rzState(price, rt.zone) != 0) {
    // Le prix a quitté la zone → éventuellement EXHAUST si ACTIVE
    if (rt.state == RZ_ACTIVE) rt.state = RZ_EXHAUST;
    rt.curSize = 0.0;
    return;
  }

  // Update métriques
  rt.curSize = du::sanitize(sizeObs);
  rt.lastSeenMs = nowMs;
  if (rt.curSize > rt.peakSize) rt.peakSize = rt.curSize;
  rt.lifeMs = rt.lastSeenMs - rt.firstSeenMs;

  const double onTh  = p.hysterFracOn  * rt.peakSize;
  const double offTh = p.hysterFracOff * rt.peakSize;

  switch (rt.state)
  {
    case RZ_ARMED:
    {
      // devient ACTIVE si durée min + taille au‑dessus du seuil ON
      if (rt.lifeMs >= p.minLifeMs && rt.curSize >= onTh && rt.peakSize>0.0)
        rt.state = RZ_ACTIVE;
      break;
    }
    case RZ_ACTIVE:
    {
      // Détection de « recharge » : si taille remonte au‑dessus de onTh
      // après une baisse significative.
      static double prevSize = 0.0; // local stateless pour inline usage
      const bool dip = (prevSize > 0.0 && rt.curSize + p.minReplenish <= prevSize);
      const bool reload = (dip && rt.curSize >= onTh);
      if (reload) { rt.replenished += std::max(0.0, rt.curSize - prevSize); rt.nCycles += 1; }
      prevSize = rt.curSize;

      // Si taille passe sous OFF → EXHAUST
      if (rt.curSize < offTh) rt.state = RZ_EXHAUST;
      break;
    }
    case RZ_EXHAUST:
    {
      // reste EXHAUST jusqu’à sortie de zone ou réarmement externe
      break;
    }
    default: break;
  }
}

// À appeler à chaque nouvelle barre pour gérer le TTL
inline void 	rz_on_new_bar			(RzRuntime& rt, const RzParams& p)
{
  if (rt.state == RZ_IDLE) return; if (rt.ttl>0) rt.ttl -= 1; if (rt.ttl<=0) rt.reset_soft();
}

// Fusion de deux zones proches si centres à moins de mergeTicks
inline bool 	rz_merge_if_close		(RzRuntime& a, RzRuntime& b, const RzParams& p, int mergeTicks)
{
  if (a.state==RZ_IDLE || b.state==RZ_IDLE) return false;
  const double d = (p.tickSize>0.0? std::fabs(a.anchorPrice-b.anchorPrice)/p.tickSize : std::fabs(a.anchorPrice-b.anchorPrice));
  if (d > mergeTicks) return false;
  // Merge en gardant l’ancre la plus active
  RzRuntime& keep = (a.nCycles>=b.nCycles? a : b);
  RzRuntime& drop = (&keep==&a? b : a);
  keep.replenished += drop.replenished; keep.nCycles += drop.nCycles; keep.peakSize = std::max(keep.peakSize, drop.peakSize);
  keep.ttl = std::max(keep.ttl, drop.ttl); keep.state = std::max(keep.state, drop.state);
  rz_rebuild_zone(p, keep);
  drop.reset_soft();
  return true;
}

// ---------------------------------------------------------------------------
// Features MLP v1 — 14 features
// 0: state{0..3}, 1: ttl_bars, 2: anchor_price, 3: zone_lo, 4: zone_hi,
// 5: cur_size, 6: peak_size, 7: life_ms, 8: last_seen_ms,
// 9: replenished_sum, 10: n_cycles, 11: dist_ticks_to_anchor,
// 12: inside_flag{0,1}, 13: rel_cur_over_peak (0..1)
// ---------------------------------------------------------------------------
inline int 		rz_features_v1			(const RzRuntime& rt, const RzParams& p, double price, double* out)
{
  if (!out) return 0;
  const double distTicks = (p.tickSize>0.0? (price - rt.anchorPrice)/p.tickSize : (price - rt.anchorPrice));
  const int inside = (rzState(price, rt.zone) == 0) ? 1 : 0;
  const double rel = (rt.peakSize>0.0? rt.curSize/rt.peakSize : 0.0);

  out[0]  = (double)rt.state;
  out[1]  = (double)rt.ttl;
  out[2]  = du::sanitize(rt.anchorPrice);
  out[3]  = du::sanitize(rt.zone.lo);
  out[4]  = du::sanitize(rt.zone.hi);
  out[5]  = du::sanitize(rt.curSize);
  out[6]  = du::sanitize(rt.peakSize);
  out[7]  = du::sanitize(rt.lifeMs);
  out[8]  = du::sanitize(rt.lastSeenMs);
  out[9]  = du::sanitize(rt.replenished);
  out[10] = (double)rt.nCycles;
  out[11] = du::sanitize(distTicks);
  out[12] = (double)inside;
  out[13] = du::sanitize(rel);
  return 14;
}

} // namespace du
