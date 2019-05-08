#include "he/he.std.h"
#include "he/he.tables.h"
#include "he/he.container.map.h"
#include "he/he.container.cache.h"
#include "he/he.container.simple.array.h"
#include "he/he.file.h"
#include "he/he.file.name.h"

#include "pa/pa.xml.builder.h"
#include "pa/pa.map.h"
#include "pa/pa.sax.pathhandler.h"

#include "xe/xe.std.h"
#include "xe/xe.dib.h"
#include "xe/xe.dc.hlp.h"

#include "si/si.std.h"
#include "si/si.wctrl.tree.h"

#include "common.h"
#include "cm_magic.h"
#include "cm_creatures.h"
#include "cm_castles.h"
#include "cm_events.h"
#include "cm_constructions.h"
#include "Army.h"
#include "rtti.h"
#include "defines.h"
#include "objects.h"
#include "nodes.h"
#include "Artifacts.h"
#include "treasury_cnst.h"
#include "Constructions.h"

#include "SpriteMgr.h"
#include "AvatarMgr.h"
#include "TextMgr.h"
#include "SoundMgr.h"

#include "resmgr.h"

#include "ifaces.h"
bool TEXT_ENTRY_ML[TET_COUNT] = {true, false, true, true, false, true, true, true, true, true, false, true, true, true, true, true, true, true, true, true, true};

iStringT MakeTextEntryName(TextEntryType t, const void* pObj)
{	
    if (t == TET_DEFAULT) {
        return L"";
    } else if (t == TET_MAP_NAME) {
        return L"Map name";
    } else if (t == TET_MAP_DESC) {
        return L"Map Description";
    } else if (t == TET_TEVENT_TEXT) {
        iTimeEvent* pTE = (iTimeEvent*)pObj;
        return iFormat(L"Time Event (%s)", pTE->m_name.CStr());
    } else if (t == TET_HERO_CNAME) {
		iHero* pHero = (iHero*)pObj;
        return iFormat(_T("Hero at (%d,%d) custom name"), pHero->Pos().x, pHero->Pos().y);
    } else if (t == TET_SHIP_CNAME) {
        iShip* pShip = (iShip*)pObj;
        return iFormat(_T("Ship at (%d,%d) custom name"), pShip->Pos().x, pShip->Pos().y);        
    } else if (t == TET_MAPITEM_MSG) {
		iMapItem* pMapItem = (iMapItem*)pObj;
        return iFormat(_T("Map item at (%d,%d) message"), pMapItem->Pos().x, pMapItem->Pos().y);
    } else if (t == TET_MAPGUARD_INTROMSG) {
        iMapGuard* pMapGuard = (iMapGuard*)pObj;
        return iFormat(_T("Map guard at (%d,%d) message"), pMapGuard->Pos().x, pMapGuard->Pos().y);
    } else if (t == TET_MAPGUARD_OKREWMSG) {
        iMapGuard* pMapGuard = (iMapGuard*)pObj;
        return iFormat(_T("Map guard at (%d,%d) OK reward message"), pMapGuard->Pos().x, pMapGuard->Pos().y);
    } else if (t == TET_MAPGUARD_CURFAILMSG) {
        iMapGuard* pMapGuard = (iMapGuard*)pObj;
        return iFormat(_T("Map guard at (%d,%d) currently failing reqs message"), pMapGuard->Pos().x, pMapGuard->Pos().y);
    } else if (t == TET_MAPGUARD_PERMFAILMSG) {
        iMapGuard* pMapGuard = (iMapGuard*)pObj;
        return iFormat(_T("Map guard at (%d,%d) perm failed reqs message"), pMapGuard->Pos().x, pMapGuard->Pos().y);
    } else if (t == TET_MEVENT_REWMSG) {
        iMapEvent* pMapEvent = (iMapEvent*)pObj;
        return iFormat(_T("Map event at (%d,%d) reward message"), pMapEvent->Pos().x, pMapEvent->Pos().y);
    } else if (t == TET_MEVENT_RFAILMSG) {
        iMapEvent* pMapEvent = (iMapEvent*)pObj;
        return iFormat(_T("Map event at (%d,%d) req failure message"), pMapEvent->Pos().x, pMapEvent->Pos().y);
    } else if (t == TET_MEVENT_ATTMSG) {
        iMapEvent* pMapEvent = (iMapEvent*)pObj;
        return iFormat(_T("Map event at (%d,%d) attack message"), pMapEvent->Pos().x, pMapEvent->Pos().y);
    } else if (t == TET_VIS_INTROMSG) {
        iVisCnst* pVisCnst = (iVisCnst*)pObj;
        return iFormat(_T("Visitable at (%d,%d) custom message"), pVisCnst->Pos().x, pVisCnst->Pos().y);
    } else if (t == TET_VIS_OKREWMSG) {
        iVisCnst* pVisCnst = (iVisCnst*)pObj;
        return iFormat(_T("Visitable at (%d,%d) OK reward message"), pVisCnst->Pos().x, pVisCnst->Pos().y);
    } else if (t == TET_VIS_CURFAILMSG) {
        iVisCnst* pVisCnst = (iVisCnst*)pObj;
        return iFormat(_T("Visitable at (%d,%d) currently failing reqs message"), pVisCnst->Pos().x, pVisCnst->Pos().y);
    } else if (t == TET_VIS_PERMFAILMSG) {
        iVisCnst* pVisCnst = (iVisCnst*)pObj;
        return iFormat(_T("Visitable at (%d,%d) perm failed reqs message"), pVisCnst->Pos().x, pVisCnst->Pos().y);
    } else if (t == TET_OWN_INTROMSG) {
        iOwnCnst* pOwnCnst = (iOwnCnst*)pObj;
        return iFormat(_T("Ownerable at (%d,%d) intro message"), pOwnCnst->Pos().x, pOwnCnst->Pos().y);
    } else if (t == TET_OWN_OKREWMSG) {
        iOwnCnst* pOwnCnst = (iOwnCnst*)pObj;
        return iFormat(_T("Ownerable at (%d,%d) OK reward message"), pOwnCnst->Pos().x, pOwnCnst->Pos().y);
    } else if (t == TET_OWN_CURFAILMSG) {
        iOwnCnst* pOwnCnst = (iOwnCnst*)pObj;
        return iFormat(_T("Ownerable at (%d,%d) currently failing reqs message"), pOwnCnst->Pos().x, pOwnCnst->Pos().y);
    } else if (t == TET_OWN_PERMFAILMSG) {
        iOwnCnst* pOwnCnst = (iOwnCnst*)pObj;
        return iFormat(_T("Ownerable at (%d,%d) perm failed reqs message"), pOwnCnst->Pos().x, pOwnCnst->Pos().y);
    } else if (t == TET_CASTLE_CNAME) {
        iCastle* pCastle = (iCastle*)pObj;
        return iFormat(_T("Castle at (%d,%d) custom message"), pCastle->Pos().x, pCastle->Pos().y);
    }

    check(0);
    return L"";
}

