#ifndef PHEROES_GAME_PLAYER_H_
#define PHEROES_GAME_PLAYER_H_

//////////////////////////////////////////////////////////////////////////
class iVisItemList : public iSortArray<iBaseMapObject*>
{
public:
	void AddItem(iBaseMapObject* pObj)
	{
		Insert(pObj,(intptr_t)pObj);
	}

	void Cleanup()
	{
		m_Array.RemoveAll();
	}

};
//////////////////////////////////////////////////////////////////////////
struct iGameEvent : iIListNode
{
	iGameEvent(fix32 _time, const iStringT& _text, const iPoint& _pos = cInvalidPoint) : m_time(_time), text(_text), pos(_pos), dstate(0) {}

	fix32		m_time;
	iStringT	text;
	uint8		dstate; // 0-9
	iPoint		pos;
};

typedef iIList<iGameEvent>			iEventList;
typedef iEventList::Iterator		iEvtIt;
typedef iEventList::ConstIterator	iEvtCIt;

//////////////////////////////////////////////////////////////////////////
struct _pvs {
	_pvs(): showInfobar(false), spellbookSsm(SSM_ALL) {}
	bool showInfobar;
	uint32 spellbookSsm;	
};

typedef struct _pvs iPlayerViewSettings;

class iPlayer : public iIListNode, public TypeAware
{
	friend class iAI_Player;
public:
	typedef iIList<iHero>				iHeroList;
	typedef iHeroList::Iterator			iHLIt;
	typedef iHeroList::ConstIterator	iCHLIt;

    typedef iIList<iShip>				iShipList;
    typedef iShipList::Iterator			iSLIt;
    typedef iShipList::ConstIterator	iCSLIt;

	typedef iIList<iOwnCnst>			iCnstList;
	typedef iCnstList::Iterator			iCLIt;
	typedef iIList<iCastle>				iCastleList;
	typedef iCastleList::Iterator		iCtLIt;
	typedef iCastleList::ConstIterator	iCCtLIt;
    //typedef iIList<iShip>			    iShipList;


	typedef iMapT<sint8> iPassMap;

public:
	IMPL_TYPEAWARE( iPlayer );
	iPlayer(PLAYER_TYPE_MASK playerTypeMask, PLAYER_ID playerId, NATION_TYPE nation, const iSize& map_siz);
	iPlayer(iPlayer& other);
	virtual ~iPlayer();

	friend class iGameWorld;
	friend class iRemote_Player;
	friend class iHero;
	
	// virtuals
	virtual void Process(fix32 t);
	virtual inline PLAYER_TYPE PlayerType() const { return PT_HUMAN; }

	//
	bool NewDay(); 
	void NewWeek(const iWeekDesc& weekDesk);
	virtual void OnSelect(bool bNewWeek, bool bAfterLoad);
	virtual void OnCaptureCastle(iCastle* pNewCastle);

	iOwnCnst* AddCnst(iOwnCnst* pCnst, bool bUpdateFog = true);

	inline PLAYER_TYPE_MASK PlayerTypeMask() const { return m_playerTypeMask; }
	inline NATION_TYPE Nation() const { return m_Nation; }
	inline PLAYER_ID PlayerId() const { return m_PlayerId; }
	inline const iMineralSet& Minerals() const { return m_Minerals; }
	inline sint32 Mineral(MINERAL_TYPE mtype) const	{ return m_Minerals.quant[mtype]; }
	sint32 AddMineral(MINERAL_TYPE mtype, sint32 count, bool bShowMessage=false);
	sint32 GetMineral(MINERAL_TYPE mtype);
	void SetMinerals(const iMineralSet& ms);	
	void AddMineralSet(const iMineralSet& ms);	
	inline const iFogMap& FogMap()  const { return m_FogMap; }
	inline iFogMap& FogMap()  { return m_FogMap; }
	inline iPassMap& PassMap() { return m_passMap; }
	inline const iVisItemList& VisItems() const { return m_visItems; }
	inline const iVisItemList& NewVisItems() const { return m_newVisItems; }
	inline const iVisItemList& OldVisItems() const { return m_oldVisItems; }

	sint32 GetMarketIdx();
	void UpdateFogMap();
	bool UpdateVisItems();
	void ResetVistItemChanges();
	void UpdatePassMap();
    void UpdatePassMapLocNearShip(const iPoint &pt);
	virtual bool NeedStopHero() const;
	virtual bool EncounteredHero() const;

