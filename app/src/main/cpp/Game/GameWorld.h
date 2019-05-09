#ifndef PHEROES_GAME_GAMEWORLD_H_
#define PHEROES_GAME_GAMEWORLD_H_
/*
 *	Game world
 */
#ifdef _DEBUG
void Debug(iStringA str);

#endif
class iGameWorld : public iGameMap
{
public:
	typedef iMapT<iBaseMapObject*>		iCoverMap;
	typedef iMapT<sint8>				iPassMap;

public:
	iGameWorld();
	~iGameWorld();

	const iStringT MapName() const;
    const iStringT MapEnglishName() const;

	inline MAP_SIZE GameWorldSize() const { return m_mapSiz; }
	inline sint32 Rand(sint32 maxval) { 
#ifdef _DEBUG
		sint32 oldseed = m_rand.GetSeed();
#endif
		sint32 res = m_rand.Rand(maxval);
#ifdef _DEBUG
		Debug(iFormat(("old seed: %ld, new seed: %ld"), oldseed, m_rand.GetSeed())); 
#endif
		return res; 
	}
	inline void SetRandSeed(uint32 newseed) { m_rand.SetNewSeed(newseed); }
	inline uint32 GetRandSeed() const { return m_rand.GetSeed(); }

	uint32 CalcGameScore() const;

	iBaseMapObject* GetCellObj(const iPoint& cell, sint32 radius = 0);
	iBaseMapObject* GetCellObj(const iPoint& cell, iStringT& name);
	void StartGame(PLAYER_ID curPlayer, bool bAfterLoad);
	iPlayer* AddPlayer(PLAYER_TYPE_MASK playerTypeMask, PLAYER_ID pid, NATION_TYPE nation, PLAYER_TYPE pt);
	void ReplacePlayer(iPlayer *o, iPlayer *n);
	void RemovePlayer(iPlayer* pPlayer, LOSE_CONDITION_TYPE lc);
	void SetHeroPos(iHero* pHero, const iPoint& pos);
	iHero* AddHero(iHero* pHero, iPlayer* pOwner, const iPoint& pos, sint8 angle, bool bAfterLoad, bool bUpdateFog = true);
	void RemoveHero(iHero* pHero, bool bResetArmy, bool bCheckVictory = true);
	void MoveHero(const iPoint& oldPos, const iPoint& newPos);
	iVisCnst* AddVisCnst(iVisCnst* pCnst);
	void RemoveVisCnst(iVisCnst* pCnst);
	iOwnCnst* AddOwnCnst(iOwnCnst* pCnst, bool bUpdateFog = true);
	void RemoveOwnCnst(iOwnCnst* pCnst);
	iCastle* AddCastle(iCastle* pCastle, bool bUpdateFog = true);
	iMapEvent* AddMapEvent(iMapEvent* pMapEvent);
	void RemoveMapEvent(iMapEvent* pMapEvent);
	iMapEvent* FindMapEvent(const iPoint& cell, PLAYER_ID pid);

	iMapMarker* AddMapMarker(iMapMarker* pMapMarker);
	iBaseMapObject* FindTeleportDestination(iVisCnst_NewTeleport* self, uint8 idx);
	iSimpleArray<iBaseMapObject*> FindTeleportDestinations(iVisCnst_NewTeleport* self, uint8 idx);
	iSimpleArray<iBaseMapObject*> FindTeleportOrigins(iVisCnst_NewTeleport* self, uint8 idx);

	iMapItem* AddMapItem(iMapItem* pMapItem);
	void RemoveMapItem(iMapItem* pMapItem);
	iMapGuard* AddMapGuard(iMapGuard* pMapGuard);
	void RemoveMapGuard(iMapGuard* pMapGuard);
	void UpdateTavernVisitors();


#ifdef MULTIPLAYER
	PLAYER_ID RealCurPlayer();

#endif
	void CaptureCastle(iHero* pHero, iCastle* pCastle);
	bool CanDig(const iPoint& cell) const;
	bool Dig(const iPoint& cell);
	bool InitUltimateArt();

	bool InitMap(sint32 w, sint32 h);
	void CleanupGameWorld();
	bool LoadFromFile(const iMapInfo& mapInfo, iFileI* pFile, bool bUpdateVisitors, bool bAfterLoad);
	bool SaveToFile(iFileI* pFile, bool MP = false);
	void InitPassMap();

	void Process(fix32 t);
	void SetNewActor(iPlayer* pNewActor, bool bAttack);
	iPlayer* NextPlayer();
	bool IsFirstPlayer(iPlayer *plr);
	void NewDay();
	void NewWeek();
	void ProcessTimeEvents(iPlayer* pActor);

	inline iPoint UltimatePos() const { return m_posUltimate; }
	inline bool IsUltimateExists() const { return m_statUltimate == 0; }
	inline uint16 ObelisksCount() const { return m_obelisksCount; }

	inline uint16 GetPathEl(sint32 xpos, sint32 ypos) const
	{
		uint16 avatar = GetAvatar(xpos,ypos);
		if (avatar != 0xFFFF && (avatar & 0xFF00) == 0xFF00) return (avatar & 0xFF);
		return 0xFFFF;
	}

	inline uint8 PathElConfig(sint32 xpos, sint32 ypos) const
	{
		uint16 pathEl = GetPathEl(xpos,ypos);
		check(pathEl != 0xFFFF);
		uint8 cfg = 0;
		if (IsValidPos(xpos, ypos-1) && GetPathEl(xpos, ypos-1) == pathEl) cfg |= 1;
		if (IsValidPos(xpos+1, ypos) && GetPathEl(xpos+1, ypos) == pathEl) cfg |= 2;
		if (IsValidPos(xpos, ypos+1) && GetPathEl(xpos, ypos+1) == pathEl) cfg |= 4;
		if (IsValidPos(xpos-1, ypos) && GetPathEl(xpos-1, ypos) == pathEl) cfg |= 8;
		return cfg;
	}