GAME_LANG GetLangNameId( iStringT& lang ){

	if( lang == _T("English") )
		return( GLNG_ENGLISH );
	else if( lang == _T("Russian") )
		return( GLNG_RUSSIAN );
	else if( lang == _T("Polish")  )
		return( GLNG_POLISH );
	else if( lang == _T("Slovak") )
		return( GLNG_SLOVAK );
	else if( lang == _T("German") )
		return( GLNG_GERMAN );
	else if( lang == _T("Czech") )
		return( GLNG_CZECH );
	else if( lang == _T("Italian") )
		return( GLNG_ITALIAN );
	else if( lang == _T("French") )
		return( GLNG_FRENCH );
	else if( lang == __T("Spanish") )
		return( GLNG_SPANISH );

	return( GLNG_COUNT );
}

LPCTSTR LANG_NAME[GLNG_COUNT] = {
    _T("English"),
    _T("Russian"),
    _T("Polish"),
    _T("Slovak"),
    _T("German"),
    _T("Czech"),
    _T("Italian"),
    _T("French"),
    _T("Spanish")
};

LPCTSTR SURF_ID[STYPE_COUNT] ={
    _T("WATER"),
    _T("SAND"),
    _T("DIRT"),
    _T("GRASS"),
    _T("SWAMP"),
    _T("LAVA"),
    _T("WASTELAND"),
    _T("DESERT"),
    _T("SNOW"),
    _T("NDESERT"),
    _T("PAVEMENT"),
    _T("NWASTELAND")
};

LPCTSTR SURF_NAMEKEY[STYPE_COUNT] = {
    _T("SURF_WATER"),
    _T("SURF_SAND"),
    _T("SURF_DIRT"),
    _T("SURF_GRASS"),
    _T("SURF_SWAMP"),
    _T("SURF_LAVA"),
    _T("SURF_WASTELAND"),
    _T("SURF_DESERT"),
    _T("SURF_SNOW"),
    _T("SURF_DESERT")
    _T("SURF_PAVEMENT"),
    _T("SURF_WASTELAND")
};

LPCTSTR MAP_SIZ_SNAME[MSIZ_COUNT] = {
    _T("S"),
    _T("M"),
    _T("L"),
    _T("XL")
};

LPCTSTR MAP_SIZ_NAMEKEY[MSIZ_COUNT] = {
    _T("MAPSIZ_SMALL"),
    _T("MAPSIZ_MEDIUM"),
    _T("MAPSIZ_LARGE"),
    _T("MAPSIZ_EXTRALARGE")
};

LPCTSTR DIFF_NAMEKEY[DFC_COUNT] = {
    _T("DIFF_EASY"),
    _T("DIFF_NORMAL"),
    _T("DIFF_HARD"),
    _T("DIFF_EXPERT"),
    _T("DIFF_IMPOSSIBLE")
};

LPCTSTR VICTCOND_NAMEKEY[VC_COUNT] = {
    _T("VC_DEFEAT_ALL_ENEMIES"),
    _T("VC_DEFEAT_HERO"),    
    _T("VC_CAPTURE_CASTLE"),    
	_T("VC_SURVIVE"),	
	_T("VC_RESOURCE"),
	_T("VC_ARTIFACT"),
	_T("VC_MONSTER"),
	_T("VC_SPELL"),
	_T("VC_ULTIMATE_ART"),
};


LPCTSTR LOSECOND_NAMEKEY[LC_COUNT] = {
    _T("LC_NOTHING"),
    _T("LC_LOSE_HERO"),
    _T("LC_LOSE_CASTLE"),	
};


LPCTSTR PLAYER_NAMEKEY[PID_COUNT] = {
    _T("PLAYER_RED"),
    _T("PLAYER_GREEN"),
    _T("PLAYER_BLUE"),
    _T("PLAYER_CYAN"),
    _T("PLAYER_MAGENTA"),
    _T("PLAYER_YELLOW")
};

LPCTSTR PLAYER_TYPE_NAMEKEY[PTM_COUNT] = {
    _T("PT_UNDEFINED"),
    _T("PT_HUMAN"),
    _T("PT_COMPUTER")
};

LPCTSTR PLAYER_TYPE_MASK_NAMEKEY[PTM_COUNT] = {
    _T("PTM_HUMAN_ONLY"),
    _T("PTM_COMPUTER_ONLY"),
    _T("PTM_HUMAN_OR_COMPUTER")
};

LPCTSTR HERO_TYPE_ID[HERO_TYPE_COUNT] = {
    _T("KNIGHT"),
    _T("BARBARIAN"),
    _T("WIZARD"),
    _T("WARLOCK"),
    _T("SORCERESS"),
    _T("NECROMANCER"),
    _T("VARIAG")
};

LPCTSTR HERO_TYPE_NAMEKEY[HERO_TYPE_COUNT] = {
    _T("HERO_TYPE_KNIGHT"),
    _T("HERO_TYPE_BARBARIAN"),
    _T("HERO_TYPE_WIZARD"),
    _T("HERO_TYPE_WARLOCK"),
    _T("HERO_TYPE_SORCERESS"),
    _T("HERO_TYPE_NECROMANCER"),
    _T("HERO_TYPE_VARIAG")    
};

