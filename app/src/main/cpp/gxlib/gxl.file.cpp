#if defined(OS_IPHONE) || defined(OS_MACOS)
#define OS_APPLE 
#endif //(OS_IPHONE || OS_MACOS)

#if defined(OS_WINCE) || defined (OS_WIN32)
#include "gxl.file.win.cpp"
#elif defined (OS_APPLE)
#include "gxl.file.iphone.cpp"
#endif
