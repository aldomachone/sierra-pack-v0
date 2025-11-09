#pragma once
#include <cstdio>
#include <cstring>
#include "sierrachart.h"

// === Logging CSV minimal v0 ===
// Objectif : donner un canal simple pour log de features.
// Notes : pas de rotation ni compression en v0. Chemin fixe configurable.

namespace du {

  enum { PERSIST_CSV_FILEPTR = 1000, PERSIST_CSV_OPENED = 1001 };

  // openCsvIfNeeded : ouvre en append si fermé
  inline bool openCsvIfNeeded(SCStudyInterfaceRef sc, const char* path)
  {
    int opened = sc.GetPersistentInt(PERSIST_CSV_OPENED);
    FILE* f = (FILE*)sc.GetPersistentPointer(PERSIST_CSV_FILEPTR);

    if (opened && f) return true;

    f = std::fopen(path, "ab"); // append binaire pour éviter issues CRLF
    if (!f) return false;

    sc.SetPersistentPointer(PERSIST_CSV_FILEPTR, f);
    sc.SetPersistentInt(PERSIST_CSV_OPENED, 1);
    return true;
  }

  inline void closeCsvIfOpen(SCStudyInterfaceRef sc)
  {
    FILE* f = (FILE*)sc.GetPersistentPointer(PERSIST_CSV_FILEPTR);
    if (f)
    {
      std::fflush(f);
      std::fclose(f);
      sc.SetPersistentPointer(PERSIST_CSV_FILEPTR, nullptr);
      sc.SetPersistentInt(PERSIST_CSV_OPENED, 0);
    }
  }

  // writeHeader : écrit une ligne d'entête si f est vide (optionnel v0)
  inline void writeHeader(SCStudyInterfaceRef sc, const char* header)
  {
    FILE* f = (FILE*)sc.GetPersistentPointer(PERSIST_CSV_FILEPTR);
    if (!f) return;
    // v0 : écrit systématiquement une fois par SetDefaults; côté appelant à gérer
    std::fwrite(header, 1, std::strlen(header), f);
    std::fwrite("\n", 1, 1, f);
  }

  // writeRow : écrit une ligne déjà formatée "a,b,c"
  inline void writeRow(SCStudyInterfaceRef sc, const char* row)
  {
    FILE* f = (FILE*)sc.GetPersistentPointer(PERSIST_CSV_FILEPTR);
    if (!f) return;
    std::fwrite(row, 1, std::strlen(row), f);
    std::fwrite("\n", 1, 1, f);
  }

} // namespace du
