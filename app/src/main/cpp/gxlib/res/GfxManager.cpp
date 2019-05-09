#include "gxl.inc.h"
#include "GfxManager.h"
#include "sprite2.h"
#include "gxl.ctr.scopeptr.h"
#include "gammatab.h"

//////////////////////////////////////////////////////////////////////////

//
// File structure:
//
// offset:    type:
// 0000			BankHeader			4 ( * uint32)		Catalog
// 0010			Data(Raw&Packed)	hdr.dataLength(b)	Raw image
// xxxx			BankImage			hdr.numSprites		Sprite props
//
// BankImage structure
//
// NOTE! Current limitation of spanned sprite file structure -
// size of pixel chunk should be less than ( sizeof(uint16) * 65535 )
// Refer to the <*1*> marked lines

const static uint16 SPFT_SPANNED	= (1 << 0);
const static uint16 SPFT_ALPHA   	= (1 << 1);

#pragma pack(push,1)
struct BankImage
{
	uint16	imageType;		// type props
	uint32	dataOffset;		// offset of the sprite data from the begining of data block ( in uint16 )
	uint16	imageWidth;
	uint16	imageHeight;
	sint16	originX;
	sint16	originY;
};

struct BankHeader
{
	uint32	magicId;
	uint32	numSprites;
	uint32	dataLength;		// in bytes!!!
	uint32	reserved1_;
};
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////

// SpriteId explanation
// SpriteId is an unique sprite identifier
// It composed of Sprite Bank Identifier + Number of sprite in the bank
// SpriteID = uint32
// Upper 16 bit is a bank number
// Lower 16 bit is a sprite number
// [...BankID : SpriteNum ]

iGfxManager::SpriteId
iGfxManager::MakeSpriteId( BankId cat, uint32 sn )
{
	check( cat < MaxSpriteBanks );
	return sn + (((SpriteId)cat) << 16);
}

iGfxManager::SpriteId
iGfxManager::GetFlippedSpriteId( SpriteId sid )
{
	return sid | (1UL<<31);
}

bool
iGfxManager::Load( BankId cat, const TCHAR* fileName, LoadMode lmode )
{
	check( cat < MaxSpriteBanks );
	if ( cat >= MaxSpriteBanks ) return false;
	bank_[cat].Unload();
	return bank_[cat].Load( fileName, lmode, gammaLevel );
}

void
iGfxManager::Unload( BankId cat )
{
	check( cat < MaxSpriteBanks );
	if ( cat >= MaxSpriteBanks ) return;
	bank_[cat].Unload();
}

void
iGfxManager::Reload()
{
	for( size_t n = 0; n != MaxSpriteBanks; ++n ) {
		if ( bank_[n].data != 0 ) {
			bank_[n].Reload( gammaLevel );
		}
	}
}

size_t
iGfxManager::NumSprites( BankId cat ) const
{
	check( cat < MaxSpriteBanks );
	if ( cat >= MaxSpriteBanks ) return false;

	return bank_[cat].props.GetSize();
}

iGfxManager::SpriteId
iGfxManager::FirstId( BankId cat ) const
{
	check( cat < MaxSpriteBanks );
	if ( cat >= MaxSpriteBanks ) return false;

	return MakeSpriteId( cat, 0 );
}

iGfxManager::SpriteId
iGfxManager::EndId( BankId cat ) const
{
	check( cat < MaxSpriteBanks );
	if ( cat >= MaxSpriteBanks ) return false;

	return MakeSpriteId( cat, bank_[cat].props.GetSize() );
}

iSize
iGfxManager::Dimension( SpriteId sid ) const
{
	uint32 catId = sid >> 16;
	uint32 sprId = sid & 0xffff;

	check( catId < MaxSpriteBanks );
	check( sprId < bank_[catId].props.GetSize() );

	const Sprite* sprPtr= bank_[catId].GetSprite( sprId );

	return iSize( sprPtr->sizeW_, sprPtr->sizeH_ );
}

iPoint
iGfxManager::Anchor( SpriteId sid ) const
{
	uint32 catId = sid >> 16;
	uint32 sprId = sid & 0xffff;

	check( catId < MaxSpriteBanks );
	check( sprId < bank_[catId].props.GetSize() );

	const Sprite* sprPtr= bank_[catId].GetSprite( sprId );

	return iPoint( sprPtr->originX_ , sprPtr->originY_);
}

void
iGfxManager::FlipSprite(SpriteId sid)
{
	uint32 catId = sid >> 16;
	uint32 sprId = sid & 0xffff;

	check( catId < MaxSpriteBanks );
	check( sprId < bank_[catId].props.GetSize() );

	const Sprite* sprPtr= bank_[catId].GetSprite( sprId );
	/*const uint16* pixels= */bank_[catId].Data( sprPtr->offset_ );

	if (dynItems[dynPos]) delete dynItems[dynPos];
	dynItems[dynPos] = new DynItem();

	if ( sprPtr->type_ & 1 ) {
		// Spanned sprite

	} else {
		// Raw sprite
		check(0);
	}

	//dynItems[dynPos].data = (uint8*)malloc(sprPtr->)
	dynPos++;
}


uint8
iGfxManager::Shadow( SpriteId sid ) const
{
	uint32 catId = sid >> 16;
	uint32 sprId = sid & 0xffff;

	check( catId < MaxSpriteBanks );
	check( sprId < bank_[catId].props.GetSize() );

	const Sprite* sprPtr= bank_[catId].GetSprite( sprId );

	return (sprPtr->type_ & 0x100) ? 1 : 0;
}

void
iGfxManager::Blit( SpriteId sid, iDib& srf, iPoint pos ) const
{
	BlitUniversal(sid, srf, pos);

	/*uint32 catId = sid >> 16;
	uint32 sprId = sid & 0xffff;

	check( catId < MaxSpriteBanks );
	check( sprId < bank_[catId].props.GetSize() );

	const Sprite* sprPtr= bank_[catId].GetSprite( sprId );
	const uint16* pixels= bank_[catId].Data( sprPtr->offset_ );

	iRect src_rect( 0, 0, sprPtr->sizeW_, sprPtr->sizeH_ );
	if(sprPtr->type_ & 0x8)
	BlitRealAlpha( sprPtr, pixels, srf, src_rect, pos );
	else
	BlitNormal( sprPtr, pixels, srf, src_rect, pos );*/
}

void
iGfxManager::BlitFlipped( SpriteId sid, iDib& srf, iPoint pos ) const
{
	BlitUniversal(sid, srf, pos, cInvalidRect, EfxNone, false, -1, 255, FlipVert);
	//uint32 catId = sid >> 16;
	//uint32 sprId = sid & 0xffff;

	////check( catId < MaxSpriteBanks );
	////check( sprId < bank_[catId].props.GetSize() );

	//const Sprite* sprPtr= bank_[catId].GetSprite( sprId );
	//const uint16* pixels= bank_[catId].Data( sprPtr->offset_ );
	//
	//iRect src_rect( 0, 0, sprPtr->sizeW_, sprPtr->sizeH_ );
	//if(sprPtr->type_ & 0x8)
	//	BlitRealAlphaFlipped( sprPtr, pixels, srf, src_rect, pos );
	//else
	//	BlitNormalFlipped( sprPtr, pixels, srf, src_rect, pos );
}


void
iGfxManager::Blit( SpriteId sid, iDib& srf, const iRect& src, iPoint pos ) const
{
	BlitUniversal(sid, srf, pos, src);
	/*uint32 catId = sid >> 16;
	uint32 sprId = sid & 0xffff;

	check( catId < MaxSpriteBanks );
	check( sprId < bank_[catId].props.GetSize() );

	const Sprite* sprPtr= bank_[catId].GetSprite( sprId );
	const uint16* pixels= bank_[catId].Data( sprPtr->offset_ );

	if(sprPtr->type_ & 0x8)
	BlitRealAlpha( sprPtr, pixels, srf, src, pos );
	else
	BlitNormal( sprPtr, pixels, srf, src, pos );*/

}

void
iGfxManager::BlitAlpha( SpriteId sid, iDib& to, iPoint pos, uint8 a ) const
{
	BlitUniversal(sid, to, pos, cInvalidRect, EfxNone, false, -1, a, FlipNone);
	//// All other effects works only for span-sprites
	//uint32 catId = sid >> 16;
	//uint32 sprId = sid & 0xffff;

	//check( catId < MaxSpriteBanks );
	//check( sprId < bank_[catId].props.GetSize() );

	//const Sprite* sprPtr= bank_[catId].GetSprite( sprId );
	//const uint16* pixels= bank_[catId].Data( sprPtr->offset_ );
	//iRect src_rect( 0, 0, sprPtr->sizeW_, sprPtr->sizeH_ );

	////if (a == 63) BlitNormal(sprPtr, pixels, to, src_rect, pos);
	////else
	//if(sprPtr->type_ & 0x8)
	//	BlitRealConstAlpha( sprPtr, pixels, to, src_rect, pos, a );
	//else
	//	BlitAlpha(sprPtr, pixels, to, src_rect, pos, a);
}

void
iGfxManager::BlitAlphaFlipped( SpriteId sid, iDib& to, iPoint pos, uint8 a ) const
{
	BlitUniversal(sid, to, pos, cInvalidRect, EfxNone, false, -1, a, FlipVert);
	//// All other effects works only for span-sprites
	//uint32 catId = sid >> 16;
	//uint32 sprId = sid & 0xffff;

	//check( catId < MaxSpriteBanks );
	//check( sprId < bank_[catId].props.GetSize() );

	//const Sprite* sprPtr= bank_[catId].GetSprite( sprId );
	//const uint16* pixels= bank_[catId].Data( sprPtr->offset_ );
	//iRect src_rect( 0, 0, sprPtr->sizeW_, sprPtr->sizeH_ );

	////if (a == 63) BlitNormal(sprPtr, pixels, to, src_rect, pos);
	////else
	//if(sprPtr->type_ & 0x8)
	//	BlitRealConstAlphaFlipped( sprPtr, pixels, to, src_rect, pos, a );
	//else
	//	BlitAlphaFlipped(sprPtr, pixels, to, src_rect, pos, a);
}


