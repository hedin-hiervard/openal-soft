//
// gxl.display.gdi.cpp
// 
// 15.03.2010 fixed bug in SwitchResolution() function, added new #define instead old ones. 
// 19.03.2010 added RAW output 
//

#include "gxl.inc.h"
#include "gxl.window.h"
#include "gxl.display.h"
#include <aygshell.h>
#include <cmnintrin.h>


#define OPTION_DSP_LOG              1   // output information to log file
#define OPTION_DIB_SECTION          1   // use dibsection 
//#define OPTION_SETBITS_REVERSED     1   // negative height in windows bitmap
//#define OPTION_SETBITS_NORMAL       1    // positive height in windows bitmap 


//////////////////////////////////////////////////////////////////////////
// some defines
//////////////////////////////////////////////////////////////////////////

#define GETRAWFRAMEBUFFER   0x00020001
#define RAW_FORMAT_565        (1)
#define RAW_FORMAT_555        (2)
#define RAW_FORMAT_OTHER        (3)


typedef struct _RawFrameBufferInfo
{
    WORD wFormat;
    WORD wBPP;
    VOID *pFramePointer;
    int  cxStride;
    int  cyStride;
    int  cxPixels;
    int  cyPixels;
} RawFrameBufferInfo; 


typedef struct tagBMDIBINFO
{
    BITMAPINFOHEADER    bih;
    RGBQUAD             bmiColors[3];
} BMDIBINFO;

//////////////////////////////////////////////////////////////////////////

// move later to class (?) 
#if defined(OPTION_DIB_SECTION)
static HDC hMainDC;
static HBITMAP hMainBit;
#endif

static BMDIBINFO BDI;
static LPVOID lpFrameBuffer;
static HMODULE hAYG_LIB;
static FARPROC fpFullScreen;
static DWORD old_orientation;
bool bDisplayNativeLandscape;
bool bDisplayFullScreenMode;
static RawFrameBufferInfo RFB;

iDisplay* main_display;


#if defined(OPTION_DSP_LOG)
static void DspLog(char* format, ...)
{        
    FILE *f;
    char buffer[2048];

    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    f = fopen("\\PHeroes.txt", "at+");
    if (f)
    {
        fprintf(f, "%s", buffer);
        fclose(f);
    }        
    va_end(args);
}
#endif


//////////////////////////////////////////////////////////////////////////
// Detect if we can use RAW output on this device
//
// Criteria:
// 1. support of escape IOCTL
// 2. supported values in raw frame buffer structure
bool iDisplay::CheckRAWOutput()
{
    HDC hDC = GetDC(HWND_DESKTOP); 
    int result = ExtEscape(hDC, GETRAWFRAMEBUFFER, 0, NULL, sizeof(RawFrameBufferInfo), (char *)&RFB);
    ReleaseDC(HWND_DESKTOP, hDC);
    if (result < 0) 
    {
#if defined(OPTION_DSP_LOG)
        DspLog("GETRAWFRAMEBUFFER is not supported.\r\n");
#endif
        // this escape not supported => RAW output not supported
        return false;   
    }

#if defined(OPTION_DSP_LOG)
    DspLog("RFB: %d %d : %d %d : %d %d %08X\r\n", 
           RFB.cxPixels, RFB.cyPixels, RFB.cxStride, RFB.cyStride, RFB.wBPP, RFB.wFormat, RFB.pFramePointer);
#endif

    // check for color type
    if (RFB.wBPP != 16 || RFB.wFormat != RAW_FORMAT_565 || RFB.cxStride != 2)
    {
#if defined(OPTION_DSP_LOG)
        DspLog("RAW: Bad color format\r\n");
#endif
        // bad format
        return false;
    }

    // check for QVGA QWVGA VGA WVGA
    if (RFB.cxPixels == 480 && RFB.cyPixels == 800 && RFB.cyStride == 2 * 480)
    {   
#if defined(OPTION_DSP_LOG)
        DspLog("WVGA detected!\r\n");
#endif
        m_bRAWOutput = true;        
        return true; // WVGA
    }
    if (RFB.cxPixels == 480 && RFB.cyPixels == 640 && RFB.cyStride == 2 * 480)
    {   
#if defined(OPTION_DSP_LOG)
        DspLog("VGA detected!\r\n");
#endif
        m_bRAWOutput = true;
        return true; // VGA
    }
    if (RFB.cxPixels == 240 && RFB.cyPixels == 400 && RFB.cyStride == 2 * 240)
    {           
#if defined(OPTION_DSP_LOG)
        DspLog("QWVGA detected!\r\n");
#endif
        m_bRAWOutput = true;
        return true; // QWVGA
    }
    if (RFB.cxPixels == 240 && RFB.cyPixels == 320 && RFB.cyStride == 2 * 240)
    {           
#if defined(OPTION_DSP_LOG)
        DspLog("QVGA detected!\r\n");
#endif
        m_bRAWOutput = true;
        return true; // QVGA
    }    

#if defined(OPTION_DSP_LOG)
    DspLog("Resolution not supported, use GDI\r\n");
#endif
    // Not supported resolution
    return false;
}

