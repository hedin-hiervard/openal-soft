
iDisplay::iDisplay()
: m_gamma(1.0f)
{}

iDisplay::~iDisplay()
{
	Destroy();
}

bool iDisplay::Init(HWND hwnd, const iSize& siz, uint32 flags)
{
	m_Flags = flags;
	m_hWnd = hwnd;
	m_Siz = siz;
	m_BackBuff.Cleanup();
	m_BackBuff.Init(m_Siz,iDib::RGB);
	if (flags & GXLF_DOUBLESIZE) m_memDC.InitDC(iSize(siz.w*2,siz.h*2));
	else m_memDC.InitDC(siz);
	
	return true;
}

void iDisplay::SetOrientation(bool bLandscape, bool bLefthander)
{
	if (bLandscape != (bool)(m_Flags&GXLF_LANDSCAPE)) {
		if (bLandscape)  m_Flags |= GXLF_LANDSCAPE;
		else m_Flags ^= GXLF_LANDSCAPE;
		iSwap(m_Siz.w,m_Siz.h);
		m_BackBuff.Resize(m_Siz);
		if (m_Flags& GXLF_DOUBLESIZE) m_memDC.InitDC(iSize(m_Siz.w*2,m_Siz.h*2));
		else m_memDC.InitDC(m_Siz); // BUGGY?!? Cotulla
	}
	if (bLefthander && (m_Flags & GXLF_LHANDER) == 0) m_Flags |= GXLF_LHANDER;
	else if (!bLefthander && (m_Flags & GXLF_LHANDER) != 0) m_Flags ^= GXLF_LHANDER;
}

void iDisplay::Destroy()
{
}

void iDisplay::msg_OnPaint(HDC hdc)
{
	DoPaint(m_BackBuff.GetSize(), hdc);
}

void iDisplay::msg_Suspend()
{
}

void iDisplay::msg_Resume()
{
}

__forceinline uint32 Cvt16to32(iDib::pixel pix)
{ return (uint32(pix & (0x1f<<11))) << 8 | (uint32(pix & (0x3f<<5))) << 5 | (uint32(pix & (0x1f))) << 3; }

void iDisplay::DoPaint(const iRect& rc, HDC hdc)
{
	iMemoryDC::pixel* pDst = m_memDC.GetPtr();
	const iDib::pixel* pSrc = m_BackBuff.GetPtr();


	// blit
	if (m_Flags & GXLF_DOUBLESIZE) {
		uint32 dstStride = m_memDC.GetWidth();
		for (uint32 yy=0; yy<m_Siz.h; ++yy) {
			for (uint32 xx=0; xx<m_Siz.w; ++xx) {
#ifdef GXL_FBGAMMA
				uint16 pix = BlitPixel(*pSrc);
#else
				uint16 pix = *pSrc;
#endif
				uint32 val = Cvt16to32(pix);
				*pDst = val;
				pDst++;
				*pDst = val;
				pDst++;
				pSrc++;
			}
			memcpy(pDst,pDst-dstStride,(dstStride*sizeof(iMemoryDC::pixel)));
			pDst+=dstStride;
		}
	} else {
		uint32 len = m_Siz.w * m_Siz.h;
		for (uint32 xx=0; xx<len; ++xx) {
#ifdef GXL_FBGAMMA
			uint16 pix = BlitPixel(*pSrc);
#else
			uint16 pix = *pSrc;
#endif
			uint32 val = Cvt16to32(pix);
			*pDst = val;
			pSrc++; pDst++;
		}
	}

	if (!hdc) {
		HDC thdc = GetDC(m_hWnd);
		BOOL bres = BitBlt(thdc,0,0,m_memDC.GetWidth(),m_memDC.GetHeight(),m_memDC,0,0,SRCCOPY);
		ReleaseDC(m_hWnd,thdc);
	} else {
		BitBlt(hdc,0,0,m_memDC.GetWidth(),m_memDC.GetHeight(),m_memDC,0,0,SRCCOPY);
	}
}

void iDisplay::DoResize(const iSize &siz)
{
    if (m_Siz == siz)
    {
        return;
    }
    if (siz.w < 320 || siz.h < 240)
    {
        // protection...
        return;
    }

    m_Siz = siz;
    m_BackBuff.Resize(m_Siz);
    if (m_Flags & GXLF_DOUBLESIZE) 
        m_memDC.Resize(iSize(m_Siz.w * 2, m_Siz.h * 2));
    else 
        m_memDC.Resize(m_Siz);
}