void
iGfxManager::BlitAlpha( SpriteId sid, iDib& to, const iRect& src, iPoint pos, uint8 a ) const
{
	BlitUniversal(sid, to, pos, src, EfxNone, false, -1, a, FlipNone);
	/*
	// All other effects works only for span-sprites
	uint32 catId = sid >> 16;
	uint32 sprId = sid & 0xffff;

	check( catId < MaxSpriteBanks );
	check( sprId < bank_[catId].props.GetSize() );

	const Sprite* sprPtr= bank_[catId].GetSprite( sprId );
	const uint16* pixels= bank_[catId].Data( sprPtr->offset_ );

	// BlitNormal(sprPtr, pixels, to, src, pos);
	if(sprPtr->type_ & 0x8)
	BlitRealConstAlpha( sprPtr, pixels, to, src, pos, a );
	else
	BlitAlpha(sprPtr, pixels, to, src, pos, a);
	*/
}

void
iGfxManager::BlitEffect( SpriteId sid, iDib& to, iPoint pos, Effects efx) const
{
	BlitUniversal(sid, to, pos, cInvalidRect, efx);
	//// All other effects works only for span-sprites
	//uint32 catId = sid >> 16;
	//uint32 sprId = sid & 0xffff;

	//check( catId < MaxSpriteBanks );
	//check( sprId < bank_[catId].props.GetSize() );

	//const Sprite* sprPtr= bank_[catId].GetSprite( sprId );
	//const uint16* pixels= bank_[catId].Data( sprPtr->offset_ );

	//switch( efx ) {

	//case EfxTransparent:
	//	BlitTransparent( sprPtr, pixels, to, pos);
	//	break;

	//case EfxShadowIso:
	//	BlitShadowIso( sprPtr, pixels, to, pos );
	//	break;

	//case EfxShadowBattle:
	//	BlitShadowIsoBattle( sprPtr, pixels, to, pos );
	//	break;
	//case EfxShadowBattleFlipped:
	//	BlitShadowIsoBattleFlipped( sprPtr, pixels, to, pos );
	//	break;
	//case EfxShadow2D:
	//	BlitShadow2D( sprPtr, pixels, to, pos );
	//	break;

	//case EfxWired:
	//	BlitWired( sprPtr, pixels, to, pos );
	//	break;
	//case EfxWiredFlipped:
	//	BlitWiredFlipped( sprPtr, pixels, to, pos );
	//	break;
	//case EfxGlow:
	//	BlitGlow( sprPtr, pixels, to, pos );
	//	break;

	//default:
	//	// Just in case
	//	check(0);
	//	Blit( sid, to, pos );
	//}
}


void
iGfxManager::BlitNormal( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos ) const
{
	/*check( sprPtr != 0 && pixels != 0 );

	iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	// correct sprite position according to anchor
	pos += origin;

	// check bounds & clip
	// __NOT_NECESSARY__ if ( (pos.x + (sint32)size.w) <= 0 || (pos.y + (sint32)size.h) <= 0) return;
	iRect src_rect(src);
	//iSize siz( srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	//iRect dst_rect(pos,siz);
	//__SAME_AS__
	iRect dst_rect( pos, size );
	if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	// calc dest ptr
	uint16* dst_clr = srf.GetPtr(dst_rect);
	const size_t dstStride = srf.GetWidth();

	// blit
	if ( sprPtr->type_ & 1 ) {
	// Spanned sprite
	//if(sprPtr->type_ & 0x8) {
	//	typedef Blitter<RealAlphaBlendOp> Blt;
	//	RealAlphaBlendOp op;
	//} else {
	typedef Blitter<CopyOp> Blt;
	CopyOp op;
	//}
	// ptr, dst_clr, dstStrude, src_rect(.x .y .w, .h )

	// <*1*>
	const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	const uint16* ptr = pixels + 2;

	if (src_rect.size() == size){
	// sprite non clipped - use lighting fast blit
	while (ptr != eptr) {
	ptr = Blt::SpanFast( op, ptr,dst_clr );
	dst_clr += dstStride;
	}
	} else {
	// sprite clipped - full blown blit
	dst_clr -= src_rect.x;

	sint32 toSkip = src_rect.y;
	while (toSkip--) ptr = Blt::SpanSkip( ptr );

	const uint16* clipIn = dst_clr + src_rect.x;
	for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	ptr = Blt::Span(op, ptr, dst_clr, clipIn, clipIn + src_rect.w );
	dst_clr	+= dstStride;
	clipIn	+= dstStride;
	}
	}
	} else {
	// Raw sprite
	const uint16* src_clr = pixels + src_rect.y*size.w + src_rect.x;

	for (uint32 yy=0; yy != dst_rect.h; ++yy) {
	//BlitDibBlock_RGB(dst_clr,src_clr,dst_rect.w);
	memcpy( dst_clr, src_clr, dst_rect.w * sizeof(uint16) );
	src_clr += size.w;
	dst_clr += dstStride;
	}
	}
	*/
}


void
iGfxManager::BlitRealAlpha( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos ) const
{
	/*	check( sprPtr != 0 && pixels != 0 );

	iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	// correct sprite position according to anchor
	pos += origin;

	// check bounds & clip
	// __NOT_NECESSARY__ if ( (pos.x + (sint32)size.w) <= 0 || (pos.y + (sint32)size.h) <= 0) return;
	iRect src_rect(src);
	//iSize siz( srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	//iRect dst_rect(pos,siz);
	//__SAME_AS__
	iRect dst_rect( pos, size );
	if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	// calc dest ptr
	uint16* dst_clr = srf.GetPtr(dst_rect);
	const size_t dstStride = srf.GetWidth();

	// blit
	if ( sprPtr->type_ & 1 ) {
	// Spanned sprite
	typedef Blitter<RealAlphaBlendOp> Blt;
	RealAlphaBlendOp op;

	const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	const uint16* ptr = pixels + 2;

	if (src_rect.size() == size){
	// sprite non clipped - use lighting fast blit
	while (ptr != eptr) {
	ptr = Blt::SpanFast( op, ptr,dst_clr );
	dst_clr += dstStride;
	}
	} else {
	// sprite clipped - full blown blit
	dst_clr -= src_rect.x;

	sint32 toSkip = src_rect.y;
	while (toSkip--) ptr = Blt::SpanSkip( ptr );

	const uint16* clipIn = dst_clr + src_rect.x;
	for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	ptr = Blt::Span(op, ptr, dst_clr, clipIn, clipIn + src_rect.w );
	dst_clr	+= dstStride;
	clipIn	+= dstStride;
	}
	}
	} else {
	// Raw sprite
	const uint16* src_clr = pixels + src_rect.y*size.w + src_rect.x;

	for (uint32 yy=0; yy != dst_rect.h; ++yy) {
	BlitDibBlockRealAlpha(dst_clr,src_clr,dst_rect.w);
	//memcpy( dst_clr, src_clr, dst_rect.w * sizeof(uint16) );
	src_clr += size.w;
	dst_clr += dstStride;
	}
	}
	*/
}


void
iGfxManager::BlitRealConstAlpha( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos, uint8 alpha ) const
{
	/*check( sprPtr != 0 && pixels != 0 );

	iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	// correct sprite position according to anchor
	pos += origin;

	// check bounds & clip
	// __NOT_NECESSARY__ if ( (pos.x + (sint32)size.w) <= 0 || (pos.y + (sint32)size.h) <= 0) return;
	iRect src_rect(src);
	//iSize siz( srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	//iRect dst_rect(pos,siz);
	//__SAME_AS__
	iRect dst_rect( pos, size );
	if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	// calc dest ptr
	uint16* dst_clr = srf.GetPtr(dst_rect);
	const size_t dstStride = srf.GetWidth();

	// blit
	if ( sprPtr->type_ & 1 ) {
	// Spanned sprite
	typedef Blitter<RealConstAlphaBlendOp> Blt;
	RealConstAlphaBlendOp op(alpha);

	const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	const uint16* ptr = pixels + 2;

	if (src_rect.size() == size){
	// sprite non clipped - use lighting fast blit
	while (ptr != eptr) {
	ptr = Blt::SpanFast( op, ptr,dst_clr );
	dst_clr += dstStride;
	}
	} else {
	// sprite clipped - full blown blit
	dst_clr -= src_rect.x;

	sint32 toSkip = src_rect.y;
	while (toSkip--) ptr = Blt::SpanSkip( ptr );

	const uint16* clipIn = dst_clr + src_rect.x;
	for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	ptr = Blt::Span(op, ptr, dst_clr, clipIn, clipIn + src_rect.w );
	dst_clr	+= dstStride;
	clipIn	+= dstStride;
	}
	}
	} else {
	// Raw sprite
	const uint16* src_clr = pixels + src_rect.y*size.w + src_rect.x;

	for (uint32 yy=0; yy != dst_rect.h; ++yy) {
	BlitDibBlockRealConstAlpha(dst_clr,src_clr,dst_rect.w, alpha);
	//memcpy( dst_clr, src_clr, dst_rect.w * sizeof(uint16) );
	src_clr += size.w;
	dst_clr += dstStride;
	}
	}
	*/
}


