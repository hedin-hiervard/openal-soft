#ifndef RESLIB_OBJECTS_H_
#define RESLIB_OBJECTS_H_

//////////////////////////////////////////////////////////////////////////
class iBaseMapObject : public TypeAware
{
public:
	IMPL_TYPEAWARE( iBaseMapObject );
	iBaseMapObject(const iPoint& pos) : m_Pos(pos){}
	
	inline const iPoint& Pos() const { return m_Pos; }
	void SetPos(const iPoint& pos) { m_Pos = pos; }
	virtual iBaseMapObject* Clone() { return new iBaseMapObject(m_Pos); }
private:
	iPoint	m_Pos;
protected:
	iBaseMapObject() {}
};

//////////////////////////////////////////////////////////////////////////
class iMapItem : public iBaseMapObject
{
public:
	IMPL_TYPEAWARE( iMapItem );
	iMapItem(const iPoint& pos, MAP_ITEM_TYPE mit, const SpriteId& sid);
	virtual ~iMapItem() {}

	inline const iStringT& Message() const { return m_message; }
	inline iStringT& Message() { return m_message; }
	inline MAP_ITEM_TYPE Type() const { return m_mapItemType; }
	inline const SpriteId& Sprite() const { return m_spriteId; }
	inline const iArmy& Guard() const { return m_guard; }
	inline iArmy& Guard() { return m_guard; }

	virtual iBaseMapObject* Clone() { 
		iMapItem *mi = new iMapItem();
		*mi = *this;
		return mi;
	}
protected:
	iArmy			m_guard;
	MAP_ITEM_TYPE	m_mapItemType;
	SpriteId		m_spriteId;
	iStringT		m_message;
protected:
	iMapItem() {}
};

//
class iMapItem_Mineral : public iMapItem
{
public:
	iMapItem_Mineral(const iPoint& pos, MINERAL_TYPE mt, sint32 count);

	inline MINERAL_TYPE& MineralType() { return m_mineralType; }
	inline sint32& MineralCount() { return m_count; }

	inline MINERAL_TYPE MineralType() const { return m_mineralType; }
	inline sint32 MineralCount() const { return m_count; }

	virtual iBaseMapObject* Clone() { 
		iMapItem_Mineral *mi = new iMapItem_Mineral();
		*mi = *this;
		return mi;
	}
private:
	MINERAL_TYPE	m_mineralType;
	sint32			m_count;
protected:
	iMapItem_Mineral() {}
};

//
class iMapItem_ManaCrystal : public iMapItem
{
public:
	iMapItem_ManaCrystal(const iPoint& pos, sint32 count);
	inline sint32 Count() const { return m_count; }

	virtual iBaseMapObject* Clone() { 
		iMapItem_ManaCrystal *mi = new iMapItem_ManaCrystal();
		*mi = *this;
		return mi;
	}
private:
	sint32			m_count;
protected:
	iMapItem_ManaCrystal() {}
};

//
class iMapItem_CampFire : public iMapItem
{
public:
	iMapItem_CampFire(const iPoint& pos);

	virtual iBaseMapObject* Clone() { 
		iMapItem_CampFire *mi = new iMapItem_CampFire();
		*mi = *this;
		return mi;
	}
protected:
	iMapItem_CampFire() {}
};

//
class iMapItem_Chest : public iMapItem
{
public:
	iMapItem_Chest(const iPoint& pos);

	virtual iBaseMapObject* Clone() { 
		iMapItem_Chest *mi = new iMapItem_Chest();
		*mi = *this;
		return mi;
	}
protected:
	iMapItem_Chest() {}
};

//
class iMapItem_SpellScroll : public iMapItem
{
public:
	iMapItem_SpellScroll(const iPoint& pos, uint16 spellId);
	inline uint16 SpellId() const { return m_spellId; }
	inline uint16& SpellId() { return m_spellId; }

	virtual iBaseMapObject* Clone() { 
		iMapItem_SpellScroll *mi = new iMapItem_SpellScroll();
		*mi = *this;
		return mi;
	}
private:
	uint16	m_spellId;
protected:
	iMapItem_SpellScroll() {}
};
//
class iMapItem_Lamp : public iMapItem
{
public:
	iMapItem_Lamp(const iPoint& pos);
	virtual iBaseMapObject* Clone() { 
		iMapItem_Lamp *mi = new iMapItem_Lamp();
		*mi = *this;
		return mi;
	}
protected:
	iMapItem_Lamp() {}
};
//
class iMapItem_KeyGuard : public iMapItem
{
public:
	iMapItem_KeyGuard(const iPoint& pos, uint8 key);
	uint8 GetKeyType() const { return m_key; }

