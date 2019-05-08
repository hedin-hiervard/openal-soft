#include "gxl.inc.h"

#if defined (OS_WIN32) || defined (OS_WINCE)
#include "gxl.window.windows.cpp"
#else
#include "gxl.window.iphone.cpp"
#endif