void
iGfxManager::BlitRealConstAlphaFlipped( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos, uint8 alpha ) const
{
	/*check( sprPtr != 0 && pixels != 0 );

	iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	// correct sprite position according to anchor
	pos += origin;

	// check bounds & clip
	// __NOT_NECESSARY__ if ( (pos.x + (sint32)size.w) <= 0 || (pos.y + (sint32)size.h) <= 0) return;
	iRect src_rect(src);
	//iSize siz( srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	//iRect dst_rect(pos,siz);
	//__SAME_AS__
	iRect dst_rect( pos, size );
	if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	// calc dest ptr
	uint16* dst_clr = srf.GetPtr(dst_rect);
	const size_t dstStride = srf.GetWidth();

	// blit
	if ( sprPtr->type_ & 1 ) {
	// Spanned sprite
	typedef Blitter<RealConstAlphaBlendOp> Blt;
	RealConstAlphaBlendOp op(alpha);

	const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	const uint16* ptr = pixels + 2;

	if (src_rect.size() == size){
	// sprite non clipped - use lighting fast blit
	while (ptr != eptr) {
	ptr = Blt::SpanFastFlipped( op, ptr,dst_clr + src_rect.w);
	dst_clr += dstStride;
	}
	} else {
	// sprite clipped - full blown blit
	dst_clr -= src_rect.x;

	sint32 toSkip = src_rect.y;
	while (toSkip--) ptr = Blt::SpanSkip( ptr );

	const uint16* clipIn = dst_clr + src_rect.x;
	for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	ptr = Blt::SpanFlipped(op, ptr, dst_clr + src.w, clipIn, clipIn + src_rect.w - 1 );
	dst_clr	+= dstStride;
	clipIn	+= dstStride;
	}
	}
	} else {
	// Raw sprite unsupported now
	check(0);
	}
	*/
}



void
iGfxManager::BlitNormalFlipped( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos ) const
{
	/*check( sprPtr != 0 && pixels != 0 );

	iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	// correct sprite position according to anchor
	pos.x -= origin.x;
	pos.y += origin.y;

	// check bounds & clip
	// __NOT_NECESSARY__ if ( (pos.x + (sint32)size.w) <= 0 || (pos.y + (sint32)size.h) <= 0) return;
	iRect src_rect(src);
	//iSize siz( srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	//iRect dst_rect(pos,siz);
	//__SAME_AS__
	iRect dst_rect( pos, size );
	if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	// calc dest ptr
	uint16* dst_clr = srf.GetPtr(dst_rect);
	const size_t dstStride = srf.GetWidth();

	// blit
	if ( sprPtr->type_ & 1 ) {
	// Spanned sprite
	typedef Blitter<CopyOp> Blt;
	CopyOp op;
	// ptr, dst_clr, dstStrude, src_rect(.x .y .w, .h )

	// <*1*>
	const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	const uint16* ptr = pixels + 2;

	if (src_rect.size() == size){
	// sprite non clipped - use lighting fast blit
	while (ptr != eptr) {
	ptr = Blt::SpanFastFlipped( op, ptr,dst_clr + src_rect.w);
	dst_clr += dstStride;
	}
	} else {
	// sprite clipped - full blown blit
	dst_clr -= src_rect.x;

	sint32 toSkip = src_rect.y;
	while (toSkip--) ptr = Blt::SpanSkip( ptr );

	const uint16* clipIn = dst_clr + src_rect.x;
	for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	ptr = Blt::SpanFlipped(op, ptr, dst_clr + src.w, clipIn, clipIn + src_rect.w - 1 );
	dst_clr	+= dstStride;
	clipIn	+= dstStride;
	}
	}
	} else {
	// Raw sprites not supported just now
	check(0);
	}
	*/
}


void
iGfxManager::BlitRealAlphaFlipped( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos ) const
{
	/*	check( sprPtr != 0 && pixels != 0 );

	iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	// correct sprite position according to anchor
	pos.x -= origin.x;
	pos.y += origin.y;

	// check bounds & clip
	// __NOT_NECESSARY__ if ( (pos.x + (sint32)size.w) <= 0 || (pos.y + (sint32)size.h) <= 0) return;
	iRect src_rect(src);
	//iSize siz( srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	//iRect dst_rect(pos,siz);
	//__SAME_AS__
	iRect dst_rect( pos, size );
	if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	// calc dest ptr
	uint16* dst_clr = srf.GetPtr(dst_rect);
	const size_t dstStride = srf.GetWidth();

	// blit
	if ( sprPtr->type_ & 1 ) {
	// Spanned sprite
	typedef Blitter<RealAlphaBlendOp> Blt;
	RealAlphaBlendOp op;
	// ptr, dst_clr, dstStrude, src_rect(.x .y .w, .h )

	// <*1*>
	const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	const uint16* ptr = pixels + 2;

	if (src_rect.size() == size){
	// sprite non clipped - use lighting fast blit
	while (ptr != eptr) {
	ptr = Blt::SpanFastFlipped( op, ptr,dst_clr + src_rect.w);
	dst_clr += dstStride;
	}
	} else {
	// sprite clipped - full blown blit
	dst_clr -= src_rect.x;

	sint32 toSkip = src_rect.y;
	while (toSkip--) ptr = Blt::SpanSkip( ptr );

	const uint16* clipIn = dst_clr + src_rect.x;
	for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	ptr = Blt::SpanFlipped(op, ptr, dst_clr + src.w, clipIn, clipIn + src_rect.w - 1 );
	dst_clr	+= dstStride;
	clipIn	+= dstStride;
	}
	}
	} else {
	// Raw sprites not supported just now
	check(0);
	}
	*/
}

void
iGfxManager::BlitWired( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos ) const
{
	/*check( sprPtr != 0 && pixels != 0 );

	iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	// correct sprite position according to anchor
	pos += origin;

	// check bounds & clip
	// __NOT_NECESSARY__ if ( (pos.x + (sint32)size.w) <= 0 || (pos.y + (sint32)size.h) <= 0) return;
	iRect src_rect(src);
	//iSize siz( srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	//iRect dst_rect(pos,siz);
	//__SAME_AS__
	iRect dst_rect( pos, size );
	if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	// calc dest ptr
	uint16* dst_clr = srf.GetPtr(dst_rect);
	const size_t dstStride = srf.GetWidth();

	// blit
	if ( sprPtr->type_ & 1 ) {
	// Spanned sprite
	typedef Blitter<CopyOp> Blt;
	//CopyOp op;
	// ptr, dst_clr, dstStrude, src_rect(.x .y .w, .h )

	// <*1*>
	const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	const uint16* ptr = pixels + 2;

	uint16 s, e, ns, ne;
	s = 0;
	e = 0;

	if (false && src_rect.size() == size){
	// sprite non clipped - use lighting fast blit
	while (ptr != eptr) {
	//ptr = Blt::SpanFastWired( op, ptr,dst_clr, &ns, &ne );
	dst_clr += dstStride;
	//for(Pixel *i = s; i != ns; s < ns ? i++ : i--)
	//*i = RGB16(255, 213, 3);
	s = ns;
	e = ne;
	}
	} else {
	// sprite clipped - full blown blit
	dst_clr -= src_rect.x;

	sint32 toSkip = src_rect.y;
	while (toSkip--) ptr = Blt::SpanSkip( ptr );

	const uint16* clipIn = dst_clr + src_rect.x;
	for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	ptr = Blt::GetSpanEdges(ptr, dst_clr, &ns, &ne);//, clipIn, clipIn + src_rect.w );
	clipIn	+= dstStride;

	if(s != 0 && ns != 0)  {
	*(dst_clr + s) = RGB16(255, 213, 3);
	for(uint16 i = s; i != ns; s < ns ? i++ : i--)
	*(dst_clr + i) = RGB16(255, 213, 3);
	for(uint16 i = e; i != ne; e < ne ? i++ : i--)
	*(dst_clr + i) = RGB16(255, 213, 3);
	}
	dst_clr	+= dstStride;
	s = ns;
	e = ne;
	}
	}
	} else {
	// Raw sprite
	const uint16* src_clr = pixels + src_rect.y*size.w + src_rect.x;

	for (uint32 yy=0; yy != dst_rect.h; ++yy) {
	//BlitDibBlock_RGB(dst_clr,src_clr,dst_rect.w);
	memcpy( dst_clr, src_clr, dst_rect.w * sizeof(uint16) );
	src_clr += size.w;
	dst_clr += dstStride;
	}
	}
	*/
}

void
iGfxManager::BlitWiredFlipped( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos ) const
{
	//check( sprPtr != 0 && pixels != 0 );

	//iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	//iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	//// correct sprite position according to anchor
	//pos += origin;

	//// check bounds & clip
	//// __NOT_NECESSARY__ if ( (pos.x + (sint32)size.w) <= 0 || (pos.y + (sint32)size.h) <= 0) return;
	//iRect src_rect(src);
	////iSize siz( srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	////iRect dst_rect(pos,siz);
	////__SAME_AS__
	//iRect dst_rect( pos, size );
	//if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	//// calc dest ptr
	//uint16* dst_clr = srf.GetPtr(dst_rect);
	//const size_t dstStride = srf.GetWidth();

	//// blit
	//if ( sprPtr->type_ & 1 ) {
	//	// Spanned sprite
	//	typedef Blitter<CopyOp> Blt;
	//	CopyOp op;
	//	// ptr, dst_clr, dstStrude, src_rect(.x .y .w, .h )

	//	// <*1*>
	//	const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	//	const uint16* ptr = pixels + 2;

	//	if (src_rect.size() == size){
	//		// sprite non clipped - use lighting fast blit
	//		while (ptr != eptr) {
	//			ptr = Blt::SpanFastWiredFlipped( op, ptr,dst_clr + src_rect.w );
	//			dst_clr += dstStride;
	//		}
	//	} else {
	//		// sprite clipped - full blown blit
	//		dst_clr -= src_rect.x;

	//		sint32 toSkip = src_rect.y;
	//		while (toSkip--) ptr = Blt::SpanSkip( ptr );

	//		const uint16* clipIn = dst_clr + src_rect.x;
	//		for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	//			ptr = Blt::SpanFastWiredFlipped(op, ptr, dst_clr + src.w);//, clipIn, clipIn + src_rect.w );
	//			dst_clr	+= dstStride;

	//			clipIn	+= dstStride;
	//		}
	//	}
	//} else {
	//	// Raw sprite
	//	const uint16* src_clr = pixels + src_rect.y*size.w + src_rect.x;

	//	for (uint32 yy=0; yy != dst_rect.h; ++yy) {
	//		//BlitDibBlock_RGB(dst_clr,src_clr,dst_rect.w);
	//		memcpy( dst_clr, src_clr, dst_rect.w * sizeof(uint16) );
	//		src_clr += size.w;
	//		dst_clr += dstStride;
	//	}
	//}
}