	virtual iBaseMapObject* Clone() { 
		iMapItem_KeyGuard *mi = new iMapItem_KeyGuard();
		*mi = *this;
		return mi;
	}

private:
	uint8	m_key;
protected:
	iMapItem_KeyGuard() {}
};

//////////////////////////////////////////////////////////////////////////
class iMapGuard : public iBaseMapObject
{
public:
	IMPL_TYPEAWARE( iMapGuard );

	iMapGuard(const iPoint& pos, CREATURE_TYPE ct=CREAT_UNKNOWN, sint32 count=RANDOM_QUANTITY, GUARD_DISPOS disp = GDISP_AGGRESSIVE, bool bNotGrow = false);
	const iStringT& Title() const;
	inline const iCreatGroup& Creatures() const { return m_creat; }
	inline iCreatGroup& Creatures() { return m_creat; }
	inline GUARD_DISPOS& Disposition() { return m_disp; } 
	inline GUARD_DISPOS Disposition() const { return m_disp; } 
	inline bool& NotGrow() { return m_bNotGrow; } 
	inline bool NotGrow() const { return m_bNotGrow; }
	inline const SpriteId& Sprite() const { return m_spriteId; }
	inline const iStringT& IntroMessage() const { return m_introMessage; }
	inline iStringT& IntroMessage() { return m_introMessage; }
	void Serialize(iDynamicBuffer& dbuff);
	void Unserialize(iDynamicBuffer& dbuff);

	inline iReqsCtr& Reqs() { return m_reqs; }
	inline iStringT& CurFailMessage() { return m_curFailMessage; }
	inline iRewardsCtr& OKRewards() { return m_okRewards; }
	inline iStringT& OKRewardMessage() { return m_okMessage; }	
	inline iRewardsCtr& PermFailRewards() { return m_permFailRewards; }
	inline iStringT& PermFailRewardMessage() { return m_permFailMessage; }
	inline GUARD_REW_BEH& OkRewBehaviour() { return m_okRewBeh; }
	inline GUARD_REW_BEH& PermFailRewBehaviour() { return m_permFailRewBeh; }

	virtual iBaseMapObject* Clone() { 
		iMapGuard *mi = new iMapGuard();
		*mi = *this;
		return mi;
	}

private:
	iStringT		m_introMessage;
	SpriteId		m_spriteId;
	iCreatGroup		m_creat;
	GUARD_DISPOS	m_disp;
	bool			m_bNotGrow;	
		
	iReqsCtr	m_reqs;
	iStringT	m_curFailMessage;
		
	iRewardsCtr	m_okRewards;
	iStringT	m_okMessage;
	GUARD_REW_BEH   m_okRewBeh;

	iRewardsCtr	m_permFailRewards;
	iStringT	m_permFailMessage;
	GUARD_REW_BEH   m_permFailRewBeh;

protected:
	iMapGuard() {}
};

//////////////////////////////////////////////////////////////////////////
class iMapEvent : public iBaseMapObject
{
public:
	IMPL_TYPEAWARE( iMapEvent );
	iMapEvent(const iPoint& pos);

	inline const iArmy& Guard() const { return m_guard; }
	inline iArmy& Guard() { return m_guard; }
	inline const iStringT& RewardMessage() const { return m_rewardMessage; }
	inline iStringT& RewardMessage() { return m_rewardMessage; }
	inline const iStringT& AttackMessage() const { return m_attackMessage; }
	inline iStringT& AttackMessage() { return m_attackMessage; }
	inline iStringT&  RFailMessage() { return m_rfailMessage; }
	inline uint32 PlayerMask() const { return m_playerMask; }
	inline uint32& PlayerMask() { return m_playerMask; }
	inline const iRewardsCtr& Rewards() const { return m_rewards; }
	inline iRewardsCtr& Rewards() { return m_rewards; }
	inline const iReqsCtr& Reqs() const { return m_reqs; }
	inline iReqsCtr& Reqs() { return m_reqs; }	
	inline uint8 & Flags() { return m_flags; }
	
	virtual iBaseMapObject* Clone() { 
		iMapEvent *mi = new iMapEvent();
		*mi = *this;
		return mi;
	}
private:
	iRewardsCtr	m_rewards;
	iReqsCtr	m_reqs;
	uint32		m_playerMask;
	iStringT	m_rewardMessage;
	iStringT	m_attackMessage;
	iStringT	m_rfailMessage;
	iArmy		m_guard;
	uint8       m_flags;
protected:
	iMapEvent() {}
};

