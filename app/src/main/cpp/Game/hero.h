#ifndef PHEROES_GAME_HERO_H_
#define PHEROES_GAME_HERO_H_

enum TGMODE {
	TGM_NONE = 0,
	TGM_ATTACK,
	TGT_TOUCH,
	TGT_INTHERO
};

enum SPELL_SET_MODES {
	SSM_ALL = 0,
	SSM_AIR,
	SSM_EARTH,
	SSM_FIRE,
	SSM_WATER,
	SSM_FAVORITES,
	SSM_COUNT
};


//////////////////////////////////////////////////////////////////////////
class iSecSkillsCtr
{
public:
	iSecSkillsCtr(iFurtSkills& furtSkills);

	SECSKILL_LEVEL SkillLevel(SECONDARY_SKILLS secSkill) const;
	void Inc(SECONDARY_SKILLS secSkill);
	void Inc(SECONDARY_SKILLS secSkill, sint32 change);
	void Set(SECONDARY_SKILLS secSkill, SECSKILL_LEVEL lvl);
	void Set(const iSecSkills& secSkills);
	void Reset();
	uint32 Count() const { return m_secSkills.GetSize(); }
	const iSecSkillEntry& At(uint32 idx) const { return m_secSkills[idx]; }
	const iSecSkills& SecSkills() const { return m_secSkills; }

private:
	iFurtSkills&	m_furtSkills;
	iSecSkills		m_secSkills;
};

//////////////////////////////////////////////////////////////////////////
class iEnchHolder 
{
public:
	iEnchHolder(iFurtSkills& furtSkills): m_furtSkills(furtSkills) {}
	void Update(iFurtSkills& furtSkills) { m_furtSkills = furtSkills; }

	void Add(intptr_t _provider, FURTHER_SKILLS _type, ENCH_DURATION _dur, sint32 _mod)
	{ 
		m_items.Add(iEnchItem(_provider, _type, _dur, _mod)); 
		if (_type != FSK_INVALID) m_furtSkills.Value(_type) += _mod;
	}

	inline uint32 Count() const { return m_items.GetSize(); }
	inline const iEnchItem& operator[](uint32 idx) const { check(idx<m_items.GetSize()); return m_items[idx]; }

	/*
	inline const iEnchItem& operator[](uint32 idx) const
	{ check(idx<m_items.GetSize()); return m_items[idx]; }

	inline sint32 Mods(FURTHER_SKILLS type) const
	{ sint32 mod=0; for (uint32 xx=0; xx<m_items.GetSize(); ++xx) if (m_items[xx].type == type) mod+=m_items[xx].mod; return mod; }
	*/

	inline bool Registered(sint32 _provider) const
	{ for (uint32 xx=0; xx<m_items.GetSize(); ++xx) if (m_items[xx].provider == _provider) return true; return false; }

	void Clear() {
		m_items.RemoveAll();		
	}

	void RemoveDur(ENCH_DURATION dur)
	{
		for (uint32 xx=0; xx<m_items.GetSize();) {
			if (m_items[xx].dur == dur) RemoveAt(xx);
			else ++xx;
		}
	}

	void RemoveAll() {
		while(m_items.GetSize() > 0)
			RemoveAt(0);			
	}

private:
	void RemoveAt(uint32 idx)
	{
		if (m_items[idx].type != FSK_INVALID) m_furtSkills.Value(m_items[idx].type) -= m_items[idx].mod;
		m_items.RemoveAt(idx);
	}
	
private:
	iFurtSkills&			m_furtSkills;
	iSimpleArray<iEnchItem>	m_items;
};

//////////////////////////////////////////////////////////////////////////
struct iNewLevelInfo {
	iNewLevelInfo() {}

	iNewLevelInfo(const iNewLevelInfo& other) : prSkillType(other.prSkillType)
	{ secSkill[0] = other.secSkill[0]; secSkill[1] = other.secSkill[1]; }

	iNewLevelInfo(PRSKILL_TYPE _pr, SECONDARY_SKILLS sk1, SECONDARY_SKILLS sk2 ) : prSkillType(_pr)
	{ secSkill[0] = sk1; secSkill[1] = sk2;}

	PRSKILL_TYPE		prSkillType;
	SECONDARY_SKILLS	secSkill[2];
};