void
iGfxManager::BlitGlow( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos ) const
{
	//check( sprPtr != 0 && pixels != 0 );

	//iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	//iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	//// correct sprite position according to anchor
	//pos += origin;

	//// check bounds & clip
	//// __NOT_NECESSARY__ if ( (pos.x + (sint32)size.w) <= 0 || (pos.y + (sint32)size.h) <= 0) return;
	//iRect src_rect(src);
	////iSize siz( srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	////iRect dst_rect(pos,siz);
	////__SAME_AS__
	//iRect dst_rect( pos, size );
	//if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	//// calc dest ptr
	//uint16* dst_clr = srf.GetPtr(dst_rect);
	//const size_t dstStride = srf.GetWidth();

	//// blit
	//if ( sprPtr->type_ & 1 ) {
	//	// Spanned sprite
	//	typedef Blitter<GlowBlendOp> Blt;
	//	GlowBlendOp op;

	//	const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	//	const uint16* ptr = pixels + 2;

	//	if (src_rect.size() == size){
	//		// sprite non clipped - use lighting fast blit
	//		while (ptr != eptr) {
	//			ptr = Blt::SpanFast( op, ptr,dst_clr );
	//			dst_clr += dstStride;
	//		}
	//	} else {
	//		// sprite clipped - full blown blit
	//		dst_clr -= src_rect.x;

	//		sint32 toSkip = src_rect.y;
	//		while (toSkip--) ptr = Blt::SpanSkip( ptr );

	//		const uint16* clipIn = dst_clr + src_rect.x;
	//		for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	//			ptr = Blt::Span(op, ptr, dst_clr, clipIn, clipIn + src_rect.w );
	//			dst_clr	+= dstStride;
	//			clipIn	+= dstStride;
	//		}
	//	}
	//}
}




void
iGfxManager::BlitAlpha( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos, uint8 a ) const
{
	//check( sprPtr != 0 && pixels != 0 );

	//// return if fully transparent
	//if ( a == 0 ) return;

	//iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	//iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	//// correct sprite position according to anchor
	//pos += origin;

	//// check bounds & clip
	//// __NOT_NECESSARY__ if ( (pos.x + (sint32)size.w) <= 0 || (pos.y + (sint32)size.h) <= 0) return;
	//iRect src_rect(src);
	////iSize siz( srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	////iRect dst_rect(pos,siz);
	////__SAME_AS__
	//iRect dst_rect( pos, size );
	//if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	//// calc dest ptr
	//uint16* dst_clr = srf.GetPtr(dst_rect);
	//const size_t dstStride = srf.GetWidth();

	//// blit
	//if ( sprPtr->type_ & 1 ) {
	//	// Spanned sprite
	//	typedef Blitter<ConstAlphaBlendOp> Blt;
	//	ConstAlphaBlendOp op(a);
	//	// ptr, dst_clr, dstStrude, src_rect(.x .y .w, .h )

	//	// <*1*>
	//	const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	//	const uint16* ptr = pixels + 2;

	//	if (src_rect.size() == size){
	//		// sprite non clipped - use lighting fast blit
	//		while (ptr != eptr) {
	//			ptr = Blt::SpanFast( op, ptr,dst_clr );
	//			dst_clr += dstStride;
	//		}
	//	} else {
	//		// sprite clipped - full blown blit
	//		dst_clr -= src_rect.x;

	//		sint32 toSkip = src_rect.y;
	//		while (toSkip--) ptr = Blt::SpanSkip( ptr );

	//		const uint16* clipIn = dst_clr + src_rect.x;
	//		for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	//			ptr = Blt::Span(op, ptr, dst_clr, clipIn, clipIn + src_rect.w );
	//			dst_clr	+= dstStride;
	//			clipIn	+= dstStride;
	//		}
	//	}
	//} else {
	//	// Raw sprite
	//	const uint16* src_clr = pixels + src_rect.y*size.w + src_rect.x;

	//	for (uint32 yy=0; yy != dst_rect.h; ++yy) {
	//		BlitDibBlockAlpha(dst_clr,src_clr,a, dst_rect.w);
	//		//memcpy( dst_clr, src_clr, dst_rect.w * sizeof(uint16) );
	//		src_clr += size.w;
	//		dst_clr += dstStride;
	//	}
	//}
}


void
iGfxManager::BlitAlphaFlipped( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos, uint8 a ) const
{
	//check( sprPtr != 0 && pixels != 0 );

	//// return if fully transparent
	//if ( a == 0 ) return;

	//iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	//iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	//// correct sprite position according to anchor
	//pos += origin;

	//// check bounds & clip
	//// __NOT_NECESSARY__ if ( (pos.x + (sint32)size.w) <= 0 || (pos.y + (sint32)size.h) <= 0) return;
	//iRect src_rect(src);
	////iSize siz( srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	////iRect dst_rect(pos,siz);
	////__SAME_AS__
	//iRect dst_rect( pos, size );
	//if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	//// calc dest ptr
	//uint16* dst_clr = srf.GetPtr(dst_rect);
	//const size_t dstStride = srf.GetWidth();

	//// blit
	//if ( sprPtr->type_ & 1 ) {
	//	// Spanned sprite
	//	typedef Blitter<ConstAlphaBlendOp> Blt;
	//	ConstAlphaBlendOp op(a);
	//	// ptr, dst_clr, dstStrude, src_rect(.x .y .w, .h )

	//	// <*1*>
	//	const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	//	const uint16* ptr = pixels + 2;

	//	if (src_rect.size() == size){
	//		// sprite non clipped - use lighting fast blit
	//		while (ptr != eptr) {
	//			ptr = Blt::SpanFastFlipped( op, ptr,dst_clr + src_rect.w);
	//			dst_clr += dstStride;
	//		}
	//	} else {
	//		// sprite clipped - full blown blit
	//		dst_clr -= src_rect.x;

	//		sint32 toSkip = src_rect.y;
	//		while (toSkip--) ptr = Blt::SpanSkip( ptr );

	//		const uint16* clipIn = dst_clr + src_rect.x;
	//		for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	//			ptr = Blt::SpanFlipped(op, ptr, dst_clr + src.w, clipIn, clipIn + src_rect.w - 1 );
	//			dst_clr	+= dstStride;
	//			clipIn	+= dstStride;
	//		}
	//	}
	//} else {
	//	// Raw sprite unsupported now
	//	check(0);
	//}
}


void
iGfxManager::BlitTransparent( const Sprite* sprPtr, const uint16* pixels, iDib& srf, iPoint pos) const
{
	//check( sprPtr != 0 && pixels != 0 );

	//if ( 0 ==(sprPtr->type_ & 1) ) {
	//	check(0); return;
	//}

	//iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	//iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	//// correct sprite position according to anchor
	//pos += origin;

	//iRect src_rect(size);
	//iRect dst_rect( pos, size );
	//if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	//// calc dest ptr
	//uint16* dst_clr = srf.GetPtr(dst_rect);
	//const size_t dstStride = srf.GetWidth();

	//// blit
	//// Spanned sprite
	//typedef Blitter<Shadow25Op> Blt;
	//Shadow25Op op;

	//// <*1*>
	//const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	//const uint16* ptr = pixels + 2;

	//if (src_rect.size() == size){
	//	// sprite non clipped - use lighting fast blit
	//	while (ptr != eptr) {
	//		ptr = Blt::SpanFast( op, ptr,dst_clr );
	//		dst_clr += dstStride;
	//	}
	//} else {
	//	// sprite clipped - full blown blit
	//	dst_clr -= src_rect.x;

	//	sint32 toSkip = src_rect.y;
	//	while (toSkip--) ptr = Blt::SpanSkip( ptr );

	//	const uint16* clipIn = dst_clr + src_rect.x;
	//	for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	//		ptr = Blt::Span( op, ptr, dst_clr, clipIn, clipIn + src_rect.w );
	//		dst_clr	+= dstStride;
	//		clipIn	+= dstStride;
	//	}
	//}
}

