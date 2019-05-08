#ifndef RESLIB_NODES_H_
#define RESLIB_NODES_H_

/*
 *	Nodes
 */
class iBaseNode : public iTreeNode, public TypeAware
{
public:
	IMPL_TYPEAWARE( iBaseNode );
	iBaseNode(const iStringT& nameKey) : m_NameKey(nameKey) {}
	virtual ~iBaseNode() {}

	inline const iStringT& NameKey() const { return m_NameKey; }

protected:
	iStringT	m_NameKey;
};

//
class iGroupNode : public iBaseNode
{
public:
	IMPL_TYPEAWARE( iGroupNode );
	iGroupNode(const iStringT& title) : iBaseNode(title) {}
};

//
class iGuardNode : public iBaseNode
{
public:
	IMPL_TYPEAWARE( iGuardNode );
	iGuardNode(CREATURE_TYPE ctype) : iBaseNode((ctype < CREAT_RANDOM)?CREAT_NAMEKEY[ctype]:RAND_GUARDS_NAMEKEY[ctype-CREAT_RANDOM]), m_CreatureType(ctype) {}

	inline iStringT Sprite() const
	{
		if (m_CreatureType < CREAT_RANDOM) return iFormat(_T("resources.minimon_%04d"),m_CreatureType);
		else return iFormat(_T("resources.rand_guards_%04d"),m_CreatureType-CREAT_RANDOM);
	}

	inline CREATURE_TYPE CreatureType() const
	{ return m_CreatureType; }

private:
	CREATURE_TYPE	m_CreatureType;
};

//
class iMineralNode : public iBaseNode
{
public:
	IMPL_TYPEAWARE( iMineralNode );
	iMineralNode(MINERAL_TYPE type) : iBaseNode((type==MINERAL_UNKNOWN)?_T("MAPRES_RAND_MINERAL"):RESOURCE_NAMEKEY[type]), m_MineralType(type) {}

	inline MINERAL_TYPE MineralType() const
	{ return m_MineralType; }

private:
	MINERAL_TYPE	m_MineralType;
};

//
class iManaCrystalNode : public iBaseNode
{
public:
	IMPL_TYPEAWARE( iManaCrystalNode );
	iManaCrystalNode() : iBaseNode(_T("MAPRES_MANACRYSTAL")) {}
};

//
class iCampFireNode : public iBaseNode
{
public:
	IMPL_TYPEAWARE( iCampFireNode );
	iCampFireNode() : iBaseNode(_T("MAPRES_CAMPFIRE")) {}
};

//
class iChestNode : public iBaseNode
{
public:
	IMPL_TYPEAWARE( iChestNode );
	iChestNode() : iBaseNode(_T("MAPRES_CHEST")) {}
};
//
class iSpellScrollNode : public iBaseNode
{
public:
	IMPL_TYPEAWARE( iSpellScrollNode );
	iSpellScrollNode() : iBaseNode(_T("MAPRES_SPELLSCROLL")) {}
};

//
class iLampNode : public iBaseNode
{
public:
	IMPL_TYPEAWARE( iLampNode );
	iLampNode() : iBaseNode(_T("MAPRES_LAMP")) {}
};
//
class iKeyGuardNode : public iBaseNode
{
public:
	IMPL_TYPEAWARE( iKeyGuardNode);
	iKeyGuardNode(uint8 key) : iBaseNode(_T("MAPRES_KEYGUARD")), m_key(key) {}
	uint8 m_key;
};

//
class iMapEventNode : public iBaseNode
{
public:
	IMPL_TYPEAWARE( iMapEventNode );
	iMapEventNode() : iBaseNode(_T("MAP_EVENT")) {}
};
//

class iMapMarkerNode : public iBaseNode
{
public:
	IMPL_TYPEAWARE( iMapMarkerNode );
	iMapMarkerNode() : iBaseNode(_T("MAP_MARKER")) {}
};
//

class iUltimateArtNode : public iBaseNode
{
public:
	IMPL_TYPEAWARE( iUltimateArtNode );
	iUltimateArtNode() : iBaseNode(_T("ART_ULTIMATE")) {}
};

//
class iDecorNode : public iBaseNode, public iVisualObject, public iIdxItem
{
public:
	IMPL_TYPEAWARE( iDecorNode );
	iDecorNode(const iStringT& id, const iStringT& nameKey)
	: iBaseNode(nameKey), iIdxItem(id) {}
};