//////////////////////////////////////////////////////////////////////////
class iShip;
class iPlayer;
class iHero : public iIListNode, public iBaseMapObject, public iArmyOwner
{
private:
	struct iCharmEntry {
		iCharmEntry(MAGIC_SPELL _spellId, sint32 _flag=0) : spellId(_spellId), flag(_flag) {}
		MAGIC_SPELL spellId;
		sint32		flag;
	};
	typedef iSimpleArray<iCharmEntry> iCharmList;

private:
	iHeroT*			m_pProto;
	iSecSkillsCtr	m_SecSkills;
	iFurtSkills		m_constFurtSkills;
	iFurtSkills		m_varFurtSkills;
	iSpellBook		m_spellBook;
	SPELL_SET_MODES	m_spellSetMode;
	
	iArtCtr		m_Artifacts;
	iEnchHolder	m_Enchs;
	
	iPlayer*	m_pOwner;
	uint32		m_Level;
	sint32		m_Experience;
	sint32		m_manaPts;
	iStringT	m_CustomName;
	uint8		m_Angle;
	sint32		m_Moves;
	//iPoint		m_NextAnchor;
	iPath		m_Path;
	iPoint		m_NextPoint;
	iCharmList	m_charms;

	bool		m_bCanDig;
	bool		m_bStop;
	bool		m_bMoving;
    
    iShip*      m_pShip;

	iMapCnst*	m_pLocation;
	bool		m_bDead;
	iPoint		m_initialPos; // initial pos of hero on map (needed to properly handle ML names)
	bool		m_bNoExping;
protected:
	void ProcessItemChanges();
	iNewLevelInfo NextLevelInfo();

public:	
	IMPL_TYPEAWARE(iHero);
	virtual iHero* GetArmyOwnerHero() { return this; }	
	virtual iPlayer* GetArmyOwnerPlayer() { return m_pOwner; }
	// Construct hero object from proto
	iHero(iHeroT* pProto, sint32 fake, bool bAfterLoad);
    
	// Serialize / Deserialize hero object
	void Serialize(iDynamicBuffer& dbuff) const;
	void Unserialize(iDynamicBuffer& dbuff, bool bInit, bool bAfterLoad, bool bSetOwner);

	void Deinit(bool bResetArmy);
	void Init(iPlayer* pOwner, const iPoint& pos, sint8 angle, bool bAfterLoad);

    void SetPos(const iPoint& pos);
	bool CheckPath() { return !m_Path.IsEmpty(); };
	bool Step();
	void Stop();
	bool NeedStop();
	inline bool Moving() const { return m_bMoving; }
	bool Exhausted();
	
	void Process(fix32 t);
	iPath::Target GetTarget(sint16 x, sint16 y, bool bIgnoreFog = false);
	bool MakePath(iPath& path, sint16 x, sint16 y);
	bool SetPath();
	void SetPath(const iPoint& cell, bool moveAllowed = false, uint32 radius = 3);
	void SetPath(const iPath& path);
	void ResetPath();	

	VISION_LEVEL VisionLevel(sint32& radius) const;
	
	//inline iArmy& Army() { return m_Army; }
	inline iEnchHolder& Enchs()	{ return m_Enchs; }
	inline const iEnchHolder& Enchs() const { return m_Enchs; }
	inline const iPath& Path() const { return m_Path; }
	inline uint8& Angle() { return m_Angle; }
	inline uint8 Angle() const { return m_Angle; }
	inline const iHeroT* Proto() const { return m_pProto; }
	inline iPlayer* Owner() const { return( m_pOwner == NULL ? NULL: m_pOwner ); }
	inline void SetOwner(iPlayer *pOwner) { m_pOwner = pOwner; }

	inline const iSpellBook& SpellBook() const { return m_spellBook; }
	inline bool HasSpell(uint16 spellId) const { return m_spellBook.HasSpell(spellId); }
	inline bool CanLearnSpell(uint16 spellId) const { return SPELL_DESCRIPTORS[spellId].level < (FurtSkill(FSK_WISDOM)+2); }
	void LearnSpell(uint16 spellId, bool bForce = false);
	inline void AddFavoriteSpell(uint16 spellId) { m_spellBook.AddFavorite(spellId); }
	inline SPELL_SET_MODES GetSpellSetMode() const { return m_spellSetMode; }
	inline void SetSpellSetMode(SPELL_SET_MODES ssm) { m_spellSetMode = ssm; }

	bool Dig();
	inline bool CanDig() const { return m_bCanDig; }

	iStringT Name() const;
    iStringT EnglishName() const;
	const HERO_TYPE Type() const;