void
iGfxManager::BlitShadowIso( const Sprite* sprPtr, const uint16* pixels, iDib& srf, iPoint pos ) const
{
	//check( sprPtr != 0 && pixels != 0 );
	//check( (sprPtr->type_ & 1) == 1 );	// sprite must be spanned

	//iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	//iPoint opos = origin + pos;

	//// sprite rect -> sprite shadow rect
	//iSize sprSz( sprPtr->sizeW_, sprPtr->sizeH_ );
	//uint32 hhgt = (sprPtr->sizeH_ + 1) / 2;
	//check( hhgt > 1 );
	//opos.x -= hhgt ;
	//opos.y += sprSz.h - hhgt - 1;
	//sprSz.w += hhgt - 1;
	//sprSz.h = hhgt;

	//if ( (opos.x + (sint32)sprSz.w) <= 0 || (opos.y + (sint32)sprSz.h) <= 0) return;
	//iRect src_rect(sprSz);
	//iSize siz = iSize(srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	//iRect dst_rect(opos,siz);
	//if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,sprSz)) return;

	//uint16* dst_clr = srf.GetPtr(dst_rect);

	//const uint16* ptr = pixels + 2;
	//const uint16* eptr = pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	//const size_t dstStride = srf.GetWidth();

	//typedef Blitter<Shadow50Op> Blt;
	//Shadow50Op op;

	//if (src_rect.size() == sprSz  ){
	//	while (ptr != eptr) {
	//		//ptr = ProcessShadowSpanLine(ptr, dst_clr);
	//		ptr = Blt::SpanFast(op, ptr, dst_clr);
	//		++dst_clr;											// shadow skew
	//		dst_clr += dstStride;
	//		//if ( ptr != eptr ) ptr = SkipSpanLine(ptr);			// shadow skip
	//		if ( ptr != eptr ) ptr = Blt::SpanSkip(ptr);			// shadow skip
	//	//	x++;
	//	}
	//} else {
	//	dst_clr -= src_rect.x;

	//	sint32 toSkip = src_rect.y;
	//	while (toSkip--) {
	//		//ptr = SkipSpanLine(ptr);
	//		ptr = Blt::SpanSkip(ptr);
	//		check( ptr != eptr );								// skip by factor of two
	//		//ptr = SkipSpanLine(ptr);
	//		ptr = Blt::SpanSkip(ptr);
	//	}

	//	const uint16* clipIn = dst_clr + src_rect.x;
	//	dst_clr += src_rect.y;									// pre-skew ( after y-clipping )
	//	for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy) {
	//		//ptr = ProcessShadowClipSpanLine(ptr, dst_clr, clipIn, clipIn + src_rect.w );
	//		ptr = Blt::Span(op, ptr, dst_clr, clipIn, clipIn + src_rect.w );
	//		clipIn += dstStride;
	//		dst_clr += dstStride;
	//		++dst_clr;
	//		if ( ptr != eptr ) ptr = Blt::SpanSkip(ptr);
	//		else break;
	//		//ptr = SkipSpanLine(ptr);							// shadow skip
	//	}
	//}
}

void
iGfxManager::BlitShadowIsoBattle( const Sprite* sprPtr, const uint16* pixels, iDib& srf, iPoint pos ) const
{
	//check( sprPtr != 0 && pixels != 0 );
	//check( (sprPtr->type_ & 1) == 1 );	// sprite must be spanned

	//iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	//iPoint opos = origin + pos;

	//// sprite rect -> sprite shadow rect
	//iSize sprSz( sprPtr->sizeW_, sprPtr->sizeH_ );
	//uint32 hhgt = (sprPtr->sizeH_ + 1) / 2;
	//check( hhgt > 1 );
	//opos.x -= hhgt ;
	//opos.y += sprSz.h - hhgt - 1;
	//sprSz.w += hhgt - 1;
	//sprSz.h = hhgt;

	//if ( (opos.x + (sint32)sprSz.w) <= 0 || (opos.y + (sint32)sprSz.h) <= 0) return;
	//iRect src_rect(sprSz);
	//iSize siz = iSize(srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	//iRect dst_rect(opos,siz);
	//if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,sprSz)) return;

	//uint16* dst_clr = srf.GetPtr(dst_rect);

	//const uint16* ptr = pixels + 2;
	//const uint16* eptr = pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	//const size_t dstStride = srf.GetWidth();

	//typedef Blitter<Shadow50Op> Blt;
	//Shadow50Op op;

	//if (src_rect.size() == sprSz  ){
	//	while (ptr != eptr) {

	//		ptr = Blt::SpanFast(op, ptr, dst_clr);
	//		dst_clr += 2;											// shadow skew
	//		dst_clr += dstStride;
	//		if ( ptr != eptr ) ptr = Blt::SpanSkip(ptr);			// shadow skip
	//		//	x++;
	//	}
	//} else {
	//	//	dst_clr -= src_rect.x;

	//	sint32 toSkip = src_rect.y;
	//	while (toSkip--) {
	//		//ptr = SkipSpanLine(ptr);
	//		ptr = Blt::SpanSkip(ptr);
	//		check( ptr != eptr );								// skip by factor of two
	//		//ptr = SkipSpanLine(ptr);
	//		ptr = Blt::SpanSkip(ptr);
	//	}

	//	const uint16* clipIn = dst_clr + src_rect.x;
	//	dst_clr += src_rect.y;									// pre-skew ( after y-clipping )
	//	for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy) {
	//		//ptr = ProcessShadowClipSpanLine(ptr, dst_clr, clipIn, clipIn + src_rect.w );
	//		ptr = Blt::Span(op, ptr, dst_clr, clipIn, clipIn + src_rect.w );
	//		clipIn += dstStride;
	//		dst_clr += dstStride;
	//		dst_clr += 2;
	//		if ( ptr != eptr ) ptr = Blt::SpanSkip(ptr);
	//		else break;
	//		//ptr = SkipSpanLine(ptr);							// shadow skip
	//	}
	//}
}

void
iGfxManager::BlitShadowIsoBattleFlipped( const Sprite* sprPtr, const uint16* pixels, iDib& srf, iPoint pos ) const
{
	//check( sprPtr != 0 && pixels != 0 );
	//check( (sprPtr->type_ & 1) == 1 );	// sprite must be spanned

	//iPoint origin( -sprPtr->originX_, sprPtr->originY_ );
	//iPoint opos = origin + pos;

	//// sprite rect -> sprite shadow rect
	//iSize sprSz( sprPtr->sizeW_, sprPtr->sizeH_ );
	//uint32 hhgt = (sprPtr->sizeH_ + 1) / 2;
	//check( hhgt > 1 );
	//opos.x -= hhgt ;
	//opos.y += sprSz.h - hhgt - 1;
	//sprSz.w += hhgt - 1;
	//sprSz.h = hhgt;

	//if ( (opos.x + (sint32)sprSz.w) <= 0 || (opos.y + (sint32)sprSz.h) <= 0) return;
	//iRect src_rect(sprSz);
	//iRect src(sprSz);
	//iSize siz = iSize(srf.GetWidth() - pos.x, srf.GetHeight() - pos.y);
	//iRect dst_rect(opos,siz);
	//if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,sprSz)) return;

	//uint16* dst_clr = srf.GetPtr(dst_rect);

	//const uint16* ptr = pixels + 2;
	//const uint16* eptr = pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	//const size_t dstStride = srf.GetWidth();

	//typedef Blitter<Shadow50Op> Blt;
	//Shadow50Op op;

	//if (src_rect.size() == sprSz  ){
	//	//uint32 x = 0;
	//	//dst_clr -= (sprPtr->sizeH_ * 2 + 50);
	//	while (ptr != eptr) {
	//		//ptr = ProcessShadowSpanLine(ptr, dst_clr);
	//		ptr = Blt::SpanFastFlipped(op, ptr, dst_clr + src_rect.w);
	//		dst_clr += 2;											// shadow skew
	//		//dst_clr += x;
	//		dst_clr += dstStride;
	//		//if ( ptr != eptr ) ptr = SkipSpanLine(ptr);			// shadow skip
	//		if ( ptr != eptr ) ptr = Blt::SpanSkip(ptr);			// shadow skip
	//	//	x++;
	//	}
	//} else {

	//	dst_clr -= src_rect.x;

	//	sint32 toSkip = src_rect.y;
	//	while (toSkip--) {
	//		//ptr = SkipSpanLine(ptr);
	//		ptr = Blt::SpanSkip(ptr);
	//		check( ptr != eptr );								// skip by factor of two
	//		//ptr = SkipSpanLine(ptr);
	//		ptr = Blt::SpanSkip(ptr);
	//	}

	//	const uint16* clipIn = dst_clr + src_rect.x;
	//	dst_clr += (src_rect.y * 2);									// pre-skew ( after y-clipping )
	//	for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy) {
	//		ptr = Blt::SpanFlipped(op, ptr, dst_clr	+ src.w, clipIn, clipIn + src.w - 1);
	//		clipIn += dstStride;
	//		dst_clr += dstStride;
	//		dst_clr += 2;
	//		if ( ptr != eptr ) ptr = Blt::SpanSkip(ptr);
	//		else break;
	//	}
	//}
}

void
iGfxManager::BlitShadow2D( const Sprite* sprPtr, const uint16* pixels, iDib& srf, iPoint pos ) const
{
	//check( sprPtr != 0 && pixels != 0 );

	//if ( 0 ==(sprPtr->type_ & 1) ) {
	//	check(0); return;
	//}

	//iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	//iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	//// correct sprite position according to anchor
	//pos += origin;
	//pos += iPoint( -1, 2 );	// shadow displacement

	//iRect src_rect(size);
	//iRect dst_rect( pos, size );
	//if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,size)) return;

	//// calc dest ptr
	//uint16* dst_clr = srf.GetPtr(dst_rect);
	//const size_t dstStride = srf.GetWidth();

	//// blit
	//// Spanned sprite
	//typedef Blitter<Shadow50Op> Blt;
	//Shadow50Op op;

	//// <*1*>
	//const uint16* eptr= pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
	//const uint16* ptr = pixels + 2;

	//if (src_rect.size() == size){
	//	// sprite non clipped - use lighting fast blit
	//	while (ptr != eptr) {
	//		ptr = Blt::SpanFast( op, ptr,dst_clr );
	//		dst_clr += dstStride;
	//	}
	//} else {
	//	// sprite clipped - full blown blit
	//	dst_clr -= src_rect.x;

	//	sint32 toSkip = src_rect.y;
	//	while (toSkip--) ptr = Blt::SpanSkip( ptr );

	//	const uint16* clipIn = dst_clr + src_rect.x;
	//	for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
	//		ptr = Blt::Span(op, ptr, dst_clr, clipIn, clipIn + src_rect.w );
	//		dst_clr	+= dstStride;
	//		clipIn	+= dstStride;
	//	}
	//}
}

