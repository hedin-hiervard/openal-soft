#define FORMAT_565 1
#define FORMAT_555 2
#define FORMAT_OTHER 3

#include "gxl.inc.h"
#include "gxl.window.h"
#include "gxl.display.h"
/////////////////////////
#include "android_specific/display_android.h"
#include "android_specific/android_common.h"
#include <pthread.h>

iDisplay::iDisplay()
: m_bInited(false)
{}

iDisplay::~iDisplay()
{
        check(!m_bInited);
}

char * Android_getSurface();
void Android_updateScreenRect(unsigned short* screen, int x1, int y1, int x2, int y2);
void Android_updateSurface();
int Android_textureWidth();
int Android_textureHeight();
void Android_surfaceUnlock();
void Android_surfaceLock();

bool iDisplay::Init(HWND hwnd, const iSize& siz, uint32 flags)
{
        check(!m_bInited);

        m_Flags = flags;

		m_Siz = siz;
		//Android_initSurface(siz.w, siz.h);
//		pthread_mutex_lock(&initMutex);
		//void *buf = Android_getSurface();
		//return;

		m_BackBuff.Init(m_Siz, iDib::RGB);

	    //display_Android_init();

        return m_bInited = true;
}

void iDisplay::Destroy()
{
        check(m_bInited);
        m_bInited = false;
}

void iDisplay::msg_OnPaint()
{
        DoPaint(m_Siz);
}

void iDisplay::msg_Suspend()
{
}

void iDisplay::msg_Resume()
{
}



inline uint32 Cvt16to32(iDib::pixel pix)
{ return (uint32(pix & (0x1f<<11))) >> 8 | (uint32(pix & (0x3f<<5))) << 5 | (uint32(pix & (0x1f))) << 19; }


extern pthread_mutex_t surfaceMutex;

/*extern "C" {
void Android_updateScreen();
void Android_initSurface(int width, int height);
void Android_updateScreenRect(const unsigned short* screen, int x1, int y1, int x2, int y2);
}*/


void iDisplay::SetOrientation(bool bLandscape, bool bLefthander)
{
	if (bLefthander && (m_Flags & GXLF_LHANDER) == 0) m_Flags |= GXLF_LHANDER;
	else if (!bLefthander && (m_Flags & GXLF_LHANDER) != 0) m_Flags ^= GXLF_LHANDER;
}

uint getSizeNextPOT(uint size);

uint textW, textH;

void iDisplay::DoPaint(const iRect& rc)
{
//    return;
	const iDib::pixel* pSrc = m_BackBuff.GetPtr();
	Android_surfaceLock();
	void *buf = Android_getSurface();

	if(!buf) {
		Android_surfaceUnlock();
	   return;
   }
  int textW = Android_textureWidth();
  int textH = Android_textureHeight();

	uint16 *line;
	if(m_Flags & GXLF_QUADSIZE) { // 4x mode
		if(!(m_Flags & GXLF_LHANDER)) {
			for (uint32 yy=0; yy< m_Siz.h; ++yy)
				{
					line = (uint16*)buf + yy * 4 * textW;

					for (uint32 xx=0; xx<m_Siz.w; ++xx)
						{
							*(line) = *pSrc;
							*(line + 1) = *pSrc;
							*(line + 2) = *pSrc;
							*(line + 3) = *pSrc;
							memcpy(line + textW, line, 8);
							memcpy(line + textW * 2, line, 8);
                            memcpy(line + textW * 3, line, 8);
                            memcpy(line + textW * 4, line, 8);
							pSrc++;
							line += 4;
						}
				}
		} else {
			for (uint32 yy=0; yy< m_Siz.h; ++yy)
				{
					line = (uint16*)buf + (m_Siz.h - yy - 1) * 2 * textW + (m_Siz.w - 1) * 2;

					for (uint32 xx=0; xx<m_Siz.w; ++xx)
						{
							*(line) = *pSrc;
							*(line + 1) = *pSrc;
							memcpy(line + textW, line, 4);

							line -= 2;
							pSrc++;
						}
				}
		}

	}
	else if(m_Flags & GXLF_DOUBLESIZE) { // 2x mode

		if(!(m_Flags & GXLF_LHANDER)) {
			for (uint32 yy=0; yy< m_Siz.h; ++yy)
			{
				line = (uint16*)buf + yy * 2 * textW;

				for (uint32 xx=0; xx<m_Siz.w; ++xx)
				{
					*(line) = *pSrc;
					*(line + 1) = *pSrc;
					memcpy(line + textW, line, 4);
					pSrc++;
					line += 2;
				}
			}
		} else {
//			for (uint32 yy=0; yy< m_Siz.h; ++yy)
//			{
//				line = (uint16*)buf + (m_Siz.h - yy - 1) * 2 * textW + (m_Siz.w - 1) * 2;
//
//				for (uint32 xx=0; xx<m_Siz.w; ++xx)
//				{
//					*(line) = *pSrc;
//					*(line + 1) = *pSrc;
//					memcpy(line + textW, line, 4);
//
//					line -= 2;
//			                pSrc++;
//				}
//			}
		}

	} else { // non-2x mode

		if(!(m_Flags & GXLF_LHANDER)) {
			for (uint32 yy=0; yy< m_Siz.h; ++yy)
			{
				//uint32 yoff = (m_Siz.h - yy);
				line = (uint16*)buf + yy * textW;

				for (uint32 xx=0; xx<m_Siz.w; ++xx)
				{
					*(line + xx) = *pSrc;
					pSrc++;
				}
			}
		} else {
			for (uint32 yy=0; yy< m_Siz.h; ++yy)
			{
				uint32 yy1024 = (m_Siz.h - yy - 1) * textW + (m_Siz.w - 1);

				for (uint32 xx=0; xx<m_Siz.w; ++xx)
				{
					*((uint16*)buf + yy1024 - xx) = *pSrc;
					pSrc++;
				}
			}
		}
	}
    Android_updateSurface();
	Android_surfaceUnlock();
}
