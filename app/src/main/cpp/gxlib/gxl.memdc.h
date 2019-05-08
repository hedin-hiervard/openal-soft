#ifndef GXLIB_GXL_MEMDC_H_
#define GXLIB_GXL_MEMDC_H_

#ifndef OS_APPLE

class iMemoryDC
{
public:
	typedef uint32 pixel;

	iMemoryDC()
	: m_hDC(NULL)
	{ }

	~iMemoryDC()
	{
		Destroy();
	}

	iMemoryDC(const iSize& siz)
	: m_hDC(NULL)
	{
		InitDC(siz);
	}

	void Destroy()
	{
		if (m_hDC) {
			::SelectObject(m_hDC,m_oldBitmap);
			::DeleteDC(m_hDC);
			m_hDC = NULL;

			m_Bits = NULL;
			if ( m_Bitmap ) {
				DeleteObject ( m_Bitmap );
				m_Bitmap = NULL;
			}
			m_Width = m_Height = 0;
			memset( &m_BitmapInfo, 0, sizeof ( BITMAPINFO ) );
		}
	}

	void InitDC(const iSize &siz)
	{
		memset( &m_BitmapInfo, 0, sizeof ( BITMAPINFO ) );
		m_BitmapInfo.bmiHeader.biSize = sizeof ( BITMAPINFOHEADER );
		m_BitmapInfo.bmiHeader.biWidth = siz.w;
		m_BitmapInfo.bmiHeader.biHeight = -(sint32)siz.h;
		m_BitmapInfo.bmiHeader.biPlanes = 1;
		m_BitmapInfo.bmiHeader.biBitCount = 32; 
		m_BitmapInfo.bmiHeader.biCompression = BI_RGB;
		m_BitmapInfo.bmiHeader.biSizeImage = siz.w * siz.h * sizeof(pixel);
		m_Bitmap = CreateDIBSection ( NULL, &m_BitmapInfo, DIB_RGB_COLORS, (void * *)&m_Bits, NULL, NULL ); 
		check(m_Bitmap);
		check(m_Bits);

		m_Width = siz.w;
		m_Height = siz.h;
		HDC ddc = ::GetDC(0);
		m_hDC = CreateCompatibleDC(ddc);
		::ReleaseDC(0,ddc);
		m_oldBitmap = ::SelectObject(m_hDC,m_Bitmap);
	}

	void Resize(const iSize &nsize)
	{
		if (nsize == GetSize()) return;
		Destroy();
		InitDC(nsize);
	}

	inline bool IsInited() const
	{ return (m_hDC != NULL); }

	inline uint32 GetWidth() const
	{ return m_Width; }

	inline uint32 GetHeight() const
	{ return m_Height; }

	inline iSize GetSize() const
	{ return iSize(m_Width,m_Height); }

	operator const HDC() const
	{ return m_hDC; }

	operator HBITMAP()
	{ return m_Bitmap; }

	inline pixel* GetPtr() 
	{ return m_Bits; }

private:
	HDC			m_hDC;
	HGDIOBJ		m_oldBitmap;

	HBITMAP		m_Bitmap;
	BITMAPINFO	m_BitmapInfo;
	uint32		m_Width;
	uint32		m_Height;
	pixel		*m_Bits;
};

#endif //OS_APPLE

#endif //GXLIB_GXL_MEMDC_H_
