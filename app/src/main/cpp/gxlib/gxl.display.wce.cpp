#define GETRAWFRAMEBUFFER   0x00020001

#define FORMAT_565 1
#define FORMAT_555 2
#define FORMAT_OTHER 3

//#include "windef.h"
//#include "gx.h"
//#include "gapi_help.h"
#include "gxl.inc.h"
#include "gxl.window.h"
#include "gxl.display.h"

/*typedef struct _RawFrameBufferInfo

#ifndef WCE_RELEASE
{
   WORD wFormat;
   WORD wBPP;
   VOID *pFramePointer;
   int  cxStride;
   int  cyStride;
   int  cxPixels;
   int  cyPixels;
} RawFrameBufferInfo;
#endif
*/

#include "gxl.gxdyn.h"
extern iGAPILoader gapi;
/////////////////////////

iDisplay::iDisplay()
: m_bVGA(false), m_pBuff(0), m_bInited(false)
{}

iDisplay::~iDisplay()
{
        check(!m_bInited);
}

bool iDisplay::Init(HWND hwnd, const iSize& siz, uint32 flags)
{
        check(!m_bInited);

        m_Flags = flags;
        m_hWnd = hwnd;
        
        // init dynamic loader
        if ( !gGapi.Init(false) ) return false;

        if (!gGapi.GXOpenDisplay(m_hWnd, GX_FULLSCREEN)) {
                return false;                           // we won't be able to draw.
        }
        
        m_Props = gGapi.GXGetDisplayProperties();

        //MessageBox(NULL, iFormat(_T("Landscape: %d, Width: %d, Height: %d, PitchX: %d, PitchY: %d\n"),m_Props.ffFormat & kfLandscape, m_Props.cxWidth, m_Props.cyHeight, m_Props.cbxPitch, m_Props.cbyPitch).CStr(), NULL, MB_OK);

	m_Siz = siz;
        m_BackBuff.Init(m_Siz,iDib::RGB);

        m_bVGA = FALSE;

        return m_bInited = true;
}

void iDisplay::SetOrientation(bool bLandscape, bool bLefthander)
{
        if (bLefthander && (m_Flags & GXLF_LHANDER) == 0) m_Flags |= GXLF_LHANDER;
        else if (!bLefthander && (m_Flags & GXLF_LHANDER) != 0) m_Flags ^= GXLF_LHANDER;
}

void iDisplay::Destroy()
{
        check(m_bInited);
        gGapi.GXCloseDisplay();
        m_bInited = false;
}

void iDisplay::msg_OnPaint(HDC hdc)
{
        DoPaint(m_Siz);
}

void iDisplay::msg_Suspend()
{
        int res = gGapi.GXSuspend();
        check(res == 1);
}

void iDisplay::msg_Resume()
{
        int res = gGapi.GXResume();
        check(res == 1);
}

void iDisplay::DoPaint(const iRect& rc, HDC)
{
        uint8 *pBuff;
        int x, y;

        pBuff = (uint8*)gGapi.GXBeginDraw();
        if (!pBuff) return;

        // blit
        const iDib::pixel* pSrc = m_BackBuff.GetPtr();

        if (!(m_Flags & GXLF_LHANDER)) 
        {
             // Right hander mode
              for (uint32 yy=0; yy< m_Siz.h; ++yy)
              {
                    for (uint32 xx=0; xx<m_Siz.w; ++xx)
                    {
                         uint32 dwVal;
#ifdef GXL_FBGAMMA
                         dwVal = BlitPixel(*pSrc);
#else
                         dwVal = *pSrc;
#endif
                         x = yy;
                         y = m_Siz.w - xx - 1;
                         *(iDib::pixel*)(pBuff + (m_Props.cbyPitch * y) + (m_Props.cbxPitch * x)) = dwVal;

                         pSrc++;
                     }
               }
          } 
          else 
          {
              // Left hander mode
              for (uint32 yy=0; yy< m_Siz.h; ++yy)
              {
                    for (uint32 xx=0; xx<m_Siz.w; ++xx)
                    {
                         uint32 dwVal;
#ifdef GXL_FBGAMMA
                         dwVal = BlitPixel(*pSrc);
#else
                         dwVal = *pSrc;
#endif
                         x = m_Siz.h - yy - 1;
                         y = xx;
                         *(iDib::pixel*)(pBuff + (m_Props.cbyPitch * y) + (m_Props.cbxPitch * x)) = dwVal;

                         pSrc++;
                     }
               }
        }

        gGapi.GXEndDraw();
}
