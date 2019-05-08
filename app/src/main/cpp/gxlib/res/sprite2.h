    #ifndef GXLIB_RES_SPRITE2_H_
#define GXLIB_RES_SPRITE2_H_
#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#endif
/// Pixel type
typedef uint16 Pixel;

/// Blitter ops
struct SetOp
{
	always_inline void operator()( Pixel* dst, const uint8* src ) const
	{ *dst = 0xffff; }
};

struct CopyOp
{
	CopyOp() {}
	CopyOp(uint8 a) {}
	always_inline void operator()( Pixel* dst, const uint8* src ) const
	{
		//return;
		*dst = *((uint16*)src); 
	}
};

struct CopyShadow50Op
{
	CopyShadow50Op() {}
	CopyShadow50Op(uint8 a) {}
	always_inline void operator()( Pixel* dst, const uint8* src ) const
	{ *dst = Darken50(Darken50(*((uint16*)src))); }
};


struct ConstAlphaBlendOp
{
	uint8 alpha;
	ConstAlphaBlendOp(uint8 a) : alpha(a) {}
	always_inline void operator()( Pixel* dst, const uint8* src) const
	{ 		
		//return;
		const uint32 a  = *((const uint16*)src);
		const uint32 b  = *((uint16*)dst);
		uint32 sb = a & 0x1f;
		uint32 sg = (a >> 5) & 0x3f;
		uint32 sr = (a >> 11) & 0x1f;
		uint32 db = b & 0x1f;
		uint32 dg = (b >> 5) & 0x3f;
		uint32 dr = (b >> 11) & 0x1f;

		*dst = (Pixel)(((alpha * (sb-db)) >> 8) + db
			| (((alpha * (sg-dg)) >> 8) + dg) << 5 
			| (((alpha * (sr-dr)) >> 8) + dr) << 11);
	}
};

struct Shadow50Op
{
	Shadow50Op(uint8 a) {}
	always_inline void operator()( Pixel* dst, const uint8* src) const
	{ 
		//return;
		*dst = (Pixel)Darken50(*dst); 
	}
};

struct RealAlphaBlendOp
{	
	RealAlphaBlendOp(uint8 a) {}
	always_inline void operator()( Pixel* dst, const uint8* src) const
	{ 
		//return;
		//*dst = *((uint16*)src);
		//return;
		
		
		const uint32 a  = *((const uint16*)src);
		const uint32 b  = *((const uint16*)dst);
		uint32 sb = a & 0x1f;
		uint32 sg = (a >> 5) & 0x3f;
		uint32 sr = (a >> 11) & 0x1f;
		uint32 db = b & 0x1f;
		uint32 dg = (b >> 5) & 0x3f;
		uint32 dr = (b >> 11) & 0x1f;

		*dst = (Pixel)(((*(src+2) * (sb-db)) >> 8) + db
			| (((*(src+2) * (sg-dg)) >> 8) + dg) << 5 
			| (((*(src+2) * (sr-dr)) >> 8) + dr) << 11);
		 
	}
};

struct RealAlphaBlendShadow50Op
{	
	RealAlphaBlendShadow50Op(uint8 a) {}
	always_inline void operator()( Pixel* dst, const uint8* src) const
	{ 
		const uint32 a  = Darken50(Darken50(*((const uint16*)src)));
		const uint32 b  = *((const uint16*)dst);
		uint32 sb = a & 0x1f;
		uint32 sg = (a >> 5) & 0x3f;
		uint32 sr = (a >> 11) & 0x1f;
		uint32 db = b & 0x1f;
		uint32 dg = (b >> 5) & 0x3f;
		uint32 dr = (b >> 11) & 0x1f;

		//if(*(src+2) == 255) {
		*dst = (Pixel)(((*(src+2) * (sb-db)) >> 8) + db
			| (((*(src+2) * (sg-dg)) >> 8) + dg) << 5 
			| (((*(src+2) * (sr-dr)) >> 8) + dr) << 11);			
		//}
	}
};