//
class iPathElNode : public iBaseNode, public iIdxItem
{
public:
	IMPL_TYPEAWARE( iPathElNode );
	iPathElNode(const iStringT& id, const iStringT& nameKey, PATH_ELEMENT_TYPE type, const SpriteId& spriteBase, SPRITE_LEVEL level, sint32 passMod)
	: iBaseNode(nameKey), iIdxItem(id), m_type(type), m_spriteBase(spriteBase), m_level(level), m_passMod(passMod) {}

	inline PATH_ELEMENT_TYPE Type() const { return m_type; }
	inline SpriteId SpriteConfig(uint8 cfg) const
	{ return m_spriteBase + iFormat(_T("_%04d"),cfg); }
	inline SPRITE_LEVEL Level() const { return m_level; }
	inline sint32 PassModVal() const { return m_passMod; }

private:
	SpriteId			m_spriteBase;
	PATH_ELEMENT_TYPE	m_type;
	SPRITE_LEVEL		m_level;
	sint32				m_passMod;
	
};

//
class iHeroNode : public iBaseNode, public iIdxItem
{
public:
	IMPL_TYPEAWARE( iHeroNode );
	iHeroNode(const iStringT& id, const iStringT& nameKey, HERO_TYPE htype, const SpriteId& smallIconId, const SpriteId& bigIconId, const SpriteId& croppedIconId)
	: iBaseNode(nameKey), iIdxItem(id), m_hType(htype), m_smallIconId(smallIconId), m_bigIconId(bigIconId), m_croppedIconId(croppedIconId), m_bAvailable(true)
	{
		// Default hero cost
		m_Cost.Reset();
		m_Cost.quant[MINERAL_GOLD] = 10000;

		/*
		// Initial hero skills
		m_furtSkills.Value(FSK_SCOUTING) = 4;	// 4 is default scouting
		m_furtSkills.Value(FSK_MANAPTS) = 1;	// By default, hero earns 1 mana point each day
		m_furtSkills.Value(FSK_WISDOM) = 1;		// By default, hero can learn any 1st level spells
		m_furtSkills.Value(FSK_MAG_AIR) = 1;	// By default, hero can cast any 1st level air spells
		m_furtSkills.Value(FSK_MAG_EARTH) = 1;	// By default, hero can cast any 1st level earth spells
		m_furtSkills.Value(FSK_MAG_FIRE) = 1;	// By default, hero can cast any 1st level fire spells
		m_furtSkills.Value(FSK_MAG_WATER) = 1;	// By default, hero can cast any 1st level water spells
		*/
	}

	inline HERO_TYPE Type() const { return m_hType; }

	inline const SpriteId& SmallIcon() const { return m_smallIconId; }
	inline const SpriteId& BigIcon() const { return m_bigIconId; }
	inline const SpriteId& CroppedIcon() const { return m_croppedIconId; }


	inline const iFurtSkills& FurtSkills() const { return m_furtSkills; }
	inline iFurtSkills& FurtSkills() { return m_furtSkills; }
	inline const iSecSkills& SecSkills() const { return m_secSkills; }
	inline iSecSkills& SecSkills() { return m_secSkills; }
	inline const iSpellList& Spells() const { return m_spells; }
	inline iSpellList& Spells() { return m_spells; }
	inline iMineralSet& Cost() { return m_Cost; }
	inline bool IsAvailable() const { return m_bAvailable; }
	inline void Reset() { m_bAvailable = true; }


	iHero* CreateObject(const iPoint& pos, PLAYER_ID owner)
	{
		iHero* pHero = new iHero(pos, this, owner);
		// FurtSkills
		pHero->FurtSkills() = m_furtSkills;
		// Secondary skills
		pHero->SecSkills() = m_secSkills;
		// Spellbook
		pHero->Spells() = m_spells;

		m_bAvailable = false;
		return pHero;
	}

private:
	bool		m_bAvailable;
	HERO_TYPE	m_hType;
	SpriteId	m_smallIconId;
	SpriteId	m_bigIconId;
	SpriteId	m_croppedIconId;
	iFurtSkills	m_furtSkills;
	iSecSkills	m_secSkills;
	iSpellList	m_spells;
	iMineralSet	m_Cost;
};

//
class iShipNode : public iBaseNode, public iIdxItem
{
public:
    IMPL_TYPEAWARE( iShipNode );
    iShipNode(const iStringT& id, const iStringT& nameKey, uint32 img_type)
        : iBaseNode(nameKey), iIdxItem(id), m_img_type(img_type)    
    {
    }

    inline int GetImgType() const { return m_img_type; }

    iShip* CreateObject(const iPoint& pos)
    {
        iShip* pShip = new iShip(pos, this);
        return pShip;
    }

private:
    int         m_img_type;
};

#endif //RESLIB_NODES_H_
