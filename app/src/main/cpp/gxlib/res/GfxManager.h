#ifndef GXLIB_RES_GFXMANAGER_H_
#define GXLIB_RES_GFXMANAGER_H_

#include "FileAccessor/FileAccessor.h"

//////////////////////////////////////////////////////////////////////////
typedef uint32 SpriteId;
//////////////////////////////////////////////////////////////////////////

/* singleton */
class iGfxManager
{
public:
	enum { MaxSpriteBanks = 16 };
	enum { MaxDynItems = 32 };

	enum Effects {
		EfxNone			= 0,
		EfxTransparent,
		EfxShadowIso,
		EfxShadowBattle,
		EfxShadow2D,
		EfxGlow,
		EfxVioletGlow,
		EfxRedGlow,
		EfxDarken
	};

	enum FlipType {
		FlipNone		= 0,
		FlipVert
	};

	enum LoadMode {
		LM_Unknown,
		LM_Memory,
		LM_MappedFile
	};

	typedef uint32	BankId;
	typedef uint32  SpriteId;


	// Helper to make SpriteUID from BankId + Sprite number
	static SpriteId MakeSpriteId( BankId cat, uint32 sn );
	static SpriteId GetFlippedSpriteId( SpriteId sid );
	static inline bool IsFlippedSprite( SpriteId sid )
	{ return (sid & (1UL<<31)) == 1; }

    void inline SetGamma( uint32 glevel )
    { gammaLevel = glevel; }

    bool Load( BankId cat, const fileaccessor::RelativeFilePath& path, LoadMode lmode );
	void Unload( BankId cat );
	void Reload();

	size_t		NumSprites( BankId cat ) const;
	SpriteId	FirstId( BankId cat ) const;
	SpriteId	EndId( BankId cat ) const;

	iSize Dimension( SpriteId sid ) const;

	// gets the rectangle of non-transparent object inside the sprite
	iRect ObjRect( SpriteId sid ) const;
	bool IsInObj( SpriteId sid, const iPoint& pt ) const;
	iPoint Anchor( SpriteId sid ) const;
	uint8 Shadow( SpriteId sid ) const;

	/// Includes all functionality of all other Blit* methods and provides the single interface for sprite blitting
	void BlitUniversal( SpriteId sid, iDib& to, iPoint pos, const iRect& src = cInvalidRect, Effects efx = EfxNone, bool bTile = false, SpriteId mid = -1, uint8 alpha = 255, FlipType flp = FlipNone) const;

	void Blit( SpriteId sid, iDib& to, iPoint pos ) const;
	void BlitFlipped( SpriteId sid, iDib& to, iPoint pos ) const;
	void Blit( SpriteId sid, iDib& to, const iRect& src, iPoint pos ) const;

	void BlitAlpha( SpriteId sid, iDib& to, iPoint pos, uint8 a ) const; // alpha - 0-63
	void BlitAlphaFlipped( SpriteId sid, iDib& to, iPoint pos, uint8 a ) const; // alpha - 0-63
	void BlitAlpha( SpriteId sid, iDib& to, const iRect& src, iPoint pos, uint8 a ) const; // alpha - 0-63
	void BlitAlphaFlipped( SpriteId sid, iDib& to, const iRect& src, iPoint pos, uint8 a ) const; // alpha - 0-63
	void BlitEffect( SpriteId sid, iDib& to, iPoint pos, Effects efx ) const;

	void BlitTile( SpriteId sid, iDib& to, const iRect& dst, uint8 alpha=255 ) const;
	void BlitTile( SpriteId sid, iDib& to, const iRect& src, const iRect& dst, uint8 alpha=255 ) const;
	void BlitTile3( SpriteId sid, iDib& to, const iRect& src, const iRect& dst ) const;

	void BlitMasked( SpriteId sid, SpriteId mid, iDib& to, iPoint pos ) const;

	void MakeDib( SpriteId sid, iDib& dst );

	size_t MemoryOccuped() const;


	struct Sprite
	{
		uint16			type_;
		uint32			offset_;	// offset from the start of data block ( bytes )
		uint16			sizeW_;
		uint16			sizeH_;
		sint16			originX_;
		sint16			originY_;
	};

	struct SpriteBank {
		size_t			dataSize;	// for statistics
		iBuff<Sprite>	props;

		const Sprite*	GetSprite( uint32 num ) const;
		const uint8*	Data( uint32 offset ) const;
		size_t			MemTotal() const;

		SpriteBank();
		~SpriteBank();

		//
		uint8*			data;
#ifdef OS_WIN32
		HANDLE			hFile;
		HANDLE			hMapping;
#else
		iFilePtr*		pFile;
		iDynamicBuffer  buf;
#endif
		void*			pMap;
		LoadMode		loadMode;
		fileaccessor::RelativeFilePath		fileName;

		bool Load( const fileaccessor::RelativeFilePath& path, LoadMode lmode, uint32 gammaLevel );
		void Unload();
		bool Reload( uint32 gammaLevel );
	};

