#include "stdafx.h"
#include "Credits.h"
#include "lzo/lzo.h"
//#include "GameMenuView.h"

#include "xau/xjp.codec.h"


const uint32 R16MASK  = 0xf800;
const uint32 G16MASK  = 0x07e0;
const uint32 B16MASK  = 0x001f;

const uint32 RB16MASK = R16MASK | B16MASK;
const uint32 RB16ROUND= 0x8010;
const uint32 G16ROUND = 0;//0x0400;

// faster blending
// in: 2 muls, 7 adds
static always_inline
uint32 blend16fast( uint32 p1, uint32 p2, uint32 a )
{
	const uint32 rbdst = p2 & RB16MASK;
	uint32 res = (rbdst + ((((p1 & RB16MASK) - rbdst) * a + RB16ROUND) >> 5)) & RB16MASK;
	const uint32 gdst = p2 & G16MASK;
	res |= (gdst + ((((p1 & G16MASK) - gdst) * a + G16ROUND) >> 5)) & G16MASK;
	return res;
}



// special 2-alpha-2-pix16 blitter
// TODO: move somewhere else
static always_inline 
const uint16* blit_16aline( const uint16* src, uint16* dst, uint32 npix )
{

	while ( npix-- ) {
		uint16 alpha = *src++;
		*dst++ = blend16fast( *src++, *dst, alpha >> 8 );
		*dst++ = blend16fast( *src++, *dst, alpha & 0x1f );
//		*dst++ = *src++;
//		*dst++ = *src++;
	}
	return src;
}

void preshift_alpha( uint16* ptr, uint32 npix )
{
	npix /= 2;
	while (npix--) {
		uint16 alpha = *ptr;
		*ptr++ = (alpha >> 3) & 0x1f1f;
		ptr+=2;
	}
}


//////////////////////////////////////////////////////////////////////////
const uint16 POW13_TABLE[256] = {
	0x0000, 0x0010, 0x0027, 0x0042, 0x0061, 0x0081, 0x00A4, 0x00C8, 0x00EE, 0x0116, 0x013F, 0x0169, 0x0194, 0x01C0, 0x01EE, 0x021C,
	0x024C, 0x027C,	0x02AD, 0x02DF, 0x0312, 0x0345, 0x0379, 0x03AE, 0x03E4, 0x041A, 0x0451, 0x0489, 0x04C1, 0x04FA, 0x0533, 0x056D,
	0x05A8, 0x05E3, 0x061E, 0x065B,	0x0697, 0x06D4, 0x0712, 0x0750, 0x078F, 0x07CE, 0x080E, 0x084E, 0x088E, 0x08CF, 0x0911, 0x0952,
	0x0995, 0x09D7, 0x0A1A, 0x0A5E, 0x0AA2, 0x0AE6,	0x0B2B, 0x0B70, 0x0BB5, 0x0BFB, 0x0C41, 0x0C87, 0x0CCE, 0x0D16, 0x0D5D, 0x0DA5,
	0x0DED, 0x0E36, 0x0E7F, 0x0EC8, 0x0F12, 0x0F5C, 0x0FA6, 0x0FF0,	0x103B, 0x1086, 0x10D2, 0x111E, 0x116A, 0x11B6, 0x1203, 0x1250,
	0x129D, 0x12EB, 0x1339, 0x1387, 0x13D5, 0x1424, 0x1473, 0x14C2, 0x1512, 0x1562,	0x15B2, 0x1602, 0x1653, 0x16A4, 0x16F5, 0x1746,
	0x1798, 0x17EA, 0x183C, 0x188F, 0x18E1, 0x1934, 0x1987, 0x19DB, 0x1A2E, 0x1A82, 0x1AD6, 0x1B2B,	0x1B7F, 0x1BD4, 0x1C29, 0x1C7F,
	0x1CD4, 0x1D2A, 0x1D80, 0x1DD6, 0x1E2D, 0x1E83, 0x1EDA, 0x1F32, 0x1F89, 0x1FE0, 0x2038, 0x2090, 0x20E8, 0x2141,	0x219A, 0x21F2,
	0x224B, 0x22A5, 0x22FE, 0x2358, 0x23B2, 0x240C, 0x2466, 0x24C1, 0x251B, 0x2576, 0x25D1, 0x262D, 0x2688, 0x26E4, 0x2740, 0x279C,
	0x27F8, 0x2855, 0x28B1, 0x290E, 0x296B, 0x29C9, 0x2A26, 0x2A84, 0x2AE1, 0x2B3F, 0x2B9D, 0x2BFC, 0x2C5A, 0x2CB9, 0x2D18, 0x2D77,
	0x2DD6, 0x2E36,	0x2E95, 0x2EF5, 0x2F55, 0x2FB5, 0x3016, 0x3076, 0x30D7, 0x3138, 0x3199, 0x31FA, 0x325B, 0x32BD, 0x331E, 0x3380,
	0x33E2, 0x3444, 0x34A7, 0x3509,	0x356C, 0x35CF, 0x3632, 0x3695, 0x36F8, 0x375C, 0x37C0, 0x3823, 0x3887, 0x38EC, 0x3950, 0x39B4,
	0x3A19, 0x3A7E, 0x3AE3, 0x3B48, 0x3BAD, 0x3C12, 0x3C78, 0x3CDE, 0x3D44, 0x3DAA, 0x3E10, 0x3E76, 0x3EDD, 0x3F43, 0x3FAA, 0x4011,
	0x4078, 0x40DF, 0x4147, 0x41AE, 0x4216, 0x427E, 0x42E6, 0x434E, 0x43B6, 0x441E, 0x4487, 0x44F0, 0x4558, 0x45C1, 0x462A, 0x4694,
	0x46FD, 0x4767, 0x47D0, 0x483A, 0x48A4, 0x490E, 0x4979, 0x49E3, 0x4A4D, 0x4AB8, 0x4B23, 0x4B8E, 0x4BF9, 0x4C64, 0x4CCF, 0x4D3B,
	0x4DA7, 0x4E12, 0x4E7E, 0x4EEA, 0x4F56, 0x4FC3, 0x502F, 0x509C, 0x5108, 0x5175, 0x51E2, 0x524F, 0x52BC, 0x532A, 0x5397, 0x5405
};