LPCTSTR VCNSTTYPE_ID[VCNST_COUNT] = {
    _T("BASIC"),
    _T("STABLES"),
    _T("GAZEBO"),
    _T("MANASOURCE"),
    _T("MLMODIFIER"),
    _T("PSMODIFIER"),
    _T("OBELISK"),
    _T("SIGN"),
    _T("DWELLING"),
    _T("WITCHHUT"),
    _T("SHRINE"),
    _T("TREASURY"),
    _T("TELEPORT"),
    _T("KEYMASTER"),
    _T("KNWLTREE"),
    _T("WINDMILL"),
    _T("WEEKLYMIN"),
    _T("NEWTELEPORT"),
    _T("HARBOR"),
    _T("WARACADEMY")
};

LPCTSTR OCNSTTYPE_ID[OCNST_COUNT] = {
    _T("BASIC"),
    _T("FURTSKILL")
};

LPCTSTR ARTTYPE_ID[ARTT_COUNT] = {
    _T("BASIC"),
    _T("FURTSKILL"),
    _T("NEGSPHERE"),
    _T("SHOFWAR"),
    _T("CURSWORD"),
    _T("MONSTERSTAT"),
    _T("MAGICSPHERE"),
    _T("ICEAMULET"),
    _T("ARROWS"),
    _T("FASTCROSSBOW")

};

LPCTSTR ARTASSIGN_ID[ART_ASSIGN_COUNT] = {
    _T("HEAD"),
    _T("NECK"),
    _T("TORSO"),
    _T("SHOULDERS"),
    _T("HANDS"),
    _T("FINGERS"),
    _T("LEGS"),
    _T("FEET"),
    _T("MISC")
};

LPCTSTR CASTLE_PREFIXES[CTLT_COUNT] = {
    _T("citadel"),
    _T("stronghold"),
    _T("tower"),
    _T("dungeon"),
    _T("fortress"),
    _T("necropolis"),    
	_T("kremlin"),
    _T("randctl"), 

};

LPCTSTR CASTLE_NAMEKEY[CTLT_COUNT] = {
    _T("CASTLE_TYPE_CITADEL"),
    _T("CASTLE_TYPE_STRONGHOLD"),
    _T("CASTLE_TYPE_TOWER"),
    _T("CASTLE_TYPE_DUNGEON"),
    _T("CASTLE_TYPE_FORTRESS"),
    _T("CASTLE_TYPE_NECROPOLIS"),    
	_T("CASTLE_TYPE_KREMLIN"),
    _T("CASTLE_TYPE_RANDOM"),         
};

LPCTSTR CTLCNST_NAMEKEY[CTLCNST_COUNT] = {
    _T("DWEL_CITD_L1"), _T("DWEL_CITD_L2"), _T("DWEL_CITD_L3"), _T("DWEL_CITD_L4"), _T("DWEL_CITD_L5"), _T("DWEL_CITD_L6"),
    _T("DWEL_STRH_L1"), _T("DWEL_STRH_L2"), _T("DWEL_STRH_L3"), _T("DWEL_STRH_L4"), _T("DWEL_STRH_L5"), _T("DWEL_STRH_L6"),
    _T("DWEL_TOWR_L1"), _T("DWEL_TOWR_L2"), _T("DWEL_TOWR_L3"), _T("DWEL_TOWR_L4"), _T("DWEL_TOWR_L5"), _T("DWEL_TOWR_L6"),
    _T("DWEL_DUNG_L1"), _T("DWEL_DUNG_L2"), _T("DWEL_DUNG_L3"), _T("DWEL_DUNG_L4"), _T("DWEL_DUNG_L5"), _T("DWEL_DUNG_L6"),
    _T("DWEL_FORT_L1"), _T("DWEL_FORT_L2"), _T("DWEL_FORT_L3"), _T("DWEL_FORT_L4"), _T("DWEL_FORT_L5"), _T("DWEL_FORT_L6"),
    _T("DWEL_NECR_L1"), _T("DWEL_NECR_L2"), _T("DWEL_NECR_L3"), _T("DWEL_NECR_L4"), _T("DWEL_NECR_L5"), _T("DWEL_NECR_L6"),
	_T("DWEL_KREM_L1"), _T("DWEL_KREM_L2"), _T("DWEL_KREM_L3"), _T("DWEL_KREM_L4"), _T("DWEL_KREM_L5"), _T("DWEL_KREM_L6"),

    _T("CTLCNST_COMMON_MGUILD1"), _T("CTLCNST_COMMON_MGUILD2"), _T("CTLCNST_COMMON_MGUILD3"), _T("CTLCNST_COMMON_MGUILD4"), _T("CTLCNST_COMMON_MGUILD5"),
    _T("CTLCNST_COMMON_MNODE"), _T("CTLCNST_COMMON_TAVERN"), _T("CTLCNST_COMMON_MARKET"), _T("CTLCNST_COMMON_TOWNHALL"), _T("CTLCNST_COMMON_CITYHALL"),
    _T("CTLCNST_COMMON_OREMINE"), _T("CTLCNST_COMMON_SAWMILL"), _T("CTLCNST_COMMON_ALCHLAB"), _T("CTLCNST_COMMON_GEMSMINE"), _T("CTLCNST_COMMON_CRYSTALMINE"), _T("CTLCNST_COMMON_SULFURMINE"),
    _T("CTLCNST_COMMON_MOAT"), _T("CTLCNST_COMMON_MTURRET"), _T("CTLCNST_COMMON_LTURRET"), _T("CTLCNST_COMMON_RTURRET"),

    _T("CTLCNST_SHOOTINGRANGE"), _T("CTLCNST_MESSHALL"), _T("CTLCNST_OAKWOOD"), _T("CTLCNST_WATERFALL"), _T("CTLCNST_MINERSGUILD"), _T("CTLCNST_UNEARTHEDGRAVES"),
     _T("CTLCNST_SCOUT_TOWER"), _T("CTLCNST_LODGE"), _T("CTLCNST_MERCHANT"),
    _T("CTLCNST_OBSERVPOST"), _T("CTLCNST_FORTIFICATION"), _T("CTLCNST_HALLOFVALHALLA"), _T("CTLCNST_ADOBE"), _T("CTLCNST_WALLOFKNOWLEDGE"), _T("CTLCNST_LIBRARY"),
    _T("CTLCNST_ALTAR"), _T("CTLCNST_MANAVORTEX"), _T("CTLCNST_TREASURY"), _T("CTLCNST_MYSTICPOUND"), _T("CTLCNST_NECRAMPLIFIER"), _T("CTLCNST_COVEROFDARKNESS"),
    
};

