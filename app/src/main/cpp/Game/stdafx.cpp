#include "stdafx.h"

#ifdef HMM_GAME_2X
sint32 cell_w = 80;
sint32 cell_h = 20;
sint32 im_coef = 10;
#else

sint32 cell_w = 40;
sint32 cell_h = 10;
sint32 im_coef = 5;
#endif

void DebugReset() {
#ifdef _DEBUGLOGGING
    iFile::Delete(_T("debug.txt"));
#endif
}

void Debug(iStringT str) {
#ifdef _DEBUGLOGGING
    FILE *f = fopen("debug.txt", "a+");
    if (f)
    {
        fwprintf(f, _T("%s\n"), (str.CStr()));
        fclose(f);
    }
#endif
}

void Debug(iStringA str) {
#ifdef _DEBUGLOGGING
    FILE *f = fopen("debug.txt", "a+");
    if (f)
    {
        fprintf(f, "%s\n", (str.CStr()));
        fclose(f);
    }
#endif
}





