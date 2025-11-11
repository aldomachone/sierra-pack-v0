#include "sierrachart.h"
#include "SignalsPack_v0.h"
SCDLLName("PACK_SIGNALS_V0")

SCSFExport scsf_SIGNAL_BOOK_IMBALANCE_v0(SCStudyInterfaceRef sc)
{
  SCSubgraphRef SG_Marker = sc.Subgraph[0];

  if (sc.SetDefaults)
  {
    sc.GraphName = "Book Imbalance v0";
    sc.AutoLoop = 0;
    sc.UpdateAlways = 1;
    sc.GraphRegion = 0;
    sc.ValueFormat = 26;
    sc.FreeDLL = 0;

    spv0::InitMarker(SG_Marker, "Book Imbalance v0");
    spv0::HideAll(sc);
    return;
  }

SCInputRef In_01_Niveaux_agrégés = sc.Input[0]; In_01_Niveaux_agrégés.Name = "01. Niveaux agrégés"; In_01_Niveaux_agrégés.SetInt(1); // Réservé pour extensions. Actuellement L1 seul.

  const int last = sc.ArraySize-1;
  if (last < 2) return;


  // Lecture L1 bid/ask si disponible
  s_MarketDepthEntry md;
  double qBid = 0.0, qAsk = 0.0;
  if (sc.GetBidMarketDepthEntryAtLevel(md, 0))
    qBid = md.Quantity;
  if (sc.GetAskMarketDepthEntryAtLevel(md, 0))
    qAsk = md.Quantity;

  double denom = qBid + qAsk;
  double Result = 0.0;
  if (denom > 0.0)
    Result = (qBid - qAsk) / denom;


  for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i)
    SG_Marker[i] = 0.0;

  double amp = fabs(Result);
  double size = amp * 10.0;
  if (size > 10.0) size = 10.0;
  SG_Marker[last] = (float)size;
}