struct GlowBlendOp
{	
	GlowBlendOp() {}
	GlowBlendOp(uint8 a) {}
	always_inline void operator()( Pixel* dst, const uint8* src) const
	{ 		
		const uint32 b  = *dst;				
		uint8 alpha = 40;
		uint32 sb = 32;
		uint32 sg = 64;
		uint32 sr = 32;
		uint32 db = b & 0x1f;
		uint32 dg = (b >> 5) & 0x3f;
		uint32 dr = (b >> 11) & 0x1f;

		*dst = (Pixel)(((alpha * (sb-db)) >> 6) + db
			| (((alpha * (sg-dg)) >> 6) + dg) << 5 
			| (((alpha * (sr-dr)) >> 6) + dr) << 11);
			
	}
};


struct GlowAlphaBlendOp
{	
	GlowAlphaBlendOp() {}
	GlowAlphaBlendOp(uint8 a) {}
	always_inline void operator()( Pixel* dst, const uint8* src) const
	{ 
		const uint32 b  = *((const uint16*)dst);					
		uint32 sr = 32;		
		uint32 sg = 64;
		uint32 sb = 32;		
		
		uint32 db = b & 0x1f;
		uint32 dg = (b >> 5) & 0x3f;
		uint32 dr = (b >> 11) & 0x1f;

		uint8 alpha = *(src + 2) >> 3;

		*dst = (Pixel)(((alpha * (sb - db)) >> 6) + db
			| (((alpha * (sg - dg)) >> 6) + dg) << 5 
			| (((alpha * (sr - dr)) >> 6) + dr) << 11);			
	}
};


struct VioletGlowAlphaBlendOp
{	
	VioletGlowAlphaBlendOp() {}
	VioletGlowAlphaBlendOp(uint8 a) {}
	always_inline void operator()( Pixel* dst, const uint8* src) const
	{ 
		const uint32 b  = *((const uint16*)dst);					
		
		uint32 c = RGB16(98, 91, 138);

		uint32 sb = c & 0x1f;
		uint32 sg = (c >> 5) & 0x3f;
		uint32 sr = (c >> 11) & 0x1f;
		
		uint32 db = b & 0x1f;
		uint32 dg = (b >> 5) & 0x3f;
		uint32 dr = (b >> 11) & 0x1f;

		uint8 alpha = *(src + 2) >> 3;

		*dst = (Pixel)(((alpha * (sb - db)) >> 6) + db
			| (((alpha * (sg - dg)) >> 6) + dg) << 5 
			| (((alpha * (sr - dr)) >> 6) + dr) << 11);			
	}
};


struct RedGlowBlendOp
{	
	RedGlowBlendOp() {}
	RedGlowBlendOp(uint8 a) {}
	always_inline void operator()( Pixel* dst, const uint8* src) const
	{ 		
		const uint32 b  = *dst;				
		uint8 alpha = 40;
		uint32 sb = 0;
		uint32 sg = 0;
		uint32 sr = 32;
		uint32 db = b & 0x1f;
		uint32 dg = (b >> 5) & 0x3f;
		uint32 dr = (b >> 11) & 0x1f;

		*dst = (Pixel)(((alpha * (sb-db)) >> 6) + db
			| (((alpha * (sg-dg)) >> 6) + dg) << 5 
			| (((alpha * (sr-dr)) >> 6) + dr) << 11);

	}
};


struct RedGlowAlphaBlendOp
{	
	RedGlowAlphaBlendOp() {}
	RedGlowAlphaBlendOp(uint8 a) {}
	always_inline void operator()( Pixel* dst, const uint8* src) const
	{ 
		const uint32 b  = *((const uint16*)dst);					
		uint32 sb = 0;
		uint32 sg = 0;
		uint32 sr = 32;		

		uint32 db = b & 0x1f;
		uint32 dg = (b >> 5) & 0x3f;
		uint32 dr = (b >> 11) & 0x1f;

		uint8 alpha = *(src + 2) >> 3;

		*dst = (Pixel)(((alpha * (sb - db)) >> 6) + db
			| (((alpha * (sg - dg)) >> 6) + dg) << 5 
			| (((alpha * (sr - dr)) >> 6) + dr) << 11);			
	}
};



