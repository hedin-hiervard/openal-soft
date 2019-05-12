#include "stdafx.h"
#include "serialize.h"

//#ifdef _HMM_GAME_
//#include "xxc/xxc.security.h"
//#endif

/*
 *	
 */
bool iMapInfo::Supported() const
{
#if defined (PC_VERSION) || defined (OS_IPHONE) || defined(OS_ANDROID)//OS_APPLE
	return true;
#else
#if defined (OS_WINCE) || (defined (OS_WIN32) && defined (DEV_VER))
#	if defined(DEV_VER)
	if (m_encType == MET_MAPDES) return true;
#	endif
	return (m_encType == MET_PUBLIC) || (m_encType == MET_PRIVATE);
#else
	return m_encType == MET_MAPDES;
#endif
#endif
}

void iMapInfo::Unserialize(iDynamicBuffer &dbuff)
{
	// Map version
	uint16 ver;
	dbuff.Read(&ver,sizeof(ver));
	if (ver != GMAP_FILE_VERSION) return;

	// Encoding type
	dbuff.Read(&m_encType,sizeof(m_encType));

	// Save timestamp
	dbuff.Read(&m_saveTime, sizeof(m_saveTime));
	
	// Random seed
	dbuff.Read(&m_rseed, sizeof(m_rseed));

	// Map size
	uint8 mapSiz; dbuff.Read(&mapSiz,sizeof(mapSiz));
	m_Size = (MAP_SIZE)mapSiz;
	
	// Map Language
	uint32 lang;
	dbuff.Read(&lang, sizeof(uint32));	
	m_supportedLang = lang;
		
	::Unserialize(dbuff, m_Id); // Engish name

	for(uint8 x=0; x<GLNG_COUNT; x++)
		if((1 << x) & m_supportedLang) {
			::Unserialize(dbuff, m_Name[x]);
			::Unserialize(dbuff, m_Description[x]);		
		}

	
	// Map version and author (string of text)
	::Unserialize(dbuff, m_Version);
	::Unserialize(dbuff, m_Author);

	// Current date (1 is default value for new game)
	dbuff.Read(&m_curDay, sizeof(m_curDay));

	// Game mode (GM_UNDEFINED for new map)
	uint16 gameMode;
	dbuff.Read(&gameMode,sizeof(gameMode));
	m_gameMode = (GameMode)gameMode;

	// Difficulty level (DFC_UNDEFINED for new game)
	sint8 gameDifLvl;
	dbuff.Read(&gameDifLvl,sizeof(gameDifLvl));
	m_Difficulty = (DIFFICULTY_LEVEL)gameDifLvl;
	
	// Read Player config
	uint16 pCount;
	dbuff.Read(&pCount, sizeof(pCount));
	bool bHumanDefined = false;
	for (uint32 xx=0; xx<pCount; ++xx){
		// Player Id
		sint8 playerId;	
		dbuff.Read(&playerId,sizeof(playerId));
		// Nation type
		uint8 nation;
		dbuff.Read(&nation,sizeof(nation));
		// Player Type Mask
		uint8 playerTypeMask;
		dbuff.Read(&playerTypeMask,sizeof(playerTypeMask));
		// Player Type (PT_UNDEFINED for new game)
		uint8 playerType;
		dbuff.Read(&playerType,sizeof(playerType));
		// Create playerInfo descriptor
		iPlayerInfo playerInfo((PLAYER_ID)playerId, (PLAYER_TYPE_MASK)playerTypeMask, (PLAYER_TYPE)playerType, (CTL_TYPE)nation);
		if (playerType == PT_UNDEFINED) {
			if (!bHumanDefined && (playerTypeMask == PTM_HUMAN_ONLY || playerTypeMask == PTM_HUMAN_OR_COMPUTER)) {
				bHumanDefined = true;
				playerInfo.m_Type = PT_HUMAN;
			} else {
				playerInfo.m_Type = PT_COMPUTER;
			}
		}
		// Player resources (undefined for new game)
		::Unserialize(dbuff, playerInfo.m_Minerals);
		// Current Hero idx (0xFFFF = undefined for new game)
		dbuff.Read(&playerInfo.m_curHeroId,sizeof(playerInfo.m_curHeroId));
		// Current Castle idx (0xFFFF = undefined for new game)
		dbuff.Read(&playerInfo.m_curCastleIdx,sizeof(playerInfo.m_curCastleIdx));
		// Keys state
		dbuff.Read(&playerInfo.m_keys,sizeof(playerInfo.m_keys));
        // current tavern visitor
        dbuff.Read(&playerInfo.m_curVisitorId, sizeof(playerInfo.m_curVisitorId));

		// Insert Player descriptor
		m_Players.Add(playerInfo);
	}

	// Current player Id (PID_NEUTRAL = undefined for new game)
	uint16 curPlayerId;
	dbuff.Read(&curPlayerId,sizeof(curPlayerId));
	if (curPlayerId == 0xFFFF)m_curPlayerId = PID_NEUTRAL;
	else m_curPlayerId = (PLAYER_ID)curPlayerId;

	// Map metrics
	uint16 w,h;
	dbuff.Read(&w,sizeof(w));
	dbuff.Read(&h,sizeof(h));
	m_metrics.w = w;
	m_metrics.h = h;
    
    uint32 u;
    dbuff.Read(&u, sizeof(u));
    m_globalProperties = u;

}