LPCTSTR FURTSKILL_ID[FSK_COUNT] = {
    _T("ATTACK"),
    _T("DEFENCE"),
    _T("POWER"),
    _T("KNOWLEDGE"),

    _T("ACTPTS"),
    _T("LOGISTICS"),
    _T("PATHFINDING"),
    _T("SCOUTING"),
    _T("VISION"),

    _T("BALLISTICS"),
    _T("LEARNING"),
    _T("DIPLOMACY"),
    _T("NECROMANCY"),

    _T("SORCERY"),
    _T("MANAPTS"),
    _T("INTELLIGENCE"),
    _T("WISDOM"),

    _T("MAG_AIR"),
    _T("MAG_EARTH"),
    _T("MAG_FIRE"),
    _T("MAG_WATER"),

    _T("RES_AIR"),
    _T("RES_EARTH"),
    _T("RES_FIRE"),
    _T("RES_WATER"),

    _T("ARCHERY"),
    _T("OFFENCE"),
    _T("AIRSHIELD"),
    _T("SHIELD"),
    _T("ARMORER"),
    _T("RANGEATTACK"),
    _T("MELEEATTACK"),
    _T("RESIST"),

    _T("HITS"),
    _T("SPEED"),
    _T("MORALE"),
    _T("LUCK"),
    _T("COUNTERSTRIKE"),

    _T("MIN_GOLD"),
    _T("MIN_ORE"),
    _T("MIN_WOOD"),
    _T("MIN_MERCURY"),
    _T("MIN_GEMS"),
    _T("MIN_CRYSTAL"),
    _T("MIN_SULFUR"),

    _T("SPELLDMG"),
    _T("FEAR"),
	_T("SEAFARING")
};

LPCTSTR PRSKILL_ID[PRSKILL_COUNT] = {
    _T("ATTACK"),
    _T("DEFENCE"),
    _T("POWER"),
    _T("KNOWLEDGE")
};

LPCTSTR SECSKILL_ID[SECSK_COUNT] = {
    _T("ESTATES"),
    _T("LEADERSHIP"),
    _T("LUCK"),
    _T("DIPLOMACY"),
    _T("AIRMAGIC"),
    _T("EARTHMAGIC"),
    _T("FIREMAGIC"),
    _T("WATERMAGIC"),
    _T("WISDOM"),
    _T("NECROMANCY"),
    _T("MYSTICISM"),
    _T("INTELLIGENCE"),
    _T("RESISTANCE"),
    _T("SORCERY"),
    _T("LEARNING"),
    _T("SCOUTING"),
    _T("LOGISTICS"),
    _T("PATHFINDING"),
    _T("ARCHERY"),
    _T("BALLISTICS"),
    _T("OFFENCE"),
    _T("ARMORER")
};

LPCTSTR PRSKILL_NAMEKEY[PRSKILL_COUNT] = {
    _T("SKILL_ATTACK"),
    _T("SKILL_DEFENCE"),
    _T("SKILL_SPOWER"),
    _T("SKILL_KNOWLEDGE")
};

LPCTSTR SECSK_NAMEKEY[SECSK_COUNT] = {
    _T("HSKILL_ESTATES"),
    _T("HSKILL_LEADERSHIP"),
    _T("HSKILL_LUCK"),
    _T("HSKILL_DIPLOMACY"),
    _T("HSKILL_AIRMAGIC"),
    _T("HSKILL_EARTHMAGIC"),
    _T("HSKILL_FIREMAGIC"),
    _T("HSKILL_WATERMAGIC"),
    _T("HSKILL_WISDOM"),
    _T("HSKILL_NECROMANCY"),
    _T("HSKILL_MYSTICISM"),
    _T("HSKILL_INTELLIGENCE"),
    _T("HSKILL_RESISTANCE"),
    _T("HSKILL_SORCERY"),
    _T("HSKILL_LEARNING"),
    _T("HSKILL_SCOUTING"),
    _T("HSKILL_LOGISTICS"),
    _T("HSKILL_PATHFINDING"),
    _T("HSKILL_ARCHERY"),
    _T("HSKILL_BALLISTICS"),
    _T("HSKILL_OFFENCE"),
    _T("HSKILL_ARMORER")
};

LPCTSTR SSLVL_NAMEKEY[SSLVL_COUNT] = {
    _T("HSKILL_LEVEL_BASIC"),
    _T("HSKILL_LEVEL_ADVANCED"),
    _T("HSKILL_LEVEL_EXPERT"),
};