	struct DynItem {
		DynItem() : data(NULL) {}
		~DynItem() { if (data) { free(data); data=NULL; } }

		SpriteId	sid;
		Sprite		sprite;
		uint8*		data;
		size_t		dataSize;
	};

	uint32		gammaLevel;
	DynItem*	dynItems[ MaxDynItems ];
	uint32		dynPos;
	SpriteBank	bank_[ MaxSpriteBanks ];

	static iGfxManager& Instance()
		{
			static iGfxManager theSingleInstance;
			return theSingleInstance;
		}
private:
	/* the class is singleton */
	iGfxManager()
	: dynPos(0)
	, gammaLevel(0)
	{ memset(dynItems,0, sizeof(dynItems)); }

	/* the class is singleton */
	~iGfxManager()
	{
		for (sint32 xx=0; xx<MaxDynItems; ++xx) {
			if (dynItems[xx]) {
				delete dynItems[xx];
				dynItems[xx] = NULL;
			}
		}
	}

	// Non copyable
	iGfxManager& operator=( const iGfxManager& );
	iGfxManager( const iGfxManager& );

	void FlipSprite(SpriteId sid);

	void BlitNormal( const Sprite* spr, const uint16* data, iDib& srf, const iRect& src, iPoint dst ) const;
	void BlitRealAlpha( const Sprite* spr, const uint16* data, iDib& srf, const iRect& src, iPoint dst ) const;
	void BlitRealConstAlpha( const Sprite* spr, const uint16* data, iDib& srf, const iRect& src, iPoint dst, uint8 alpha) const;
	void BlitNormalFlipped( const Sprite* spr, const uint16* data, iDib& srf, const iRect& src, iPoint dst ) const;
	void BlitRealAlphaFlipped( const Sprite* spr, const uint16* data, iDib& srf, const iRect& src, iPoint dst ) const;
	void BlitRealConstAlphaFlipped( const Sprite* spr, const uint16* data, iDib& srf, const iRect& src, iPoint dst, uint8 alpha) const;

	void BlitAlpha( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos, uint8 a) const;
	void BlitAlphaFlipped( const iGfxManager::Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos, uint8 a) const;
	void BlitTransparent( const Sprite* spr, const uint16* data, iDib& srf, iPoint dst) const;
	void BlitShadowIso( const Sprite* spr, const uint16* data, iDib& srf, iPoint dst ) const;
	void BlitShadowIsoBattle( const Sprite* spr, const uint16* data, iDib& srf, iPoint dst ) const;
	void BlitShadowIsoBattleFlipped( const Sprite* spr, const uint16* data, iDib& srf, iPoint dst ) const;
	void BlitShadow2D( const Sprite* spr, const uint16* data, iDib& srf, iPoint dst ) const;
	void BlitWired( const Sprite* spr, const uint16* data, iDib& srf, iPoint dst ) const;
	void BlitWired( const Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos ) const;
	void BlitWiredFlipped( const Sprite* spr, const uint16* data, iDib& srf, iPoint dst ) const;
	void BlitWiredFlipped( const Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos ) const;
	void BlitGlow( const Sprite* spr, const uint16* data, iDib& srf, iPoint dst ) const;
	void BlitGlow( const Sprite* sprPtr, const uint16* pixels, iDib& srf, const iRect& src, iPoint pos ) const;

#define BLITWRK(Op) void BlitWorker##Op(const uint8* pixels, uint16* dst_clr, iSize size, const size_t dstStride, iRect& src_rect, iRect& src, iRect& dst_rect, uint8 alpha, uint8 bpp, bool spanned, bool realAlpha, FlipType flp, Effects efx) const
	BLITWRK(CopyOp);
	BLITWRK(GlowBlendOp);
	BLITWRK(GlowAlphaBlendOp);
	BLITWRK(RedGlowBlendOp);
	BLITWRK(RedGlowAlphaBlendOp);
	BLITWRK(VioletGlowAlphaBlendOp);
	BLITWRK(Shadow25Op);
	BLITWRK(Shadow50Op);
	BLITWRK(CopyShadow50Op);
	BLITWRK(RealAlphaBlendShadow50Op);
	BLITWRK(RealAlphaBlendOp);
	BLITWRK(RealConstAlphaBlendOp);
	BLITWRK(ConstAlphaBlendOp);

	//	void BlitWorkerCopy(const uint8* pixels, uint16* dst_clr, iSize size, const size_t dstStride, iRect& src_rect, iRect& src, iRect& dst_rect, uint8 alpha, uint8 bpp, bool spanned, bool realAlpha, FlipType flp, Effects efx) const;
//	void BlitWorkerGlowAlphaBlend(const uint8* pixels, uint16* dst_clr, iSize size, const size_t dstStride, iRect& src_rect, iRect& src, iRect& dst_rect, uint8 alpha, uint8 bpp, bool spanned, bool realAlpha, FlipType flp, Effects efx) const;

};



#endif //GXLIB_RES_GFXMANAGER_H_