	// Tavern Visitor
	void UpdateTavernVisitor(iHero* pNewVisitor = NULL);
	iHero* RecruitTavernVisitor(iCastle* pTarget);
	inline const iHero* TavernVisitor() const { return m_pTavernVisitor; }

	// Keys
	bool HasKey(uint8 key) const { check(key<6); return (m_keys & (1<<key)) > 0; }
	void AddKey(uint8 key);
	uint8 GetKeys() const { return m_keys; }
	void SetKeys(uint8 keys) { m_keys = keys; }

	// 
	inline bool DefeatState() const { return HeroCount()>0 || CastleCount()>0; }

	// Castles
	iCastle* NextCastle();
	iCastle* PrevCastle();
	void SetCurCastle(iCastle* pCastle);
	iCastle* AddCastle(iCastle* pCastle, bool& bGameResult, bool bUpdateFog);
	iCastle* RemoveCastle(iCastle* pCastle, bool bCheckVictory = true);
	uint32 CurCastleIdx() const;

	inline iCastle* CurCastle() { return m_pCurCastle; }
	inline uint32 CastleCount() const { return m_Castles.Count(); }
	inline iCtLIt CastleFirst() { return m_Castles.First(); }
	inline iCtLIt CastleEnd() { return m_Castles.End(); }
	inline iCCtLIt CastleFirst() const { return m_Castles.First(); }
	inline iCCtLIt CastleEnd() const { return m_Castles.End(); }
	// Heroes
	iHero* NextHero();
	iHero* PrevHero();
	void PrvSetCurHero(uint16 hid);
	void SetCurHero(iHero* pHero);
	iHero* AddHero(iHero* pHero, bool bUpdateFog = true);
	iHero* RemoveHero(iHero* pHero, bool bCheckVictory = true);
	uint32 CurHeroIdx() const;
    
	// Misc 
	void NextHeroCastle();
	void PrevHeroCastle();


	inline iHero* CurHero()	{ return m_pCurHero; }
	inline const iHero* CurHero() const { return m_pCurHero; }
	inline uint32 HeroCount() const	{ return m_Heroes.Count(); }
	inline iCHLIt HeroFirst() const { return m_Heroes.First(); }
	inline iHLIt HeroFirst() { return m_Heroes.First(); }
	inline iCHLIt HeroEnd() const { return m_Heroes.End(); }
	inline iHLIt HeroEnd() { return m_Heroes.End(); }

	// FurtSkills
	inline const iFurtSkills& FurtSkills() const { return m_furtSkills; }
	inline iFurtSkills& FurtSkills() { return m_furtSkills; }

	// Puzzle
	uint16 OpenObelisk( );
	uint8 OpenedPuzzles() const;
	uint8 TotalPuzzles() const { return 45; }
	uint8 PuzzledCard(uint8 idx) const;
	
	// view settings
	iPlayerViewSettings& ViewSettings() { return m_viewSettings; }
	inline void SetHeroMovedFlag() { m_bHeroMovedFlag = true; }
	
	// victory condition
	VICTORY_CONDITION_TYPE MeetsVictoryCondition() const;
	LOSE_CONDITION_TYPE  MeetsLoseCondition() const;

	inline void SetRemoved(bool bRemoved) { m_bRemoved = bRemoved; }
	inline bool Removed() const { return m_bRemoved; }
	
	void DiscoverFullMap();
public:
	iCnstList	m_Cnsts;

protected:
	PLAYER_TYPE_MASK	m_playerTypeMask;
	iVisItemList		m_visItems;
	iVisItemList		m_newVisItems;
	iVisItemList		m_oldVisItems;
	iPassMap			m_passMap;
	bool				m_bDirtPassMap;
	iFurtSkills			m_furtSkills;

	iHeroList			m_Heroes;
	iHero*				m_pCurHero;

	iHero*				m_pTavernVisitor;    

	iCastleList			m_Castles;
	iCastle*			m_pCurCastle;
	iMineralSet			m_Minerals;
	PLAYER_ID			m_PlayerId;
	iFogMap				m_FogMap;
	NATION_TYPE			m_Nation;

	uint8				m_keys;

	uint16				m_openedObelisks;
	uint8				m_puzzleCards[45];
private:
	iPlayerViewSettings m_viewSettings;
	bool	m_bHeroMovedFlag;
	bool	m_bRemoved;
};					


#endif //PHEROES_GAME_PLAYER_H_