	inline const iWeekDesc& WeekDesc() const { return m_weekDesc; }
	inline uint32 GameDay() const { return m_CurDay; }

	/* custom string from Hash for current language */
	const iStringT& GetText(TextEntryType t, void* pObj) const;
	const iStringT& GetText(const iStringT& key) const;
    const iStringT& GetEnglishText(const iStringT& key) const;
	inline iPlayer* FindPlayer(PLAYER_ID pid)
	{		
		for (iPLIt it=m_Players.First(); it != m_Players.End(); ++it) {
			if ( (*it)->PlayerId() == pid) return *it;
		}		
		return NULL;
	}

	DIFFICULTY_LEVEL DifficultyLevel() const { check(m_Difficulty!=DFC_UNDEFINED); return m_Difficulty; }

	uint32 TeleportsCount() const { return m_teleports.GetSize(); }
	iVisCnst_Teleport* GetTeleport(sint32 idx) { if(idx < 0 || idx > (sint32)m_teleports.GetSize() - 1) return NULL; else return m_teleports[idx]; }
	iVisCnst_Teleport* FindTeleport(sint32 x, sint32 y) 
	{ for (uint32 nn=0; nn<m_teleports.GetSize(); ++nn) if (m_teleports[nn]->Pos().x == x && m_teleports[nn]->Pos().y == y) return m_teleports[nn]; return NULL; }
	
	iVisCnst_NewTeleport* FindNewTeleport(sint32 x, sint32 y);	

	inline uint16 GetAvatar(sint32 x, sint32 y) const { return GetAt(x,y).avatar; }

	inline iPlayer* ActPlayer() { return m_pActPlayer; }

	inline iPlayer* CurPlayer() { return m_pCurPlayer; }
	inline const iPlayer* CurPlayer() const { return m_pCurPlayer; }

	inline iHero* CurHero() { return (m_pCurPlayer)?m_pCurPlayer->CurHero():NULL; }
	inline const iHero* CurHero() const { return (m_pCurPlayer)?m_pCurPlayer->CurHero():NULL; }

	inline iCastle* CurCastle() { return (m_pCurPlayer)?m_pCurPlayer->CurCastle():NULL; }
	inline const iCastle* CurCastle() const { return (m_pCurPlayer)?m_pCurPlayer->CurCastle():NULL; }
	inline bool IsCurPlayerActive() const { return m_pCurPlayer == m_pActPlayer; }		

	iCOCIt OwnCnstFirst() const { return m_OwnCnsts.First(); }
	iCOCIt OwnCnstEnd() const { return m_OwnCnsts.End(); }
	
	iCPLIt PlayerFirst() const { return m_Players.First(); }
	iCPLIt PlayerEnd() const { return m_Players.End(); }

	sint32 HumanPlayers() const;
	inline iMapInfo::GameMode GameMode() const { return m_gameMode; }
    iShipList       m_Ships;
	iPlayerList		m_Players;
	iPlayer*		m_pObserver;
	iVisCnstList	m_VisCnsts;
	iOwnCnstList	m_OwnCnsts;
	iCastleList		m_Castles;
	iMapItemList	m_MapItems;
	iMapEventList	m_MapEvents;
	iMapMarkerList	m_MapMarkers;
	iMapGuardList	m_MapGuards;
	uint32			m_CurDay;
	iCoverMap		m_CoverMap;
	iVanishItem*	m_pVanItem;
	iPassMap		m_PassMap;

    // ships        
    
    iShip* AddShip(const iPoint& pos, uint32 img_type);
    void RemoveShip(iShip *pShip);
    void SetShipPos(iShip* pShip, const iPoint& pos);
    bool IsCellNearBeach(const iPoint &pos);
    inline bool IsShipsEnabled() { return m_globalProperties & GMP_ENABLE_SHIPS ? true : false;};
	inline uint8 LevelCap() { return m_levelCap; }

	inline const iVictoryCondition& VictoryCondition() { return m_victoryCondition; }
	inline const iLoseCondition& LoseCondition() { return m_loseCondition; }

	uint8 PosCropsSprites(const iPoint& pos) const;
	void SetupVC();
private:
	void GenerateGuards(CREATURE_TYPE ct, sint32 week);
	
private:
	typedef iSimpleArray<iVisCnst_Teleport*> iTeleportList;
	typedef iSimpleArray<iVisCnst_NewTeleport*> iNewTeleportList;
	
	iTeleportList		m_teleports;
	iNewTeleportList	m_newteleports;
	iTimeEventMgr		m_timeEventMgr;
	iRandomizer			m_rand;
	iWeekDesc			m_weekDesc;
	iPlayer*			m_pCurPlayer;
	iPlayer*			m_pActPlayer;
	iStringT			m_Id;
	iStringT			m_Name[GLNG_COUNT];
	iStringT			m_Desc[GLNG_COUNT];
	iStringT			m_Version;
	iStringT			m_Author;
	MAP_SIZE			m_mapSiz;
	iMapInfo::GameMode	m_gameMode;
	DIFFICULTY_LEVEL	m_Difficulty;
	
	iPoint				m_posUltimate;
	uint8				m_radUltimate;
	uint8				m_statUltimate;
	uint16				m_obelisksCount;
	uint32				m_supportedLang;
	iTextMgr			m_ctMgr;
	uint32				m_globalProperties;
	uint8				m_levelCap;
	iWeekDesc*			m_nextWeek;			
	iVictoryCondition   m_victoryCondition;
	iLoseCondition		m_loseCondition;
};


#endif //PHEROES_GAME_GAMEWORLD_H_
