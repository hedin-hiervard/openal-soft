#define FORMAT_565 1
#define FORMAT_555 2
#define FORMAT_OTHER 3

#include "gxl.inc.h"
#include "gxl.window.h"
#include "gxl.display.h"
/////////////////////////
#include "iphone_common.h"
//#include "display_iphone.h"
#include <pthread.h>

extern pthread_mutex_t initMutex;
extern pthread_mutex_t backbufMutex;


iDisplay::iDisplay()
: m_bInited(false)
{}

iDisplay::~iDisplay()
{
        check(!m_bInited);
}

bool iDisplay::Init(HWND hwnd, const iSize& siz, uint32 flags)
{
        check(!m_bInited);

        m_Flags = flags;
        
		m_BackBuff.Init(siz,iDib::RGB);

		pthread_mutex_init(&initMutex, NULL);
        pthread_mutex_init(&backbufMutex, NULL);
	
        return m_bInited = true;
}

void iDisplay::Destroy()
{
        check(m_bInited);
        m_bInited = false;
}

void iDisplay::msg_OnPaint()
{
        DoPaint(iRect());
}

void iDisplay::msg_Suspend()
{
}

void iDisplay::msg_Resume()
{
}

uint getSizeNextPOT(uint size);

void iDisplay::msg_Resize()
{
	iSize size( GetWindowWidth(), GetWindowHeight() );

    pthread_mutex_lock(&backbufMutex);    
	if( m_BackBuff.GetSize() == size ) {
        pthread_mutex_unlock(&backbufMutex);            
		return; 
    }
    pthread_mutex_unlock(&backbufMutex);            
	
 	macos_initScreen(size.w, size.h);
    
    pthread_mutex_lock(&backbufMutex);    
	m_BackBuff.Resize( size );
    pthread_mutex_unlock(&backbufMutex);

}


inline uint32 Cvt16to32(iDib::pixel pix)
{ return (uint32(pix & (0x1f<<11))) >> 8 | (uint32(pix & (0x3f<<5))) << 5 | (uint32(pix & (0x1f))) << 19; }


void iDisplay::SetOrientation(bool bLandscape, bool bLefthander)
{
	// do nothing
}

void iDisplay::DoPaint(const iRect& rc)
{
    pthread_mutex_lock(&backbufMutex);

	iDib::pixel* pSrc = m_BackBuff.GetPtr();
    
	iSize size = m_BackBuff.GetSize();
	
    pthread_mutex_lock(&initMutex);
    
	void *buf = macos_getSurface();
	if(!buf) {
        pthread_mutex_unlock(&initMutex);

		macos_initScreen(m_BackBuff.GetSize().w, m_BackBuff.GetSize().h);

        pthread_mutex_lock(&initMutex);
		buf = macos_getSurface();
	}
	
	for (sint32 yy=m_BackBuff.GetSize().h; yy>0; --yy)
	{
		sint32 off = yy *  m_BackBuff.GetSize().w;
		for (sint32 xx=0; xx< m_BackBuff.GetSize().w; ++xx)
		{
			*((uint16*)buf + off + xx) = *pSrc;
			pSrc++;
		}
	}
    pthread_mutex_unlock(&backbufMutex);
    pthread_mutex_unlock(&initMutex);
 	macos_updateScreen();
}