//////////////////////////////////////////////////////////////////////////
class iMapMarker : public iBaseMapObject
{
public:
	IMPL_TYPEAWARE( iMapMarker );
	iMapMarker(const iPoint& pos);		
	uint32& InIndex() { return m_inIndex; }
	virtual iBaseMapObject* Clone() { 
		iMapMarker *mi = new iMapMarker();
		*mi = *this;
		return mi;
	}
private:	
	uint32 m_inIndex;
protected:
	iMapMarker() {}
};


//////////////////////////////////////////////////////////////////////////
class iHeroNode;
class iHero : public iBaseMapObject
{
public:
	IMPL_TYPEAWARE( iHero );
	iHero(const iPoint& pos, iHeroNode* proto, PLAYER_ID owner);

	inline const iHeroNode* Proto() const { return m_pProto; }
	inline iHeroNode* Proto() { return m_pProto; }
	
	// Owner
	inline PLAYER_ID Owner() const { return m_Owner; }
	inline PLAYER_ID& Owner() { return m_Owner; }
	// Level
	inline uint32 Level() const { return m_Level; }
	inline uint32& Level() { return m_Level; }
	// Experience
	inline sint32 Experience() const { return m_Experience; }
	inline sint32& Experience() { return m_Experience; }
	// FurtSkills
	inline const iFurtSkills& FurtSkills() const { return m_FurtSkills; }
	inline iFurtSkills& FurtSkills() { return m_FurtSkills; }
	// Secondary skills
	inline const iSecSkills& SecSkills() const { return m_SecSkills; }
	inline iSecSkills& SecSkills() { return m_SecSkills; }
	// Army
	inline const iArmy& Army() const { return m_army; }
	inline iArmy& Army() { return m_army; }
	// Artifacts
	inline const iArtifactList& Artifacts() const { return m_artifacts; }
	inline iArtifactList& Artifacts() { return m_artifacts; }
	// Spellbook
	inline const iSpellList& Spells() const { return m_spells; }
	inline iSpellList& Spells() { return m_spells; }
	inline bool IsCustomSpells() const { return m_bCustomSpells; }
	inline void SetCustomSpells(bool bCustomSpells = true) { m_bCustomSpells = bCustomSpells; }
	// Custom name
	inline const iStringT& CustomName() const { return m_CustomName; }
	inline iStringT& CustomName() { return m_CustomName; }
	inline bool& NoExping() { return m_noExping; }

	virtual iBaseMapObject* Clone() { 
		iHero *mi = new iHero();
		*mi = *this;
		return mi;
	}

private:
	iArmy			m_army;
	iHeroNode*		m_pProto;
	PLAYER_ID		m_Owner;
	uint32			m_Level;
	sint32			m_Experience;
	iFurtSkills		m_FurtSkills;
	iSecSkills		m_SecSkills;
	iArtifactList	m_artifacts;
	iSpellList		m_spells;
	bool			m_bCustomSpells;
	iStringT		m_CustomName;
	bool			m_noExping;
protected:
	iHero() {}
};

//////////////////////////////////////////////////////////////////////////
class iShipNode;
class iShip : public iBaseMapObject
{
public:
    IMPL_TYPEAWARE( iShip );
    iShip(const iPoint& pos, iShipNode* proto);

    inline const iShipNode* Proto() const { return m_pProto; }
    inline iShipNode* Proto() { return m_pProto; }

    virtual iBaseMapObject* Clone()
    { 
        iShip *mi = new iShip();
        *mi = *this;
        return mi;
    }

    // Custom name
    inline const iStringT& CustomName() const { return m_CustomName; }
    inline iStringT& CustomName() { return m_CustomName; }


private:
    iShipNode*		m_pProto;
    iStringT		m_CustomName;
protected:
    iShip() {}
};

//
class iCastle;
class iPlayer
{
public:
	iPlayer(PLAYER_ID playerId, PLAYER_TYPE_MASK ptm) : m_PlayerId(playerId), m_PlayerTypeMask(ptm), m_pMainCastle(NULL), m_bCreateHero(false) {}

	iCastle*			m_pMainCastle;
	bool				m_bCreateHero;
	PLAYER_ID			m_PlayerId;
	PLAYER_TYPE_MASK	m_PlayerTypeMask;
};

// 
struct iBattleObstacle {
	iBattleObstacle(const SpriteId& sprite, const iPoint& offset, uint8 size, uint8 level)
	: m_surfMask(0), m_sprite(sprite), m_offset(offset), m_size(size), m_level(level) {}
	uint16		m_surfMask;
	SpriteId	m_sprite;
	iPoint		m_offset;
	uint8		m_size;
	uint8		m_level;
};

#endif //RESLIB_OBJECTS_H_
