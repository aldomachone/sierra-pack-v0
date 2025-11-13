#pragma once
#include <cstdio>
#include <cstring>
#include <cstddef>
#include "sierrachart.h"


namespace du {

// Politique I/O globale (Pack_v0)
// - Après MLP_IO_MAX_ERRORS erreurs OPEN/WRITE/FLUSH sur un même fichier,
//   les I/O MLP sont gelées pour au moins MLP_IO_RETRY_MS millisecondes.
// - Cette politique est appliquée via MLP_IoRegisterError() / MLP_IoIsDisabled().

#ifdef PACK_V0_IO_MAX_ERRORS
static const int MLP_IO_MAX_ERRORS = PACK_V0_IO_MAX_ERRORS;
#else
static const int MLP_IO_MAX_ERRORS = 100;    // nb max d'erreurs avant coupure
#endif

#ifdef PACK_V0_IO_RETRY_MS
static const int MLP_IO_RETRY_MS = PACK_V0_IO_RETRY_MS;
#else
static const int MLP_IO_RETRY_MS = 60000;    // délai de retry auto en ms
#endif
// ============================================================================
// REGISTRE MLP — PROFIL GLOBAL / CONVENTIONS
// ============================================================================
// 1) Version de schéma (MLP_SchemaVer)
//
//   - SchemaVer = 2
//       -> Toutes les variantes 2.x sont considérées COMPTABLES entre elles
//          (même ordre des champs, même sémantique).
//       -> Si une rupture majeure de format est introduite (ordre/champs changent),
//          incrémenter MLP_SchemaVer() à 3.
//
//   - Fichiers impactés :
//       * CSV : colonnes fixes + champs #INFO / #PARAMS / #META / #LAYOUT
//       * BIN : struct MLP_BinRowHeader + payload X[] (float32 ou float16)
//
// ---------------------------------------------------------------------------
// 2) Conventions de label / target MLP
//
//   - Label binaire dérivé des triggers :
//       * y = +1  -> TrigUp != 0  ET TrigDn == 0  (signal UP seul)
//       * y = -1  -> TrigDn != 0  ET TrigUp == 0  (signal DOWN seul)
//       * y =  0  -> aucun trigger ou conflit (TrgUp=TrgDn=0, ou les deux actifs)
//
//   - Horizon temporel (à fixer par engine / dataset) :
//       * y correspond au mouvement cible sur H barres / ms (à documenter côté notebook),
//         par ex. y = +1 si prix >= +X ticks dans les N prochaines barres.
//
//   - Le label n’est PAS stocké explicitement dans le header MLP,
//     il est reconstruit côté notebook à partir des colonnes TrigUp / TrigDn.
//
// ---------------------------------------------------------------------------
// 3) Conventions "featSig" (signature de layout)
//
//   - featSig = MLP_FeatureSignature(FeatureCount, FeatNames[])
//       * hash stable basé sur la liste ordonnée des noms de features.
//       * permet d’identifier un layout (X[0..N-1]) même si le fichier CSV/BIN
//         ne documente pas entièrement la structure.
//
//   - Règle : un changement d’ordre / ajout / suppression de feature DOIT
//     entraîner un nouveau featSig (calcul automatique), donc un nouveau layout.
//
//   - Utilisation :
//       * FeatSig est loggé dans :
//           - Header CSV (#INFO,…,FeatSig=…)
//           - Colonnes CSV (champ FeatSig ligne par ligne)
//           - Header BIN (champ FeatSig dans MLP_BinRowHeader)
//
// ---------------------------------------------------------------------------
// 4) Registre des engines MLP (à compléter au fur et à mesure)
//
//   Format recommandé par engine :
//
//   Engine : <Nom logique de l’engine>
//   -----------------------------------------------------------------
//   - Nom algo (AlgoName)    : "<AlgoName utilisé dans le CSV/BIN>"
//   - Version algo (AlgoVer) : <int>
//   - Description rapide     : <résumé en une ligne>
//   - FeatureCount           : <N> (taille de X[])
//   - Layout X[i] (ordre)    :
//         X[0]  = <NomFeature0>  (Subgraph / formule associée)
//         X[1]  = <NomFeature1>
//         ...
//         X[N-1]= <NomFeatureN-1>
//
//   - Notes :
//       * FeatSig réel = valeur observée dans les fichiers (champ FeatSig),
//         calculé par MLP_FeatureSignature(N, FeatNames[]).
//       * Si le layout change, incrémenter AlgoVer et noter la nouvelle version ici.
//
//   Exemple de fiche à remplir:
//
//   Engine : DOM_PULLSTACK_MOMENTUM_MLP
//   -----------------------------------------------------------------
//   - AlgoName    : "DOM_PULLSTACK"
//   - AlgoVer     : 1
//   - Description : Score momentum basé sur DOM Pull/Stack + gating qualité DOM
//   - FeatureCount: 8
//   - Layout X[i] :
//         X[0] = DomNearBid      (SG0)
//         X[1] = DomNearAsk      (SG1)
//         X[2] = DomMidBid       (SG2)
//         X[3] = DomMidAsk       (SG3)
//         X[4] = DomFarBid       (SG4)
//         X[5] = DomFarAsk       (SG5)
//         X[6] = SpreadTicks     (SG6)
//         X[7] = DomQualityScore (SG7)
//
//   - Notes :
//         FeatSig = <à relever dans un fichier réel>.
//
// ---------------------------------------------------------------------------
// 5) #META dans les CSV (profil contexte fichier)
//
//   - Ligne facultative de type :
//         #META,Symbol=<Sym>,Timeframe=<TF>,DOMDepth=<N>,<Extra>
//
//   - Exemple :
//         #META,Symbol=XBTUSD-BMEX,Timeframe=DOM_100ms,DOMDepth=60,
//               Engine=DOM_PULLSTACK,Side=BID
//
//   - Objectif :
//         Permet de relier rapidement un fichier CSV à son contexte
//         (symbole, timeframe logique, profondeur DOM, engine, côté BID/ASK, etc.)
// ============================================================================


////////////////////////////////////////////////////////////////////////////////////////////////////
// Constantes / version de schéma
//
// Politique de compatibilité :
// - SchemaVer = 2 : toutes les variantes 2.x sont supposées compatibles entre elles
// - Si tu fais une rupture de format (ordre/champs change), incrémente à 3

// Slots persistants MLP par défaut (peuvent être overridés) :
// 100..109 : CSV main
// 110..119 : BIN main
// 120..139 : signatures / params
// 140..149 : diagnostics X[] / sanitize
// 150..159 : registre erreurs I/O (MLP_TrackError)
// 200..209 : stats / normalisation (EMA score, etc.)
////////////////////////////////////////////////////////////////////////////////////////////////////// CSV/BIN: version de schéma
////// CSV/BIN: version de schéma
static inline constexpr int MLP_SchemaVer() { return 2; }

// Si on est inclus via le pack, verrouiller la cohérence de version.
#ifdef PACK_V0_SCHEMA_REV
static_assert(
  MLP_SchemaVer() == PACK_V0_SCHEMA_REV,
  "UtilitaireMLP.h: MLP_SchemaVer() doit rester synchronisé avec PACK_V0_SCHEMA_REV."
);
#endif

static inline int               MLP_BinMagic						()	{ return 0x4D4C5032 ; } 	// 'M' 'L' 'P' '2'

static inline int               MLP_DetectEndian              		()								// 0 = little endian, 1 = big endian
{
  unsigned 						x = 1u														;
  const unsigned char* 			p = (const unsigned char*)&x								;
  return 							(p[0] == 1u) ? 0 : 1									;
}

enum 							MLP_ErrorCode														// Classes d'erreur I/O pour le logger
{
  MLP_ERR_OPEN   				= 1,
  MLP_ERR_WRITE  				= 2,
  MLP_ERR_FLUSH  				= 3,
  MLP_ERR_FORMAT 				= 4
};

// Loggue une erreur I/O une seule fois par type (bitmask persistant)
static inline void              MLP_TrackError						(SCStudyInterfaceRef sc,
int                 			errCode	,
const char*         			msg		,
const int           			pMask = 150)
{
  int& mask = sc.GetPersistentInt(pMask);
  if(errCode <= 0 || errCode >= 31)
  {
    // fallback: log direct si code hors range
    sc.AddMessageToLog(msg, 1);
    return;
  }
  const int bit = (1 << errCode);
  if((mask & bit) != 0)
    return;

  mask |= bit;
  sc.AddMessageToLog(msg, 1);
}
static inline int 				MLP_MaxI							(const int a, const int b)	{ return a > b ? a : b; }
static inline int 				MLP_MinI							(const int a, const int b)	{ return a < b ? a : b; }
static inline int 				MLP_ClampI							(const int v, const int lo, const int hi)
{
  return v < lo ? lo : (v > hi ? hi : v);
}
static inline double 			MLP_ClampD							(const double v, const double lo, const double hi)
{
  return v < lo ? lo : (v > hi ? hi : v);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Gestion d'état I/O (coupure après trop d'erreurs + retry optionnel)
// 150 : bitmask erreurs (MLP_TrackError)
// 151 : flag I/O désactivée
// 152 : prochaine tentative de retry (ms)
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Bufferisation explicite des fichiers (CSV / BIN)
// Réduit le nombre d'appels I/O système pour les logs fréquents.
////////////////////////////////////////////////////////////////////////////////////////////////////

static const size_t 			MLP_IO_BUFFER_BYTES = 64 * 1024; // Initialisation à 4096 si préférences

static inline void 				MLP_InitFileBuffer					(FILE* f)
{
  if(!f)
    return;

  // Full buffered. Si setvbuf échoue, on ignore: stdio garde son buffer par défaut.
#if defined(_MSC_VER)
  setvbuf(f, nullptr, _IOFBF, (unsigned)MLP_IO_BUFFER_BYTES);
#else
  setvbuf(f, nullptr, _IOFBF, MLP_IO_BUFFER_BYTES);
#endif
}

static inline long long 		MLP_NowMs							(SCStudyInterfaceRef sc);
static inline bool              MLP_IoIsDisabled					(SCStudyInterfaceRef sc,
                                                                     int                 retryCooldownMs = -1,
                                                                     const int           pDisabled       = 151,
                                                                     const int           pNextRetry      = 152)
{
  int& disabled = sc.GetPersistentInt(pDisabled);
  if(!disabled)
    return false;

  if(retryCooldownMs <= 0)
    return true;

  int& nextRetry = sc.GetPersistentInt(pNextRetry);
  const long long now = MLP_NowMs(sc);

  if(now >= (long long)nextRetry)
  {
    // On réactive la tentative I/O après le cooldown
    disabled  = 0;
    nextRetry = 0;
    return false;
  }

  return true;
}

// Enregistre une erreur I/O et coupe les I/O après maxErr erreurs.
// Loggue un message générique de désactivation une seule fois (code errCode+16).
static inline void               MLP_IoRegisterError                (SCStudyInterfaceRef sc,
                                                                     int&                errCnt,
                                                                     int                 maxErr,
                                                                     int                 errCode,
                                                                     const char*         msgOnDisable,
                                                                     int                 retryCooldownMs = -1,
                                                                     const int           pDisabled       = 151,
                                                                     const int           pNextRetry      = 152)
{
  ++errCnt;

  if(maxErr <= 0)
    return;

  if(errCnt < maxErr)
    return;

  int& disabled = sc.GetPersistentInt(pDisabled);
  if(disabled)
    return;

  disabled = 1;

  if(retryCooldownMs > 0)
  {
    const long long now = MLP_NowMs(sc);
    int&            nextRetry = sc.GetPersistentInt(pNextRetry);
    nextRetry = (int)(now + retryCooldownMs);
  }

  if(msgOnDisable && msgOnDisable[0] != '\0')
  {
    const int disableCode = MLP_ClampI(errCode + 16, 1, 30);
    MLP_TrackError(sc, disableCode, msgOnDisable);
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Temps en ms
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline long long 		MLP_NowMs							(SCStudyInterfaceRef sc)
{
  const double days = sc.CurrentSystemDateTime.GetAsDouble(); // jours
  const double ms   = days * 24.0 * 60.0 * 60.0 * 1000.0;
  return (long long)(ms + 0.5);
}
static inline void 				MLP_TodayYMD						(SCStudyInterfaceRef sc, int& y, int& m, int& d)
{
  int h, mi, s;
  sc.CurrentSystemDateTime.GetDateTimeYMDHMS(y, m, d, h, mi, s);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// String utils
////////////////////////////////////////////////////////////////////////////////////////////////////
// Remplace une seule occurrence de "%Y%m%d" par la date du jour.
static inline 					SCString MLP_ApplyDatePattern		(SCStudyInterfaceRef sc, const SCString& pattern)
{
  const char* src = pattern.GetChars();
  const char* tag = strstr(src, "%Y%m%d");
  if(!tag)
    return pattern;

  int y, m, d;
  MLP_TodayYMD(sc, y, m, d);

  char ymd[9];
  snprintf(ymd, sizeof(ymd), "%04d%02d%02d", y, m, d);

  char 			out[1024]								;
  const size_t 	leftLen  = (size_t)(tag - src)			;
  const size_t 	rightLen = strlen(src) - leftLen - 7	;

  snprintf(out, sizeof(out), "%.*s%s%.*s",
           (int)leftLen, src,
           ymd,
           (int)rightLen, tag + 7);

  return SCString(out);
}
// Insère suffixe "_partNN" avant l'extension (si présente).
static inline 					SCString MLP_InsertPartSuffix		(const SCString& base, int part)
{
  const char* 	s   		= 	base.GetChars()	;
  const char* 	dot 		= 	strrchr(s, '.')	;
  char 			out				[1060]			;

  if(dot)
  {
    const size_t stemLen = (size_t)(dot - s);
    snprintf(out, sizeof(out), "%.*s_part%02d%s",
             (int)stemLen, s,
             MLP_ClampI(part, 0, 99),
             dot);
  }
  else
  {
    snprintf(out, sizeof(out), "%s_part%02d", s, MLP_ClampI(part, 0, 99));
  }

  return SCString(out);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Feature signature
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline unsigned 			MLP_FeatureSignature				(int featureCount, const char* const* featNames)
{
  unsigned sig = 2166136261u; // FNV-like

  for(int i = 0; i < featureCount; ++i)
  {
    const char* s = (featNames && featNames[i]) ? featNames[i] : "";
    while(*s)
    {
      sig ^= (unsigned)(unsigned char)(*s++);
      sig *= 16777619u;
    }
    sig ^= (unsigned)i;
    sig *= 16777619u;
  }

  sig ^= (unsigned)featureCount;
  return sig;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Validation des signatures de features
////////////////////////////////////////////////////////////////////////////////////////////////////

// Vérifie que (featureCount, featSig) match une référence attendue pour un algo donné.
// Loggue un message une seule fois par session si mismatch.
static inline void 				MLP_ValidateFeatSig					(
    SCStudyInterfaceRef sc,
    const char*         algoName,
    int                 schemaVer,
    int                 featureCount,
    const char* const*  featNames,
    int                 expectedCount,
    unsigned            expectedSig,
    const int           pWarnOnce = 160) // persistent pour "warn une seule fois"
{
  int& done = sc.GetPersistentInt(pWarnOnce);
  if(done)
    return;

  const unsigned sig = MLP_FeatureSignature(featureCount, featNames);

  if(sig == expectedSig && featureCount == expectedCount)
  {
    // Tout est cohérent, on ne checkera plus cette session
    done = 1;
    return;
  }

  SCString msg;
  msg.Format(
      "MLP_ValidateFeatSig[%s]: schema=%d, expected (count=%d, sig=%08X), got (count=%d, sig=%08X)",
      algoName,
      schemaVer,
      expectedCount,
      expectedSig,
      featureCount,
      sig);

  sc.AddMessageToLog(msg, 1);
  done = 1;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// CSV header
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void 				MLP_CSV_WriteHeader					(FILE*              f,
                                       int                schemaVer,
                                       int                algoVer,
                                       const char*        algoName,
                                       int                contractVer,
                                       int                sessionId,
                                       int                featureCount,
                                       const char* const* featNames,
                                       unsigned           featSig)
{
  if(!f)
    return;

  fprintf(
      f,
      "UTC_MS,Symbol,Algo,AlgoVer,SchemaVer,ContractVer,SessionId,BarIndex,GateOK,Stale,ScoreRaw,ScoreEMA,ScoreNorm,TrigUp,TrigDn,FeatSig");

  for(int i = 0; i < featureCount; ++i)
  {
    if(featNames && featNames[i] && featNames[i][0])
      fprintf(f, ",X%02d_%s", i, featNames[i]);
    else
      fprintf(f, ",X%02d", i);
  }

  fprintf(f, "\n");

  fprintf(f,
          "#INFO,Algo=%s,AlgoVer=%d,SchemaVer=%d,ContractVer=%d,SessionId=%d,FeatCount=%d,FeatSig=%u\n",
          algoName ? algoName : "",
          algoVer,
          schemaVer,
          contractVer,
          sessionId,
          featureCount,
          featSig);
}
// Ligne de profil optionnelle pour relier rapidement un fichier à son contexte.
// A appeler une fois après ouverture du CSV si tu connais ces infos.
static inline void              MLP_CSV_WriteMetaProfile      		(FILE*       f,
                                                               const char* symbol,
                                                               const char* timeframe,
                                                               int         domDepth,
                                                               const char* extra)
{
  if(!f)
    return;

  const char* s  = symbol    ? symbol    : "";
  const char* tf = timeframe ? timeframe : "";
  const char* ex = extra     ? extra     : "";

  // ex peut contenir des paires k=v supplémentaires, par ex:
  // "Engine=DOM_PULLSTACK,Side=BID"
  fprintf(f,
          "#META,Symbol=%s,Timeframe=%s,DOMDepth=%d,%s\n",
          s, tf, domDepth, ex);
}

static inline void              MLP_CSV_MaybeWriteMeta           (SCStudyInterfaceRef sc,
                                                               FILE*               f,
                                                               const char*         symbol,
                                                               const char*         timeframe,
                                                               int                 domDepth,
                                                               const char*         extra,
                                                               const int           pFlag = 132)
{
  if(!f)
    return;

  int& done = sc.GetPersistentInt(pFlag);
  if(done)
    return;

  MLP_CSV_WriteMetaProfile(f, symbol, timeframe, domDepth, extra);
  done = 1;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Utils fichier
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline long 				MLP_FileTellEnd						(FILE* f)
{
  if(!f)	return 0				;

  fseek	(f, 0, SEEK_END)			;
  long 	 pos 			= ftell(f)	;
  if	(pos < 0) pos 	= 0			;
  return pos						;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// CSV Open + rotation (date + taille) avec gestion d'état I/O
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline FILE*             MLP_CSV_OpenIfNeeded               (SCStudyInterfaceRef sc,
                                                                     const SCString&     rotatingPattern,
                                                                     const char*         algoName,
                                                                     int                 algoVer,
                                                                     int                 contractVer,
                                                                     int                 featureCount,
                                                                     const char* const*  featNames,
                                                                     const int           maxBytesPerPart, // octets
                                                                     const int           flushIdleMs,     // ms
                                                                     const int           pFileId   = 100,
                                                                     const int           pHdrFlag  = 101,
                                                                     const int           pDateYMD  = 102,
                                                                     const int           pSession  = 103,
                                                                     const int           pPartNo   = 104,
                                                                     const int           pBytes    = 105,
                                                                     const int           pLastFlMs = 106,
                                                                     const int           pErrCnt   = 107)
{
  int& lastYMD   = sc.GetPersistentInt(pDateYMD);
  int& sessionId = sc.GetPersistentInt(pSession);
  int& partNo    = sc.GetPersistentInt(pPartNo);
  int& nbytes    = sc.GetPersistentInt(pBytes);
  int& hdrDone   = sc.GetPersistentInt(pHdrFlag);
  int& lastFlush = sc.GetPersistentInt(pLastFlMs);
  int& errCnt    = sc.GetPersistentInt(pErrCnt);



	// Si I/O coupée, on ne tente rien
	if(MLP_IoIsDisabled(sc, MLP_IO_RETRY_MS))
  return nullptr;


  int y, m, d;
  MLP_TodayYMD(sc, y, m, d);
  const int todayYMD = y * 10000 + m * 100 + d;

  if(sessionId == 0)
    sessionId = (int)(MLP_NowMs(sc) & 0x7FFFFFFF);

  FILE* f = (FILE*)sc.GetPersistentPointer(pFileId);

  // Rotation par date
  if(f && lastYMD != todayYMD)
  {
    if(fflush(f) != 0)
    {
      MLP_TrackError(sc, MLP_ERR_FLUSH, "MLP_CSV_OpenIfNeeded #ERROR: fflush failed (date rotate)");
      MLP_IoRegisterError(
          sc,
          errCnt,
		  MLP_IO_MAX_ERRORS,
          MLP_ERR_FLUSH,
          "MLP_CSV_OpenIfNeeded #ERROR: I/O disabled after too many FLUSH errors (date rotate)",
          MLP_IO_RETRY_MS);
    }
    fclose(f);
    f = nullptr;
    sc.SetPersistentPointer(pFileId, nullptr);
    hdrDone = 0;
    nbytes  = 0;
    partNo  = 0;
  }

  // Ouverture
  if(!f)
  {
    SCString path = MLP_ApplyDatePattern(sc, rotatingPattern);
    path          = MLP_InsertPartSuffix(path, partNo);

  #ifdef _MSC_VER
    fopen_s(&f, path.GetChars(), "ab");
  #else
    f = fopen(path.GetChars(), "ab");
  #endif

    if(!f)
    {
      SCString msg;
      msg.Format("MLP_CSV_OpenIfNeeded #ERROR: fopen failed path=%s", path.GetChars());
      MLP_TrackError(sc, MLP_ERR_OPEN, msg.GetChars());

      MLP_IoRegisterError(
          sc,
          errCnt,
          MLP_IO_MAX_ERRORS,
          MLP_ERR_OPEN,
          "MLP_CSV_OpenIfNeeded #ERROR: I/O disabled after too many OPEN errors",
          MLP_IO_RETRY_MS);

      return nullptr;
    }

    // Bufferisation explicite avant tout I/O sur f
    MLP_InitFileBuffer(f);

    sc.SetPersistentPointer(pFileId, f);
    lastYMD   = todayYMD;
    hdrDone   = 0;
    nbytes    = (int)MLP_FileTellEnd(f);
    lastFlush = (int)MLP_NowMs(sc);
  }

  // Entête
  if(f && hdrDone == 0)
  {
    const unsigned featSig = MLP_FeatureSignature(featureCount, featNames);
    MLP_CSV_WriteHeader(
        f,
        MLP_SchemaVer(),
        algoVer,
        algoName,
        contractVer,
        sessionId,
        featureCount,
        featNames,
        featSig);

    nbytes  = (int)MLP_FileTellEnd(f);
    hdrDone = 1;
  }

  // Flush inactivité
  if(f && flushIdleMs > 0)
  {
    const long long now = MLP_NowMs(sc);
    if(now - (long long)lastFlush >= (long long)flushIdleMs)
    {
      if(fflush(f) != 0)
      {
        MLP_TrackError(sc, MLP_ERR_FLUSH, "MLP_CSV_OpenIfNeeded #ERROR: fflush failed (idle flush)");
        MLP_IoRegisterError(
            sc,
            errCnt,
            MLP_IO_MAX_ERRORS,
            MLP_ERR_FLUSH,
            "MLP_CSV_OpenIfNeeded #ERROR: I/O disabled after too many FLUSH errors (idle flush)",
            MLP_IO_RETRY_MS);
      }
      lastFlush = (int)now;
    }
  }

  // Rotation par taille
  if(f && maxBytesPerPart > 0 && nbytes >= maxBytesPerPart)
  {
    if(fflush(f) != 0)
    {
      MLP_TrackError(sc, MLP_ERR_FLUSH, "MLP_CSV_OpenIfNeeded #ERROR: fflush failed (size rotate)");
      MLP_IoRegisterError(
          sc,
          errCnt,
          MLP_IO_MAX_ERRORS,
          MLP_ERR_FLUSH,
          "MLP_CSV_OpenIfNeeded #ERROR: I/O disabled after too many FLUSH errors (size rotate)",
          MLP_IO_RETRY_MS);
    }
    fclose(f);
    f = nullptr;
    sc.SetPersistentPointer(pFileId, nullptr);
    hdrDone = 0;
    nbytes  = 0;
    partNo  = MLP_ClampI(partNo + 1, 0, 99);

    SCString path = MLP_ApplyDatePattern(sc, rotatingPattern);
    path          = MLP_InsertPartSuffix(path, partNo);

  #ifdef _MSC_VER
    fopen_s(&f, path.GetChars(), "ab");
  #else
    f = fopen(path.GetChars(), "ab");
  #endif

    if(!f)
    {
      SCString msg;
      msg.Format("MLP_CSV_Rotate #ERROR: fopen failed path=%s", path.GetChars());
      MLP_TrackError(sc, MLP_ERR_OPEN, msg.GetChars());

      MLP_IoRegisterError(
          sc,
          errCnt,
          MLP_IO_MAX_ERRORS,
          MLP_ERR_OPEN,
          "MLP_CSV_OpenIfNeeded #ERROR: I/O disabled after too many OPEN errors (rotate)",
          MLP_IO_RETRY_MS);

      return nullptr;
    }

    // Bufferisation explicite avant tout I/O sur f
    MLP_InitFileBuffer(f);

    sc.SetPersistentPointer(pFileId, f);

    const unsigned featSig = MLP_FeatureSignature(featureCount, featNames);
    MLP_CSV_WriteHeader(
        f,
        MLP_SchemaVer(),
        algoVer,
        algoName,
        contractVer,
        sessionId,
        featureCount,
        featNames,
        featSig);

    nbytes    = (int)MLP_FileTellEnd(f);
    lastFlush = (int)MLP_NowMs(sc);
  }

  return f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CSV ligne
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline int               MLP_CSV_LogRow                      (FILE*          f, 
                                                                     long long      utcMs,
                                                                     const char*    symbol,
                                                                     const char*    algoName,
                                                                     int            algoVer,
                                                                     int            contractVer,
                                                                     int            sessionId,
                                                                     int            barIndex,
                                                                     int            gateOK,
                                                                     int            stale,
                                                                     double         scoreRaw,
                                                                     double         scoreEMA,
                                                                     double         scoreNorm,
                                                                     int            trigUp,
                                                                     int            trigDn,
                                                                     unsigned       featSig,
                                                                     const double*  X,
                                                                     int            featureCount,
                                                                     int&           errCnt)
{
  if(!f)
    return 0;

  long delta = 0;
  int  r     = 0;

  // Sécurité si symbol / algoName sont nullptr
  const char* sym = symbol   ? symbol   : "";
  const char* alg = algoName ? algoName : "";

  // Ligne fixe (sans les features)
  r = fprintf(
          f,
          "%lld,%s,%s,%d,%d,%d,%d,%d,%d,%d,%.10g,%.10g,%.10g,%d,%d,%u",
          utcMs,
          sym,
          alg,
          algoVer,
          MLP_SchemaVer(),
          contractVer,
          sessionId,
          barIndex,
          gateOK,
          stale,
          scoreRaw,
          scoreEMA,
          scoreNorm,
          trigUp,
          trigDn,
          featSig);
  if(r < 0)
  {
    ++errCnt;
    return 0;
  }
  delta += r;

  // Features
  for(int i = 0; i < featureCount; ++i)
  {
    r = fprintf(f, ",%.10g", X ? X[i] : 0.0);
    if(r < 0)
    {
      ++errCnt;
      return 0;
    }
    delta += r;
  }

  // Fin de ligne
  r = fprintf(f, "\n");
  if(r < 0)
  {
    ++errCnt;
    return 0;
  }
  delta += r;

  if(delta <= 0)
    return 0;

  if(delta > 0x7FFFFFFF)
    delta = 0x7FFFFFFF;

  return (int)delta;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CSV LogRow + accumulation de bytes avec gestion d'erreurs I/O
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline int               MLP_CSV_LogRowAcc                  (SCStudyInterfaceRef sc,
                                                                     FILE*              f,
                                                                     long long          utcMs,
                                                                     const char*        symbol,
                                                                     const char*        algoName,
                                                                     int                algoVer,
                                                                     int                contractVer,
                                                                     int                sessionId,
                                                                     int                barIndex,
                                                                     int                gateOK,
                                                                     int                stale,
                                                                     double             scoreRaw,
                                                                     double             scoreEMA,
                                                                     double             scoreNorm,
                                                                     int                trigUp,
                                                                     int                trigDn,
                                                                     unsigned           featSig,
                                                                     const double*      X,
                                                                     int                featureCount,
                                                                     int&               errCnt,
                                                                     const int          pBytes = 105)
{
  // On mémorise l'état avant, pour détecter une vraie erreur I/O
  const int errBefore = errCnt;

  int delta = MLP_CSV_LogRow(
      f,
      utcMs,
      symbol,
      algoName,
      algoVer,
      contractVer,
      sessionId,
      barIndex,
      gateOK,
      stale,
      scoreRaw,
      scoreEMA,
      scoreNorm,
      trigUp,
      trigDn,
      featSig,
      X,
      featureCount,
      errCnt);

  // delta <= 0 : pas de bytes écrits (erreur ou fichier nul)
  if(delta <= 0)
  {
    // Si errCnt a augmenté, on a bien une erreur d'écriture/flush
    if(errCnt > errBefore)
    {
      
      MLP_TrackError(sc, MLP_ERR_WRITE, "MLP_CSV_LogRowAcc #ERROR: write/flush failed");
      MLP_IoRegisterError(
          sc,
          errCnt,
          MLP_IO_MAX_ERRORS,
          MLP_ERR_WRITE,
          "MLP_CSV_LogRowAcc #ERROR: I/O disabled after too many WRITE errors",
          MLP_IO_RETRY_MS);
    }

    return delta;
  }

  int& nbytes = sc.GetPersistentInt(pBytes);
  nbytes += delta;
  return delta;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// CSV hyper-params + #PARAMS on-change
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void 				MLP_CSV_LogParams					(FILE*   f,
                                     const char* algoName,
                                     int     algoVer,
                                     int     contractVer,
                                     int     sessionId,
                                     double  alphaPct,
                                     double  thUp,
                                     double  thDn,
                                     int     refrMs,
                                     int     warmupBars,
                                     int&    errCnt)
{
  if(!f)
    return;

  if(fprintf(
         f,
         "#PARAMS,Algo=%s,AlgoVer=%d,SchemaVer=%d,ContractVer=%d,SessionId=%d,AlphaPct=%.3f,ThUp=%.6f,ThDn=%.6f,RefractMs=%d,WarmupBars=%d\n",
         algoName ? algoName : "",
         algoVer,
         MLP_SchemaVer(),
         contractVer,
         sessionId,
         alphaPct,
         thUp,
         thDn,
         refrMs,
         warmupBars) < 0)
  {
    ++errCnt;
  }
}
// Hash pour signature hyper-paramètres
static inline unsigned 			MLP_HashU32Mix						(unsigned h, unsigned v)
{
  h ^= v + 0x9e3779b9u + (h << 6) + (h >> 2);
  return h;
}
static inline unsigned 			MLP_EncodeDouble1e6					(double x)
{
  const double scale = 1000000.0;
  long long    q     = (long long)(x * scale);
  unsigned     lo    = (unsigned)q;
  unsigned     hi    = (unsigned)((unsigned long long)q >> 32);
  return lo ^ hi;
}
static inline unsigned 			MLP_ParamsSignature					(double alphaPct,
                                           double thUp,
                                           double thDn,
                                           int    refrMs,
                                           int    warmupBars)
{
  unsigned h = 2166136261u;

  h = MLP_HashU32Mix(h, MLP_EncodeDouble1e6(alphaPct));
  h = MLP_HashU32Mix(h, MLP_EncodeDouble1e6(thUp));
  h = MLP_HashU32Mix(h, MLP_EncodeDouble1e6(thDn));
  h = MLP_HashU32Mix(h, (unsigned)refrMs);
  h = MLP_HashU32Mix(h, (unsigned)warmupBars);

  return h;
}
static inline void 				MLP_CSV_MaybeLogParams				(SCStudyInterfaceRef sc,
                                          FILE*              f,
                                          const char*        algoName,
                                          int                algoVer,
                                          int                contractVer,
                                          int                sessionId,
                                          double             alphaPct,
                                          double             thUp,
                                          double             thDn,
                                          int                refrMs,
                                          int                warmupBars,
                                          int&               errCnt,
                                          const int          pParamSig = 120)
{
  if(!f)
    return;

  unsigned sig = MLP_ParamsSignature(alphaPct, thUp, thDn, refrMs, warmupBars);
  int&     lastSig = sc.GetPersistentInt(pParamSig);

  if((unsigned)lastSig == sig)
    return;

  lastSig = (int)sig;

  MLP_CSV_LogParams(
      f,
      algoName,
      algoVer,
      contractVer,
      sessionId,
      alphaPct,
      thUp,
      thDn,
      refrMs,
      warmupBars,
      errCnt);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// CSV close
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void 				MLP_CSV_CloseIfOpen					(SCStudyInterfaceRef sc, const int pFileId = 100)
{
  FILE* f = (FILE*)sc.GetPersistentPointer(pFileId);
  if(f)
  {
    if(fflush(f) != 0)
    {
      MLP_TrackError(sc, MLP_ERR_FLUSH, "MLP_CSV_CloseIfOpen #ERROR: fflush failed");
    }

    fclose(f);
    sc.SetPersistentPointer(pFileId, nullptr);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// BIN (header + open/rotate + write)
////////////////////////////////////////////////////////////////////////////////////////////////////
enum MLP_DType
{
  MLP_DTYPE_F32 = 1,
  MLP_DTYPE_F16 = 2
};

// Magic + version de header fichier BIN
static const int 				MLP_BIN_MAGIC_FILE      = 0x4D4C5046						; // 'MLPF'
static const int 				MLP_BIN_FILE_HEADER_VER = 1									;

struct 							MLP_BinFileHeader
{
  int      						MagicFile													;	// ex: 0x4D4C5046 = 'MLPF'
  int      						HeaderVer													;	// 1
  int      						SchemaVer													;
  int      						AlgoVer														;
  int      						ContractVer													;
  int      						SessionId													;
  int      						FeatureCount												;
  unsigned 						FeatSig														;
  int      						DType														;	// F32/F16
  int      						MetaLen														;	// 0 pour l’instant
  int      						ParamsLen													;	// 0 pour l’instant
  int      						Reserved[4]													;	// 0
}																							;
// Verrous de layout binaire (doc + garde-fou)
static_assert(sizeof(MLP_BinFileHeader) == 60,
              "MLP_BinFileHeader: taille inattendue, format BIN potentiellement cassé.");
static_assert(offsetof(MLP_BinFileHeader, DType) == 32,
              "MLP_BinFileHeader: offset DType modifié, vérifier compat BIN.");
 
struct                          MLP_BinRowHeader											{
  long long 					UtcMs														;
  int       					BarIndex													;
  int       					GateOK														;
  int       					Stale														;
  float     					ScoreRaw													;
  float     					ScoreEMA													;
  int       					TrigUp														;
  int       					TrigDn														;
  int       					FeatureCount												;
  unsigned  					FeatSig														;
  int       					ContractVer													;
  int       					SessionId													;
  int       					AlgoVer														;
  int       					SchemaVer													;
  int       					DType														;	// MLP_DTYPE_F32 ou MLP_DTYPE_F16 pour X
  int       					Magic														;	// 0x4D4C5032 = 'MLP2'
  int       					Endian														;	// 0 = little, 1 = big
};
static_assert(sizeof(MLP_BinRowHeader) == 64,
              "MLP_BinRowHeader: taille inattendue, format BIN potentiellement cassé.");
static_assert(offsetof(MLP_BinRowHeader, DType) == 60,
              "MLP_BinRowHeader: offset DType modifié, vérifier compat BIN.");

static inline 					FILE* MLP_BIN_Get					(SCStudyInterfaceRef sc, const int pFileId = 110)
{
  return (FILE*)sc.GetPersistentPointer(pFileId);
}
static inline void 				MLP_BIN_Set							(SCStudyInterfaceRef sc, FILE* f, const int pFileId = 110)
{
  sc.SetPersistentPointer(pFileId, f);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// BIN Open + rotation (date + taille) avec gestion d'état I/O + bufferisation
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline FILE*             MLP_BIN_OpenIfNeeded               (SCStudyInterfaceRef sc	,
const SCString&					rotatingPattern												,
const int      					maxBytesPerPart												,
const int      					pFileId 				= 110								,
const int      					pDateYMD				= 112								,
const int      					pPartNo 				= 113								,
const int      					pBytes  				= 114								,
const int      					pErrCnt 				= 115								)
{
int&							lastYMD 				= sc.GetPersistentInt(pDateYMD)		;
int&							partNo  				= sc.GetPersistentInt(pPartNo)		;
int&							nbytes  				= sc.GetPersistentInt(pBytes)		;
int&							errCnt  				= sc.GetPersistentInt(pErrCnt)		;

  // Politique I/O : coupure après erreurs + retry
  if(MLP_IoIsDisabled(sc, MLP_IO_RETRY_MS))
    return nullptr;

  int y, m, d;
  MLP_TodayYMD(sc, y, m, d);
  const int todayYMD = y * 10000 + m * 100 + d;

  FILE* f = MLP_BIN_Get(sc, pFileId);

  // Rotation par date
  if(f && lastYMD != todayYMD)
  {
    if(fflush(f) != 0)
    {
      MLP_TrackError(sc, MLP_ERR_FLUSH, "MLP_BIN_OpenIfNeeded #ERROR: fflush failed (date rotate)");
      MLP_IoRegisterError(
          sc,
          errCnt,
          MLP_IO_MAX_ERRORS,
          MLP_ERR_FLUSH,
          "MLP_BIN_OpenIfNeeded #ERROR: I/O disabled after too many FLUSH errors (date rotate)",
          MLP_IO_RETRY_MS);
    }
    fclose(f);
    f = nullptr;
    MLP_BIN_Set(sc, nullptr, pFileId);
    nbytes = 0;
    partNo = 0;
  }

  // Ouverture
  if(!f)
  {
    SCString path = MLP_ApplyDatePattern(sc, rotatingPattern);
    path          = MLP_InsertPartSuffix(path, partNo);

  #ifdef _MSC_VER
    fopen_s(&f, path.GetChars(), "ab");
  #else
    f = fopen(path.GetChars(), "ab");
  #endif

    if(!f)
    {
      SCString msg;
      msg.Format("MLP_BIN_OpenIfNeeded #ERROR: fopen failed path=%s", path.GetChars());
      MLP_TrackError(sc, MLP_ERR_OPEN, msg.GetChars());
      MLP_IoRegisterError(
          sc,
          errCnt,
          MLP_IO_MAX_ERRORS,
          MLP_ERR_OPEN,
          "MLP_BIN_OpenIfNeeded #ERROR: I/O disabled after too many OPEN errors",
          MLP_IO_RETRY_MS);
      return nullptr;
    }

    // Bufferisation explicite avant tout fwrite
    MLP_InitFileBuffer(f);

    MLP_BIN_Set(sc, f, pFileId);
    lastYMD = todayYMD;
    nbytes  = (int)MLP_FileTellEnd(f);
  }

  // Rotation par taille
  if(f && maxBytesPerPart > 0 && nbytes >= maxBytesPerPart)
  {
    if(fflush(f) != 0)
    {
      MLP_TrackError(sc, MLP_ERR_FLUSH, "MLP_BIN_OpenIfNeeded #ERROR: fflush failed (size rotate)");
      MLP_IoRegisterError(
          sc,
          errCnt,
          MLP_IO_MAX_ERRORS,
          MLP_ERR_FLUSH,
          "MLP_BIN_OpenIfNeeded #ERROR: I/O disabled after too many FLUSH errors (size rotate)",
          MLP_IO_RETRY_MS);
    }
    fclose(f);
    f = nullptr;
    MLP_BIN_Set(sc, nullptr, pFileId);
    nbytes = 0;
    partNo = MLP_ClampI(partNo + 1, 0, 99);

    SCString path = MLP_ApplyDatePattern(sc, rotatingPattern);
    path          = MLP_InsertPartSuffix(path, partNo);

  #ifdef _MSC_VER
    fopen_s(&f, path.GetChars(), "ab");
  #else
    f = fopen(path.GetChars(), "ab");
  #endif

    if(!f)
    {
      SCString msg;
      msg.Format("MLP_BIN_Rotate #ERROR: fopen failed path=%s", path.GetChars());
      MLP_TrackError(sc, MLP_ERR_OPEN, msg.GetChars());
      MLP_IoRegisterError(
          sc,
          errCnt,
          MLP_IO_MAX_ERRORS,
          MLP_ERR_OPEN,
          "MLP_BIN_OpenIfNeeded #ERROR: I/O disabled after too many OPEN errors (rotate)",
          MLP_IO_RETRY_MS);
      return nullptr;
    }

    // Bufferisation explicite avant tout fwrite
    MLP_InitFileBuffer(f);

    MLP_BIN_Set(sc, f, pFileId);
    nbytes = (int)MLP_FileTellEnd(f);
  }

  return f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Float16 (IEEE 754 half) conversion minimaliste
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline unsigned short 	MLP_F32_to_F16						(float f)
{
  union
  {
    float    f;
    unsigned u;
  } v{f};

  unsigned sign = (v.u >> 31) & 0x1;
  unsigned exp  = (v.u >> 23) & 0xFF;
  unsigned mant =  v.u & 0x7FFFFF;

  if(exp == 255)
  {
    // Inf/NaN
    unsigned short h = (unsigned short)((sign << 15) | 0x7C00 | (mant ? 0x1 : 0));
    return h;
  }

  int newExp = (int)exp - 127 + 15;

  if(newExp >= 0x1F)
  {
    // overflow -> inf
    return (unsigned short)((sign << 15) | 0x7C00);
  }

  if(newExp <= 0)
  {
    if(newExp < -10)
      return (unsigned short)(sign << 15);

    mant |= 0x800000;
    int shift      = 14 - newExp;
    unsigned short h = (unsigned short)((sign << 15) | (mant >> shift));
    return h;
  }

  unsigned short h = (unsigned short)((sign << 15) | (newExp << 10) | (mant >> 13));
  return h;
}
static inline int 				MLP_BIN_LogRow(
    FILE*                  f,
    const MLP_BinRowHeader& H,
    const double*          X,
    int                    featureCount,
    int                    useF16, // MLP_DTYPE_F32 ou MLP_DTYPE_F16
    int&                   errCnt)
{
  if(!f)
    return 0;

  long delta = 0;

  // Header binaire
  if(fwrite(&H, sizeof(H), 1, f) != 1)
  {
    ++errCnt;
    return 0;
  }
  delta += (long)sizeof(H);

  // Payload features
  if(featureCount > 0 && X)
  {
    if(useF16 == MLP_DTYPE_F16)
    {
      for(int i = 0; i < featureCount; ++i)
      {
        unsigned short h = MLP_F32_to_F16((float)X[i]);
        if(fwrite(&h, sizeof(h), 1, f) != 1)
        {
          ++errCnt;
          return 0;
        }
        delta += (long)sizeof(h);
      }
    }
    else // F32
    {
      for(int i = 0; i < featureCount; ++i)
      {
        float xf = (float)X[i];
        if(fwrite(&xf, sizeof(xf), 1, f) != 1)
        {
          ++errCnt;
          return 0;
        }
        delta += (long)sizeof(xf);
      }
    }
  }

  if(delta <= 0)
    return 0;

  if(delta > 0x7FFFFFFF)
    delta = 0x7FFFFFFF;

  return (int)delta;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// BIN LogRow + accumulate bytes + TrackError sur erreurs d'écriture
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// BIN LogRow + accumulate bytes + header fichier + TrackError sur erreurs d'écriture
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline int 				MLP_BIN_LogRowAcc(
    SCStudyInterfaceRef    sc,
    FILE*                  f,
    const MLP_BinRowHeader& H,
    const double*          X,
    int                    featureCount,
    int                    useF16,
    int&                   errCnt,
    const int              pBytes = 114)
{
  if(!f)
    return 0;

  // Compteur de taille fichier pour cette part
  int& nbytes = sc.GetPersistentInt(pBytes);

  // Si fichier encore vide pour cette part -> écrire le header fichier BIN minimal
  if(nbytes == 0)
  {
    MLP_BinFileHeader FH;
    FH.MagicFile   = MLP_BIN_MAGIC_FILE;
    FH.HeaderVer   = MLP_BIN_FILE_HEADER_VER;

    // Infos globales reprises depuis le header de ligne H
    FH.SchemaVer   = H.SchemaVer;
    FH.AlgoVer     = H.AlgoVer;
    FH.ContractVer = H.ContractVer;
    FH.SessionId   = H.SessionId;
    FH.FeatureCount= H.FeatureCount;
    FH.FeatSig     = H.FeatSig;
    FH.DType       = H.DType;

    FH.MetaLen     = 0;
    FH.ParamsLen   = 0;
    FH.Reserved[0] = 0;
    FH.Reserved[1] = 0;
    FH.Reserved[2] = 0;
    FH.Reserved[3] = 0;

    if(fwrite(&FH, sizeof(FH), 1, f) != 1)
    {
      ++errCnt;
      MLP_TrackError(sc, MLP_ERR_WRITE, "MLP_BIN_LogRowAcc #ERROR: file header write failed");
      MLP_IoRegisterError(
          sc,
          errCnt,
          MLP_IO_MAX_ERRORS,
          MLP_ERR_WRITE,
          "MLP_BIN_LogRowAcc #ERROR: I/O disabled after too many WRITE errors (file header)",
          MLP_IO_RETRY_MS);
      return 0;
    }

    nbytes += (int)sizeof(FH);
  }

  const int errBefore = errCnt;

  int delta = MLP_BIN_LogRow(
      f,
      H,
      X,
      featureCount,
      useF16,
      errCnt);

  if(delta <= 0)
  {
    if(errCnt > errBefore)
    {
      MLP_TrackError(sc, MLP_ERR_WRITE, "MLP_BIN_LogRowAcc #ERROR: write failed");
      MLP_IoRegisterError(
          sc,
          errCnt,
          MLP_IO_MAX_ERRORS,
          MLP_ERR_WRITE,
          "MLP_BIN_LogRowAcc #ERROR: I/O disabled after too many WRITE errors",
          MLP_IO_RETRY_MS);
    }
    return delta;
  }

  // Mise à jour de la taille totale
  nbytes += delta;
  return delta;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Close + reset persistants CSV/BIN
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void 				MLP_CloseAllIfOpen_Ext				(SCStudyInterfaceRef sc,
                                          const int          pCsvFileId   = 100,
                                          const int          pCsvHdrFlag  = 101,
                                          const int          pCsvDateYMD  = 102,
                                          const int          pCsvSession  = 103,
                                          const int          pCsvPartNo   = 104,
                                          const int          pCsvBytes    = 105,
                                          const int          pCsvLastFlMs = 106,
                                          const int          pCsvErrCnt   = 107,
                                          const int          pBinFileId   = 110,
                                          const int          pBinDateYMD  = 112,
                                          const int          pBinPartNo   = 113,
                                          const int          pBinBytes    = 114,
                                          const int          pBinErrCnt   = 115)
{
///////////////////////////////////////////////////////////////////// Fermeture CSV
  MLP_CSV_CloseIfOpen(sc, pCsvFileId);

///////////////////////////////////////////////////////////////////// Fermeture BIN
  FILE* fb = MLP_BIN_Get(sc, pBinFileId);
  if(fb)
  {
    if(fflush(fb) != 0)
      MLP_TrackError(sc, MLP_ERR_FLUSH, "MLP_CloseAllIfOpen_Ext #ERROR: fflush failed (BIN)");

    fclose		(fb)												;
    MLP_BIN_Set	(sc, nullptr, pBinFileId)							;
  }

///////////////////////////////////////////////////////////////////// Reset CSV
  int&	csvHdr   			= sc.GetPersistentInt(pCsvHdrFlag)		;
  int&	csvDate  			= sc.GetPersistentInt(pCsvDateYMD)		;
  int&	csvSess  			= sc.GetPersistentInt(pCsvSession)		;
  int&	csvPart  			= sc.GetPersistentInt(pCsvPartNo)		;
  int&	csvBytes 			= sc.GetPersistentInt(pCsvBytes)		;
  int&	csvLastFl			= sc.GetPersistentInt(pCsvLastFlMs)		;
  int&	csvErr   			= sc.GetPersistentInt(pCsvErrCnt)		;
		
		csvHdr   			= 0										;
		csvDate  			= 0										;
		csvSess  			= 0										;
		csvPart  			= 0										;
		csvBytes 			= 0										;
		csvLastFl			= 0										;
		csvErr   			= 0										;
	
///////////////////////////////////////////////////////////////////// Reset BIN	
  int&	binDate 			= sc.GetPersistentInt(pBinDateYMD)		;
  int&	binPart 			= sc.GetPersistentInt(pBinPartNo)		;
  int&	binBytes			= sc.GetPersistentInt(pBinBytes)		;
  int&	binErr  			= sc.GetPersistentInt(pBinErrCnt)		;
	
		binDate 			= 0										;
		binPart 			= 0										;
		binBytes			= 0										;
		binErr  			= 0										;

///////////////////////////////////////////////////////////////////// Reset état I/O global (coupure / retry)
  sc.GetPersistentInt(151)	= 0										;
  sc.GetPersistentInt(152)	= 0										;
}

///////////////////////////////////////////////////////////////////// Variante simple (ancienne API)
static inline void 				MLP_CloseAllIfOpen					(SCStudyInterfaceRef sc)
{
  MLP_CloseAllIfOpen_Ext	(sc)									;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers vecteur de features X[]
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void 				MLP_FillX_FromSubgraphs				(SCStudyInterfaceRef sc,
                                           const int*         sgIndices,
                                           int                featureCount,
                                           int                barIndex,
                                           double*            X)
{
  if(!X || !sgIndices || featureCount <= 0)
    return;

  const int nsg = 60			; // Sierra Chart expose 60 subgraphs

  const int asz = sc.ArraySize	;
  const int idx = barIndex		;

  const int validBar = (idx >= 0 && idx < asz) ? idx : (asz > 0 ? asz - 1 : 0);

  for(int i = 0; i < featureCount; ++i)
  {
    int sg = sgIndices[i];
    if(sg >= 0 && sg < nsg)
      X[i] = sc.Subgraph[sg].Data[validBar];
    else
      X[i] = 0.0;
  }
}
static inline void				MLP_FillX_FromRange					(SCStudyInterfaceRef sc,
                                int                 				sgFirst		,
                                int                 				featureCount,
                                int                 				barIndex	,
                                double*             				X)
{
  if(!X || featureCount <= 0)
    return;

  const int nsg = 60; // <<— remplacé ici
  const int asz = sc.ArraySize;
  const int idx = barIndex;

  const int validBar = (idx >= 0 && idx < asz) ? idx : (asz > 0 ? asz - 1 : 0);

  for(int i = 0; i < featureCount; ++i)
  {
    int sg = sgFirst + i;
    if(sg >= 0 && sg < nsg)
      X[i] = sc.Subgraph[sg].Data[validBar];
    else
      X[i] = 0.0;
  }
}
static inline void 				MLP_CheckFeatureCount				(SCStudyInterfaceRef sc,
                                         int                 expectedCount,
                                         int                 actualCount,
                                         int&                errCnt)
{
  if(expectedCount == actualCount)
    return;

  ++errCnt;
  SCString msg;
  msg.Format("MLP_CheckFeatureCount #WARN: expected=%d actual=%d",
             expectedCount,
             actualCount);
  sc.AddMessageToLog(msg, 1);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction header binaire standard
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void              MLP_BuildBinHeader           		(MLP_BinRowHeader&  H,
                                                               long long          utcMs,
                                                               int                barIndex,
                                                               int                gateOK,
                                                               int                stale,
                                                               double             scoreRaw,
                                                               double             scoreEMA,
                                                               int                trigUp,
                                                               int                trigDn,
                                                               int                featureCount,
                                                               unsigned           featSig,
                                                               int                contractVer,
                                                               int                sessionId,
                                                               int                algoVer,
                                                               int                dType)
{
  H.UtcMs        = utcMs;
  H.BarIndex     = barIndex;
  H.GateOK       = gateOK;
  H.Stale        = stale;
  H.ScoreRaw     = (float)scoreRaw;
  H.ScoreEMA     = (float)scoreEMA;
  H.TrigUp       = trigUp;
  H.TrigDn       = trigDn;
  H.FeatureCount = featureCount;
  H.FeatSig      = featSig;
  H.ContractVer  = contractVer;
  H.SessionId    = sessionId;
  H.AlgoVer      = algoVer;
  H.SchemaVer    = MLP_SchemaVer();
  H.DType        = dType;
  H.Magic        = MLP_BinMagic();
  H.Endian       = MLP_DetectEndian();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// SessionId unifié / robuste
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline int 				MLP_ResolveSessionId				(SCStudyInterfaceRef sc,
                                       SCInputRef&        In_SessionId,
                                       const int          pSess = 103)
{
  int v = In_SessionId.GetInt();
  if(v > 0)
    return v;

  int& stored = sc.GetPersistentInt(pSess);
  if(stored == 0)
  {
    long long ms = MLP_NowMs(sc);
    stored       = (int)(ms & 0x7FFFFFFF);
    if(stored == 0)
      stored = 1;
  }
  return stored;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Gating / critère de log + label
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline bool 				MLP_ShouldLogBar					(int gateOK,
                                    int stale,
                                    int trigUp,
                                    int trigDn,
                                    int mode = 1)
{
  if(!gateOK || stale)
    return false;

  if(mode == 1)
  {
    // mode 1: uniquement si un trigger est actif
    return (trigUp != 0) || (trigDn != 0);
  }

  // mode 0 (ou autre): log dès que gateOK && !stale
  return true;
}
static inline int 				MLP_BinaryLabelFromTriggers			(int trigUp,
                                              int trigDn)
{
  const int up = (trigUp != 0) ? 1 : 0;
  const int dn = (trigDn != 0) ? 1 : 0;

  if(up && !dn)
    return +1;
  if(dn && !up)
    return -1;
  return 0;
}
static inline double 			MLP_ClipScoreToRange				(double x,
                                          double lo,
                                          double hi)
{
  if(x < lo)
    return lo;
  if(x > hi)
    return hi;
  return x;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Normalisation score EMA (approx 3-sigma)
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline double 			MLP_NormalizeScoreEMA				(SCStudyInterfaceRef sc,
                                           double             scoreSigned,
                                           double             alphaPctNorm,
                                           const int          pEma    = 200,
                                           const int          pInited = 201)
{
  double& emaAbs = sc.GetPersistentDouble(pEma);
  int&    inited = sc.GetPersistentInt(pInited);

  double absScore = (scoreSigned >= 0.0 ? scoreSigned : -scoreSigned);

  double a = alphaPctNorm * 0.01;
  if(a < 0.001)
    a = 0.001;
  if(a > 0.999)
    a = 0.999;

  if(!inited)
  {
    emaAbs = absScore;
    inited = 1;
  }
  else
  {
    emaAbs = a * absScore + (1.0 - a) * emaAbs;
  }

  if(emaAbs <= 0.0)
    return 0.0;

  double norm = scoreSigned / (3.0 * emaAbs); // approx "3-sigma"
  if(norm < -3.0)
    norm = -3.0;
  if(norm > 3.0)
    norm = 3.0;
  return norm;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Pattern de fichier à partir des inputs texte
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline 					SCString MLP_MakePatternFromInputs	(const SCInputRef& In_Folder,
                                                 const SCInputRef& In_Prefix,
                                                 const SCInputRef& In_Suffix)
{
  SCString folder = In_Folder.GetString();
  SCString prefix = In_Prefix.GetString();
  SCString suffix = In_Suffix.GetString();

  if(folder.GetLength() > 0)
  {
    const char last = folder[folder.GetLength() - 1];
    if(last != '\\' && last != '/')
      folder += "\\";
  }

  SCString pattern;
  pattern.Format("%s%s%%Y%%m%%d%s",
                 folder.GetChars(),
                 prefix.GetChars(),
                 suffix.GetChars());

  return pattern;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Log INFO une seule fois
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void 				MLP_LogInfoOnce						(SCStudyInterfaceRef sc,
                                   FILE*              f,
                                   const char*        msg,
                                   int&               errCnt,
                                   const int          pFlag = 130)
{
  if(!f || !msg)
    return;

  int& done = sc.GetPersistentInt(pFlag);
  if(done)
    return;

  if(fprintf(f, "#INFO,%s\n", msg) < 0)
  {
    ++errCnt;
  }
  else
  {
    done = 1;
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Debug / layout des features
////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void 				MLP_AssertFeatLayout				(SCStudyInterfaceRef sc,
                                        FILE*              f,
                                        int                featureCount,
                                        const char* const* featNames,
                                        const int*         sgIndices,
                                        int&               errCnt,
                                        const int          pDone = 131)
{
  if(!f || featureCount <= 0)
    return;

  int& done = sc.GetPersistentInt(pDone);
  if(done)
    return;

  if(fprintf(f, "#LAYOUT,Idx,FeatName,SgIndex\n") < 0)
  {
    ++errCnt;
    return;
  }

  for(int i = 0; i < featureCount; ++i)
  {
    const char* name = (featNames && featNames[i]) ? featNames[i] : "";
    int         sg   = (sgIndices ? sgIndices[i] : -1);

    if(fprintf(f, "#LAYOUT,%d,%s,%d\n", i, name, sg) < 0)
    {
      ++errCnt;
      return;
    }
  }

  done = 1;
}
// ============================================================================
// Sanity-check et nettoyage des vecteurs de features X[]
// ============================================================================

// Test portables de "double fini"
static inline int 				MLP_IsFiniteD(double x)
{
#if defined(_MSC_VER)
  return _finite(x) != 0;
#else
  return isfinite(x) != 0;
#endif
}

// Compte le nombre de valeurs non finies dans X[]
static inline int 				MLP_CountBadX(const double* X, int featureCount)
{
  if(!X || featureCount <= 0)
    return 0;

  int bad = 0;
  for(int i = 0; i < featureCount; ++i)
  {
    if(!MLP_IsFiniteD(X[i]))
      ++bad;
  }
  return bad;
}

// Clamp des valeurs extrêmes et compte le nombre de clamp appliqués
static inline int 				MLP_ClampAndCountX(double* X, int featureCount, double absMax)
{
  if(!X || featureCount <= 0 || absMax <= 0.0)
    return 0;

  int clamped = 0;
  for(int i = 0; i < featureCount; ++i)
  {
    if(!MLP_IsFiniteD(X[i]))
      continue;

    const double v      = X[i];
    const double vClamp = MLP_ClampD(v, -absMax, absMax);
    if(vClamp != v)
    {
      X[i] = vClamp;
      ++clamped;
    }
  }
  return clamped;
}

// Mode de sanitization
enum 							MLP_SanitizeMode
{
  MLP_SANITIZE_ZERO = 0, // NaN/Inf -> 0.0
  MLP_SANITIZE_EMA  = 1  // NaN/Inf -> EMA persistante par feature
};

// Version simple historique: NaN/Inf -> replacement (par défaut 0.0)
static inline void 				MLP_SanitizeX(double* X, int featureCount, double replacement = 0.0)
{
  if(!X || featureCount <= 0)
    return;

  for(int i = 0; i < featureCount; ++i)
  {
    if(!MLP_IsFiniteD(X[i]))
      X[i] = replacement;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Version avancée: sanitization + clamp + diagnostic
// - mode ZERO = comportement historique (NaN/Inf -> 0)
// - mode EMA  = NaN/Inf -> EMA persistante (pBaseEma + i)
// - clampAbsMax > 0 => clamp |X[i]| à cette borne, avec diagnostic
// - log #INFO une seule fois par session via MLP_LogInfoOnce
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void 				MLP_SanitizeAndReportX(
    SCStudyInterfaceRef			sc										,
    FILE*              			f										,
    double*            			X										,
    int                			featureCount							,
    int&               			errCnt									,
    const int          			pFlag             = 140					,
    MLP_SanitizeMode   			mode              = MLP_SANITIZE_ZERO	,
    double             			emaAlpha          = 0.10				,  // 0 < emaAlpha < 1
    int                			pBaseEma          = -1					,    // bloc de persistants EMA, ou <0 pour désactiver
    double             			clampAbsMax       = 0.0					,   // 0 => pas de clamp
    int                			maxBadIdxLog      = 16					,    // nb max d'indices NaN/Inf loggués
    int                			maxClampIdxLog    = 8					)     // nb max d'indices clampés loggués
{
  if(!X || featureCount <= 0)
    return;

  const bool useEma = (mode == MLP_SANITIZE_EMA
                       && pBaseEma >= 0
                       && featureCount > 0
                       && emaAlpha > 0.0
                       && emaAlpha < 1.0);

  int       bad     = 0;
  int       clamped = 0;
  SCString  badIdxList;
  SCString  clampIdxList;

///////////////////////////////////////////////////////////////////////////// 1) Passe principale: mise à jour EMA sur valeurs finies, détection NaN/Inf, clamp
  for(int i = 0; i < featureCount; ++i)
  {
    double v        = X[i];
    const bool finite = MLP_IsFiniteD(v);

///////////////////////////////////////////////////////////////////////////// Mise à jour EMA sur valeurs finies
    if(useEma)
    {
      double& ema = sc.GetPersistentDouble(pBaseEma + i);
      if(finite)
      {
        // EMA standard; si ema==0 au début, ça converge de toute façon
        ema = (1.0 - emaAlpha) * ema + emaAlpha * v;
      }
    }

///////////////////////////////////////////////////////////////////////////// NaN/Inf détectés: on note, on corrigera dans une deuxième passe
    if(!finite)
    {
      if(bad < maxBadIdxLog)
      {
        if(bad > 0)
          badIdxList.Append(",");

        SCString tmp;
        tmp.Format("%d", i);
        badIdxList.Append(tmp);
      }
      ++bad;
      continue;
    }

///////////////////////////////////////////////////////////////////////////// Clamp éventuel des valeurs extrêmes
    if(clampAbsMax > 0.0)
    {
      const double vClamp = MLP_ClampD(v, -clampAbsMax, clampAbsMax);
      if(vClamp != v)
      {
        X[i] = vClamp;

        if(clamped < maxClampIdxLog)
        {
          if(clamped > 0)
            clampIdxList.Append(",");

          SCString tmp;
          tmp.Format("%d", i);
          clampIdxList.Append(tmp);
        }
        ++clamped;
      }
    }
  }

///////////////////////////////////////////////////////////////////////////// 2) Deuxième passe: correction effective des NaN/Inf
  if(bad > 0)
  {
    for(int i = 0; i < featureCount; ++i)
    {
      if	(MLP_IsFiniteD(X[i]))	continue				;

      if	(useEma)
      {
        double& ema = sc.GetPersistentDouble(pBaseEma + i)	;
        X[i]        = ema									; // si ema==0 faute d'historique, on tombe de facto sur 0
      }
      else	{X[i] = 0.0;}
    }
  }

///////////////////////////////////////////////////////////////////////////// 3) Pas d'anomalie => rien à logguer
  if(bad <= 0 && clamped <= 0)	return;

///////////////////////////////////////////////////////////////////////////// 4) Construction du message de diagnostic #INFO (une seule fois)
  const char* modeStr = (mode == MLP_SANITIZE_EMA ? "EMA" : "ZERO");

  SCString	msg;
			msg.Format("MLP_SanitizeX: bad=%d, clamped=%d, mode=%s",
				bad,
				clamped,
				modeStr);

  if(bad > 0 && badIdxList.GetLength() > 0)
  {
    SCString extra;
    extra.Format(", badIdx=[%s]%s",
                 badIdxList.GetChars(),
                 bad > maxBadIdxLog ? ",(troncation)" : "");
    msg.Append(extra);
  }

  if(clamped > 0 && clampAbsMax > 0.0 && clampIdxList.GetLength() > 0)
  {
    SCString extra;
    extra.Format(", clampIdx=[%s]%s, clampAbsMax=%g",
                 clampIdxList.GetChars(),
                 clamped > maxClampIdxLog ? ",(troncation)" : "",
                 clampAbsMax);
    msg.Append(extra);
  }

  MLP_LogInfoOnce(sc, f, msg.GetChars(), errCnt, pFlag);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper haut niveau : log CSV + BIN + sanitize X[]
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void 				MLP_LogAllFormats											(
    SCStudyInterfaceRef			sc															,
    FILE*              			fCsv														,
    FILE*              			fBin														,
    const SCString&    			symbolSC													,
    const char*        			algoName													,
    int                			algoVer														,
    int                			contractVer													,
    int                			sessionId													,
    int                			barIndex													,
    int                			gateOK														,
    int                			stale														,
    double             			scoreRawSigned												,
    double             			scoreEmaSigned												,
    double             			alphaPctNorm												,
    int                			trigUp														,
    int                			trigDn														,
    unsigned           			featSig														,
    double*            			X															,
    int                			featureCount												,
    int                			binDType													,	// MLP_DTYPE_F32 ou MLP_DTYPE_F16
    int&               			errCntCsv													,
    int&               			errCntBin													,
    int                			logMode = 1													)	// 1 = triggers only, 0 = toutes les barres gateOK
{
///////////////////////////////////////////////////////////////////////////////////////////// 	1) Gating global
  if(!MLP_ShouldLogBar(gateOK, stale, trigUp, trigDn, logMode))	return						;

///////////////////////////////////////////////////////////////////////////////////////////// 	2) Normalisation EMA du score (alpha pilotable par input)
  double 			scoreNorm	= MLP_NormalizeScoreEMA	(sc, scoreEmaSigned, alphaPctNorm);

///////////////////////////////////////////////////////////////////////////////////////////// 	3) Horodatage et symbole
  const long long 	utcMs		= MLP_NowMs				(sc)								;
  const char*    	sym 		= symbolSC.GetChars		()									;
  if(!sym)			sym 		= ""														;

///////////////////////////////////////////////////////////////////////////////////////////// 	4) Sanitize X en place + #INFO une seule fois (si CSV dispo)
  if(X && featureCount > 0 && fCsv)
  {
    MLP_SanitizeAndReportX(sc, fCsv, X, featureCount, errCntCsv)							;
  }

///////////////////////////////////////////////////////////////////////////////////////////// 	5) CSV
  if	(fCsv)																				{
		MLP_CSV_LogRowAcc													(
        sc																	,
        fCsv																,
        utcMs																,
        sym																	,
        algoName															,
        algoVer																,
        contractVer															,
        sessionId															,
        barIndex															,
        gateOK																,
        stale																,
        scoreRawSigned														,
        scoreEmaSigned														,
        scoreNorm															,
        trigUp																,
        trigDn																,
        featSig																,
        X																	,
        featureCount														,
        errCntCsv)															;
  }

///////////////////////////////////////////////////////////////////////////////////////////// 	6) BIN
  if	(fBin && X && featureCount > 0)
  {
    MLP_BinRowHeader H																		;
    MLP_BuildBinHeader																		(
        H																					,
        utcMs																				,
        barIndex																			,
        gateOK																				,
        stale																				,
        scoreRawSigned																		,
        scoreEmaSigned																		,
        trigUp																				,
        trigDn																				,
        featureCount																		,
        featSig																				,
        contractVer																			,
        sessionId																			,
        algoVer																				,
        binDType)																			;

    MLP_BIN_LogRowAcc	(sc, fBin, H, X, featureCount, binDType, errCntBin)	;
  }
}

} // namespace du