#ifdef _HMM_GAME_

const iStringT& iMapInfo::MapName() const {
	if((1 << gSettings.GetLanguage()) & m_supportedLang) return m_Name[gSettings.GetLanguage()];
	else return m_Name[GLNG_ENGLISH];
}

const iStringT& iMapInfo::MapEnglishName() const {
	return m_Name[GLNG_ENGLISH];
}

const iStringT& iMapInfo::MapDescription() const {
	if((1 << gSettings.GetLanguage()) & m_supportedLang) return m_Description[gSettings.GetLanguage()];
	else return m_Description[GLNG_ENGLISH];
}

#endif

void iMapInfo::Serialize(iDynamicBuffer &dbuff) const
{
	// Map file version
	dbuff.Write(GMAP_FILE_VERSION);
	// Encoding type (MET_PUBLIC for saved game)
	dbuff.Write((uint16)m_encType);
	// Save timestamp
	dbuff.Write(m_saveTime);
	// Random seed
	dbuff.Write(m_rseed);
	// Map size
	dbuff.Write((uint8)m_Size);
	// Map Supported Languages
	dbuff.Write((uint32)m_supportedLang);	

	// Map id (english name)
	::Serialize(dbuff, m_Id);
	// Map name and description (string of text)
	for(uint8 xx=0; xx<GLNG_COUNT; xx++)
		if(m_supportedLang & (1 << xx)) {
			::Serialize(dbuff, m_Name[xx]);
			::Serialize(dbuff, m_Description[xx]);
		}
	
	// Map version and author (string of text)
	::Serialize(dbuff, m_Version);
	::Serialize(dbuff, m_Author);
	// Current date
	dbuff.Write(m_curDay);
	// Game mode 
	dbuff.Write((uint16)m_gameMode);
	// Difficulty
	dbuff.Write((sint8)m_Difficulty);
	// Player list
	dbuff.Write((uint16)m_Players.GetSize());
	for (uint8 x=0; x<m_Players.GetSize(); x++) {
		// Player Id
		dbuff.Write((uint8)m_Players[x].m_Id);
		//Nation type
		dbuff.Write((uint8)m_Players[x].m_Nation);
		// Player Type Mask
		dbuff.Write((uint8)m_Players[x].m_TypeMask);
		// Player Type
		dbuff.Write((uint8)m_Players[x].m_Type);
		// Player resources
		::Serialize(dbuff, m_Players[x].m_Minerals);
		// Current Hero Id
		dbuff.Write((uint16)m_Players[x].m_curHeroId);
		// Current Castle idx
		dbuff.Write((uint16)m_Players[x].m_curCastleIdx);
		// Keys
		dbuff.Write((uint8)m_Players[x].m_keys);
        // current tavern visitor
        dbuff.Write((uint16)m_Players[x].m_curVisitorId);
	}
	// Current player
	if(m_curPlayerId == PID_NEUTRAL)
		dbuff.Write((uint16)0xFFFF);
	else
		dbuff.Write((uint16)m_curPlayerId);
	// Map width and height
	dbuff.Write((uint16)m_metrics.w);
	dbuff.Write((uint16)m_metrics.h);

    // Global map properties
    dbuff.Write((uint32)m_globalProperties);    
}

