#include "gxl.inc.h"
#include "gxl.memory.h"
#include "gxl.file.h"
#include "gxl.dib.h"
#include "gxl.dibfont.h"

//
iDibFont::iDibFont()
{}

iDibFont::~iDibFont()
{}

bool iDibFont::Init(iDynamicBuffer& buff, sint32 baseLine)
{
	m_baseLine = baseLine;
	memset(m_idxList, 0xFFFF, sizeof(m_idxList));
	uint16 cidx = 0;
	uint32 cc;

	for (cc=0x20; cc<=0x7E; ++cc) m_idxList[cc] = cidx++;
	for (cc=0xA3; cc<=0xA5; ++cc) m_idxList[cc] = cidx++;
	for (cc=0x20AC; cc<=0x20AC; ++cc) m_idxList[cc] = cidx++;
	for (cc=0xC0; cc<=0x17E; ++cc) m_idxList[cc] = cidx++;
	for (cc=0x401; cc<=0x46A; ++cc) m_idxList[cc] = cidx++;


	uint32 el_cnt; buff.Read(el_cnt);
	m_elDesc.Allocate(el_cnt);
	uint8 fh; buff.Read(fh); m_fntHeight = fh;

	for (uint32 xx=0; xx<el_cnt; ++xx) {
		buff.Read(m_elDesc[xx].posX);
		buff.Read(m_elDesc[xx].width);
		buff.Read(m_elDesc[xx].reserved);
	}

	uint32 rawBufSiz; buff.Read(rawBufSiz);
	m_rawFont.Allocate(rawBufSiz);
	buff.Read(m_rawFont.GetPtr(), rawBufSiz);

	return true;
}

iDib::pixel BlendPixels(iDib::pixel dst, const iDib::pixel src, uint8 a)
{
		uint8 inv_a = 255-a;
		uint32 sr = a * ((src & (0x1F<<11)) >> 11);
		uint32 sg = a * ((src & (0x3F<<5)) >> 5);
		uint32 sb = a * ((src & 0x1F));
		uint32 dr = inv_a * ((dst & (0x1F<<11)) >> 11);
		uint32 dg = inv_a * ((dst & (0x3F<<5)) >> 5);
		uint32 db = inv_a * ((dst & 0x1F));
		return (uint16)(((sr+dr)>>8)<<11 | ((sg+dg)>>9)<<6 | ((sb+db)>>8));
}

sint32 iDibFont::ComposeChar(iCharW c, iDib& surf, const iPoint& pos, const ComposeProps& cmpProps, Decor layer) const
{
	if (c == 0x20 || c == 0xa0) return m_elDesc[m_idxList[0x20]].width + 1;
	uint32 stride = m_rawFont.GetSize()/m_fntHeight;		

    if(c > MaxCharCode || c < 0) return 0;
    
	uint32 posx = m_elDesc[m_idxList[c]].posX;
	uint32 width = m_elDesc[m_idxList[c]].width;
	const uint8* ptr = m_rawFont.GetPtr() + (posx + stride + 1);
	for (uint32 yy=1; yy<m_fntHeight; ++yy) {		
		iDib::pixel lineClr =  (cmpProps.fillType == Gradient)?cmpProps.gradient.GradValue(yy):cmpProps.faceColor;
		for(uint32 xx=0; xx<=width; xx++) {			
			if (layer == DecNone) {
				sint32 ox = pos.x + xx;
				sint32 oy = pos.y + yy;
				if (surf.IsValidPos(ox, oy)) {
					if(*ptr > 0) {
						uint16 alpha = ((uint16)*ptr) * ((uint16)cmpProps.alpha) / 256;
						iDib::pixel blpix = BlendPixels(surf.GetPixel(ox, oy), lineClr, (uint8)alpha);
						surf.PutPixel(ox, oy, blpix);
					}
				}
			} /* else if (layer == DecShadow) {
				sint32 ox = pos.x + xx + 1;
				sint32 oy = pos.y + yy + 1;
				if (surf.IsValidPos(ox, oy)) {
					iDib::pixel pix = (cmpProps.alpha == 255)?cmpProps.borderColor:iDib::BlendPixels(surf.GetPixel(ox, oy), cmpProps.borderColor,cmpProps.alpha);
					surf.PutPixel(ox, oy, pix);						
				}
			} else if (layer == DecBorder) {
				surf.FillRect(iRect(pos.x + xx - 1, pos.y + yy-1,3,3), cmpProps.borderColor, cmpProps.alpha);
			} */
			ptr ++;
		}
		ptr += stride;	
		ptr -= width +1; 
	}			
	return width + 1;
}

void iDibFont::ComposeText(iDib& dib, const iPoint& pos, const ComposeProps& cmpProps, const iStringT& text, const iRect& orc, Alignment al, const iPoint& offset)
{
	iPoint op(pos);
	//FontConfig fc(fcc);

	if (orc != cInvalidRect) {
		//iRect orc = AlignRect(GetTextSize(text,fc),rect,al);
		//op = orc.point();
	}

	op += offset;

	// move baseline
	sint32 foy = op.y;

	const iCharW* ptr = text.CStr();
	const iCharW* lastChar = ptr+text.Length();

	while (ptr != lastChar) {
		// Shadow or border
		// if (fc.cmpProps.decor != iDibFont::DecNone) m_Fonts[fc.fontSize].ComposeChar(*ptr,dib,iPoint(op.x,foy),fc.cmpProps, fc.cmpProps.decor);
		// Text face
		op.x += ComposeChar(*ptr,dib,iPoint(op.x,foy),cmpProps, iDibFont::DecNone);
		++ptr;
	}
}