inline uint32 int_pow(uint32 val, uint32 pow)
{
	uint32 res = val;
	while (--pow) {
		res = (res*val)>>8;
	}
	return res;
}

sint32 int_log(sint32 x) {
	sint32 t,y;
	y=0xa65af;
	if(x<0x00008000) x<<=16,              y-=0xb1721;
	if(x<0x00800000) x<<= 8,              y-=0x58b91;
	if(x<0x08000000) x<<= 4,              y-=0x2c5c8;
	if(x<0x20000000) x<<= 2,              y-=0x162e4;
	if(x<0x40000000) x<<= 1,              y-=0x0b172;
	t=x+(x>>1); if((t&0x80000000)==0) x=t,y-=0x067cd;
	t=x+(x>>2); if((t&0x80000000)==0) x=t,y-=0x03920;
	t=x+(x>>3); if((t&0x80000000)==0) x=t,y-=0x01e27;
	t=x+(x>>4); if((t&0x80000000)==0) x=t,y-=0x00f85;
	t=x+(x>>5); if((t&0x80000000)==0) x=t,y-=0x007e1;
	t=x+(x>>6); if((t&0x80000000)==0) x=t,y-=0x003f8;
	t=x+(x>>7); if((t&0x80000000)==0) x=t,y-=0x001fe;
	x=0x80000000-x;
	y-=x>>15;
	return y;
}