struct RealConstAlphaBlendOp
{	
	uint8 alpha;
	RealConstAlphaBlendOp(uint8 a) : alpha(a) {}

	always_inline void operator()( Pixel* dst, const uint8* src) const
	{ 
		//return;
		const uint32 a  = *((uint16*)src);
		const uint32 b  = *dst;				
		/*uint32 sb = (a & 0xf) << 1;
		uint32 sg = ((a >> 4) & 0xf) << 2;
		uint32 sr = ((a >> 8) & 0xf) << 1;
		*/
		uint32 sb = a & 0x1f;
		uint32 sg = (a >> 5) & 0x3f;
		uint32 sr = (a >> 11) & 0x1f;

		uint8 al =  *(src+2) * alpha >> 8;
		//uint8 al = (((a >> 12) & 0xf) << 2) * alpha >> 8;

		uint32 db = b & 0x1f;
		uint32 dg = (b >> 5) & 0x3f;
		uint32 dr = (b >> 11) & 0x1f;

		*dst = (Pixel)(((al * (sb-db)) >> 8) + db
			| (((al * (sg-dg)) >> 8) + dg) << 5 
			| (((al * (sr-dr)) >> 8	) + dr) << 11);		
	}
};


struct Shadow25Op
{
	Shadow25Op(uint8 a) {}
	always_inline void operator()( Pixel* dst, const uint8* ) const
	{ *dst = (Pixel)Darken25(*dst); }
};

struct DotedOp
{
	always_inline void operator()( Pixel* dst, const uint8* ) const
	{ if ( !((size_t)dst & 3) ) *dst = (uint16)Darken50(*dst);}
};

static Pixel tintGradient[32] = {
	0x4101,	0x4101,	0x4921,	0x4942,	0x5142,	0x5162,	0x5983,	0x61a3,
	0x61e3,	0x6a04,	0x7224,	0x7a65,	0x8285,	0x8ac6,	0x8ae6,	0x9327,
	0xa368,	0xa389,	0xabc9,	0xb40a,	0xbc4b,	0xc48c,	0xcccd,	0xd4ed,
	0xdd2e,	0xe54f,	0xed90,	0xedd0,	0xf5f1,	0xf611,	0xf632,	0xf652
};

struct TintShadow
{
	always_inline Pixel Blend( Pixel a, Pixel b ) const
	{
		return ( (a & 0xf7de) >> 1 ) + ((b & 0xf7de) >> 1);
	}
	always_inline void operator()( Pixel* dst, const Pixel* ) const
	{ 
		//static uint8 rpt = 31;
		Pixel chnl = (*dst >> 6) & 0x1f;
		//*dst = Darken25((chnl) | (chnl << 6) | (chnl << 11));
		*dst = Blend(tintGradient[chnl], 0x39e7 );
	} 
};


/// Blitter loop
/// With given: source ptr, dest ptr, counter, operation
//#define DUFFS_BLT
#ifndef DUFFS_BLT
#	define BLTLOOP( SRC, DST, CNT, OP, BPP ) \
	for( ; CNT != 0; CNT--, SRC+=BPP, ++DST ) OP( DST, SRC ); \

//#define BLTLOOP(SRC, DST, CNT, OP, BPP) \
//	memcpy(DST, SRC, CNT)

/* */
#	define BLTLOOPREVERSE( SRC, DST, CNT, OP, BPP ) \
	for( ; CNT != 0; --CNT, SRC+=BPP, --DST ) OP( DST, SRC ); \
	/* */
#define BLTLOOPBACK( SRC, DST, CNT, OP ) \
	for( ; CNT != 0; --CNT ) OP ( --DST, --SRC ); \
	/* */