LPCTSTR SPELL_NAMEKEY[MSP_COUNT] = {
    _T("SPNAME_MAGICARROW"),
    _T("SPNAME_PROTEARTH"),
    _T("SPNAME_HASTE"),
    _T("SPNAME_SHIELD"),
    _T("SPNAME_DISRAY"),
    _T("SPNAME_LIGHTNINGBOLT"),
    _T("SPNAME_PRECISION"),
    _T("SPNAME_AIRSHIELD"),
    _T("SPNAME_HOLYWORD"),
    _T("SPNAME_COUNTERSTRIKE"),
    _T("SPNAME_RESURRECT"),
    _T("SPNAME_AIRELEMENTAL"),

    _T("SPNAME_PROTAIR"),
    _T("SPNAME_SLOW"),
    _T("SPNAME_STONESKIN"),
    _T("SPNAME_VISIONS"),
    _T("SPNAME_EARTHQUAKE"),
    _T("SPNAME_SORROW"),
    _T("SPNAME_METEORSHOWER"),
    _T("SPNAME_TOWNPORTAL"),
    _T("SPNAME_IMPLOSION"),
    _T("SPNAME_EARTHELEMENTAL"),

    _T("SPNAME_BLOODLUST"),
    _T("SPNAME_PROTWATER"),
    _T("SPNAME_CURSE"),
    _T("SPNAME_BLIND"),
    _T("SPNAME_WEAKNESS"),
    _T("SPNAME_DEATHRIPPLE"),
    _T("SPNAME_FIREBALL"),
    _T("SPNAME_MISFORTUNE"),
    _T("SPNAME_ANIMATEDEAD"),
    _T("SPNAME_FIREBLAST"),
    _T("SPNAME_ARMAGEDDON"),
    _T("SPNAME_FIREELEMENTAL"),

    _T("SPNAME_BLESS"),
    _T("SPNAME_PROTFIRE"),
    _T("SPNAME_DISPEL"),
    _T("SPNAME_CURE"),
    _T("SPNAME_COLDRAY"),
    _T("SPNAME_FORTUNE"),
    _T("SPNAME_MIRTH"),
    _T("SPNAME_COLDRING"),
    _T("SPNAME_ANTIMAGIC"),
    _T("SPNAME_PRAYER"),
    _T("SPNAME_WATERELEMENTAL"),

    _T("SPNAME_SUMMONSPRITES")
};

LPCTSTR MINERAL_ID[MINERAL_COUNT] = {
    _T("GOLD"),
    _T("ORE"),
    _T("WOOD"),
    _T("MERCURY"),
    _T("GEMS"),
    _T("CRYSTAL"),
    _T("SULFUR")
};

LPCTSTR MINERAL_NAMEKEY[MINERAL_COUNT] = {
    _T("MINERAL_GOLD"),
    _T("MINERAL_ORE"),
    _T("MINERAL_WOOD"),
    _T("MINERAL_MERCURY"),
    _T("MINERAL_GEM"),
    _T("MINERAL_CRYSTAL"),
    _T("MINERAL_SULFUR")
};

LPCTSTR RESOURCE_NAMEKEY[MINERAL_COUNT] = {
    _T("MAPRES_GOLD"),
    _T("MAPRES_ORE"),
    _T("MAPRES_WOOD"),
    _T("MAPRES_MERCURY"),
    _T("MAPRES_GEM"),
    _T("MAPRES_CRYSTAL"),
    _T("MAPRES_SULFUR")
};

LPCTSTR RAND_GUARDS_NAMEKEY[7] = {
    _T("RAND_GUARD"),
    _T("RAND_GUARD1"),
    _T("RAND_GUARD2"),
    _T("RAND_GUARD3"),
    _T("RAND_GUARD4"),
    _T("RAND_GUARD5"),
    _T("RAND_GUARD6")
};

LPCTSTR ART_LEVEL_ID[ART_LEVEL_COUNT] = {
    _T("NONE"),
    _T("TREASURE"),
    _T("MINOR"),
    _T("MAJOR"),
    _T("RELICT")
};