//////////////////////////////////////////////////////////////////////////
always_inline sint32 Displace(sint32 num, const sint32 siz)
{
	sint32 fsqr = int_sqrt( (256 * num) / siz );
	sint32 fcoef= -65536 / fsqr + 256;
	//sint32 fcoef2 = ilog( 256 * num / siz) / 256; 
	sint32 fmax = ((fcoef * 3 * num) / (2*siz)) / 256;
	//sint32 fmax = ((num / (2*siz)) * 3 * ( fcoef )) / 256;

	// here fmax ~ [0..256] ( 0..1 in fp )
	check( fmax >= -256 && fmax <= 256 );
	return (fmax * (rand() - RAND_MAX/2)) / RAND_MAX;

	// for reference >>>
	//float _fmax = 256 *    ((float)(num>>8) / (float)(siz+siz)) * 3.0f * (-1/sqrt((float(num>>8)/siz))+1)/*log(float(num>>8)/siz)*/;
	//sint32 res = sint32((((float)rand())/(float)RAND_MAX - 0.5f) * _fmax * 256.0f);
	//return sint32((((float)rand()) / (float)RAND_MAX - 0.5f) * fmax);
}

void DivideGrid(iMapT<uint8>& buff, sint32 x, sint32 y, sint32 width, sint32 height, sint32 c1, sint32 c2, sint32 c3, sint32 c4)
{
	sint32 Edge1, Edge2, Edge3, Edge4, Middle;
	sint32 newWidth = width >> 1;
	sint32 newHeight = height >> 1;

	if (width >= 512 || height >= 512){
		Middle = ((c1 + c2 + c3 + c4) >> 2) + Displace(newWidth + newHeight, buff.GetWidth());	//Randomly displace the midpoint!
		Edge1 = (c1 + c2) >> 1;	//Calculate the edges by averaging the two corners of each edge.
		Edge2 = (c2 + c3) >> 1;
		Edge3 = (c3 + c4) >> 1;
		Edge4 = (c4 + c1) >> 1;
		
		//Make sure that the midpoint doesn't accidentally "randomly displaced" past the boundaries!
		if (Middle < 0) {
			Middle = 0;
		} else if (Middle > 255) {
			Middle = 255;
		}
		
		//Do the operation over again for each of the four new grids.			
		DivideGrid(buff, x, y, newWidth, newHeight, c1, Edge1, Middle, Edge4);
		DivideGrid(buff, x + newWidth, y, newWidth, newHeight, Edge1, c2, Edge2, Middle);
		DivideGrid(buff, x + newWidth, y + newHeight, newWidth, newHeight, Middle, Edge2, c3, Edge3);
		DivideGrid(buff, x, y + newHeight, newWidth, newHeight, Edge4, Middle, Edge3, c4);
	} else	{
		//This is the "base case," where each grid piece is less than the size of a pixel.
		//The four corners of the grid piece will be averaged and drawn as a single pixel.

		uint8 c = uint8((c1 + c2 + c3 + c4) >> 2);
		buff.GetAt(x>>8,y>>8) = c;
	}	
}

void GenerateClouds(iMapT<uint8>& buff)
{
	sint32 c1, c2, c3, c4;
	c1 = c2 = c3 = c4 = 32;
	DivideGrid(buff, 0, 0, buff.GetWidth()<<8 , buff.GetHeight()<<8 , c1, c2, c3, c4);	
}
//////////////////////////////////////////////////////////////////////////
//sint32 t1, t2, t3, t4;

void iCreditsComposer::Init()
{
#if 0
	iFilePtr sdataFile = OpenWin32File(gDataPath + L"game.pix");
	if (!sdataFile) return;
	
	uint32 tmpBuffLen;
	
	xjp::decoder_t dec;
	void* tmpBuff;
	
/*	check( dec.width == 320 );
	check( dec.height== 720 );*/
	m_back.Init(iSize(320, 720), iDib::RGB);

	sdataFile->Read(&tmpBuffLen, sizeof(tmpBuffLen));	
	tmpBuff = malloc( tmpBuffLen );
	sdataFile->Read(tmpBuff, tmpBuffLen);
	
	xjp::dec_init( &dec, NULL, tmpBuff );
	
	xjp::dec_do( &dec, m_back.GetPtr(), 2 );
	xjp::dec_done( &dec );

	free( tmpBuff ); 	
	
	m_backWVGA.Init(iSize(480, 1080), iDib::RGB);

	sdataFile->Read(&tmpBuffLen, sizeof(tmpBuffLen));	
	tmpBuff = malloc( tmpBuffLen );
	sdataFile->Read(tmpBuff, tmpBuffLen);

	xjp::dec_init( &dec, NULL, tmpBuff );
	
	xjp::dec_do( &dec, m_backWVGA.GetPtr(), 2 );
	xjp::dec_done( &dec );

	free( tmpBuff ); 	
	
	// main logo
	sdataFile->Read(&tmpBuffLen, sizeof(tmpBuffLen));
	tmpBuff = malloc( tmpBuffLen );
	sdataFile->Read(tmpBuff, tmpBuffLen);
	xjp::dec_init( &dec, NULL, tmpBuff );
	m_logo = (uint16*)malloc( 3 * dec.width * dec.height );
	m_logoSize.w = dec.width;
	m_logoSize.h = dec.height;
	xjp::dec_do( &dec, m_logo, 6);
	xjp::dec_done( &dec );
	free( tmpBuff );

	// preconvert alpha
	preshift_alpha( m_logo, m_logoSize.w * m_logoSize.h );
#endif
	
	m_pos = 0;
    m_frame = 0;
    m_timer = fix32::zero;
}

