#include "sierrachart.h"
  SCDLLName("PACK_SIGNALS_V0")

  SCSFExport scsf_SIGNAL_DOM_PULL_STACK_v0(SCStudyInterfaceRef sc)
  {
    SCSubgraphRef SG = sc.Subgraph[0];

    if (sc.SetDefaults)
    {
      sc.GraphName = "DOM Pull/Stack v0";
      sc.AutoLoop = 0;
      sc.UpdateAlways = 1;
      sc.GraphRegion = 0;
      sc.ValueFormat = 26;
      sc.FreeDLL = 0;

      SG.Name = "DOM Pull/Stack v0";
      SG.DrawStyle = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
      SG.PrimaryColor = RGB(255,255,255);
      SG.DrawZeros = 0;
      sc.DisplayStudyName = 0;
      sc.DisplayStudyInputValues = 0;
      sc.DisplayStudyNameValueInWindowsFlags = 0;
      return;
    }

SCInputRef In_01_Seuil = sc.Input[0]; In_01_Seuil.Name = "01. Seuil"; In_01_Seuil.SetFloat(0.05); // Ecart relatif vs EMA

    const int last = sc.ArraySize - 1;
    if (last < 2) return;


// Δ quantités best bid/ask en ms récents (proxy: bar-to-bar)
s_MarketDepthEntry mdB0, mdA0;
double prevB=0.0, prevA=0.0, nowB=0.0, nowA=0.0;
if (sc.GetBidMarketDepthEntryAtLevel(mdB0, 0)) nowB = mdB0.Quantity;
if (sc.GetAskMarketDepthEntryAtLevel(mdA0, 0)) nowA = mdA0.Quantity;
// Proxy très simple: utiliser Close[last-1] pour changer de bar; ici on n'a pas snapshot N-1 DOM → approx 0
// On encode un état : stacking si nowB+nowA augmente au-dessus seuil
double thr = In_01_Seuil.GetFloat();
double sum = nowB + nowA;
static double ema=0.0; double alpha = 0.2;
if (sc.UpdateStartIndex==0) ema = sum;
ema = alpha*sum + (1.0-alpha)*ema;
double Result = (sum - ema) / (ema>0.0?ema:1.0);
if (Result>-thr) Result = 0.0;
// Valeur positive = stacking, négative = pulling
Result = (sum>=ema*(1.0+thr)) ? (sum/ema - 1.0) : ( (sum<=ema*(1.0-thr)) ? -(1.0 - sum/ema) : 0.0 );


    // Efface l'historique sauf la dernière barre
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i) SG[i] = 0.0f;

    // Taille du marqueur = clamp(|Result|*10, 0..10)
    double amp = Result; if (amp < 0) amp = -amp;
    double size = amp * 10.0; if (size > 10.0) size = 10.0;
    SG[last] = (float)size;
  }