void
iGfxManager::BlitWired( const Sprite* sprPtr, const uint16* pixels, iDib& srf, iPoint pos) const
{
	check( sprPtr != 0 && pixels != 0 );

	iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	BlitWired(sprPtr, pixels, srf, iRect(0,0,size.w,size.h), pos);
}

void
iGfxManager::BlitWiredFlipped( const Sprite* sprPtr, const uint16* pixels, iDib& srf, iPoint pos) const
{
	check( sprPtr != 0 && pixels != 0 );

	iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	BlitWiredFlipped(sprPtr, pixels, srf, iRect(0,0,size.w,size.h), pos);
}

void
iGfxManager::BlitGlow( const Sprite* sprPtr, const uint16* pixels, iDib& srf, iPoint pos) const
{
	check( sprPtr != 0 && pixels != 0 );

	iPoint origin( sprPtr->originX_, sprPtr->originY_ );
	iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	BlitGlow(sprPtr, pixels, srf, iRect(pos.x,pos.y,size.w,size.h), pos);
	//BlitGlow(sprPtr, pixels, srf, iRect(0,0,size.w,size.h), pos);
}

//
void
iGfxManager::BlitTile( SpriteId sid, iDib& srf, const iRect& dst, uint8 alpha ) const
{
	BlitTile(sid,srf,Dimension(sid),dst, alpha);
}

void
iGfxManager::BlitTile( SpriteId sid, iDib& srf, const iRect& src, const iRect& dst, uint8 alpha ) const
{
	if (!src.w || !src.h) return;

	// Tile Dib
	uint32 numx = dst.w / src.w;
	uint32 numy = dst.h / src.h;
	if ((dst.w % src.w) != 0) numx ++;
	if ((dst.h % src.h) != 0) numy ++;

	for (uint32 yy=0; yy<numy; ++yy) {
		for (uint32 xx=0; xx<numx; ++xx) {
			sint32 w = iMIN(src.w,dst.w - (xx*src.w));
			sint32 h = iMIN(src.h,dst.h - (yy*src.h));
            if(alpha != 255)
                Blit( sid, srf, iRect(src.x,src.y,w,h), iPoint( dst.x+xx*src.w, dst.y+yy*src.h ) );
            else
                BlitAlpha( sid, srf, iRect(src.x,src.y,w,h), iPoint( dst.x+xx*src.w, dst.y+yy*src.h ), alpha );
		}
	}
}

void
iGfxManager::BlitTile3( SpriteId sid, iDib& srf, const iRect& src, const iRect& dst ) const
{
	uint32 sH = src.h;
	uint32 sW = 0;

	if (!sH || src.w%3 != 0 || !(sW=src.w/3)){
		check(0); return;
	}

	Blit( sid, srf, iRect(src.x,src.y,sW,sH), dst.point() );
	Blit( sid, srf, iRect(src.x+sW*2, src.y,sW,sH), iPoint(dst.x+dst.w-sW, dst.y) );
	BlitTile( sid, srf, iRect(src.x+sW,src.y,sW,sH), iRect(dst.x+sW,dst.y,dst.w-sW*2,sH) );
}

void
iGfxManager::MakeDib( SpriteId sid, iDib& dst )
{
	// reside dib
	// same as blit
}

size_t
iGfxManager::MemoryOccuped() const
{
	size_t total = 0;
	for( uint32 n = 0; n != MaxSpriteBanks; ++n ) {
		total += bank_[n].MemTotal();
	}
	return total;
}

// ....

iGfxManager::SpriteBank::SpriteBank()
{
	data = 0;
	dataSize = 0;
#ifdef OS_WIN32
	hFile = INVALID_HANDLE_VALUE;
	hMapping = INVALID_HANDLE_VALUE;
#else
	pFile = NULL;
	//buf.Clear();
#endif
	pMap = NULL;
	loadMode = LM_Unknown;
}

iGfxManager::SpriteBank::~SpriteBank()
{
	Unload();
}


///////////////////////////////////////////////////////////////////////////////
struct ApplyGammaMethod
{
	ScopedArray<uint16> trans_;
	ApplyGammaMethod( const uint8* table )
	{
		uint16* mapping = new uint16[65536];
		trans_.Reset( mapping );
		for (uint32 rr=0; rr<0x20; ++rr) {
			for (uint32 gg=0; gg<0x40; ++gg) {
				for (uint32 bb=0; bb<0x20; ++bb) {
					*mapping++ = (((uint32)table[rr])<<11) | (((uint32)table[32+gg])<<5) | ((uint32)table[bb]);
				}
			}
		}
	}

	struct Operator
	{
		const uint16* tab_;
		always_inline Operator( const uint16* t ) : tab_(t) {}
		always_inline void operator()( Pixel* ptr ) const
		{
			*ptr = tab_[*ptr];
		}
	};

	always_inline Operator makeop() const
	{ return Operator(trans_.Get()); }

};


//////////////////////////////////////////////////////////////////////////////

