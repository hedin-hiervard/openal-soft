#include "gxl.inc.h"
#include "gxl.ctr.array.h"
#include "gxl.audio.h"

#if defined(OS_WIN32) || defined (OS_WINCE)
#include "gxl.audio.windows.cpp"
#elif defined(OS_APPLE)
#include "gxl.audio.iphone.cpp"
#endif