bool iMapInfo::ReadMapInfo(iFileI* pFile, bool checkver)
{
	// Map version
	uint16 ver;
	pFile->Read(&ver,sizeof(ver));
#ifndef _DEBUG
	if (checkver && ver != GMAP_FILE_VERSION) return false;
#endif

	// Encoding type
	pFile->Read(&m_encType,sizeof(m_encType));

	// Save timestamp
	pFile->Read(&m_saveTime, sizeof(m_saveTime));
	
	// Random seed
	pFile->Read(&m_rseed, sizeof(m_rseed));

	// Map size
	uint8 mapSiz; pFile->Read(&mapSiz,sizeof(mapSiz));
	m_Size = (MAP_SIZE)mapSiz;
	
	// Map Language
	uint32 lang;
	pFile->Read(&lang, sizeof(uint32));	
	m_supportedLang = lang;
	
	::Unserialize(pFile, m_Id); // Engish name

	// read name and description for each supported language
	for(uint8 xx=0; xx<GLNG_COUNT; xx++)
		if(m_supportedLang & (1 << xx)) {
			::Unserialize(pFile, m_Name[xx]);
			::Unserialize(pFile, m_Description[xx]);		
		}
	
	// Map version and author (string of text)
	::Unserialize(pFile, m_Version);
	::Unserialize(pFile, m_Author);

	// Current date (1 is default value for new game)
	pFile->Read(&m_curDay, sizeof(m_curDay));

	// Game mode (GM_UNDEFINED for new map)
	uint16 gameMode;
	pFile->Read(&gameMode,sizeof(gameMode));
	m_gameMode = (GameMode)gameMode;

	// Difficulty level (DFC_UNDEFINED for new game)
	sint8 gameDifLvl;
	pFile->Read(&gameDifLvl,sizeof(gameDifLvl));
	m_Difficulty = (DIFFICULTY_LEVEL)gameDifLvl;
	
	// Read Player config
	uint16 pCount;
	pFile->Read(&pCount, sizeof(pCount));
	bool bHumanDefined = false;
	for (uint32 xx=0; xx<pCount; ++xx){
		// Player Id
		sint8 playerId;	
		pFile->Read(&playerId,sizeof(playerId));
		// Nation type
		uint8 nation;
		pFile->Read(&nation,sizeof(nation));
		// Player Type Mask
		uint8 playerTypeMask;
		pFile->Read(&playerTypeMask,sizeof(playerTypeMask));
		// Player Type (PT_UNDEFINED for new game)
		uint8 playerType;
		pFile->Read(&playerType,sizeof(playerType));
		// Create playerInfo descriptor
		iPlayerInfo playerInfo((PLAYER_ID)playerId, (PLAYER_TYPE_MASK)playerTypeMask, (PLAYER_TYPE)playerType, (CTL_TYPE)nation);
		if (playerType == PT_UNDEFINED) {
			if (!bHumanDefined && (playerTypeMask == PTM_HUMAN_ONLY || playerTypeMask == PTM_HUMAN_OR_COMPUTER)) {
				bHumanDefined = true;
				playerInfo.m_Type = PT_HUMAN;
			} else {
				playerInfo.m_Type = PT_COMPUTER;
			}
		}
		// Player resources (undefined for new game)
		::Unserialize(pFile, playerInfo.m_Minerals);
		// Current Hero idx (0xFFFF = undefined for new game)
		pFile->Read(&playerInfo.m_curHeroId,sizeof(playerInfo.m_curHeroId));
		// Current Castle idx (0xFFFF = undefined for new game)
		pFile->Read(&playerInfo.m_curCastleIdx,sizeof(playerInfo.m_curCastleIdx));
		// Keys state
		pFile->Read(&playerInfo.m_keys,sizeof(playerInfo.m_keys));
		// current tavern visitor 
		pFile->Read(&playerInfo.m_curVisitorId, sizeof(playerInfo.m_curVisitorId));
		// Insert Player descriptor
		m_Players.Add(playerInfo);
	}

	// Current player Id (PID_NEUTRAL = undefined for new game)
	uint16 curPlayerId;
	pFile->Read(&curPlayerId,sizeof(curPlayerId));
	if (curPlayerId == 0xFFFF)m_curPlayerId = PID_NEUTRAL;
	else m_curPlayerId = (PLAYER_ID)curPlayerId;

	// Map metrics
	uint16 w,h;
	pFile->Read(&w,sizeof(w));
	pFile->Read(&h,sizeof(h));
	m_metrics.w = w;
	m_metrics.h = h;

    // Cotulla: read GP
    uint32 u;
    pFile->Read(&u, sizeof(u));
    m_globalProperties = u;
	return true;
}