//////////////////////////////////////////////////////////////////////////

bool iDisplay::InitRAW()
{
    return true;
}

bool iDisplay::PreInitRAW()
{
    int sdx, sdy;

    sdx = GetSystemMetrics(SM_CXSCREEN); // 240
    sdy = GetSystemMetrics(SM_CYSCREEN); // 320

    // frame buffer sizes  
    m_FullFBSiz.w = RFB.cxPixels;
    m_FullFBSiz.h = RFB.cyPixels;

    // we check only for 480, because supported RAW formats
    // listed in CheckRAWOutput()
    if (RFB.cxPixels == 480)
        m_bDoubled = true;
    else
        m_bDoubled = false;

    // game buffer sizes
    // TODO: this values are hardcoded, but they are also passed to iDisplay::Init    
    m_PartSiz.w = 320;
    m_PartSiz.h = 240;

    // 320x240 or 640x480    
    if (m_bDoubled)
    {
        m_PartFBSiz.w = 480;
        m_PartFBSiz.h = 640;
    }
    else
    {
        m_PartFBSiz.w = 240;
        m_PartFBSiz.h = 320;
    }

    m_FullSiz.w = sdy;
    m_FullSiz.h = sdx;

    // default is "part" mode
    // change settings here for default full screen mode
    m_FBSiz = m_PartFBSiz;
    m_Siz = m_PartSiz;
    bDisplayNativeLandscape = false;
    bDisplayFullScreenMode = false;
    m_bFullScreen = false;

#if defined(OPTION_DSP_LOG)
    DspLog("Screen: %d %d : %d\r\n", sdx, sdy, m_bDoubled);
    DspLog("Frame size: %d %d : %d %d\r\n", m_FullFBSiz.w, m_FullFBSiz.h, m_PartFBSiz.w, m_PartFBSiz.h);
    DspLog("Game size: %d %d : %d %d\r\n", m_FullSiz.w, m_FullSiz.h, m_PartSiz.w, m_PartSiz.h);
#endif
    return true;
}

void iDisplay::DeinitRAW()
{

}

