#ifndef RESLIB_SPRITEMGR_H_
#define RESLIB_SPRITEMGR_H_


// we use different types for Exporter and for mapeditor/game
// Exporter doesn't need to load all Dib data at start up
// so we store path to dib file instead of real bitmap data
#ifndef BUILD_EXPORTER
typedef iDib iSpriteDib;
#else

class iSpriteDib // : public iDib
{
public:
    iSpriteDib() : m_hash(0) {};
    ~iSpriteDib() {};
    bool Init(iStringT &path);
    void SetRect(const iRect& rc) { m_rc = rc;};
    uint32 GetHash() { return m_hash; }; 

    bool operator == (const iSpriteDib &s)
    {
        if (m_hash == s.m_hash)
        {
            if (m_fn.Compare(s.m_fn) == 0)
            {
                return true;
            }
        }
        return false;
    }

    bool LoadDib(iDib &dib) const;
    const iStringT & GetPath() const { return m_fn; };
    const iRect & GetRect() const { return m_rc; };

protected:
    iStringT m_fn;
    uint32 m_hash;
    iRect m_rc;
    
};

#endif
/*
 *	
 */
struct iSprite
{
	void Init(const iSpriteDib& _dib, const iPoint& _anchor, bool _spanned, bool _crop, bool _alpha, uint8 _shadow, const iRect& rc=cInvalidRect);

	uint32	id;
	iPoint	anchor;
	bool	spanned;
	bool	crop;
	bool    alpha;
	uint8	shadow;
	iSpriteDib	dib;
};

/*
 *	
 */
struct iConstDef
{
	iConstDef() {}
	iConstDef(const SpriteId& spriteName, const iStringT& constName, uint32 seqCnt) 
	: m_SpriteName(spriteName), m_ConstName(constName), m_SeqCnt(seqCnt) {}

	SpriteId	m_SpriteName;
	iStringT	m_ConstName;
	uint32		m_SeqCnt;
};

typedef iSimpleArray<iConstDef>	iConstDefList;

/*
 *	
 */
class iSpriteMgr
{
public:
	//
	struct KeyVal {
		KeyVal(){}
		iStringT	m_Key;
		iSprite		m_Value;
		inline KeyVal( const iStringT& k) : m_Key( k ){}
	};
private:
	//
	struct KeyValHF {
		uint32 operator() ( const iStringT& str )
		{ return iTables::HashFn( str ); }
		iStringT operator() ( KeyVal& pair ) 
		{ return pair.m_Key; }
	};

	uint8		m_CellFactor;
	iStringT	m_ResPath;
	iSprite		m_InvalidSprite;
	bool		m_Packed;

public:
	// Predefined sprites
	iDib		m_pdggTiles;
	iDib		m_pdggTransWS;
	iDib		m_pdggTransMask;
	iDib		m_pdggFog;
	iDib		m_pdggGrid;
	iDib		m_pdggPathEl;
	iDib		m_pdggHeroes;
	iDib		m_pdggFlags;

	// Predefined interface elements

	iSpriteMgr()
	{}

	void SetPacked() {
		m_Packed = true;
	}

	typedef iHashTable< const iStringT&, KeyVal, KeyValHF > iKVHash;
	typedef iKVHash::Iterator SpriteIt;
	typedef iKVHash::ConstIterator ConstSpriteIt;
	iKVHash			m_SpriteHash;
	iConstDefList	m_ConstDefList;

	inline uint8 GetCellFactor() const
	{ return m_CellFactor;}

	inline const iStringT& GetResPath() const
	{ return m_ResPath; }

	const iSprite* GetSpritePtr(const iStringT& name) const
	{
		iKVHash::ConstIterator it = m_SpriteHash.Find(name);
		if (it==m_SpriteHash.End()) return &m_InvalidSprite;
		const iSprite* result = &(*it).m_Value;
		if (result) return result;
		return &m_InvalidSprite;
	}

	bool Init(const iStringT& path, const iStringT& fname);

	bool InitFromCache(iFileI* cache );
	bool SaveCache( iFileI* cache );
		
	const iSprite& operator[] (const iStringT& name) const
	{
		iKVHash::ConstIterator it = m_SpriteHash.Find(name);
		check (it!=m_SpriteHash.End());
		return (*it).m_Value;
	}
	bool AddSprite(const iStringT& name, const iStringT& chunk, const iPoint& anchor, bool spanned, bool crop, uint8 shadow, uint32 seqX, uint32 seqY, const iStringT& constName, bool alpha);
	void LoadMemPng(iDib &dib, iStringT name);

private:
        
	iSprite& operator[] (const iStringT& name)
	{
		iKVHash::Iterator it = m_SpriteHash.Find(name);
		check (it!=m_SpriteHash.End());
		return (*it).m_Value;
	}
};

#endif //RESLIB_SPRITEMGR_H_