#else
#	define BLTLOOP( SRC, DST, CNT, OP, BPP ) \
	size_t tcnt_ = (CNT + 7) / 8;		\
	switch( CNT % 8 ) {					\
	case 0: do{	OP( DST++, SRC+=BPP );	\
	case 7:		OP( DST++, SRC+=BPP );	\
	case 6:		OP( DST++, SRC+=BPP );	\
	case 5:		OP( DST++, SRC+=BPP );	\
	case 4:		OP( DST++, SRC+=BPP );	\
	case 3:		OP( DST++, SRC+=BPP );	\
	case 2:		OP( DST++, SRC+=BPP );	\
	case 1:		OP( DST++, SRC+=BPP );	\
} while ( --tcnt_ != 0 );	\
};									\

#	define BLTLOOPREVERSE( SRC, DST, CNT, OP, BPP ) \
for( ; CNT != 0; --CNT, SRC+=BPP, --DST ) OP( DST, SRC ); \
/* */

	/* */
#	define BLTLOOPBACK( SRC, DST, CNT, OP ) \
	size_t tcnt_ = (CNT + 7) / 8;		\
	check( tcnt_ != 0 );				\
	switch( CNT % 8 ) {					\
	case 0: do{	OP( --DST, --SRC );	\
	case 7:		OP( --DST, --SRC );	\
	case 6:		OP( --DST, --SRC );	\
	case 5:		OP( --DST, --SRC );	\
	case 4:		OP( --DST, --SRC );	\
	case 3:		OP( --DST, --SRC );	\
	case 2:		OP( --DST, --SRC );	\
	case 1:		OP( --DST, --SRC );	\
} while ( --tcnt_ != 0 );	\
};									\
	/* */

#endif
struct DstSrc
{
	Pixel*			dst;	// ptr to the surface
	const uint8*	src;	// ptr to the spanned sprite (1 byte)
	always_inline DstSrc( Pixel* d, const uint8* s ) : dst( d ), src( s ) {}
	always_inline void inc( size_t val, uint8 bpp ) { src += val*bpp; dst += val; }
	always_inline void revinc( size_t val, uint8 bpp ) { src += val*bpp; dst -= val; }
};


struct DstSrcMask
{
	Pixel*			dst;	// ptr to the surface
	const uint8*	src;	// ptr to the raw texture image 
	const uint8*	mask;	// ptr to the spanned mask sprite
	always_inline DstSrcMask( Pixel* d, const uint8* s, const uint8* m ) : dst( d ), src( s ), mask( m ) {}
	always_inline void inc( size_t val, uint8 bpp ) { src += val*bpp; dst += val; mask += val*bpp; }
	always_inline void incnomask( size_t val, uint8 bpp ) { src += val*bpp; dst += val; }
};

struct ClipSpan
{
	const Pixel*	in;
	const Pixel*	out;
	always_inline ClipSpan( const Pixel* i, const Pixel* o ) : in( i ), out( o ) {}
};

//////////////////////////////////////////////////////////////////////////

template< typename OP >
struct Blitter {

	typedef OP OpType;

	//// Non clipped span blit
	static always_inline
		void SegmentFast( OP op, DstSrc ptr, size_t count, uint8 bpp, bool flipped )
	{
		if(!flipped) {
			BLTLOOP( ptr.src, ptr.dst, count, op, bpp );
		}
		else {
			BLTLOOPREVERSE( ptr.src, ptr.dst, count, op, bpp );
		}
	}
		
	//// Non clipped masked span blit
	static always_inline
		void SegmentFast( OP op, DstSrcMask ptr, size_t count, uint8 bpp )
	{
		BLTLOOP( ptr.src, ptr.dst, count, op, bpp );
	}
	