void iCreditsComposer::Cleanup()
{
	//free( m_logo );
}


#if UNDER_CE == 300
#pragma optimize ("g",off)
#endif

void iCreditsComposer::Process(fix32 t)
{
    m_timer += t;
    if(m_timer >= fix32(0.1f)) {
        m_timer = fix32::zero;
        m_frame++;
    }
}

void iCreditsComposer::Compose(iDib& surface, const iPoint& pos)
{
#if 0
	if(gApp.Surface().GetWidth() == 320) {
		uint32 xval = ((m_pos++)/2) % 720;

		if (xval < 240) {
			m_back.CopyRectToDibXY(surface,iRect(0,720 - xval,320,xval), iPoint());
			m_back.CopyRectToDibXY(surface,iRect(0,0,320,240-xval), iPoint(0,xval));
		} else {
			m_back.CopyRectToDibXY(surface,iRect(0,720 - xval,320,240), iPoint());
		}
	} else {
		uint32 xval = ((m_pos++)/2) % 1080;
		
		uint32 height = gApp.Surface().GetHeight();
		uint32 width = gApp.Surface().GetWidth();
		if(xval < height) {
			m_backWVGA.CopyRectToDibXY(surface,iRect(0,1080 - xval,width,xval), iPoint());
			m_backWVGA.CopyRectToDibXY(surface,iRect(0,0,width,height-xval), iPoint(0,xval));
		} else {
			m_backWVGA.CopyRectToDibXY(surface,iRect(0,1080 - xval,width,height), iPoint());
		}
	}
#else
	uint16 sid;
#if defined(ROYAL_BOUNTY)
	sid = PDGG(BTN_MENU);
#else
#if defined(FULL_VERSION)
	sid = PDGG(MAINMENU_DELUXE);
#else
	sid = PDGG(MAINMENU_BACK);
#endif
#endif
    iRect rc = AlignRect(gGfxMgr.Dimension(sid), iRect(iPoint(), surface.GetSize()), AlignCenter);
    gApp.Surface().Fill(surface.GetPixel(rc.point()));
    
	gGfxMgr.Blit(sid, gApp.Surface(), rc);
#ifdef PC_VERSION
    rc.x = iMAX(rc.x, 0);
    rc.y = iMAX(rc.y, 0);
	gApp.Surface().Fill(surface.GetPixel(rc.point()));
	gGfxMgr.Blit(sid, gApp.Surface(), rc);
#endif
	
#if defined (FULL_VERSION)
	uint32 alpha = 128 + 128 * sin(((float)m_frame));
	gGfxMgr.BlitAlpha(PDGG(MAINMENU_DELUXE_OVERLAY), gApp.Surface(), rc, alpha);	
	
#endif
	
#ifndef ROYAL_BOUNTY 
#ifndef OS_IPHONE 
	//gGfxMgr.Blit(PDGG(TITLE_HMM), gApp.Surface(), rc + iPoint(20, 20));		
#endif
#endif
	
	
	/*	uint32 xval = ((m_pos++)/2) % 1080;

#if 1
	xval = 0;
#endif
	
	if(xval < 320) {
		m_back.CopyRectToDibXY(surface,iRect(0,1080 - xval,480,xval), iPoint());
		m_back.CopyRectToDibXY(surface,iRect(0,0,480,320-xval), iPoint(0,xval));
	} else {
		m_back.CopyRectToDibXY(surface,iRect(0,1080 - xval,480,320), iPoint());
	}
 */
#endif


	/*
	// motion parameters
	float p = t * 0.6f;
	float q = p;

	// setup cluster ellipse coefficients
	cluster[0].width  = (sint32)(90 + 60 * (float)pow( ( cos(q*0.4 + 0.14) + 1.0 ) / 2.0 , 0.7));
	cluster[0].height = (sint32)(100 + 48 * (float)pow( ( sin(q*0.6 + 0.28) + 1.0 ) / 2.0 , 0.7));

	// setup point position
	cluster[0].x = (sint32)(cluster[0].cx + cluster[0].width  * (float)cos(q*0.9 + 1.14));
	cluster[0].y = (sint32)(cluster[0].cy + cluster[0].height * (float)sin(q*1.2 + 0.34));

	// cluster light displacements
	cluster[0].r.x = (sint32)(44 * (float)sin(p*1.5 + 1.5));
	cluster[0].r.y = (sint32)(28 * (float)sin(p*1.5 + 1.5));
	cluster[0].g.x = (sint32)(42 * (float)sin(p*2.2 + 3.5));
	cluster[0].g.y = (sint32)(32 * (float)sin(p*2.2 + 3.5));
	cluster[0].b.x = (sint32)(39 * (float)sin(p*1.2 + 0.6));
	cluster[0].b.y = (sint32)(31 * (float)sin(p*1.2 + 0.6));

	// cluster -> fast cluster
	FASTCLUSTER fast[1];
	fast[0].x   = (int)cluster[0].x;
	fast[0].y   = (int)cluster[0].y;
	fast[0].r.x = (int)cluster[0].r.x;
	fast[0].r.y = (int)cluster[0].r.y;
	fast[0].g.x = (int)cluster[0].g.x;
	fast[0].g.y = (int)cluster[0].g.y;
	fast[0].b.x = (int)cluster[0].b.x;
	fast[0].b.y = (int)cluster[0].b.y;

	// draw bump logo to surface
	sint32 offset=0;

	uint8* pclip = clip+256*4-128;

	// render bump map
	for (int y=0; y<240; y++) {
		const uint8* imageptr = m_image.GetPtr() + y*m_image.GetWidth() + 1;
		iDib::pixel* dest = surface.GetPtr() + y*surface.GetWidth();
		for (int x=0; x<320; x++) {
			// get normal from bump map
			int nx = bump_buffer[offset*2]   + x;
			int ny = bump_buffer[offset*2+1] + y;

			// setup normal coords
			int nx0 = nx - cluster[0].x;
			int ny0 = ny - cluster[0].y;

			// red component normal coords
			int r_nx0 = nx0 + cluster[0].r.x;
			int r_ny0 = ny0 + cluster[0].r.y;

			// green component normal coords
			int g_nx0 = nx0 + cluster[0].g.x;
			int g_ny0 = ny0 + cluster[0].g.y;

			// blue component normal coords
			int b_nx0 = nx0 + cluster[0].b.x;
			int b_ny0 = ny0 + cluster[0].b.y;

			// clip coords
			r_nx0 = pclip[r_nx0];
			r_ny0 = pclip[r_ny0];
			g_nx0 = pclip[g_nx0];
			g_ny0 = pclip[g_ny0];
			b_nx0 = pclip[b_nx0];
			b_ny0 = pclip[b_ny0];

			uint8 ival = *imageptr;
			*dest++ = RGB16(clip256[ ival ][light[ r_nx0 + (r_ny0<<8) ]], clip256[ ival ][light[ g_nx0 + (g_ny0<<8) ]], clip256[ ival ][light[ b_nx0 + (b_ny0<<8) ]]);

			// next
			offset++;
			imageptr++;
		}
	}
*/

	// time
	t+=dt;

	// Compose credits
	m_scrPos ++;
	uint32 composed = 0;
	if (m_bShowCredits) {
		sint32 pixpos = m_scrPos;
		sint32 spos = 70 + (13-pixpos%14);
		sint32 cline = (pixpos / 14) - 10;

		const iSimpleArray<iStringT>& crtext = gGame.ItemMgr().m_credits;
		for (sint32 yy=0; yy<10; ++yy) {
			iTextComposer::FontConfig fc(dlgfc_plain);
			fc.cmpProps.decor = iDibFont::DecBorder;
			if (spos < 120) fc.cmpProps.alpha = (spos - 70) * 255 / 50;
			else  if (spos > 160) fc.cmpProps.alpha = (210 - spos) * 255 / 50;
			else fc.cmpProps.alpha = 255;
			if (cline >= 0 && cline < (sint32)crtext.GetSize() ) {
#if defined(OS_APPLE) || defined(OS_ANDROID)
				gTextComposer.TextOut(fc, gApp.Surface(), iPoint(), crtext[cline], iRect(75,spos + 10,320,14), AlignTop);
#else
				gTextComposer.TextOut(fc, gApp.Surface(), iPoint(), crtext[cline], iRect(0,spos,gApp.Surface().GetWidth(),14), AlignTop);				
#endif
				++composed;
			}

			spos += 14;
			cline++;
		}
	}

	if (!composed) {
		m_bEnd = true;
	}


//	gGfxMgr.Blit( PDGG(TITLE), gApp.Surface(), AlignRect(gGfxMgr.Dimension(PDGG(TITLE)), gApp.Surface().GetSize(), AlignTop).point());
}