// Reorder player list - PT_HUMAN players first, and then PT_COMPUTER type players.
void iMapInfo::ReorderPlayers()
{
	iSimpleArray<iPlayerInfo> defPl;
	for (uint32 pidx = 0; pidx<m_Players.GetSize();) {
		check(m_Players[pidx].m_Type != PT_UNDEFINED);
		if (m_Players[pidx].m_Type == PT_HUMAN) {
			defPl.Add(m_Players[pidx]);
			m_Players.RemoveAt(pidx);
		} else ++pidx;
	}
//	check(defPl.GetSize() > 0);
	m_gameMode = (defPl.GetSize() == 1)?GM_SPLAYER:GM_HOTSEAT;
	defPl.Add(m_Players);
	m_Players = defPl;
}

#ifdef MULTIPLAYER
void iMapInfo::PrepareForNetworkPlay(PLAYER_ID ownId, bool host)
{
	m_gameMode = GM_NETWORK;	
	m_Difficulty  = DFC_EASY;
	for (uint32 pidx = 0; pidx<m_Players.GetSize(); pidx++) {		
		if(m_Players[pidx].m_Id == ownId) { // that's us
			m_Players[pidx].m_Type = PT_HUMAN;			
		}
		else {
			const iMPManager::GameUserList list = gMPMgr.GetGameUserList();
			bool found = false;
			for(uint32 xx=0; xx<list.GetSize(); xx++)
				if(list[xx].m_color == m_Players[pidx].m_Id) {					
					found = true;
				} 
			if(found) {			
				//if(m_Players[pidx].m_Type == PT_HUMAN ||
				//(m_Players[pidx].m_Type == PT_COMPUTER && gMPMgr.GetHostType() == iMPManager::HT_JOINED))
				m_Players[pidx].m_Type = PT_REMOTE;
			} else {
				if(gMPMgr.GetHostType() == iMPManager::HT_JOINED)
					m_Players[pidx].m_Type = PT_REMOTE;
				else
					m_Players[pidx].m_Type = PT_COMPUTER;
			}
		}	
	}		
}
#endif

bool iMapInfo::NotOfAnyType() const
{
	return (((m_globalProperties & GMP_GMODE_ACTION) == 0) && 
			((m_globalProperties & GMP_GMODE_ADVENTURE) == 0) && 
			((m_globalProperties & GMP_GMODE_MULTIPLAYER) == 0));
}

/*
 *	
 */
iGameMap::iGameMap()
{
}

iGameMap::~iGameMap()
{
}


