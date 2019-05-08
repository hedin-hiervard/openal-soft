#ifndef GXLIB_GXL_COMMON_DEF_H_
#define GXLIB_GXL_COMMON_DEF_H_

/*
 *	Presets
 */

/*
 * Common defines
 */

#ifdef WIN32
#	define VARARG __cdecl
#else
#	define VARARG

#endif
#ifndef always_inline
//#if defined(__GNUC__) && (__GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ > 0)
//#    define always_inline __attribute__((always_inline)) inline
#if defined(_MSC_VER)
#	define always_inline __forceinline
#else
#    define always_inline __inline__
#endif


#endif
#ifdef _DEBUG
#	define ODS(x) OutputDebugString(x);
#	define check(x) assert(x);
#	define verify(x) assert(x);
#else
#	define ODS(x)
#	define check(x)
#	define verify(x)

#endif
#define _DIM_(x) (sizeof(x) / sizeof(x[0]))
#define _END_(x) (&(x[_DIM_(x)]))

/*
 * Base types definations
 */

#define MAX_MICE_COUNT 11

#if defined(OS_WIN32) || defined (OS_WINCE)
	
	typedef __int8				sint8;		// -128 to 127
	typedef __int16				sint16;		// -32,768 to 32,767
	typedef __int32				sint32;		// -2,147,483,648 to 2,147,483,647
	typedef __int64				sint64;		// -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807
	typedef int					sint;		// -2,147,483,648 to 2,147,483,647

	typedef unsigned __int8		uint8;		// 255
	typedef unsigned __int16	uint16;		// 65,535
	typedef unsigned __int32	uint32;		// 4,294,967,295
	typedef unsigned __int64	uint64;		// 18,446,744,073,709,551,616
	typedef unsigned int		uint;		// 4,294,967,295

	typedef CHAR	iCharA;					// generic 8bit char
	typedef WCHAR	iCharW;					// 16bit Unicode char
	typedef TCHAR	iCharT;					// depended on project defines
#else // OS_APPLE, Linux


	typedef int8_t     sint8;
	typedef int16_t    sint16;
	typedef int32_t    sint32;
	typedef int64_t    sint64;

	typedef u_int8_t     uint8;
	typedef u_int16_t    uint16;
	typedef u_int32_t    uint32;
	typedef u_int64_t    uint64;

	typedef long LONG;

	typedef wchar_t TCHAR;

	typedef struct tagRECT
	{
		LONG    left;
		LONG    top;
		LONG    right;
		LONG    bottom;
	} RECT, *PRECT;

	typedef struct tagPOINT
	{
		LONG  x;
		LONG  y;
	} POINT, *PPOINT;

	typedef struct _POINTL      /* ptl  */
	{
		LONG  x;
		LONG  y;
	} POINTL, *PPOINTL;

	typedef struct tagSIZE
	{
		LONG        cx;
		LONG        cy;
	} SIZE, *PSIZE, *LPSIZE;

	typedef SIZE               SIZEL;
	typedef SIZE               *PSIZEL, *LPSIZEL;

	typedef char	iCharA;					// generic 8bit char
	typedef wchar_t	iCharW;					// 16bit Unicode char
	typedef wchar_t	iCharT;					// depended on project defines
	typedef wchar_t WCHAR;

	#define _vsnwprintf vsnwprintf
	#define _vsnprintf vsnprintf
	#define _wtoi wtoi
	#define _strdup strdup
	#define _stricmp stricm
	typedef unsigned short WORD;
	typedef unsigned long DWORD, *LPDWORD;
	typedef sint16 INT;

	#define OutputDebugString
	#define OutputDebugStringW

	typedef uint32 UINT32;

	#define CP_ACP		0		/* default to ANSI code page */
	#define CP_OEMCP	1		/* default to OEM  code page */
	
	typedef unsigned int uint, UINT;
	typedef void *LPVOID;

	typedef uint64 LRESULT, LPARAM, WPARAM;

	#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name

	DECLARE_HANDLE            (HWND);

	//#undef BOOL
//typedef bool BOOL;
	#define FALSE false

	typedef WCHAR *LPCTSTR;
	typedef const WCHAR *LPCWSTR;

	#define Sleep sleep
	#define MessageBox(a, b, c, d)

	#define _tcslen wcslen
	#define _tcschr wcschr

	#define _T(x) L ## x

	#pragma pack(push,2)
	typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
	} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

	#pragma pack(pop)

	typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
	} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

	typedef unsigned char BYTE;

	typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
	} RGBQUAD;


	typedef struct tagBITMAPINFO {
		BITMAPINFOHEADER    bmiHeader;
		RGBQUAD             bmiColors[1];
	} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;


	#define BI_RGB        0L
	#define BI_RLE8       1L
	#define BI_RLE4       2L
	#define BI_BITFIELDS  3L
	#define BI_JPEG       4L
	#define BI_PNG        5L

	//#define GetTickCount clock

	sint32 GetTickCount();

	typedef uint32 UINT_PTR;
	typedef wchar_t *LPWSTR;
	typedef int HMODULE;

	//#define assert(x)
//#define fopen fopen_iphone


#endif //OS_APPLE
//
extern const uint32 iRandTable[256];
typedef uint8 MouseId;

enum MouseButtonID{

	MBI_LEFT	= 0,
	MBI_RIGHT	= 1,
	MBI_MIDDLE	= 2
};

typedef uint8 MouseButtonId;

#endif //GXLIB_GXL_COMMON_DEF_H_