bool
iGfxManager::SpriteBank::Load( const TCHAR* filename, LoadMode lmode, uint32 gammaLevel )
{
	fileName = filename;
	if (lmode == LM_Memory) {
		iFilePtr file = OpenWin32File(filename);
		if (!file) return false;

		// NOTE! Reading POD structures is safe but beware _PACKING_

		// read header
		BankHeader header;
		file->Read( &header, sizeof(BankHeader) );

		// read pixels grand totale
		data = (uint8*)malloc( header.dataLength );
		file->Read( data, header.dataLength );
		dataSize = header.dataLength;

		file->Seek(16,iFileI::FSEEK_CUR);
		//uint32 fpos=file->GetPosition();

		props.Allocate( header.numSprites );
		iGfxManager::Sprite* spr = props.GetPtr();
		// read sprite records
		for( uint32 n = 0; n != header.numSprites; ++n ) {
			BankImage img;
			file->Read( &img, sizeof(BankImage) );

			spr[n].offset_	= img.dataOffset;
			spr[n].sizeW_	= img.imageWidth;
			spr[n].sizeH_	= img.imageHeight;
			spr[n].originX_	= img.originX;
			spr[n].originY_ = img.originY;
			spr[n].type_	= img.imageType;
		}
		// postprocess sprites and apply gamma
		/*if ( gammaLevel != 0 ) {
		typedef Blitter<ApplyGammaMethod::Operator> Blt;
		check( gammaLevel < gammaTabNum );
		ApplyGammaMethod method( gammaTabs[gammaLevel] );
		for( uint32 n = 0; n != header.numSprites; ++n ) {
		// hack!! hack!!
		if ( spr[n].type_ & 1 ) {
		uint16* pixels= (uint16*)Data( spr[n].offset_ );
		uint16* ptr = pixels + 2;
		uint16* eptr = pixels + (uint32)((*pixels) << 16 | *(pixels + 1)) + 2;
		while ( ptr != eptr ) {
		ptr = Blt::ProcessSpan( method.makeop(), ptr );
		}
		} else {
		uint16* pixels= (uint16*)Data( spr[n].offset_ );
		uint16* ptr = pixels;
		uint16* eptr = pixels + spr[n].sizeH_ * spr[n].sizeW_;
		ApplyGammaMethod::Operator op = method.makeop();
		while ( ptr != eptr ) {
		op( ptr++ );
		}
		}
		}

		}*/
	} else if (lmode == LM_MappedFile) {
#if defined OS_WIN32
		hFile = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
#elif defined OS_WINCE
		hFile = CreateFileForMapping( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
#endif
#ifdef OS_WIN32
		//PORTTODO
		if ( INVALID_HANDLE_VALUE == hFile ) {
			DWORD err = GetLastError();
			return false;
		}
		hMapping = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
		if ( 0 == hMapping ) {
			DWORD err = GetLastError();
			return false;
		}
		pMap = MapViewOfFile( hMapping, FILE_MAP_READ, 0, 0, 0 );
		if ( NULL == pMap ) {
			DWORD err = GetLastError();
			return false;
		}
		// NOTE! Reading POD structures is safe but beware _PACKING_

		// read header
		BankHeader header = *(BankHeader*)pMap;

		// read pixels grand totale
		data = (uint8*)pMap + sizeof(BankHeader);
		dataSize = header.dataLength;

		//	file->Seek(16,iFileI::FSEEK_CUR);
		//	uint32 fpos=file->GetPosition();

		props.Allocate( header.numSprites );
		iGfxManager::Sprite* spr = props.GetPtr();
		// read sprite records
		BankImage* recPtr = (BankImage*)((uint8*)pMap + sizeof(BankHeader) + header.dataLength + 16);
		for( uint32 n = 0; n != header.numSprites; ++n ) {
			BankImage img = *recPtr++;

			spr[n].offset_	= img.dataOffset;
			spr[n].sizeW_	= img.imageWidth;
			spr[n].sizeH_	= img.imageHeight;
			spr[n].originX_	= img.originX;
			spr[n].originY_ = img.originY;
			spr[n].type_	= img.imageType;
		}
#endif
	} else {
		check("Unknown load mode!" == 0);
		return false;
	}
	loadMode = lmode;

	return true;
}

void
iGfxManager::SpriteBank::Unload()
{
#ifdef OS_WIN32
	//PORTTODO
	props.Clean();
	if (loadMode == LM_Memory) {
		free( data );
	} else if (loadMode == LM_MappedFile) {
		if ( pMap ) {
			UnmapViewOfFile( pMap );
			pMap = 0;
		}
		if ( hMapping ) {
			CloseHandle( hMapping );
			hMapping = INVALID_HANDLE_VALUE;
		}
		if ( hFile ) {
			CloseHandle( hFile );
			hFile = INVALID_HANDLE_VALUE;
		}
	}
#endif
}

bool
iGfxManager::SpriteBank::Reload( uint32 gammaLevel )
{
	if ( loadMode != LM_MappedFile ) {
		Unload();
		return Load( fileName.CStr(), loadMode, gammaLevel );
	}
	return true;
}

const iGfxManager::Sprite*
iGfxManager::SpriteBank::GetSprite( uint32 num ) const
{
	check( num < 65536 );		// just in case someone pass full sprite id
	check( num < props.GetSize() );
	return props.GetPtr() + num;
}

const uint8*
iGfxManager::SpriteBank::Data( uint32 offset ) const
{
	return data + offset;
}

size_t
iGfxManager::SpriteBank::MemTotal() const
{
	return dataSize + props.GetSize() * sizeof(iGfxManager::Sprite);
}



inline void BlitDibBlockRealAlpha(iDib::pixel *dst, const uint8 *src, uint32 size)
{
	for (uint32 xx=0; xx<size; ++xx, ++dst, src+=3) {
		const uint16 a  = *((const uint16*)src);
		const uint16 b  = *dst;
		uint32 sb = a & 0x1f;
		uint32 sg = (a >> 5) & 0x3f;
		uint32 sr = (a >> 11) & 0x1f;
		uint32 db = b & 0x1f;
		uint32 dg = (b >> 5) & 0x3f;
		uint32 dr = (b >> 11) & 0x1f;

		*dst = (iDib::pixel)(((*(src+2) * (sb-db)) >> 8) + db
			| (((*(src+2) * (sg-dg)) >> 8) + dg) << 5
			| (((*(src+2) * (sr-dr)) >> 8) + dr) << 11);

	}
}

inline void BlitDibBlockRealConstAlpha(iDib::pixel *dst, const uint8 *src, uint32 size, uint8 alpha)
{
	for (uint32 xx=0; xx<size; ++xx, ++dst, src+=3) {
		const uint16 a  = *((const uint16*)src);
		const uint16 b  = *dst;
		uint32 sb = a & 0x1f;
		uint32 sg = (a >> 5) & 0x3f;
		uint32 sr = (a >> 11) & 0x1f;
		uint32 db = b & 0x1f;
		uint32 dg = (b >> 5) & 0x3f;
		uint32 dr = (b >> 11) & 0x1f;

		*dst = (iDib::pixel)(((*(src+2) * alpha * (sb-db)) >> 8) + db
			| (((*(src+2) * alpha * (sg-dg)) >> 8) + dg) << 5
			| (((*(src+2) * alpha * (sr-dr)) >> 8) + dr) << 11);

	}
}

void
iGfxManager::BlitMasked( SpriteId sid, SpriteId mid, iDib& srf, iPoint pos ) const
{
//	return;
	uint32 catSId = sid >> 16;
	uint32 sprSId = sid & 0xffff;
	check( catSId < MaxSpriteBanks );
	check( sprSId < bank_[catSId].props.GetSize() );

	uint32 catMId = mid >> 16;
	uint32 sprMId = mid & 0xffff;
	check( catMId < MaxSpriteBanks );
	check( sprMId < bank_[catMId].props.GetSize() );

	const Sprite* sprPtr= bank_[catSId].GetSprite( sprSId );
	const uint8* sprPix= bank_[catSId].Data( sprPtr->offset_ );

	const Sprite* mskPtr= bank_[catMId].GetSprite( sprMId );
	const uint8* mskPix= bank_[catMId].Data( mskPtr->offset_ );

	check( (sprPtr->type_ & SPFT_SPANNED) == 0 );	// sprite should be RAW
	check( (mskPtr->type_ & SPFT_SPANNED) == 1 );	// while mask is spanned
	check( (mskPtr->type_ & SPFT_ALPHA) == 0   );   // alpha not supported currently

	//-------------------------------------------------------

	iPoint sprOrigin( sprPtr->originX_, sprPtr->originY_ );
	iSize  sprSize( sprPtr->sizeW_, sprPtr->sizeH_ );

	// correct sprite position according to anchor
	pos += sprOrigin;

	iRect src_rect(sprSize);
	iRect dst_rect( pos, sprSize ); // ? was 'siz' = srf.Size - pos
	if (!iClipper::iClipRectRect(dst_rect,srf.GetSize(),src_rect,sprSize)) return;

	// calc dest ptr
	uint16* dptr = srf.GetPtr(dst_rect);
	const uint8* sptr = sprPix;
	// <*1*>
	const uint8* eptr= mskPix + ((*((uint32*)mskPix))) + 4;	// first 4 bytes are an offset in bytes
	const uint8* mptr = mskPix + 4;
	const size_t dstStride = srf.GetWidth();
	const size_t srcStride = sprSize.w * 2;

	// Spanned sprite
	typedef Blitter<CopyOp> Blt;
	CopyOp op;

	if (src_rect.size() == sprSize){
		// sprite non clipped - use lighting fast blit
		while (mptr != eptr) {
			mptr = Blt::MaskedSpanFast(op, mptr, sptr, dptr, 2 );
			dptr += dstStride;
			sptr += srcStride;
		}
	} else {
		// sprite clipped - full blown blit
		dptr -= src_rect.x;

		sint32 toSkip = src_rect.y;
		sptr += sprSize.w * toSkip * 2;
		while (toSkip--) mptr = Blt::SpanSkip( mptr, 2 );

		const uint16* clipIn = dptr + src_rect.x;
		for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){
			mptr = Blt::MaskedSpan( op, mptr, sptr, dptr, clipIn, clipIn + src_rect.w, 2 );
			dptr	+= dstStride;
			clipIn	+= dstStride;
			sptr	+= srcStride;
		}

	}
}

#define BLITWRK_BODY(Op) \
void iGfxManager::BlitWorker##Op(const uint8* pixels, uint16* dst_clr, iSize size, const size_t dstStride, iRect& src_rect, iRect& src, iRect& dst_rect, uint8 alpha, uint8 bpp, bool spanned, bool realAlpha, FlipType flp, Effects efx) const \
{ \
	typedef Blitter<Op> Blt;	\
	Op op(alpha);\
	bool flipped = (flp == FlipVert);\
	if (spanned) {		\
		const uint8* eptr= pixels + (*((uint32*)pixels)) + 4;\
const uint8* ptr = pixels + 4;\
		uint32 imgOffset = (flipped ? src.w : 0);\
		uint32 clipOffset = (efx == EfxShadowBattle ? src.w : src_rect.w);\
		if (src_rect.size() == size){\
			while (ptr != eptr) {				\
				ptr = Blt::SpanFast( op, ptr, dst_clr + imgOffset, bpp, flipped );\
				dst_clr += dstStride;\
				if(efx == EfxShadowBattle) {\
					dst_clr += 2;\
					if ( ptr != eptr ) ptr = Blt::SpanSkip(ptr, bpp);\
				} else if(efx == EfxShadowIso) {\
					dst_clr++;\
					if ( ptr != eptr ) ptr = Blt::SpanSkip(ptr, bpp);\
				}\
			}\
		} else {\
			dst_clr -= src_rect.x;\
			sint32 toSkip = src_rect.y;\
			while (toSkip--) {\
				ptr = Blt::SpanSkip( ptr, bpp );\
				if(efx == EfxShadowBattle || efx == EfxShadowIso) \
					ptr = Blt::SpanSkip( ptr, bpp );				\
			}\
			const uint16* clipIn = dst_clr + src_rect.x;		\
			for (sint32 yy=0; yy<(sint32)src_rect.h; ++yy){\
				if(ptr != eptr)\
					ptr = Blt::Span(op, ptr, dst_clr + imgOffset, clipIn, clipIn + clipOffset, bpp, flipped );			\
				else\
					break;\
				dst_clr	+= dstStride;\
				clipIn	+= dstStride;\
				if(efx == EfxShadowIso) {\
					dst_clr++;\
					if ( ptr != eptr ) \
						ptr = Blt::SpanSkip(ptr, bpp);\
					else \
						break;\
				} else if(efx == EfxShadowBattle) {\
					dst_clr += 2;\
					if ( ptr != eptr ) \
						ptr = Blt::SpanSkip(ptr, bpp);\
					else \
						break;\
				}\
			}	\
		}\
	} else {\
		const uint8* src = pixels + (src_rect.y*size.w + src_rect.x) * bpp;		\
		for (uint32 yy=0; yy != dst_rect.h; ++yy) {\
			Blt::BlitRawLine(op, src, dst_clr, dst_rect.w, bpp);\
			src += size.w * bpp;\
			dst_clr += dstStride;\
		}\
	}\
}

BLITWRK_BODY(CopyOp)
BLITWRK_BODY(GlowBlendOp)
BLITWRK_BODY(GlowAlphaBlendOp)
BLITWRK_BODY(RedGlowBlendOp)
BLITWRK_BODY(RedGlowAlphaBlendOp)
BLITWRK_BODY(VioletGlowAlphaBlendOp)
BLITWRK_BODY(Shadow25Op)
BLITWRK_BODY(Shadow50Op)
BLITWRK_BODY(CopyShadow50Op)
BLITWRK_BODY(RealAlphaBlendShadow50Op)
BLITWRK_BODY(RealAlphaBlendOp)
BLITWRK_BODY(RealConstAlphaBlendOp)
BLITWRK_BODY(ConstAlphaBlendOp)

#define DO_BLIT(Op) BlitWorker##Op(pixels, dst_clr, size, dstStride, src_rect, src, dst_rect, alpha, bpp, (sprPtr->type_ & SPFT_SPANNED), (sprPtr->type_ & SPFT_ALPHA), flp, efx)
//#define DO_BLIT(Op)