void iDisplay::DrawRAW(HDC hDC)
{
    int x, y;
    uint8 *pBuff;
    uint32 dwVal;
    const iDib::pixel* pSrc = m_BackBuff.GetPtr();
    iDib::pixel* pDst;
    iDib::pixel* pLDst;

    pBuff = (uint8*)RFB.pFramePointer;
    if (m_bFullScreen == false)
    {
        uint32 y_start = (m_FullFBSiz.h - m_FBSiz.h) / 2;
        uint32 fill_size = m_FullFBSiz.w * 2 * y_start;
        uint32 down_y = (m_FullFBSiz.h - y_start);

        // 1. fill start of frame
        memset(pBuff, 0, fill_size);        
        // 2. fill end of frame
        memset(pBuff + down_y, 0, fill_size);

        pBuff += (m_FullFBSiz.w * 2 * y_start);
    }

    uint32 height = m_Siz.h;
    uint32 width = m_Siz.w;

    if (m_bDoubled)
    {
        // doubled 
        if (!(m_Flags & GXLF_LHANDER)) 
        {
             // Right hander mode
            uint32 line_dx = m_FBSiz.w;
            uint32 line_dy = m_FBSiz.h;

            pLDst = ((iDib::pixel*)pBuff) + line_dx * (line_dy - 1);
            for (uint32 yy = 0; yy < height; ++yy) // 240
            {                   
                pDst = pLDst;
                for (uint32 xx = 0; xx < width; ++xx) // 320
                {
#if 1
                    dwVal = *pSrc;

                    *pDst = dwVal;
                    pDst++;

                    *pDst = dwVal;
                    pDst -= line_dx;

                    *pDst = dwVal;
                    pDst--;

                    *pDst = dwVal;  
                    pDst -= line_dx;

#else
                    // not used now... version with word store,
                    // but have unaligned problems
                    dwVal = *pSrc;
                    DWORD dw = dwVal | (dwVal << 16);

                    *(PDWORD)pDst = dw;
                    pDst -= line_dx;

                    *(PDWORD)pDst = dw;
                    pDst -= line_dx;

#endif
                    pSrc++;
                }
                pLDst += 2;
            }
        } 
        else 
        {
            uint32 line_dx = m_FullFBSiz.w;

            pLDst = ((iDib::pixel*)pBuff) + line_dx - 1;
            // Left hander mode 
            for (uint32 yy = 0; yy < height; ++yy) // 240
            {                   
                pDst = pLDst;
                for (uint32 xx = 0; xx < width; ++xx) // 320
                {
                    dwVal = *pSrc;

                    *pDst = dwVal;
                    pDst--;

                    *pDst = dwVal;
                    pDst += line_dx;

                    *pDst = dwVal;
                    pDst++;

                    *pDst = dwVal;  
                    pDst += line_dx;

                    pSrc++;
                }
                pLDst -= 2;
            }
        }

    } 
    else  //if (m_bDoubled)
    { 
        // not doubled
        if (!(m_Flags & GXLF_LHANDER)) 
        {
#if 0
            // Right hander mode
            for (uint32 yy = 0; yy < height; ++yy)        
            {
                for (uint32 xx = 0; xx < width; ++xx)
                {                   
                    dwVal = *pSrc;
                    x = yy;
                    y = width - xx - 1;
                    *(iDib::pixel*)(pBuff + (m_FullFBSiz.w * 2 * y) + (2 * x)) = dwVal;
                    pSrc++;
                }
            }
#else
            uint32 line_dx = m_FBSiz.w;
            uint32 line_dy = m_FBSiz.h;

            pLDst = ((iDib::pixel*)pBuff) + line_dx * (line_dy - 1);

            // Left hander mode 
            for (uint32 yy = 0; yy < height; ++yy)
            {
                pDst = pLDst;
                for (uint32 xx = 0; xx < width; ++xx)
                {
                    dwVal = *pSrc;

                    *pDst = dwVal;
                    pDst -= line_dx;

                    pSrc++;
                }
                pLDst++;
            }
#endif
        } 
        else 
        {
#if 0
            // Left hander mode 
            for (uint32 yy = 0; yy < height; ++yy)
            {
                for (uint32 xx = 0; xx < width; ++xx)
                {
                    dwVal = *pSrc;
                    x = height - yy - 1;
                    y = xx;
                    *(iDib::pixel*)(pBuff + (m_FullFBSiz.w * 2 * y) + (2 * x)) = dwVal;
                    pSrc++;
                }
            }
#else
            uint32 line_dx = m_FullFBSiz.w;

            pLDst = ((iDib::pixel*)pBuff) + line_dx - 1;

            // Left hander mode 
            for (uint32 yy = 0; yy < height; ++yy)
            {
                pDst = pLDst;
                for (uint32 xx = 0; xx < width; ++xx)
                {
                    dwVal = *pSrc;

                    *pDst = dwVal;
                    pDst += line_dx;

                    pSrc++;
                }
                pLDst--;
            }
#endif
        }

    } // if (m_bDoubled)

}

//////////////////////////////////////////////////////////////////////////