	inline uint32 Level() const { return m_Level; }
	inline sint32& Experience() { return m_Experience; }
	inline sint32 Experience() const { return m_Experience; }
	uint8 Scouting() const;
	inline uint32 GetValueIndex() const { return m_constFurtSkills.Value(FSK_ATTACK)*10 + m_constFurtSkills.Value(FSK_DEFENCE)*8 + m_constFurtSkills.Value(FSK_POWER)*8 + m_constFurtSkills.Value(FSK_KNOWLEDGE)*6; }

	inline sint32 Moves() const { return m_Moves; }
	inline void SetMoves(uint32 moves) { m_Moves = moves; }
	void AddMoves(sint32 quant) { m_Moves += quant; }
	sint32 TotalMoves() const;
	sint32 MoveCostBonus() const;
	
	static sint32 LevelBound(uint32 level);
	bool ReachNextLevel() const;
	inline bool& NoExping() { return m_bNoExping; }

	// initial pos (for ML purposes)
	inline const iPoint& InitialPos() const { return m_initialPos; }

	// Secondary skills
	inline iSecSkillsCtr& SecSkills() { return m_SecSkills; }
	inline const iSecSkillsCtr& SecSkills() const { return m_SecSkills; }

	// Further skills
	inline iFurtSkills& ConstFurtSkills() { return m_constFurtSkills; }
	inline const iFurtSkills& ConstFurtSkills() const { return m_constFurtSkills; }
	inline iFurtSkills& VarFurtSkills() { return m_varFurtSkills; }
	inline const iFurtSkills& VarFurtSkills() const { return m_varFurtSkills; }
	sint32 FurtSkill(FURTHER_SKILLS furtSkill) const;
	iFurtSkills GetFullFurtSkills() const;
	
	// Artifacts
	inline iArtCtr& Artifacts() { return m_Artifacts; }
	inline const iArtCtr& Artifacts() const { return m_Artifacts; }
	bool NeedArtifact(uint16 artId) const;
	void CaptureArtifacts(const iArtList& artList);
	bool HasSpecFlag(SPEC_HERO_FLAGS flag) const;

	// Opertaions
	inline sint32 ConvExpPts(sint32 exppts) const { exppts += (exppts*FurtSkill(FSK_LEARNING))/100; return exppts; }
	sint32 ReceiveExperience(sint32 points);
	sint32 RiseSkeletons(sint32 enCas, sint32 enHpCas);
	iRewardItem* ReceiveReward(const iRewardItem& reward);
	uint32 ReceiveRewards(const iRewardsCtr& rewards, bool showMsg, const iCharT* title, const iCharT* text);
	void FinalizeReward(const iRewardItem* item);

	// Mana
	inline sint32 ManaPts() const { return m_manaPts; }
	inline sint32& ManaPts() { return m_manaPts; }
	sint32 MaxManaPts() const;

	// Heroes' location (map construction)
	inline iMapCnst* GetLocation() const { return m_pLocation; }
	void SetLocation(iMapCnst* pLocation);

	// Charms
	void CastSpell(MAGIC_SPELL spell);
	void AddCharm(MAGIC_SPELL spell, sint32 flag);
	bool CharmedBy(MAGIC_SPELL spell, sint32& flag) const 
	{ 
		for (uint32 xx=0; xx<m_charms.GetSize(); ++xx) {
			if (m_charms[xx].spellId == spell) {
				flag = m_charms[xx].flag;
				return true; 
			}
		}
		return false; 
	}

	//
	void NewDay();
	void NewWeek(const iWeekDesc& weekDesk);
	void OnEndBattle(bool bWin);
	void OnSelect(bool bAfterLoad);

    inline bool InShip() const { return m_pShip != 0; };
    inline iShip* GetShip() { return m_pShip; };

	// requirements
	bool MeetsReqs(iReqsCtr& reqs);
	bool PermanentlyFailedReqs(iReqsCtr& reqs);

	/// Returns if losing this hero means defeat
	bool IsCrucial();

	virtual bool CanBeEmpty() const { return false; }
    
private:	
	void ResetHeroArmy(bool bAfterLoad);
	void StartMoving();
	void StopMoving();
	sint32 TNLPerc() const;
public:
	void ActivateConstruction(iMapCnst* pCnst);

	void SiegeCastle(iCastle* pCastle);
	void CaptureCastle(iCastle* pCastle, bool bEnter);
	void EnterCastle(iCastle* pCastle);

	void TouchMapItem(iMapItem* pMapItem, bool bPostBattle);
	void TouchMapGuard(iMapGuard* pMapGuard, bool bPostBattle);
	void AttackHero(iHero* pHero);
	void MeetHero(iHero* pHero);
    void SetShip(iShip *pShip) { m_pShip = pShip;};
};



#endif //PHEROES_GAME_HERO_H_
