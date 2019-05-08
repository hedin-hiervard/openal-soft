#include "gxl.inc.h"
#include "gxl.window.h"

#if defined (OS_WINCE) && !defined (GXL_NOGAPI)
//#	include <gx.h>
#include "gapi_help.h"
#endif 

#include "gxl.display.h"

#if defined( OS_MACOS )
#include "pthread.h"
extern pthread_mutex_t backbufMutex;
#endif


#ifdef GXL_FBGAMMA
// Help help help ! Get rid of that !
// We have to do something about it
// waste 128K for making a pixels look brighter is unacceptable
iDib::pixel GAMMA_TABLE[65536];
//
// err.. its not going  to be inline anyway
// the source code must be available at the point of 'inlinization'
always_inline iDib::pixel BlitPixel(iDib::pixel pix)
{ return GAMMA_TABLE[pix]; }
#endif

#if defined OS_WINCE
#	if defined (GXL_NOGAPI)
#		include "gxl.display.gdi.cpp"
#	else
#		include "gxl.display.wce.cpp"
#	endif
#elif defined OS_WIN32
#	include "gxl.display.w32.cpp"
//#include "gxl.display.gdi.cpp"
#elif defined OS_IPHONE
#include "gxl.display.iphone.cpp"
#elif defined OS_MACOS
#include "gxl.display.macos.cpp"
#endif // OS_WINCE


#ifdef GXL_FBGAMMA
uint32 gamma_for( float gamma, uint32 cur, uint32 maxval )
{
	gamma = 1.15f + (gamma - 1.0f)*2.33f;
	gamma = 1.0f / (gamma);
	uint32 res = (uint32)( powf( cur/(float)maxval, gamma ) * (maxval-1) + 0.5f );
	if ( res >= maxval ) res = maxval-1;
	return res;
}

void iDisplay::RebuildGammaTables()
{
	uint32 GAMMA_TABLE_R[0x20];
	uint32 GAMMA_TABLE_G[0x40];
	uint32 GAMMA_TABLE_B[0x20];

	for (uint32 xx=0; xx<0x20; ++xx) {
		//GAMMA_TABLE_B[xx] = GAMMA_TABLE_R[xx] = iCLAMP<uint32>(0,0x1F,(uint32)pow((float)xx,m_gamma));
		GAMMA_TABLE_B[xx] = GAMMA_TABLE_R[xx] = gamma_for( m_gamma, xx, 32 );
	}
	for (uint32 xx=0; xx<0x40; ++xx) {
		//GAMMA_TABLE_G[xx] = iCLAMP<uint32>(0,0x3F,(uint32)pow((float)xx,m_gamma));
		GAMMA_TABLE_G[xx] = gamma_for( m_gamma, xx, 64 );
	}

	iDib::pixel* ptbl = GAMMA_TABLE;
	for (uint32 rr=0; rr<0x20; ++rr) {
		for (uint32 gg=0; gg<0x40; ++gg) {
			for (uint32 bb=0; bb<0x20; ++bb) {
				*ptbl++ = (GAMMA_TABLE_R[rr]<<11) | (GAMMA_TABLE_G[gg]<<5) | (GAMMA_TABLE_B[bb]);
			}
		}
	}
}

#else // GXL_FBGAMMA

void iDisplay::RebuildGammaTables()
{}

#endif // GXL_FBGAMMA

void iDisplay::SetGamma(float gamma)
{
	m_gamma = gamma;
	RebuildGammaTables();
}