bool iDisplay::InitGDI()
{
    memset(&BDI, 0, sizeof(BDI));
    BDI.bih.biSize = sizeof(BITMAPINFOHEADER);
    BDI.bih.biBitCount = 16;
    BDI.bih.biWidth = m_FullFBSiz.w;
#if defined(OPTION_SETBITS_REVERSED)
    BDI.bih.biHeight = -m_FullFBSiz.h;
#else
    BDI.bih.biHeight = m_FullFBSiz.h;
#endif
    BDI.bih.biPlanes = 1;
    BDI.bih.biSizeImage = m_FullFBSiz.w * m_FullFBSiz.h * 2; 
    BDI.bih.biCompression = BI_BITFIELDS;

    // set 5-6-5 format for speed
    *(int*)&BDI.bmiColors[2] = 0x1f;
    *(int*)&BDI.bmiColors[1] = 0x7e0;
    *(int*)&BDI.bmiColors[0] = 0xf800; 

#if defined(OPTION_DIB_SECTION)
    
#if defined(OPTION_DSP_LOG)
    DspLog("DIB Section\r\n");
#endif

    HDC hDC = GetDC(HWND_DESKTOP);
    hMainBit = CreateDIBSection(hDC, (PBITMAPINFO)&BDI, DIB_RGB_COLORS, &lpFrameBuffer, 0, 0);
    if (hMainBit == 0)
    {
        ReleaseDC(HWND_DESKTOP, hDC);
        return false;
    }

    hMainDC = CreateCompatibleDC(hDC);
    if (hMainDC == 0)
    {
        DeleteObject(hMainBit);
        ReleaseDC(HWND_DESKTOP, hDC);
        return false;
    }
    SelectObject(hMainDC, hMainBit);
    ReleaseDC(HWND_DESKTOP, hDC);

#else  // OPTION_DIB_SECTION

#if defined(OPTION_DSP_LOG)
    DspLog("DI Bits\r\n");
#endif

    // allocate frame buffer
    lpFrameBuffer = malloc(m_FullFBSiz.w * m_FullFBSiz.h * 2);
    if (lpFrameBuffer == NULL)
    {
        DeleteDC(hMainDC);
        DeleteObject(hMainBit);
        return false;
    }

#endif // OPTION_DIB_SECTION
    return true;
}

bool iDisplay::PreInitGDI()
{
    // frame buffer sizes  
    m_FullFBSiz.w = GetSystemMetrics(SM_CXSCREEN); // 240
    m_FullFBSiz.h = GetSystemMetrics(SM_CYSCREEN); // 320
  
    // detect if display in DMDO_0 landscape
    if (m_FullFBSiz.w > m_FullFBSiz.h)
        m_bLandscape = true;
    else
        m_bLandscape = false;

	m_bDoubled = false;
    
	/*// TODO: maybe better numbers here?
    if (m_FullFBSiz.w > 480 || m_FullFBSiz.h > 480)
        m_bDoubled = true;
    else
        m_bDoubled = false;
	*/

    // game buffer sizes
    // TODO: this values are hardcoded, but they are also passed to iDisplay::Init    
    m_PartSiz.w = 320;
    m_PartSiz.h = 240;

    if (m_bLandscape)
    {
        m_PartFBSiz.w = 320;
        m_PartFBSiz.h = 240;

        m_FullSiz.w = m_FullFBSiz.w;
        m_FullSiz.h = m_FullFBSiz.h;
    }
    else
    {
        m_PartFBSiz.w = 240;
        m_PartFBSiz.h = 320;

        m_FullSiz.w = m_FullFBSiz.h;
        m_FullSiz.h = m_FullFBSiz.w;
    }

    // default is "part" mode
    // change settings here for default full screen mode
    m_FBSiz = m_PartFBSiz;
    m_Siz = m_PartSiz;
    bDisplayNativeLandscape = m_bLandscape;
    bDisplayFullScreenMode = false;
    m_bFullScreen = false;

#if defined(OPTION_DSP_LOG)
    DspLog("Screen: %d\r\n", m_bLandscape);
    DspLog("Frame size: %d %d : %d %d\r\n", m_FullFBSiz.w, m_FullFBSiz.h, m_PartFBSiz.w, m_PartFBSiz.h);
    DspLog("Game size: %d %d : %d %d\r\n", m_FullSiz.w, m_FullSiz.h, m_PartSiz.w, m_PartSiz.h);
#endif

    // check if screen at least 240x320
    if (m_FullFBSiz.w < m_PartFBSiz.w || m_FullFBSiz.h < m_PartFBSiz.h)
    {
#if defined(OPTION_DSP_LOG)
        DspLog("ERR: Screen is too small for game\r\n");
#endif
        return false;
    } 
    return true;
}