LPCTSTR CREAT_ID[CREAT_COUNT] = {
    _T("PEASANT"), _T("ARCHER"), _T("PIKEMAN"), _T("MONK"), _T("CAVALRY"), _T("PALADIN"),
    _T("GOBLIN"), _T("ORC"), _T("WARGRIDER"), _T("OGRE"), _T("TROLL"), _T("CYCLOP"),
    _T("YOUNGMAGE"), _T("WHITEWOLF"), _T("LIVINGARMOR"), _T("PEGASUS"), _T("MAGE"), _T("THOR"),
    _T("CENTAUR"), _T("GARGOYLE"), _T("GRIFFIN"),_T("MINOTAUR"), _T("HYDRA"), _T("REDDRAGON"),
    _T("SPRITE"), _T("DWARF"), _T("ELF"), _T("DRUID"), _T("UNICORN"), _T("FIREBIRD"),
    _T("SKELETON"), _T("ZOMBIE"), _T("LICH"), _T("VAMPIRE"), _T("BLACKKNIGHT"), _T("PLAGUE"),
    // Variags
    _T("HUNTER"),
    _T("BLACKSMITH"),
    _T("MERCENARY"),
    _T("WITCH"),
    _T("MAGI"),
    _T("VITYAZ"),

    // updates1                        // updates2                      
    /* Knight */                       /* Knight */                     
    _T("MILITIA"),                     _T("CITIZEN"),                   
    _T("CROSSBOWMAN"),                 _T("SHARPSHOOTER"),              
    _T("HEAVY_PIKEMAN"),               _T("HALBERDIER"),                
    _T("EXORCIST"),                    _T("BISHOP"),                    
    _T("HEAVY_CAVALRY"),               _T("KNIGHT"),                    
    _T("CRUSADER"),                    _T("HOLY_WARRIOR"),              
                                                                        
    /* Barbarian */                    /* Barbarian */                  
    _T("CONTAGIOUS_GOBLIN"),           _T("GOBLIN_WARRIOR"),            
    _T("ORC_CHIEF"),                   _T("ORC_WARRIOR"),               
    _T("WILD_WARG"),                   _T("WARG_WARRIOR"),              
    _T("OGRE_CHIEF"),                  _T("OGRE_SHAMAN"),               
    _T("BALLISTA_TROLL"),              _T("TROLL_SHAMAN"),              
    _T("ANCIENT_CYCLOPS"),             _T("DIRE_CYCLOPS"),              
                                                                        
    /* Wizard */                       /* Wizard */                     
    _T("NEOPHYTE"),                    _T("MAGE_APPRENTICE"),           
    _T("RED_WOLF"),                    _T("TWO_HEADED_WOLF"),           
    _T("STEEL_GUARD"),                 _T("WAR_MACHINE"),               
    _T("ROYAL_PEGASUS"),               _T("BLINDING_PEGASUS"),          
    _T("AIR_MAGE"),                    _T("PROPHET"),                   
    _T("LEGEND"),                      _T("DEMIGOD"),                   
                                                                        
    /* Warlock */                      /* Warlock */                    
    _T("CENTAUR_RANGER"),              _T("CENTAUR_LORD"),              
    _T("OBSIDIAN_GARGOYLE"),           _T("STONE_GARGOYLE"),            
    _T("SNAKE_GRIFFIN"),               _T("ROYAL_GRIFFIN"),             
    _T("MINOTAUR_REAPER"),             _T("DIRE_MINOTAUR"),             
    _T("POISONOUS_HYDRA"),             _T("ANCIENT_HYDRA"),             
    _T("FIRE_DRAGON"),                 _T("BLACK_DRAGON"),              
                                                                        
    /* Sorcer */                       /* Sorcer */                     
    _T("ELUSIVE_SPRITE"),              _T("POISONOUS_SPRITE"),          
    _T("AXE_HURLER"),                  _T("BERZERKER"),                 
    _T("ELF_RANGER"),                  _T("ELF_HUNTER"),                
    _T("GRAND_DRUID"),                 _T("OUTCAST"),                   
    _T("ROYAL_UNICORN"),               _T("GHOST_UNICORN"),             
    _T("PHOENIX"),                     _T("MAGIC_FIREBIRD"),            
                                                                        
    /* Necromant */                    /* Necromant */                  
    _T("SKELETON_ARCHER"),             _T("SKELETON_WARRIOR"),          
    _T("CONTAGIOUS_ZOMBIE"),           _T("TOUGH_ZOMBIE"),              
    _T("ARCHLICH"),                    _T("POWERFUL_LICH"),             
    _T("NOSFERATU"),                   _T("VAMPIRE_LORD"),              
    _T("DEATH_KNIGHT"),                _T("BLACK_GRAIL_KNIGHT"),        
    _T("DEATH"),                       _T("FEAR"),                      
                                                                        
    /* Variags */                      /* Variags */                    
    _T("OLD_HUNTER"),                  _T("MATURE_HUNTER"),             
    _T("NOBLE_BLACKSMITH"),            _T("WEREWOLF"),                  
    _T("BOYARIN"),                     _T("ROYAL_GUARD"),               
    _T("ENCHANTER"),                   _T("YAGA"),                      
    _T("LESHIY"),                      _T("GREAT_MAGI"),              
    _T("BOGATYR"),                     _T("VOLOT"),                     

    // neutrals
    _T("ROGUE"), 
    _T("NOMAD"), 
    _T("GHOST"), 
    _T("GENIE"), 
    _T("MEDUSA"), 
    _T("EARTHELEMENTAL"), 
    _T("AIRELEMENTAL"), 
    _T("FIREELEMENTAL"), 
    _T("WATERELEMENTAL"),    
    _T("TATAR_ARCHER"),
    _T("GORYNYCH")

};

