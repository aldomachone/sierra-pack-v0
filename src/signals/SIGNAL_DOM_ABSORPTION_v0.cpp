#include "sierrachart.h"
#include <math.h>

SCDLLName("PACK_SIGNALS_V0")

SCSFExport scsf_SIGNAL_DOM_ABSORPTION_v0(SCStudyInterfaceRef sc)
{
    // Subgraph
    SCSubgraphRef SG = sc.Subgraph[0];

    // Inputs
    SCInputRef In_01_p95_Volume = sc.Input[0];
    SCInputRef In_02_Max_DeltaP = sc.Input[1];

    if (sc.SetDefaults)
    {
        sc.GraphName   = "DOM Absorption v0";
        sc.AutoLoop    = 0;
        sc.UpdateAlways = 1;
        sc.GraphRegion = 0;
        sc.ValueFormat = 26;
        sc.FreeDLL     = 0;

        SG.Name       = "DOM Absorption v0";
        SG.DrawStyle  = DRAWSTYLE_TRANSPARENT_CIRCLE_VARIABLE_SIZE;
        SG.PrimaryColor = RGB(255, 255, 255);
        SG.DrawZeros  = 0;

        // Inputs
        In_01_p95_Volume.Name    = "01. p95 Volume";
        In_01_p95_Volume.SetFloat(1000.0f);

        In_02_Max_DeltaP.Name    = "02. Max DeltaP";
        In_02_Max_DeltaP.SetFloat(0.0f); // mouvement max de prix toléré

        // Nettoyage affichages
        sc.DisplayStudyName                         = 0;
        sc.DisplayStudyInputValues                  = 0;
        sc.DisplayStudyNameValueInWindowsFlags      = 0;

        return;
    }

    const int last = sc.ArraySize - 1;
    if (last < 1)
        return;

    // Efface l'historique sur la zone recalculée
    int start = sc.UpdateStartIndex;
    if (start < 0)
        start = 0;
    if (start > last)
        start = last;

    for (int i = start; i <= last; ++i)
        SG[i] = 0.0f;

    // Absorption approximée: grand volume sur la barre avec faible ΔP
    const double vol = sc.Volume[last];
    const double dp  = fabs((double)sc.Close[last] - (double)sc.Close[last - 1]);

    const double thrV = In_01_p95_Volume.GetFloat();
    const double thrP = In_02_Max_DeltaP.GetFloat();

    double result = 0.0;

    if (thrV > 0.0)
    {
        if (vol >= thrV && dp <= thrP)
            result = vol / thrV;
    }

    // Taille du marqueur = clamp(|result|*10, 0..10)
    double amp = result;
    if (amp < 0.0)
        amp = -amp;

    double size = amp * 10.0;
    if (size > 10.0)
        size = 10.0;

    SG[last] = (float)size;
}