void iDisplay::DeinitGDI()
{
#if defined(OPTION_DIB_SECTION)
    DeleteDC(hMainDC);
    DeleteObject(hMainBit);
    hMainDC = 0;
    hMainBit = 0;
    lpFrameBuffer = NULL;
#else
    if (lpFrameBuffer)
    {
        free(lpFrameBuffer);
        lpFrameBuffer = NULL;
    }
#endif
}

void iDisplay::DrawGDI(HDC hdc)
{
    HDC odc = hdc;
    if (!odc) odc = GetDC(m_hWnd);    
    if (!odc) return; 

    if (m_bLandscape)
        DoScreenPaint(odc);
    else
        DoScreenPaintRotated(odc);    

#if defined(OPTION_DIB_SECTION)

    BitBlt(odc, 0, 0, m_FullFBSiz.w, m_FullFBSiz.h, hMainDC, 0, 0, SRCCOPY);

#elif defined(OPTION_SETBITS_REVERSED)
    // ohoh, WM42, WM50 have buggy SetDIBitsToDevice, so use StretchDIBits instead
    StretchDIBits(odc, 0, 0,
        m_FullFBSiz.w, m_FullFBSiz.h,
        0, 0, m_FullFBSiz.w, m_FullFBSiz.h,
        (VOID**)lpFrameBuffer, (BITMAPINFO*)&BDI, DIB_RGB_COLORS, SRCCOPY);

#else 

    SetDIBitsToDevice(odc, 0, 0, 
        m_FullFBSiz.w, m_FullFBSiz.h,
        0, 0, 0, m_FullFBSiz.h,
        (VOID**)lpFrameBuffer, (BITMAPINFO*)&BDI, DIB_RGB_COLORS);

#endif // USE_DIB_SECTION
    if(!hdc) ReleaseDC(m_hWnd, odc);    
}




//////////////////////////////////////////////////////////////////////////

iDisplay::iDisplay()
: m_bVGA(false), m_bInited(false), m_bFullScreen(false), m_bLandscape(false), m_bDoubled(false), m_bRAWOutput(false)
{
    // HACK for iInput...
    main_display = this;
}

iDisplay::~iDisplay()
{       
	check(!m_bInited);
    main_display = NULL;
}

