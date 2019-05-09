#ifndef GXLIB_GXL_DISPLAY_H_
#define GXLIB_GXL_DISPLAY_H_

#define GXL_NOGAPI

#ifdef OS_WINCE
#elif defined(OS_WIN32)
#include "gxl.memdc.h"

#endif
/*
 * iDisplay
 */
class iDisplay : public iDispMsgHnd
{
private:
#if defined (OS_WINCE)
#	if !defined (GXL_NOGAPI)
	GXDisplayProperties	m_Props;
	uint8*				m_pBuff;
#	endif
    bool                m_bRAWOutput;
	bool				m_bVGA;
    bool                m_bFullScreen;
    iSize				m_FullFBSiz;  // FullFrameBufferSize
    iSize				m_PartFBSiz;  // PartFrameBufferSize
    iSize				m_FBSiz;   // CurrentFrameBufferSize
    iSize				m_FullSiz; // Full game size
    iSize				m_PartSiz; // Part game size
    bool                m_bLandscape; // true is DMO_0 angle is landscape mode (example Toshiba G910(?))
    bool                m_bDoubled;
#elif defined(OS_WIN32)
	iMemoryDC			m_memDC;
	bool				m_bVGA;
#endif //OS_WINCE

	iDib				m_BackBuff;
	float				m_gamma;
	iSize				m_Siz;
#ifdef OS_WIN32
	HWND				m_hWnd;
#endif
	uint32				m_Flags;
	bool				m_bInited;

private:
	void RebuildGammaTables();

public:
	iDisplay();
	~iDisplay();
	bool Init(HWND hwnd, const iSize& siz, uint32 flags);
	void SetGamma(float gamma);
	void SetOrientation(bool bLandscape, bool bLefthander);
	void Destroy();
#ifdef OS_WIN32
	void msg_OnPaint(HDC hdc);
#else
	void msg_OnPaint();
#endif
	void msg_Suspend();
	void msg_Resume();

#if defined( OS_MACOS )
	void msg_Resize();
#endif

#ifdef OS_WIN32
	void DoPaint(const iRect& rc, HDC hdc = 0);
#else
	void DoPaint(const iRect& rc);
#endif
	inline iDib& GetSurface() { return m_BackBuff; }
	inline iSize SurfMetrics() const { return m_BackBuff.GetSize(); }
#if defined (OS_WINCE)
    // switch FullScreen <-> QVGA
    // for example 240x400 <-> 240x320
    void SwitchResolution(bool full);
    void DoScreenPaint(HDC hdc);
    void DoScreenPaintRotated(HDC hdc);
    void DoScreenPaintRotatedDouble(HDC hdc);
    void CvtScrCoor(sint16& px, sint16& py);
    bool CheckRAWOutput();
    bool PreInitRAW();
    bool PreInitGDI();
    bool InitRAW();
    bool InitGDI();
    void DeinitRAW();
    void DeinitGDI();
    void DrawRAW(HDC hDC);
    void DrawGDI(HDC hDC);
#endif
#ifdef OS_WIN32
    void DoResize(const iSize &siz);

#endif
};


#endif //GXLIB_GXL_DISPLAY_H_