LPCTSTR CREAT_NAMEKEY[CREAT_COUNT] = 
{
    _T("CREATURE_PEASANT_F1"),
    _T("CREATURE_ARCHER_F1"),
    _T("CREATURE_PIKEMAN_F1"),
    _T("CREATURE_MONK_F1"),
    _T("CREATURE_CAVALRY_F1"),
    _T("CREATURE_PALADIN_F1"),

    _T("CREATURE_GOBLIN_F1"),
    _T("CREATURE_ORC_F1"),
    _T("CREATURE_WARG_RIDER_F1"),
    _T("CREATURE_OGRE_F1"),
    _T("CREATURE_TROLL_F1"),
    _T("CREATURE_CYCLOP_F1"),

    _T("CREATURE_YOUNG_MAGE_F1"),
    _T("CREATURE_WHITE_WOLF_F1"),
    _T("CREATURE_LIVING_ARMOR_F1"),
    _T("CREATURE_PEGASUS_F1"),
    _T("CREATURE_MAGE_F1"),
    _T("CREATURE_THOR_F1"),

    _T("CREATURE_CENTAUR_F1"),
    _T("CREATURE_GARGOYLE_F1"),
    _T("CREATURE_GRIFFIN_F1"),
    _T("CREATURE_MINOTAUR_F1"),
    _T("CREATURE_HYDRA_F1"),
    _T("CREATURE_REDDRAGON_F1"),

    _T("CREATURE_SPRITE_F1"),
    _T("CREATURE_DWARF_F1"),
    _T("CREATURE_ELF_F1"),
    _T("CREATURE_DRUID_F1"),
    _T("CREATURE_UNICORN_F1"),
    _T("CREATURE_FIREBIRD_F1"),

    _T("CREATURE_SKELETON_F1"),
    _T("CREATURE_ZOMBIE_F1"),
    _T("CREATURE_LICH_F1"),
    _T("CREATURE_VAMPIRE_F1"),
    _T("CREATURE_BLACK_KNIGHT_F1"),
    _T("CREATURE_PLAGUE_F1"),

    _T("CREATURE_HUNTER_F1"),
    _T("CREATURE_BLACKSMITH_F1"),
    _T("CREATURE_MERCENARY_F1"),
    _T("CREATURE_WITCH_F1"),
    _T("CREATURE_MAGI_F1"),
    _T("CREATURE_VITYAZ_F1"),

    /////////////////////////////////////////////////////////////////////////////////
    // upgrade 1                            // Update 2                            
    /////////////////////////////////////////////////////////////////////////////////
    /* Knight */                            /* Knight */                                                                    
    _T("CREATURE_MILITIA_F1"),              _T("CREATURE_CITIZEN_F1"),               
    _T("CREATURE_CROSSBOWMAN_F1"),          _T("CREATURE_SHARPSHOOTER_F1"),          
    _T("CREATURE_HEAVY_PIKEMAN_F1"),        _T("CREATURE_HALBERDIER_F1"),            
    _T("CREATURE_EXORCIST_F1"),             _T("CREATURE_BISHOP_F1"),                
    _T("CREATURE_HEAVY_CAVALRY_F1"),        _T("CREATURE_KNIGHT_F1"),                
    _T("CREATURE_CRUSADER_F1"),             _T("CREATURE_HOLY_WARRIOR_F1"),          
                                                                                     
    /* Barbarian */                         /* Barbarian */                          
    _T("CREATURE_CONTAGIOUS_GOBLIN_F1"),    _T("CREATURE_GOBLIN_WARRIOR_F1"),        
    _T("CREATURE_ORC_CHIEF_F1"),            _T("CREATURE_ORC_WARRIOR_F1"),           
    _T("CREATURE_WILD_WARG_F1"),            _T("CREATURE_WARG_WARRIOR_F1"),          
    _T("CREATURE_OGRE_CHIEF_F1"),           _T("CREATURE_OGRE_SHAMAN_F1"),           
    _T("CREATURE_BALLISTA_TROLL_F1"),       _T("CREATURE_TROLL_SHAMAN_F1"),          
    _T("CREATURE_ANCIENT_CYCLOPS_F1"),      _T("CREATURE_DIRE_CYCLOPS_F1"),          
                                                                                     
    /* Wizard */                            /* Wizard */                             
    _T("CREATURE_NEOPHYTE_F1"),             _T("CREATURE_MAGE_APPRENTICE_F1"),       
    _T("CREATURE_RED_WOLF_F1"),             _T("CREATURE_TWO_HEADED_WOLF_F1"),       
    _T("CREATURE_STEEL_GUARD_F1"),          _T("CREATURE_WAR_MACHINE_F1"),           
    _T("CREATURE_ROYAL_PEGASUS_F1"),        _T("CREATURE_BLINDING_PEGASUS_F1"),      
    _T("CREATURE_AIR_MAGE_F1"),             _T("CREATURE_PROPHET_F1"),               
    _T("CREATURE_LEGEND_F1"),               _T("CREATURE_DEMIGOD_F1"),               
                                                                                     
    /* Warlock */                           /* Warlock */                            
    _T("CREATURE_CENTAUR_RANGER_F1"),       _T("CREATURE_CENTAUR_LORD_F1"),          
    _T("CREATURE_OBSIDIAN_GARGOYLE_F1"),    _T("CREATURE_STONE_GARGOYLE_F1"),        
    _T("CREATURE_SNAKE_GRIFFIN_F1"),        _T("CREATURE_ROYAL_GRIFFIN_F1"),         
    _T("CREATURE_MINOTAUR_REAPER_F1"),      _T("CREATURE_DIRE_MINOTAUR_F1"),         
    _T("CREATURE_POISONOUS_HYDRA_F1"),      _T("CREATURE_ANCIENT_HYDRA_F1"),         
    _T("CREATURE_FIRE_DRAGON_F1"),          _T("CREATURE_BLACK_DRAGON_F1"),          
                                                                                     
    /* Sorcer */                            /* Sorcer */                             
    _T("CREATURE_ELUSIVE_SPRITE_F1"),       _T("CREATURE_POISONOUS_SPRITE_F1"),      
    _T("CREATURE_AXE_HURLER_F1"),           _T("CREATURE_BERZERKER_F1"),             
    _T("CREATURE_ELF_RANGER_F1"),           _T("CREATURE_ELF_HUNTER_F1"),            
    _T("CREATURE_GRAND_DRUID_F1"),          _T("CREATURE_OUTCAST_F1"),               
    _T("CREATURE_ROYAL_UNICORN_F1"),        _T("CREATURE_GHOST_UNICORN_F1"),         
    _T("CREATURE_PHOENIX_F1"),              _T("CREATURE_MAGIC_FIREBIRD_F1"),        
                                                                                     
    /* Necromant */                         /* Necromant */                          
    _T("CREATURE_SKELETON_ARCHER_F1"),      _T("CREATURE_SKELETON_WARRIOR_F1"),      
    _T("CREATURE_CONTAGIOUS_ZOMBIE_F1"),    _T("CREATURE_TOUGH_ZOMBIE_F1"),          
    _T("CREATURE_ARCHLICH_F1"),             _T("CREATURE_POWERFUL_LICH_F1"),         
    _T("CREATURE_NOSFERATU_F1"),            _T("CREATURE_VAMPIRE_LORD_F1"),          
    _T("CREATURE_DEATH_KNIGHT_F1"),         _T("CREATURE_BLACK_GRAIL_KNIGHT_F1"),    
    _T("CREATURE_DEATH_F1"),                _T("CREATURE_FEAR_F1"),                  
                                                                                     
    /* Variags */                           /* Variags */                            
    _T("CREATURE_OLD_HUNTER_F1"),           _T("CREATURE_MATURE_HUNTER_F1"),         
    _T("CREATURE_NOBLE_BLACKSMITH_F1"),     _T("CREATURE_WEREWOLF_F1"),              
    _T("CREATURE_BOYARIN_F1"),              _T("CREATURE_ROYAL_GUARD_F1"),           
    _T("CREATURE_ENCHANTER_F1"),            _T("CREATURE_YAGA_F1"),                  
    _T("CREATURE_LESHIY_F1"),               _T("CREATURE_GREAT_MAGI_F1"),          
    _T("CREATURE_BOGATYR_F1"),              _T("CREATURE_VOLOT_F1"),                 
                                              

    //////////////////////////////////////////////////////////////////////////
    // Neutral
    //////////////////////////////////////////////////////////////////////////
    _T("CREATURE_ROGUE_F1"),
    _T("CREATURE_NOMAD_F1"),
    _T("CREATURE_GHOST_F1"),
    _T("CREATURE_GENIE_F1"),
    _T("CREATURE_MEDUSA_F1"),
    _T("CREATURE_EARTHELEMENTAL_F1"), 
    _T("CREATURE_AIRELEMENTAL_F1"), 
    _T("CREATURE_FIREELEMENTAL_F1"), 
    _T("CREATURE_WATERELEMENTAL_F1"),
    _T("CREATURE_TATAR_ARCHER_F1"),
    _T("CREATURE_GORYNYCH_F1")
};