bool iDisplay::Init(HWND hwnd, const iSize& siz, uint32 flags)
{
    bool res;
    
	check(!m_bInited);

#if defined (OPTION_DSP_LOG)
    SYSTEMTIME st;
    GetLocalTime(&st);
    DspLog("\r\n\r\nStart display init at %02d.%02d.%d  %02d:%02d\r\n", 
        st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
#endif

	m_Flags = flags;
	m_hWnd = hwnd;
	
    m_Siz = siz;  
    m_BackBuff.Init(m_Siz,iDib::RGB);

    // get current rotation angle 
    DEVMODE dev;
    dev.dmSize = sizeof(DEVMODE);
    dev.dmFields = DM_DISPLAYORIENTATION;
    ChangeDisplaySettingsEx(NULL, &dev, 0, CDS_TEST, NULL);    
    old_orientation = dev.dmDisplayOrientation;

#if defined (OPTION_DSP_LOG)
    DspLog("Screen orientation for launch was %d\r\n", old_orientation);
#endif

    // set new rotation angle
    dev.dmFields = DM_DISPLAYORIENTATION;
    dev.dmDisplayOrientation = DMDO_0;
    ChangeDisplaySettingsEx(NULL, &dev, 0, CDS_RESET, NULL);

    // check if this system support RAW output or not
    CheckRAWOutput();

    // init and check size of screen
    if (m_bRAWOutput)
        res = PreInitRAW();
    else
        res = PreInitGDI();

    if (!res)
    {
#if defined(OPTION_DSP_LOG)
        DspLog("PreInit failed!\r\n");
#endif
        return false;
    }

    // go to full screen mode
    hAYG_LIB  = LoadLibrary( L"AYGSHELL.DLL" );
    if (hAYG_LIB)
    {
        fpFullScreen = GetProcAddress(hAYG_LIB, L"SHFullScreen");       
        if (fpFullScreen)
        {
            ((BOOL (*)(HWND, DWORD))fpFullScreen)(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON);
        }
        //((BOOL (*)(HWND, DWORD))fpFullScreen)(m_hWnd, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON | SHFS_SHOWSTARTICON);
    }


    // init any kind of resources
    if (m_bRAWOutput)    
        res = InitRAW();
    else
        res = InitGDI();

    if (!res)
    {
#if defined(OPTION_DSP_LOG)
        DspLog("Init failed!\r\n");
#endif
        return false;
    }

#if defined(OPTION_DSP_LOG)
    DspLog("Init done!\r\n");
#endif

	return m_bInited = true;
}

void iDisplay::SetOrientation(bool bLandscape, bool bLefthander)
{
	/* GXLF_LANDSCAPE not used longer
    so ignore bLandscape
    */
	if (bLefthander && (m_Flags & GXLF_LHANDER) == 0) m_Flags |= GXLF_LHANDER;
	else if (!bLefthander && (m_Flags & GXLF_LHANDER) != 0) m_Flags ^= GXLF_LHANDER;
}

void iDisplay::Destroy()
{
	check(m_bInited);
	m_BackBuff.Cleanup();

    if (m_bRAWOutput)
        DeinitRAW();
    else
        DeinitGDI();

    if (fpFullScreen)
    {
        ((BOOL (*)(HWND, DWORD))fpFullScreen)(m_hWnd, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON | SHFS_SHOWSTARTICON);
    }

    // restore rotation angle 
    DEVMODE dev;
    dev.dmSize = sizeof(DEVMODE);
    dev.dmFields = DM_DISPLAYORIENTATION;
    dev.dmDisplayOrientation = old_orientation;
    ChangeDisplaySettingsEx(NULL, &dev, 0, CDS_RESET, NULL);

    m_bInited = false;
}

void iDisplay::msg_OnPaint(HDC hdc)
{
	DoPaint(m_Siz, hdc);
}

void iDisplay::msg_Suspend()
{
}

void iDisplay::msg_Resume()
{
}

// switch FullScreen <-> QVGA 
void iDisplay::SwitchResolution(bool full)
{
    if (full == m_bFullScreen)
        return;

    if (full)
    {
        m_FBSiz = m_FullFBSiz;
        m_Siz = m_FullSiz;
        bDisplayFullScreenMode = true;
    }
    else
    {
        m_FBSiz = m_PartFBSiz;
        m_Siz = m_PartSiz;
        bDisplayFullScreenMode = false;
    }
    m_BackBuff.Resize(m_Siz);
    m_bFullScreen = full;
}

//////////////////////////////////////////////////////////////////////////

// this function draw screen from full back buffer without rotation
void iDisplay::DoScreenPaint(HDC hdc)
{
    int x, y;
    uint8 *pBuff;
    const iDib::pixel* pSrc = m_BackBuff.GetPtr();

    pBuff = (uint8*)lpFrameBuffer;
    if (m_bFullScreen == false)
    {
        uint32 y_start = (m_FullFBSiz.h - m_FBSiz.h) / 2;
        uint32 x_start = (m_FullFBSiz.w - m_FBSiz.w) / 2;

        pBuff += (m_FullFBSiz.w * 2 * y_start) + 2 * x_start;
        // SetDIBitsToDevice is much more slow, so clearing full buffer doesn't down performance
        memset(lpFrameBuffer, 0, m_FullFBSiz.w * m_FullFBSiz.h * 2);
    }

    uint32 height = m_Siz.h;
    uint32 width = m_Siz.w;

    if (!(m_Flags & GXLF_LHANDER)) 
    {
        // Right hander mode
        for (uint32 yy = 0; yy <height; ++yy)
        {
#if defined(OPTION_SETBITS_REVERSED)
            for (uint32 xx = 0; xx < width; ++xx)
#else
            for (int xx = width; xx >= 0; xx--)
#endif
            {
                uint32 dwVal;
                dwVal = *pSrc;
                x = xx;        
                y = yy;
                *(iDib::pixel*)(pBuff + (m_FullFBSiz.w * 2 * y) + (2 * x)) = dwVal;
                pSrc++;
            }
        }
    } 
    else 
    {
        // Left hander mode
        for (uint32 yy = 0; yy < height; ++yy)
        {
#if defined(OPTION_SETBITS_REVERSED)
            for (uint32 xx = 0; xx < width; ++xx)
#else
            for (int xx = width - 1; xx >= 0; xx--)
#endif
            {
                uint32 dwVal;
                dwVal = *pSrc;
                x = width - xx - 1; 
                y = height - yy - 1;
                *(iDib::pixel*)(pBuff + (m_FullFBSiz.w * 2 * y) + (2 * x)) = dwVal;
                pSrc++;
            }
        }
    }

}

//////////////////////////////////////////////////////////////////////////

// this function draw screen from backbuffer with rotation
void iDisplay::DoScreenPaintRotated(HDC hdc)
{
    int x, y;
    uint8 *pBuff;
    const iDib::pixel* pSrc = m_BackBuff.GetPtr();

    pBuff = (uint8*)lpFrameBuffer;
    if (m_bFullScreen == false)
    {
        uint32 y_start = (m_FullFBSiz.h - m_FBSiz.h) / 2;
        uint32 x_start = (m_FullFBSiz.w - m_FBSiz.w) / 2;

        pBuff += (m_FullFBSiz.w * 2 * y_start) + 2 * x_start;
        // SetDIBitsToDevice is much more slow, so clearing full buffer doesn't down performance
        memset(lpFrameBuffer, 0, m_FullFBSiz.w * m_FullFBSiz.h * 2);
    }

    uint32 height = m_Siz.h;
    uint32 width = m_Siz.w;
    if (!(m_Flags & GXLF_LHANDER)) 
    {
        // Right hander mode
        for (uint32 yy = 0; yy < height; ++yy)        
        {
#if defined(OPTION_SETBITS_REVERSED)
            for (uint32 xx = 0; xx < width; ++xx)
#else
            for (int xx = width - 1; xx >= 0 ; xx--)
#endif
            {
                uint32 dwVal;
                dwVal = *pSrc;
                x = yy;
                y = width - xx - 1;
                *(iDib::pixel*)(pBuff + (m_FullFBSiz.w * 2 * y) + (2 * x)) = dwVal;
                pSrc++;
            }
        }
    } 
    else 
    {
        // Left hander mode 
        for (uint32 yy = 0; yy < height; ++yy)
        {
#if defined(OPTION_SETBITS_REVERSED)
            for (uint32 xx = 0; xx < width; ++xx)
#else
            //__prefetch((void*)pSrc);
            for (int xx = width - 1; xx >= 0; xx--)
#endif
            {
                uint32 dwVal;
                dwVal = *pSrc;
                x = height - yy - 1;
                y = xx;
                *(iDib::pixel*)(pBuff + (m_FullFBSiz.w * 2 * y) + (2 * x)) = dwVal;
                pSrc++;
            }
        }
    }  
}

//////////////////////////////////////////////////////////////////////////

void iDisplay::DoPaint(const iRect& rc, HDC hdc)
{
    if (m_bRAWOutput)
        DrawRAW(hdc);
    else
        DrawGDI(hdc);  
}

void iDisplay::CvtScrCoor(sint16& px, sint16& py)
{
    sint16 npx = px;
    sint16 npy = py;

    if (!m_bFullScreen)
    {         
        uint32 y_start = (m_FullFBSiz.h - m_FBSiz.h) / 2;
        uint32 x_start = (m_FullFBSiz.w - m_FBSiz.w) / 2;

        if (m_bDoubled) x_start /= 2, y_start /= 2;

        npx -= x_start;
        npy -= y_start;
    }

   // if (npx < 0 || npy < 0 || npx >= 

    if (m_bLandscape)
    {
        if (m_Flags & GXLF_LHANDER)
        {
            // right
            px = npx;
            py = npy;
        }
        else
        {
            // left
            px = m_FBSiz.w - npx - 1;
            py = m_FBSiz.h - npy - 1;
        }
    } 
    else
    {
        if (m_Flags & GXLF_LHANDER)
        {
            // right
            px = npy;
            if (m_bDoubled) 
                py = m_FBSiz.w / 2 - npx - 1;
            else
                py = m_FBSiz.w - npx - 1;

        }
        else
        {
            // left
            if (m_bDoubled)                 
                px = m_FBSiz.h / 2 - npy - 1;
            else
                px = m_FBSiz.h - npy - 1;
            py = npx;
        }
    }

    if (px < 0 || py < 0 || px >= m_Siz.w || py >= m_Siz.h)
    {
        px = -1;
        py = -1;
        return;
    }

    if (m_Flags & GXLF_DOUBLESIZE) { px /= 2; py /= 2; }

}
// END OF FILE