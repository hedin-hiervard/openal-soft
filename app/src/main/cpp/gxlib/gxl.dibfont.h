#ifndef GXLIB_GXL_DIBFONT_H_
#define GXLIB_GXL_DIBFONT_H_

//
class iDibFont
{
private:
	struct CharDesc {
		uint16	posX;
		uint8	width;
		uint8	reserved;
	};

	enum {
		MaxCharCode = 0x20AD,
		InvCharIdx	= 0xFFFF
	};

public:
	enum Decor {
		DecNone,
		DecShadow,
		DecBorder
	};

	enum FillType {
		Solid,
		Gradient
	};

	struct ComposeProps {
		ComposeProps(const ComposeProps& other)
			: fillType(other.fillType), gradient(other.gradient), faceColor(other.faceColor), borderColor(other.borderColor), decor(other.decor), alpha(other.alpha) {}

		ComposeProps(iDib::pixel _faceColor, iDib::pixel _borderColor = cColor_Black, Decor _decor = DecShadow, uint8 _alpha = 255) 
		: faceColor(_faceColor), borderColor(_borderColor), decor(_decor), alpha(_alpha), fillType(Solid), gradient(0,0) {}
		
		ComposeProps(const iGradient& _gradient, iDib::pixel _borderColor = cColor_Black, Decor _decor = DecShadow, uint8 _alpha = 255) 
		: gradient(_gradient), borderColor(_borderColor), decor(_decor), alpha(_alpha), fillType(Gradient), faceColor(0) {}

		FillType		fillType;
		iGradient		gradient;
		iDib::pixel		faceColor;
		iDib::pixel		borderColor;
		Decor			decor;
		uint8			alpha;
	};

public:
	iDibFont();
	~iDibFont();
	bool Init(iDynamicBuffer& buff, sint32 baseLine);

	inline bool IsValidChar(iCharW c) const 
	{ return ( c < MaxCharCode && m_idxList[c] != InvCharIdx); }
	inline bool IsValidString(iStringT& str) const 
	{ for (uint32 nn=0; nn<str.Length(); ++nn) if (!IsValidChar(str[nn])) return false; return true; }

	inline sint32 GetDefLeading() const 
	{ return m_fntHeight+1; }
	inline uint32 GetCharWidth(iCharW c) const
	{ if (!IsValidChar(c)) return 0; return m_elDesc[m_idxList[c]].width + 1; }
	inline iSize GetCharSize(iCharW c) const 
	{ return iSize(GetCharWidth(c), m_fntHeight); }
	inline uint32 GetFontHeight() const 
	{ return m_fntHeight; }

	sint32 ComposeChar(iCharW c, iDib& surf, const iPoint& pos, const ComposeProps& cmpProps, Decor layer) const;
	void ComposeText(iDib& dib, const iPoint& pos, const ComposeProps& cmpProps, const iStringT& text, const iRect& orc=cInvalidRect, Alignment al=AlignTopLeft, const iPoint& offset = iPoint());

private:
	uint32				m_fntHeight;
	iBuff<CharDesc>		m_elDesc;
	uint16				m_idxList[MaxCharCode];
	sint32				m_baseLine;
	iBuff<uint8>		m_rawFont;
};




#endif //GXLIB_GXL_DIBFONT_H_
