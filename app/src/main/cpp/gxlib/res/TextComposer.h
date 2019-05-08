#ifndef GXLIB_RES_TEXTCOMPOSER_H_
#define GXLIB_RES_TEXTCOMPOSER_H_

class iTextComposer
{
public:
	typedef uint8 FontSize;

	struct FontConfig {
		FontConfig(FontSize _fontSize, const iDibFont::ComposeProps& _cmpProps, sint32 _lineHeight = 11) 
		: fontSize(_fontSize), cmpProps(_cmpProps), lineHeight(_lineHeight) {}
		
		FontConfig(const FontConfig& other) 
		: fontSize(other.fontSize), cmpProps(other.cmpProps), lineHeight(other.lineHeight) {}

		FontSize					fontSize;
		iDibFont::ComposeProps		cmpProps;
		sint32						lineHeight;
	};

	static iTextComposer& Instance()
		{
			static iTextComposer theSingleInstance;
			return theSingleInstance;
		}
public:
	bool Init(const iStringT& path, uint8 fntCount, uint32 commonKey, uint32 fileKey, uint32 fileHdrKey, uint16 fileVersion);
	void SetFontSize(FontSize fs);
	iStringT CropString(const iStringT& text, FontConfig &fc, uint32 width, bool bDot=true);
	iSize GetTextSize(const iStringT& text, FontConfig cfg);
	uint16 GetFontHeight(FontSize fs);
	uint16 GetCharHeight(FontSize fs);
	iSize GetTextBoxSize(const iStringT& text, uint32 width, FontConfig fc);
	void TextOut(const FontConfig &fc, iDib& dib, const iPoint& pos, const iStringT& text, const iRect& orc=cInvalidRect, Alignment al = AlignTopLeft, const iPoint& offset = iPoint());
	void TextOut(iDib& dib, const iPoint& pos, const iStringT& text, const iRect& orc=cInvalidRect, Alignment al=AlignTopLeft, const iPoint& offset = iPoint());
    iSize TextCut(iStringT& text, const uint32 limlen, const FontConfig &fc);
	sint32 TextBoxOut(const FontConfig &fc, iDib& dib, const iStringT& text, const iRect& orc);
	sint32 TextBoxOut(iDib& dib, const iStringT& text, const iRect& orc);
	//inline sint32 GetCharWidth(iCharW c, FontConfig fc) { return m_Fonts[fc.fontSize].GetCharWidth(c); }
	void TextOutWithScroll(const FontConfig &fc, iDib& dib, const iPoint& pos, const iStringT& text, const iRect& orc=cInvalidRect, Alignment al = AlignTopLeft, const iPoint& offset = iPoint(), sint32 scrollPos = 0);	
	inline void SetRelSizes(FontSize *fs) { for(uint32 xx=0; xx<3; xx++) m_relSizes[xx] = fs[xx]; }
private:
	bool ProcessChar(const iCharW* &text, uint32 &sid, iTextComposer::FontConfig& cfg);
private:
	iTextComposer(); // singleton
	FontConfig	m_fontConfig;
	iDibFont	*m_Fonts;
	uint8		m_fntCount;
	FontSize	m_relSizes[3];
};


#endif //GXLIB_RES_TEXTCOMPOSER_H_
