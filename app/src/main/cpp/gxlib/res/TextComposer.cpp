#include "gxl.inc.h"
#include "gxl.dibfont.h"
#include "TextComposer.h"
#include "GfxManager.h"
#include "lzo/lzo.h"
//#include "xxc/xxc.app.h"

//////////////////////////////////////////////////////////////////////////
static const iCharW CCODE = '#';
static const iStringT dots(_T("..."));

inline uint8 HexChar2Int(iCharW c)
{
	if (c >= '0' && c <= '9') return c - '0';
	else if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	else {
		check(0);
		return 0;
	}
}

bool iTextComposer::ProcessChar(const iCharW* &text, SpriteId &sid, iTextComposer::FontConfig& cfg)
{
	if (*text == CCODE) {
		++text;
		uint16 r,g,b,a;
		switch (*text){
			case 'I': 
				++text;
				r = HexChar2Int(*text++);
				g = HexChar2Int(*text++);
				b = HexChar2Int(*text++);
				a = HexChar2Int(*text++);
				sid = (r<<12) | (g<<8) | (b<<4) | a;
				break;
			case 'A':
				++text;
				r = HexChar2Int(*text++);
				g = HexChar2Int(*text++);
				cfg.cmpProps.alpha = r<<4 | g;
				break;
			case 'S':
 				++text;
				r = HexChar2Int(*text++);
				check(r < m_fntCount);
				cfg.fontSize = m_relSizes[r];
				break;
			case 'F': 
				++text;
				r = HexChar2Int(*text++);
				g = HexChar2Int(*text++);
				b = HexChar2Int(*text++);
				cfg.cmpProps.faceColor = r<<12|g<<7|b<<1;
				break;
			case 'B': 
				++text;
				r = HexChar2Int(*text++);
				g = HexChar2Int(*text++);
				b = HexChar2Int(*text++);
				cfg.cmpProps.borderColor = r<<12|g<<7|b<<1;
				break;
		}
		return false;
	} else if (*text == '\r') {
		text++;
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
iTextComposer::iTextComposer()
: m_fontConfig(0, RGB16(255,255,255))
{
	for(uint32 xx=0; xx<3; xx++)
		m_relSizes[xx] = (FontSize)xx;
}

bool iTextComposer::Init(const iStringT& path, uint8 fntCount, uint32 commonKey, uint32 fileKey, uint32 fileHdrKey, uint16 fileVersion)
{
    

	m_fntCount = fntCount;

	// Init fonts
	iFilePtr pFile = OpenWin32File(path);
	if (!pFile) return false;

	iDynamicBuffer fileBuff(pFile->GetSize());
	fileBuff.IncSize(pFile->GetSize() );
	pFile->Read(fileBuff.GetData(),pFile->GetSize());
	pFile.reset();

	m_Fonts = new iDibFont[fntCount];

	for (uint32 nn=0; nn<m_fntCount; ++nn) {
		if (!m_Fonts[nn].Init(fileBuff, (nn==2)?2:0)) return false;
	}
	
	return true;
}

void iTextComposer::SetFontSize(FontSize fs)
{
	m_fontConfig.fontSize = fs;
}

iStringT iTextComposer::CropString(const iStringT& text, FontConfig& fc, uint32 width, bool bDot)
{
	uint32 txtWidth = GetTextSize(text, fc).w;
	if (text.Empty() || txtWidth <= width) return text;

	uint32 dotsWidth = GetTextSize(dots, fc).w;
	uint32 strLen = text.Length();
	do {
		--strLen;
		txtWidth -= m_Fonts[fc.fontSize].GetCharWidth(text[strLen]);
	} while (strLen && (txtWidth+dotsWidth) > width);

	if (!strLen) return iStringT();
	return text.Left(strLen) + dots;
}

iSize iTextComposer::GetTextSize(const iStringT& text, FontConfig cfg)
{
	iSize res(2,m_Fonts[cfg.fontSize].GetFontHeight());

	
	const iCharW* ptr = text.CStr();
	const iCharW* lastChar = ptr+text.Length();

	while (ptr != lastChar) {
		SpriteId sid = -1;
		if (ProcessChar(ptr, sid, cfg)) {
			sint32 nw = m_Fonts[cfg.fontSize].GetCharWidth(*ptr);
			res.w += nw;
			++ptr;
		} else if (sid != -1) res.w += iGfxManager::Instance().Dimension(sid).w + iGfxManager::Instance().Anchor(sid).x;
	}

	return res;
}

iSize iTextComposer::GetTextBoxSize(const iStringT& text, uint32 width, FontConfig fc)
{
	check(width > 0);
	if (text.Empty()) return iSize();

	sint32 height = 0;
	const iCharT* pTB = text.CStr();
	const iCharT* pCur = pTB;
	const iCharT* pLB = pTB;
	const iCharT* pEnd = pTB + text.Length();
	const iCharT* pLS = 0;
	SpriteId sid;
	FontConfig ffc = fc;	
	sint32 fheight = m_Fonts[fc.fontSize].GetFontHeight();

	while (pEnd > pCur) {
		if (!ProcessChar(pCur, sid, ffc)) continue;
		if (*pCur == '\n') {
			height += fheight;
			++pCur;
			pLB = pCur;
			pLS = 0;
		} else {
			if (*pCur == ' ') {
				pLS = pCur;
			}
			iSize ts = GetTextSize(text.Mid(pLB-pTB, pCur-pLB+1), ffc);
			if (ts.w > width) {
				if (pLS) {
					pCur = pLS;
				} else pCur--;
				height += fheight;
				++pCur;
				pLB = pCur;
				pLS = 0;
			} else {
				++pCur;
			}
		}
	}

	if (pCur != pLB) {
		height += fheight;
	}

	return iSize(width, height);
}

void iTextComposer::TextOut(const FontConfig &fcc, iDib& dib, const iPoint& pos, const iStringT& text, const iRect& rect, Alignment al, const iPoint& offset)
{
	iPoint op(pos);
	FontConfig fc(fcc);

	if (rect != cInvalidRect) {
		iRect orc = AlignRect(GetTextSize(text,fc),rect,al);
		op = orc.point();
	}

	op += offset;

	// move baseline
	sint32 foy = op.y;
	
	const iCharW* ptr = text.CStr();
	const iCharW* lastChar = ptr+text.Length();


	while (ptr != lastChar) {
		SpriteId sid = -1;
		if (ProcessChar(ptr, sid, fc)) {
			// Shadow or border
			if (fc.cmpProps.decor != iDibFont::DecNone) m_Fonts[fc.fontSize].ComposeChar(*ptr,dib,iPoint(op.x,foy),fc.cmpProps, fc.cmpProps.decor);
			// Text face
			sint32 nw = m_Fonts[fc.fontSize].ComposeChar(*ptr,dib,iPoint(op.x,foy),fc.cmpProps, iDibFont::DecNone);
			op.x += nw;
			++ptr;
		} else if (sid != -1) {
			if (fc.cmpProps.alpha == 255) iGfxManager::Instance().Blit( sid, dib, iPoint(op.x+1,op.y+2));
			else iGfxManager::Instance().BlitAlpha( sid, dib, iPoint(op.x+1,op.y+2), fc.cmpProps.alpha >> 2);
			op.x += iGfxManager::Instance().Dimension(sid).w + iGfxManager::Instance().Anchor(sid).x;
		} else {
		}
	}

}

void iTextComposer::TextOut(iDib& dib, const iPoint& pos, const iStringT& text, const iRect& rect, Alignment al, const iPoint& offset)
{
	FontConfig fc(m_fontConfig);
	TextOut(fc,dib,pos,text,rect,al, offset);
}

iSize iTextComposer::TextCut(iStringT& text, const uint32 limlen, const FontConfig &fc){
 
    iStringT limtxt = text;    
    iSize sz = GetTextSize(text, fc);  
    
    if( sz.w>limlen ){
        
        limtxt.SetLength(limtxt.Length()-2);
        limtxt += _T("..");
        
        sz = GetTextSize(limtxt, fc);
        
        while( sz.w>limlen ){
        
            limtxt.SetLength(limtxt.Length()-3);
            limtxt += _T("..");
            sz = GetTextSize(limtxt, fc);
        }        
    }
    
    text = limtxt;
    
    return sz;
}

void iTextComposer::TextOutWithScroll(const FontConfig &fcc, iDib& dib, const iPoint& pos, const iStringT& text, const iRect& rect, Alignment al, const iPoint& offset, sint32 scrollPos)
{
	iDib tmp;
	tmp.Init(GetTextSize(text, fcc), iDib::RGB);
	scrollPos %= tmp.GetWidth();
	dib.CopyRectToDibXY(tmp, iRect(iPoint(rect.x, rect.y + (rect.h - tmp.GetHeight()) / 2), rect.size()), iPoint(scrollPos, 0));
	TextOut(fcc, tmp, iPoint(), text);
	tmp.CopyRectToDibXY(dib, iRect(scrollPos, 0, rect.w, rect.h), iPoint(rect.x, rect.y + (rect.h - tmp.GetHeight()) / 2));
	//tmp.CopyRectToDibXY(dib, iRect(0, 0, tmp.GetWidth(), tmp.GetHeight()), rect.point(), 255);
	
/*	sint32 sum = scrollPos - 5;
	sint32 x = -1;
	for(;; x++) {
		if(x >= (sint32)text.Length()) x = 0;
		sum -= m_Fonts[fcc.fontSize].GetCharWidth(text[x]);
		if(sum <= 0) { sum += m_Fonts[fcc.fontSize].GetCharWidth(text[x]); break; }
	}
	iStringT toshow = text.Right(text.Length() - x);
	uint32 y = 0, curwidth = 0;
	for(;;)
		if(y < toshow.Length() && curwidth + m_Fonts[fcc.fontSize].GetCharWidth(toshow[y]) < rect.w - 20) {
			
			curwidth += m_Fonts[fcc.fontSize].GetCharWidth(toshow[y]);
			y++;
		} else
			break;
	toshow = toshow.Left(y);
	TextOut(fcc, dib, pos - iPoint(sum - 5, 0), toshow);	
 */
}

sint32 iTextComposer::TextBoxOut(const FontConfig &fc, iDib& dib, const iStringT& text, const iRect& orc)
{
	if (text.Empty()) return 0;

	sint32 height = 0;
	iRect rect(orc);
	//rect.h = 11;

	const iCharT* pTB = text.CStr();
	const iCharT* pCur = pTB;
	const iCharT* pLB = pTB;
	const iCharT* pEnd = pTB + text.Length();
	const iCharT* pLS = 0;
	SpriteId sid;
	FontConfig ffc = fc;
	sint32 fheight = m_Fonts[fc.fontSize].GetFontHeight();

	while (pEnd > pCur) {
		if (!ProcessChar(pCur, sid, ffc)) continue;
		if (*pCur == '\n') {
			if (pLB != pCur) {
				TextOut(ffc, dib, rect.point(), text.Mid(pLB - pTB, pCur - pLB), rect, AlignTop);
			}
			height += fheight;
			rect.y += fheight;
			++pCur;
			pLB = pCur;
			pLS = 0;
		} else {
			if (*pCur == ' ') {
				pLS = pCur;
			}
			iSize ts = GetTextSize(text.Mid(pLB-pTB, pCur-pLB+1), ffc);
			if (ts.w > rect.w) {
				if (pLS) {
					pCur = pLS;
				} else pCur--;
				if (pLB != pCur) {
					TextOut(ffc, dib, rect.point(), text.Mid(pLB - pTB, pCur - pLB + 1), rect, AlignTop);
				}
				height += fheight;
				rect.y += fheight;
				++pCur;
				pLB = pCur;
				pLS = 0;
			} else {
				++pCur;
			}
		}
	}

	if (pCur != pLB) {
		TextOut(ffc, dib, rect.point(), text.Mid(pLB - pTB, pCur - pLB), rect, AlignTop);
		height += fheight;
	}

	/* 
	sint32 lineBegin=0;
	sint32 linePos = 0;
	sint32 height = 0;
	iRect rect(orc);
	rect.h=10;

	sint32 spos = text.Find(_T(' '),linePos);
	while (1) {
		iStringT tTxt = text.Mid(lineBegin, (spos ==-1)?0:spos-lineBegin);
		iSize sSiz = GetTextSize(tTxt, fc );
		if ( sSiz.w > orc.w && linePos>lineBegin) {
			iStringT oText = text.Mid(lineBegin, linePos-lineBegin-1);
			TextOut(fc,dib,rect.point(),oText,rect,AlignCenter);
			height += fheight;
			rect.y += fheight;
			lineBegin = linePos;
		}
		if (spos == -1) break;
		linePos = spos+1;
		spos = text.Find(_T(' '),linePos);
	}
	TextOut(fc,dib,rect.point(),text.Mid(lineBegin),rect,AlignCenter);
	height += fheight;
	*/

	return height;
}

sint32 iTextComposer::TextBoxOut(iDib& dib, const iStringT& text, const iRect& orc)
{
	FontConfig fc(m_fontConfig);
	return TextBoxOut(fc, dib, text, orc);
}



uint16 iTextComposer::GetFontHeight(FontSize fs)
{
	return m_Fonts[fs].GetFontHeight();
}

uint16 iTextComposer::GetCharHeight(FontSize fs)
{
    switch (fs)
    {
    case 0: return 10;
    case 1: return 14;
    case 2: return 16;
    case 3: return 24;
    case 4: return 30;
    case 5: return 36;
    }    
    return 0;
}