#if UNDER_CE == 300
#pragma optimize ("g",on)
#endif


void iCreditsComposer::InitLight()
{
	// calc lighting envmap
	for (sint32 y=0; y<256; y++) {
		for (sint32 x=0; x<256; x++) {
			// setup normal
			sint32 nx = (x-128) * 2;
			sint32 ny = (y-128) * 2;
			sint32 nz = 255 - int_sqrt(nx*nx + ny*ny);
			if (nz < 0) nz = 0;

			// phong illumination
			uint32 i = ( nz*110 + int_pow(nz,5)*110 ) >> 8 ;
			int color = iCLAMP<uint32>(0,255,i);
			light[y*256+x]= (uint8)color;
		}
	}
}

void iCreditsComposer::InitBump()
{
	iMapT<uint8> clouds(512,256);
	GenerateClouds(clouds);
	m_image.Init(322,242);
	uint8* pSrc = clouds.GetPtr();
	uint8* pDst = m_image.GetPtr();
	for (uint32 yy=0; yy<242; ++yy){
		memcpy(pDst, pSrc, 322);
		pDst+=322;
		pSrc+=512;
	}

	// convert to environment x,y array
	for (uint32 y=0; y<240; y++) {
		for (uint32 x=0; x<320; x++) {
			// setup the normal from source heightmap
			sint32 nx = (sint32)(m_image.GetAt(x,y)) - (sint32)(m_image.GetAt(x+2,y)) + 128;
			sint32 ny = (sint32)(m_image.GetAt(x,y)) - (sint32)(m_image.GetAt(x,y+2)) + 128;

			// clip
			nx = iCLAMP<sint32>(0,255,nx);
			ny = iCLAMP<sint32>(0,255,ny);

			// store environment x,y
			bump_buffer[2*(y*320+x)+0] = (uint8)nx;
			bump_buffer[2*(y*320+x)+1] = (uint8)ny;
		}
	}
}

void iCreditsComposer::InitMotionBlur()
{
	for(uint32 i=0;i<256;i++) {
		for(uint32 j=0;j<256;j++) {
			uint32 blured = (i+j*7)/8;
			combine[i][j] = (uint8)blured;
		}
	}
}

void iCreditsComposer::StartCredits()
{
	check(!m_bShowCredits);
	m_bEnd = false;
	m_bShowCredits = true;
	m_scrPos = 15;
	
}

void iCreditsComposer::StopCredits()
{
	check(m_bShowCredits);
	m_bShowCredits = false;
}