	//// Clipped span blit 
	static always_inline
		void Segment( OP op, DstSrc ptr, size_t count, ClipSpan clip, uint8 bpp, bool flipped )
	{
		if(!flipped) {
			// validate clip params
			check( clip.in < clip.out );
			sint32 clipBoundDiff = clip.in - ptr.dst;
			size_t leftClipSkips = min( (size_t)max( clipBoundDiff, 0L ), count );
			check( leftClipSkips <= count );
					
			ptr.inc(leftClipSkips, bpp);
			count -= leftClipSkips;
			if ( 0 != count && ptr.dst < clip.out ) {
				check( ptr.dst <= clip.out );
				size_t copyCount = min( (size_t)(clip.out - ptr.dst), count );
				check( copyCount <= count );
				count -= copyCount;
				BLTLOOP( ptr.src, ptr.dst, copyCount, op, bpp );
			}	
		} else {
			// validate clip params
			check( clip.in < clip.out );
			sint32 clipBoundDiff = ptr.dst - clip.out;
			size_t rightClipSkips = min( (size_t)max( clipBoundDiff, 0L ), count );
			check( rightClipSkips <= count );
	
			ptr.revinc(rightClipSkips, bpp);			
			count -= rightClipSkips;
		
			if ( 0 != count && ptr.dst > clip.in ) {			
				size_t copyCount = min( (size_t)(ptr.dst - clip.in), count );
				check( copyCount <= count );
				count -= copyCount;
				BLTLOOPREVERSE( ptr.src, ptr.dst, copyCount, op, bpp );
			}
		}		
	}	
	

	//// Clipped masked span blit
	static always_inline
		void Segment( OP op, DstSrcMask ptr, size_t count, ClipSpan clip, uint8 bpp )
	{
		// validate clip params
		check( clip.in < clip.out );
		sint32 clipBoundDiff = clip.in - ptr.dst;
		size_t leftClipSkips = min( (size_t)max( clipBoundDiff, 0L ), count );
		check( leftClipSkips <= count );

		ptr.inc(leftClipSkips, bpp);		
		count -= leftClipSkips;

		if ( 0 != count && ptr.dst < clip.out ) {
			check( ptr.dst <= clip.out );
			size_t copyCount = min( (size_t)(clip.out - ptr.dst), count );
			check( copyCount <= count );
			count	-= copyCount;
			ptr.mask += copyCount * bpp;
			BLTLOOP( ptr.src, ptr.dst, copyCount, op, bpp );
		}
	}

	//////////////////////////////////////////////////////////////////////////

	//// Skips one span
	static always_inline 
		const uint8* SpanSkip( const uint8* src, uint8 bpp )
	{
		uint32 code;
		do {
			code = *((const uint32*)src);
			uint32 len	= code & 0xffff;
			src += len * bpp + 4;
		} while ( !(code & 0x80000000) );
		return src;
	}
	
	//// Blits non clipped span line 
	static always_inline 
		const uint8* SpanFast( OP op, const uint8* src, Pixel* dst, uint8 bpp, bool flipped )
	{
		DstSrc ptr( dst, src );
		uint32 code;
		do {
			code = *((const uint32*)ptr.src); 
			ptr.src += 4; // 4 bytes
			if ( code == 0x80000000 ) break; // stop clipping if stop marker (the elder bit) encountered

			uint32 len = code & 0xffff; // last 16 bits are length
			uint32 offset = ((code >> 16) & 0x7fff); // first 15 bits are offset			
			if(!flipped)
				ptr.dst += offset;
			else
				ptr.dst -= offset;
			SegmentFast(op, ptr, len, bpp, flipped );
			if(!flipped)
				ptr.inc(len, bpp);
			else
				ptr.revinc(len, bpp);
		} while ( !(code & 0x80000000) );

		return ptr.src;
	}
	
//	static inline 
//		const Pixel* GetSpanEdges(const Pixel* src, Pixel* dst, uint16 *start, uint16 *end)
//	{
//		DstSrc ptr( dst, src );
//		uint32 code;
//		*start = 0;
//		bool bEmpty = true;
//		do {
//			check(0);
//			code = *ptr.src++;
//			if ( code == 0x8000 ) break;
//
//			uint32 len = code & 0x00ff;
//			uint32 offset = ((code >> 8) & 0x007f);
//			if(offset > 0) bEmpty = false;
//			ptr.dst += offset;
//			if(!*start)
//				*start = offset - 1;
//			//*ptr.dst = RGB16(255, 213, 3);
//			//*(ptr.dst - 1) = RGB16(255, 213, 3);
//			//SegmentFast( op, ptr, len );
//			ptr += len;
//		} while ( !(code & 0x8000) );		
//
//		if(!bEmpty) {			
//			//	*ptr.dst = RGB16(255, 213, 3); 
//			*end = ptr.dst - dst;
//			//*(ptr.dst + 1) = RGB16(255, 213, 3); 
//		} else {
//			*start = 0;
//		}
//
//		return ptr.src;
//	}

	
	//// Blits clipped span line
	static inline 
		const uint8* Span( OP op, const uint8* src, Pixel* dst, const Pixel* clipIn, const Pixel* clipOut, uint8 bpp, bool flipped )
	{
		DstSrc ptr( dst, src );
		const ClipSpan clip( clipIn, clipOut );
		uint32 code;
		do {
			code = *((const uint32*)ptr.src);
			ptr.src += 4;
			if ( code == 0x80000000 ) break;	

			uint32 len = code & 0xffff;
			uint32 offset = ((code >> 16) & 0x7fff);
			if(!flipped)
				ptr.dst += offset;
			else
				ptr.dst -= offset;
			Segment( op, ptr, len, clip, bpp, flipped );
			if(!flipped)
				ptr.inc(len, bpp);
			else
				ptr.revinc(len, bpp);
		} while ( !(code & 0x80000000) );

		return ptr.src;
	}
	