LPCTSTR MAGIC_SPELL_ID[MSP_COUNT] = {
    _T("MAGICARROW"),
    _T("PROTEARTH"),
    _T("HASTE"),
    _T("SHIELD"),
    _T("DISRAY"),
    _T("LIGHTNINGBOLT"),
    _T("PRECISION"),
    _T("AIRSHIELD"),
    _T("HOLYWORD"),
    _T("COUNTERSTRIKE"),
    _T("RESURRECT"),
    _T("AIRELEMENTAL"),
    _T("PROTAIR"),
    _T("SLOW"),
    _T("STONESKIN"),
    _T("VISIONS"),
    _T("EARTHQUAKE"),
    _T("SORROW"),
    _T("METEORSHOWER"),
    _T("TOWNPORTAL"),
    _T("IMPLOSION"),
    _T("EARTHELEMENTAL"),
    _T("BLOODLUST"),
    _T("PROTWATER"),
    _T("CURSE"),
    _T("BLIND"),
    _T("WEAKNESS"),
    _T("DEATHRIPPLE"),
    _T("FIREBALL"),
    _T("MISFORTUNE"),
    _T("ANIMATEDEAD"),
    _T("FIREBLAST"),
    _T("ARMAGEDDON"),
    _T("FIREELEMENTAL"),
    _T("BLESS"),
    _T("PROTFIRE"),
    _T("DISPEL"),
    _T("CURE"),
    _T("COLDRAY"),
    _T("FORTUNE"),
    _T("MIRTH"),
    _T("COLDRING"),
    _T("ANTIMAGIC"),
    _T("PRAYER"),
    _T("WATERELEMENTAL"),
    _T("SUMMONSPRITES")
};

LPCTSTR MAGIC_SPELL_NAMEKEY[MSP_COUNT] = {
    _T("SPNAME_MAGICARROW"),
    _T("SPNAME_PROTEARTH"),
    _T("SPNAME_HASTE"),
    _T("SPNAME_SHIELD"),
    _T("SPNAME_DISRAY"),
    _T("SPNAME_LIGHTNINGBOLT"),
    _T("SPNAME_PRECISION"),
    _T("SPNAME_AIRSHIELD"),
    _T("SPNAME_HOLYWORD"),
    _T("SPNAME_COUNTERSTRIKE"),
    _T("SPNAME_RESURRECT"),
    _T("SPNAME_AIRELEMENTAL"),
    _T("SPNAME_PROTAIR"),
    _T("SPNAME_SLOW"),
    _T("SPNAME_STONESKIN"),
    _T("SPNAME_VISIONS"),
    _T("SPNAME_EARTHQUAKE"),
    _T("SPNAME_SORROW"),
    _T("SPNAME_METEORSHOWER"),
    _T("SPNAME_TOWNPORTAL"),
    _T("SPNAME_IMPLOSION"),
    _T("SPNAME_EARTHELEMENTAL"),
    _T("SPNAME_BLOODLUST"),
    _T("SPNAME_PROTWATER"),
    _T("SPNAME_CURSE"),
    _T("SPNAME_BLIND"),
    _T("SPNAME_WEAKNESS"),
    _T("SPNAME_DEATHRIPPLE"),
    _T("SPNAME_FIREBALL"),
    _T("SPNAME_MISFORTUNE"),
    _T("SPNAME_ANIMATEDEAD"),
    _T("SPNAME_FIREBLAST"),
    _T("SPNAME_ARMAGEDDON"),
    _T("SPNAME_FIREELEMENTAL"),
    _T("SPNAME_BLESS"),
    _T("SPNAME_PROTFIRE"),
    _T("SPNAME_DISPEL"),
    _T("SPNAME_CURE"),
    _T("SPNAME_COLDRAY"),
    _T("SPNAME_FORTUNE"),
    _T("SPNAME_MIRTH"),
    _T("SPNAME_COLDRING"),
    _T("SPNAME_ANTIMAGIC"),
    _T("SPNAME_PRAYER"),
    _T("SPNAME_WATERELEMENTAL"),
    _T("SPNAME_SUMMONSPRITES")
};


LPCTSTR REWARD_TYPE_ID[RIT_COUNT] = {
    _T("MINERAL"),
    _T("EXPERIENCE"),
    _T("MANAPTS"),  
    _T("TRAVELPTS"),    
    _T("MORALE"),
    _T("LUCK"), 
    _T("FURTSKILL"),    
    _T("ARTIFACT"), 
    _T("MAGICSPELL"),   
    _T("CREATGROUP")
};

LPCTSTR MAGSCHOOL_ID[MST_COUNT] = {
    _T("AIR"),
    _T("EARTH"),
    _T("FIRE"),
    _T("WATER")     
};

LPCTSTR CREAT_BONUS_ID[CBT_COUNT] = {
    _T("CREAT_ATTACK"),
    _T("CREAT_DEFENCE"),
    _T("CREAT_SPEED")

};

LPCTSTR COLOR_KEYMASTER[6] = {

	_T("tent_red"),		//red
	_T("tent_green"),	//green
	_T("tent_blue"),	//blue
	_T("tent_cyan"),	//cyan
	_T("tent_magenta"),	//magenta
	_T("tent_yellow")	//yellow
};

LPCTSTR COLOR_KEYMASTER_SNOW[6] = {

	_T("tent_red_snow"),	//red
	_T("tent_green_snow"),	//green
	_T("tent_blue_snow"),	//blue
	_T("tent_cyan_snow"),	//cyan
	_T("tent_magenta_snow"),//magenta
	_T("tent_yellow_snow")	//yellow
};

uint32 COLOR_KEYMASTER_CODE[6] = {

	0xFF0000,	//red
	0x05e201,   //green
	0x0000FF,	//blue
	0x00BFFF,	//cyan
	0xFF00FF,	//magenta
	0xFFFF00	//yellow
};