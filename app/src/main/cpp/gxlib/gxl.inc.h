#ifndef GXLIB_GXL_INC_H_
#define GXLIB_GXL_INC_H_

#if defined(OS_IPHONE) || defined(OS_MACOS)
#define OS_APPLE
#endif //(OS_IPHONE || OS_MACOS)

#if defined(OS_WIN32) || defined(OS_MACOS)
#define PC_VERSION
#endif //(OS_IPHONE || OS_MACOS)


#define WIN32_LEAN_AND_MEAN
#if defined(OS_WIN32) || defined(OS_WINCE)
#include <windows.h>
#else
#include <stdarg.h>
#include <wchar.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#endif
#include <stdio.h>
#include <stdlib.h>

#if defined(OS_WIN32) || defined(OS_WINCE)
#include <TCHAR.H>
#include <ASSERT.H>
#ifndef OS_WINCE
#include <objidl.h>
#endif

#endif
#if defined(OS_APPLE) || defined(OS_LINUX) || defined(OS_ANDROID)
#include <assert.h>
#include <new>
#endif
#ifdef OS_LINUX
#include <wctype.h>

#endif
#ifdef OS_WINCE
#if _WIN32_WCE == 300
#	include <dbgapi.H>
#elif _WIN32_WCE >= 420
#	include <ASSERT.H>
#else
#error Unknown version of WindowsCE
#endif

#endif
#if (_MSC_VER < 1310) && !defined(for)
#define for if (0) {} else for

#endif
#if !defined(NDEBUG)
//#define MEM_TRACK
#endif
#include "gxl.memdbg.h"

#ifdef OS_APPLE
#include "iphone_common.h"
#endif

#include "gxl.common.def.h"
#include "gxl.common.tpl.h"
#include "gxl.fixpoint.h"
#include "gxl.math.h"
#include "gxl.rnd.h"
#include "gxl.smartptr.h"
#include "gxl.debug.h"
#include "gxl.metrics.h"
#include "gxl.string.std.h"
#include "gxl.string.h"
#include "gxl.memory.h"
#include "gxl.file.h"
#include "gxl.dib.h"


// flags
const uint32 GXLF_DOUBLESIZE	= 0x01;
const uint32 GXLF_LANDSCAPE		= 0x02;
const uint32 GXLF_DEV_LANDSCAPE	= 0x04;
const uint32 GXLF_DEV_VGARES	= 0x08;
const uint32 GXLF_LHANDER		= 0x10;
const uint32 GXLF_ENABLESOUND	= 0x20;
const uint32 GXLF_REALVGA		= 0x40;
const uint32 GXLF_FULLSCREEN	= 0x80;
const uint32 GXLF_QUADSIZE	    = 0x100;

#endif //GXLIB_GXL_INC_H_