	//// Blits non clipped masked span line 
	//// ( NOTE: mask is a span sprite! ) returns ptr to the mask
	static inline 
		const uint8* MaskedSpanFast( OP op, const uint8* mask, const uint8* src, Pixel* dst, uint8 bpp )
	{
		DstSrcMask ptr( dst, src, mask );
		uint32 code;
		do {			
			code = *((const uint32*)ptr.mask);
			ptr.mask += 4;
			if ( code == 0x80000000 ) break;	

			uint32 len = code & 0xffff;
			uint32 offset = ((code >> 16) & 0x7fff);
			ptr.incnomask(offset, bpp);
			SegmentFast( op, ptr, len, bpp );
			ptr.inc(len, bpp);
		} while ( !(code & 0x80000000) );

		return ptr.mask;
	}

	//// Blits clipped masked span line
	//// ( NOTE: mask is a span sprite! ) returns ptr to the mask
	static inline 
		const uint8* MaskedSpan( OP op, const uint8* mask, const uint8* src, Pixel* dst, const Pixel* clipIn, const Pixel* clipOut, uint8 bpp )
	{
		DstSrcMask ptr( dst, src, mask );
		const ClipSpan clip( clipIn, clipOut );
		uint32 code;
		do {			
			code = *((const uint32*)ptr.mask);
			ptr.mask += 4;
			if ( code == 0x80000000 ) break;	

			uint32 len = code & 0xffff;
			uint32 offset = ((code >> 16) & 0x7fff);
			ptr.incnomask(offset, bpp);	
			Segment( op, ptr, len, clip, bpp );
			ptr.inc(len, bpp);
		} while ( !(code & 0x80000000) );

		return ptr.mask;
	}

	//// Blit a raw line	
	static inline 
		void BlitRawLine( OP op, const uint8* src, Pixel* dst, uint32 cnt, uint8 bpp )
	{
		BLTLOOP(src, dst, cnt, op, bpp);
	}

	/// Gets left and right non-transparent bounds of and object inside a spanned line
	static const uint8* GetBounds( const uint8* ptr, uint8 bpp, sint32& left, sint32& right, bool& empty ) 
	{		
		left = right = 0; // 0 and 0 means line is empty
		bool leftset = false;
		empty = true;
		uint32 code;
		do {
			code = *((const uint32*)ptr);
			ptr += 4;
			if ( code == 0x80000000 ) break;	

			empty = false;
			uint32 len = code & 0xffff;
			uint32 offset = ((code >> 16) & 0x7fff);
			if(!leftset) {
				leftset = true;
				left = offset;
			}			
			ptr += len * bpp;			
			right += offset + len; // right bound is moved to the end of current data block
			
		} while ( !(code & 0x80000000) );

		return ptr;
	}

};

//////////////////////////////////////////////////////////////////////////

#undef BLTLOOP
#undef BLTLOOPBACK

#endif //GXLIB_RES_SPRITE2_H_