void iGfxManager::BlitUniversal( SpriteId sid, iDib& to, iPoint pos, const iRect& srcRect /*= cInvalidRect*/, Effects efx /*= EfxNone*/, bool bTile /*= false*/, SpriteId mid /*= -1*/, uint8 alpha /*= 255*/, FlipType flp /*= FlipNone*/ ) const
{
	//return;
	if(mid != -1)
	{
		BlitMasked(sid, mid, to, pos);
		return;
	}


	uint32 catId = sid >> 16;
	uint32 sprId = sid & 0xffff;

	check( catId < MaxSpriteBanks );
	check( sprId < bank_[catId].props.GetSize() );

	const Sprite* sprPtr= bank_[catId].GetSprite( sprId );
	const uint8* pixels= bank_[catId].Data( sprPtr->offset_ );

	iRect src_rect;
	if(srcRect == cInvalidRect)
		src_rect = iRect( 0, 0, sprPtr->sizeW_, sprPtr->sizeH_ );
	else
		src_rect = srcRect;


	iPoint origin( (flp == FlipVert ? (-1) : (1)) * sprPtr->originX_, sprPtr->originY_ );
	iSize  size( sprPtr->sizeW_, sprPtr->sizeH_ );

	iRect src = iSize(sprPtr->sizeW_, sprPtr->sizeH_);
	iRect dst_rect;
	uint16* dst_clr;
	size_t dstStride;

#if 1
	if(efx == EfxShadowBattle) {
		// sprite rect -> sprite shadow rect
		iPoint opos = pos + origin;
		iSize sprSz( sprPtr->sizeW_, sprPtr->sizeH_ );

		uint32 hhgt = (sprPtr->sizeH_ + 1) / 2;
		check( hhgt > 1 );
		opos.x -= hhgt * 3 / 2 - 3;
		if(flp == FlipVert)
			opos.x -= hhgt ;
		opos.y += hhgt / 2 + 5;
		sprSz.w += hhgt - 1;
		sprSz.h = hhgt;

		src = sprSz;

		if ( (opos.x + (sint32)sprSz.w) <= 0 || (opos.y + (sint32)sprSz.h) <= 0) return;
		iSize siz = iSize(to.GetWidth() - pos.x, to.GetHeight() - pos.y);
		dst_rect = iRect(opos,siz);

		src_rect = iRect(sprSz);

		size = sprSz;
		// check bounds & clip
		iClipper::iClipRectRect(dst_rect,to.GetSize(),src_rect,size);


		// calc dest ptr
		dst_clr = to.GetPtr(dst_rect);
		dstStride = to.GetWidth();
	} else if(efx == EfxShadowIso) {
		iPoint opos = pos + origin;
		iSize sprSz( sprPtr->sizeW_, sprPtr->sizeH_ );
		uint32 hhgt = (sprPtr->sizeH_ + 1) / 2;
		check( hhgt > 1 );
		opos.x -= hhgt ;
		opos.y += sprSz.h - hhgt - 1;
		sprSz.w += hhgt - 1;
		sprSz.h = hhgt;

		if ( (opos.x + (sint32)sprSz.w) <= 0 || (opos.y + (sint32)sprSz.h) <= 0) return;
		iSize siz = iSize(to.GetWidth() - opos.x,to.GetHeight() - opos.y);

		iRect dst_rect(opos,siz);
		dst_rect = iRect(opos,siz);

		// check bounds & clip
		iRect src_rect_old(src_rect);
		iRect dst_rect_old(dst_rect);
		src_rect.w = sprSz.w;
		src_rect.h = sprSz.h;
		//size.w =10000;
		//size.h = 10000;
		if (!iClipper::iClipRectRect(dst_rect,to.GetSize(),src_rect, sprSz)) return;
		// calc dest ptr
		/*src_rect.w = src_rect_old.w;
		src_rect.h -= (src_rect.y - src_rect_old.y) * 2;
		src_rect.y = src_rect_old.y + (src_rect.y - src_rect_old.y) * 2;
		 */
		//dst_rect = dst_rect_old;
		if(src_rect.y > src_rect_old.y)
			dst_rect.x += (src_rect.y - src_rect_old.y);
		dst_clr = to.GetPtr(dst_rect);
		dstStride = to.GetWidth();
	} else if(efx == EfxShadow2D) {
		pos += origin + iPoint( -1, 2 );	// shadow displacement
		// check bounds & clip
		dst_rect = iRect(pos, size);
		if (!iClipper::iClipRectRect(dst_rect,to.GetSize(),src_rect,size)) return;

		// calc dest ptr
		dst_clr = to.GetPtr(dst_rect);
		dstStride = to.GetWidth();
	} else
#endif
	{
		pos += origin;
		dst_rect = iRect(pos,size);
		// check bounds & clip
		if (!iClipper::iClipRectRect(dst_rect,to.GetSize(),src_rect,size)) return;

		// calc dest ptr
		dst_clr = to.GetPtr(dst_rect);
		dstStride = to.GetWidth();
	}

	// blit!
	uint8 bpp = sprPtr->type_ & SPFT_ALPHA ? 3 : 2;	 // bytes per pixel in the source DIB


	//DO_BLIT(CopyOp);
	//return;

	if(efx == EfxShadowBattle || efx == EfxShadow2D || efx == EfxShadowIso) {
		// !!
		DO_BLIT(Shadow50Op);
	} else if(efx == EfxGlow) {
		if(sprPtr->type_ & SPFT_ALPHA)
			DO_BLIT(GlowAlphaBlendOp);
		else
			DO_BLIT(GlowBlendOp);
	} else if(efx == EfxRedGlow) {
		if(sprPtr->type_ & SPFT_ALPHA)
			DO_BLIT(RedGlowAlphaBlendOp);
		else
			DO_BLIT(RedGlowBlendOp);
	}
	else if(efx == EfxVioletGlow) {
		if(sprPtr->type_ & SPFT_ALPHA)
			DO_BLIT(VioletGlowAlphaBlendOp);
		else
			check(0);

	} else if(efx == EfxTransparent) {
		DO_BLIT(Shadow25Op);
	}
	else if(efx == EfxDarken) {
		if(sprPtr->type_ & SPFT_ALPHA)
			DO_BLIT(RealAlphaBlendShadow50Op);
		else
			DO_BLIT(CopyShadow50Op);
	} else if(sprPtr->type_ & SPFT_ALPHA) {
		if(alpha == 255)
			DO_BLIT(RealAlphaBlendOp);
		else
			DO_BLIT(RealConstAlphaBlendOp);
	} else {
		if(alpha == 255)
			DO_BLIT(CopyOp);
		else
			DO_BLIT(ConstAlphaBlendOp);
	}

}

iRect iGfxManager::ObjRect( SpriteId sid ) const
{
	uint32 catId = sid >> 16;
	uint32 sprId = sid & 0xffff;

	check( catId < MaxSpriteBanks );
	check( sprId < bank_[catId].props.GetSize() );

	const Sprite* sprPtr= bank_[catId].GetSprite( sprId );
	const uint8* pixels= bank_[catId].Data( sprPtr->offset_ );

	//check(sprPtr->type_ & SPFT_ALPHA);
	uint8 bpp = sprPtr->type_ & SPFT_ALPHA ? 3 : 2;	 // bytes per pixel in the source DIB

	sint32 maxleft = sprPtr->sizeW_;
	sint32 maxright = 0;
	sint32 maxtop = 0;
	sint32 maxbot = 0;
	bool prevlineobj = false;

	if (sprPtr->type_ & SPFT_SPANNED) {
		const uint8* eptr= pixels + (*((uint32*)pixels)) + 4;	// first 4 bytes are an offset in bytes
		const uint8* ptr = pixels + 4;

		for (sint32 yy=0; yy<(sint32)sprPtr->sizeH_; ++yy){
			if(ptr != eptr) {
				sint32 left, right;
				bool empty;
				ptr = Blitter<CopyOp>::GetBounds(ptr, bpp, left, right, empty);
				if(!empty) {
					maxleft = iMIN<>(left, maxleft);
					maxright = iMAX<>(right, maxright);
				}
				if(!empty && maxtop == 0)
					maxtop = yy;
				else if(empty && prevlineobj) {
					maxbot = yy;
					prevlineobj = false;
				}

				if(!empty)
					prevlineobj = true;
			}
			else
				break;
		}
		if(prevlineobj)
			maxbot = sprPtr->sizeH_;
	} else {
		// Raw sprite not supported right now
		check(0);
	}
	return iRect(maxleft, maxtop, maxright-maxleft, maxbot-maxtop);
}

bool iGfxManager::IsInObj( SpriteId sid, const iPoint& pt ) const
{
	uint32 catId = sid >> 16;
	uint32 sprId = sid & 0xffff;

	check( catId < MaxSpriteBanks );
	check( sprId < bank_[catId].props.GetSize() );

	const Sprite* sprPtr= bank_[catId].GetSprite( sprId );
	const uint8* pixels= bank_[catId].Data( sprPtr->offset_ );

	//check(sprPtr->type_ & SPFT_ALPHA);
	uint8 bpp = sprPtr->type_ & SPFT_ALPHA ? 3 : 2;	 // bytes per pixel in the source DIB

	sint32 maxleft = sprPtr->sizeW_;
	sint32 maxright = 0;
	sint32 maxtop = 0;
	sint32 maxbot = 0;
	bool prevlineobj = false;

	if (sprPtr->type_ & SPFT_SPANNED) {
		const uint8* eptr= pixels + (*((uint32*)pixels)) + 4;	// first 4 bytes are an offset in bytes
		const uint8* ptr = pixels + 4;

		for (sint32 yy=0; yy<(sint32)sprPtr->sizeH_; ++yy){
			if(ptr != eptr) {
				sint32 left, right;
				bool empty;
				ptr = Blitter<CopyOp>::GetBounds(ptr, bpp, left, right, empty);

				// skip until our clicked line
				if(yy != pt.y)
					continue;

				if(empty)
					return false; // this line has no pixels

				return pt.x >= left && pt.x <= right;
			}
			else
				break;
		}
		if(prevlineobj)
			maxbot = sprPtr->sizeH_;
	} else {
		// Raw sprite not supported right now
		check(0);
	}
	return false;
}
