#include "stdafx.h"

#include "common.h"
#include "cm_creatures.h"
#include "cm_magic.h"

#include "cm_castles.h"

const uint32 CTL_HEROES[CTLT_COUNT] = {
	HTM_KNIGHT,
	HTM_BARBARIAN,
	HTM_WIZARD,
	HTM_WARLOCK,
	HTM_SORCERESS,
	HTM_NECROMANCER,
	HTM_VARIAG
};

#if !defined(_HMM_GAME_) || defined(ROYAL_BOUNTY)
#define SPRITE(name) 0
#else
#define SPRITE(name) PDGG_SINT16(name)
#endif 


/// an association between castle constructions and their graphical implementation
/// -1 on the second place means draw always, -1 on the first place means end of the list
sint16 CTL_GFX_INDEX[CTLT_COUNT][27][2];

void init_castle_cnsts()
{
    static sint16 temp[CTLT_COUNT][27][2] = {
        
        // citadel
        {
            {SPRITE(CTL_CITADEL_ORE_MINE), CTLCNST_OREMINE},
            {SPRITE(CTL_CITADEL_SAWMILL), CTLCNST_SAWMILL},
            {SPRITE(CTL_CITADEL_MARKETPLACE), CTLCNST_MARKETPLACE},
            {SPRITE(CTL_CITADEL_WATCH_TOWER), CTLCNST_OBSERVPOST},
            {SPRITE(CTL_CITADEL_ARCHERY_TOWER), CTLCNST_DWELL_ARCHTOWER},
            {SPRITE(CTL_CITADEL_SHOOTING_RANGE), CTLCNST_SHOOTINGRANGE},
            {SPRITE(CTL_CITADEL_PEASANT_HUT), CTLCNST_DWELL_PEASNHUT},
            {SPRITE(CTL_CITADEL_GUARDHOUSE), CTLCNST_DWELL_GHOUSE},
            {SPRITE(CTL_CITADEL_TAVERN), CTLCNST_TAVERN},
            {SPRITE(CTL_CITADEL_MAGE1), CTLCNST_MAGEGUILD_L1},
            {SPRITE(CTL_CITADEL_MAGE2), CTLCNST_MAGEGUILD_L2},
            {SPRITE(CTL_CITADEL_MAGE3), CTLCNST_MAGEGUILD_L3},
            {SPRITE(CTL_CITADEL_CATHEDRAL), CTLCNST_DWELL_CATHEDRAL},
            {SPRITE(CTL_CITADEL_ARENA), CTLCNST_DWELL_JARENA},
            {SPRITE(CTL_CITADEL_ABBEY), CTLCNST_DWELL_ABBEY},
            {SPRITE(CTL_CITADEL_CRYSTAL_MINE), CTLCNST_CRYSTALMINE},
            {SPRITE(CTL_CITADEL_MOAT), CTLCNST_MOAT},
            {SPRITE(CTL_CITADEL_LTURRET), CTLCNST_LTURRET},
            {SPRITE(CTL_CITADEL_MTURRET), CTLCNST_MTURRET},
            {SPRITE(CTL_CITADEL_RTURRET), CTLCNST_RTURRET},
            {SPRITE(CTL_CITADEL_FORTIFICATION), CTLCNST_FORTIFICATION},
            {SPRITE(CTL_CITADEL_WALL), -1},
            {SPRITE(CTL_CITADEL_CITY_HALL), CTLCNST_CITYHALL},
            {SPRITE(CTL_CITADEL_TOWN_HALL), CTLCNST_TOWNHALL},
            {SPRITE(CTL_CITADEL_BACKGROUND), -1},
            {-1, -1}
        },
        
        // Stronghold
        {
            {SPRITE(CTL_STRONGHOLD_MOAT), CTLCNST_MOAT},
            {SPRITE(CTL_STRONGHOLD_GOBLIN_BARRACKS), CTLCNST_DWELL_GBARRACKS},
            {SPRITE(CTL_STRONGHOLD_ORC_TOWER), CTLCNST_DWELL_ORCTOWER},
            {SPRITE(CTL_STRONGHOLD_TAVERN), CTLCNST_TAVERN},
            {SPRITE(CTL_STRONGHOLD_TROLL_BRIDGE), CTLCNST_DWELL_TRBRIDGE},
            {SPRITE(CTL_STRONGHOLD_MTURRET), CTLCNST_MTURRET},
            {SPRITE(CTL_STRONGHOLD_WALL), -1},
            {SPRITE(CTL_STRONGHOLD_RTURRET), CTLCNST_RTURRET},
            {SPRITE(CTL_STRONGHOLD_LTURRET), CTLCNST_LTURRET},
            {SPRITE(CTL_STRONGHOLD_MAGE3), CTLCNST_MAGEGUILD_L3},
            {SPRITE(CTL_STRONGHOLD_MAGE2), CTLCNST_MAGEGUILD_L2},
            {SPRITE(CTL_STRONGHOLD_MAGE1), CTLCNST_MAGEGUILD_L1},
            {SPRITE(CTL_STRONGHOLD_MARKETPLACE), CTLCNST_MARKETPLACE},
            {SPRITE(CTL_STRONGHOLD_CRYSTAL_MINE), CTLCNST_CRYSTALMINE},
            {SPRITE(CTL_STRONGHOLD_SAWMILL), CTLCNST_SAWMILL},
            {SPRITE(CTL_STRONGHOLD_ORE_MINE), CTLCNST_OREMINE},
            {SPRITE(CTL_STRONGHOLD_ADOBE), CTLCNST_ADOBE},
            {SPRITE(CTL_STRONGHOLD_OGRE_FORT), CTLCNST_DWELL_OGREFORT},
            {SPRITE(CTL_STRONGHOLD_PYRAMID), CTLCNST_DWELL_PYRAMID},
            {SPRITE(CTL_STRONGHOLD_VALHALLA), CTLCNST_HALLOFVALHALLA},
            {SPRITE(CTL_STRONGHOLD_CITY_HALL), CTLCNST_CITYHALL},
            {SPRITE(CTL_STRONGHOLD_TOWN_HALL), CTLCNST_TOWNHALL},
            {SPRITE(CTL_STRONGHOLD_MESS_HALL), CTLCNST_MESSHALL},
            {SPRITE(CTL_STRONGHOLD_WARG_CAVE), CTLCNST_DWELL_WARGCAVE},
            {SPRITE(CTL_STRONGHOLD_BACKGROUND), -1},
            {-1, -1}
        },
        
        // Tower
        {
            {SPRITE(CTL_TOWER_ORE_MINE), CTLCNST_OREMINE},
            {SPRITE(CTL_TOWER_GEMS_MINE), CTLCNST_GEMSMINE},
            {SPRITE(CTL_TOWER_CAVERN), CTLCNST_CAVERN},
            {SPRITE(CTL_TOWER_WHITE_WOLF_DEN), CTLCNST_DWELL_WHITEWOLFDEN},
            {SPRITE(CTL_TOWER_MAGIC_SCHOOL), CTLCNST_DWELL_MAGICSCHOOL},
            {SPRITE(CTL_TOWER_CLOUD_CASTLE), CTLCNST_DWELL_CLDPALACE},
            {SPRITE(CTL_TOWER_SAWMILL), CTLCNST_SAWMILL},
            {SPRITE(CTL_TOWER_MARKETPLACE), CTLCNST_MARKETPLACE},
            {SPRITE(CTL_TOWER_WALL_OF_KNOWLEDGE), CTLCNST_WALLOFKNOWLEDGE},
            {SPRITE(CTL_TOWER_LIBRARY), CTLCNST_LIBRARY},
            {SPRITE(CTL_TOWER_MAGE5), CTLCNST_MAGEGUILD_L5},
            {SPRITE(CTL_TOWER_MAGE4), CTLCNST_MAGEGUILD_L4},
            {SPRITE(CTL_TOWER_MAGE3), CTLCNST_MAGEGUILD_L3},
            {SPRITE(CTL_TOWER_MAGE2), CTLCNST_MAGEGUILD_L2},
            {SPRITE(CTL_TOWER_MAGE1), CTLCNST_MAGEGUILD_L1},
            {SPRITE(CTL_TOWER_MAGIC_NODE), CTLCNST_MAGICNODE},
            {SPRITE(CTL_TOWER_CITY_HALL), CTLCNST_CITYHALL},
            {SPRITE(CTL_TOWER_TOWN_HALL), CTLCNST_TOWNHALL},
            {SPRITE(CTL_TOWER_MTURRET), CTLCNST_MTURRET},
            {SPRITE(CTL_TOWER_MAGE_TOWER), CTLCNST_DWELL_MAGETOWER},
            {SPRITE(CTL_TOWER_TAVERN), CTLCNST_TAVERN},
            {SPRITE(CTL_TOWER_FACTORY), CTLCNST_DWELL_FACTORY},
            {SPRITE(CTL_TOWER_MAGIC_CLOUD), CTLCNST_DWELL_MAGICCLOUD},
            {SPRITE(CTL_TOWER_MOAT), CTLCNST_MOAT},
            {SPRITE(CTL_TOWER_BACKGROUND), -1},
            {-1, -1}
        },
        
        // dungeon
        {
            {SPRITE(CTL_DUNGEON_MAGIC_NODE), CTLCNST_MAGICNODE},
            {SPRITE(CTL_DUNGEON_MARKETPLACE), CTLCNST_MARKETPLACE},
            {SPRITE(CTL_DUNGEON_TAVERN), CTLCNST_TAVERN},
            {SPRITE(CTL_DUNGEON_MAGE5), CTLCNST_MAGEGUILD_L5},
            {SPRITE(CTL_DUNGEON_MAGE4), CTLCNST_MAGEGUILD_L4},
            {SPRITE(CTL_DUNGEON_MAGE3), CTLCNST_MAGEGUILD_L3},
            {SPRITE(CTL_DUNGEON_MAGE2), CTLCNST_MAGEGUILD_L2},
            {SPRITE(CTL_DUNGEON_MAGE1), CTLCNST_MAGEGUILD_L1},
            {SPRITE(CTL_DUNGEON_ALTAR), CTLCNST_ALTAR},
            {SPRITE(CTL_DUNGEON_ORE_MINE), CTLCNST_OREMINE},
            {SPRITE(CTL_DUNGEON_SULPHUR_MINE), CTLCNST_SULFURMINE},
            {SPRITE(CTL_DUNGEON_MTURRET), CTLCNST_MTURRET},
            {SPRITE(CTL_DUNGEON_CASTLE), -1},
            {SPRITE(CTL_DUNGEON_CRYPT), CTLCNST_DWELL_CRYPT},
            {SPRITE(CTL_DUNGEON_SAWMILL), CTLCNST_SAWMILL},
            {SPRITE(CTL_DUNGEON_DRAGON_CAVE), CTLCNST_DWELL_DRAGCAVE},
            {SPRITE(CTL_DUNGEON_HYDRA_POND), CTLCNST_DWELL_HYDRAPOND},
            {SPRITE(CTL_DUNGEON_MOAT), CTLCNST_MOAT},
            {SPRITE(CTL_DUNGEON_MANA_VORTEX), CTLCNST_MANAVORTEX},
            {SPRITE(CTL_DUNGEON_LABYRINTH), CTLCNST_DWELL_LABYRINTH},
            {SPRITE(CTL_DUNGEON_GRIFFIN_TOWER), CTLCNST_DWELL_GRIFTOWER},
            {SPRITE(CTL_DUNGEON_CITY_HALL), CTLCNST_CITYHALL},
            {SPRITE(CTL_DUNGEON_TOWN_HALL), CTLCNST_TOWNHALL},
            {SPRITE(CTL_DUNGEON_WATERFALL), CTLCNST_WATERFALL},
            {SPRITE(CTL_DUNGEON_CENTAUR_CAVE), CTLCNST_DWELL_CENTCAVE},
            {SPRITE(CTL_DUNGEON_BACKGROUND), -1},
            {-1, -1}
        },
        
        // fortress
        {
            {SPRITE(CTL_FORTRESS_MAGE5), CTLCNST_MAGEGUILD_L5},
            {SPRITE(CTL_FORTRESS_MAGE4), CTLCNST_MAGEGUILD_L4},
            {SPRITE(CTL_FORTRESS_MAGE3), CTLCNST_MAGEGUILD_L3},
            {SPRITE(CTL_FORTRESS_MAGE2), CTLCNST_MAGEGUILD_L2},
            {SPRITE(CTL_FORTRESS_MAGE1), CTLCNST_MAGEGUILD_L1},
            {SPRITE(CTL_FORTRESS_SAWMILL), CTLCNST_SAWMILL},
            {SPRITE(CTL_FORTRESS_RED_TOWER), CTLCNST_DWELL_REDTOWER},
            {SPRITE(CTL_FORTRESS_ORE_MINE), CTLCNST_OREMINE},
            {SPRITE(CTL_FORTRESS_ALCHLAB), CTLCNST_ALCHLAB},
            {SPRITE(CTL_FORTRESS_UNICORN_GLADE), CTLCNST_DWELL_UNGLADE},
            {SPRITE(CTL_FORTRESS_MYSTIC_POND), CTLCNST_MYSTICPOUND},
            {SPRITE(CTL_FORTRESS_MTURRET), CTLCNST_MTURRET},
            {SPRITE(CTL_FORTRESS_CASTLE), -1},
            {SPRITE(CTL_FORTRESS_MOAT), CTLCNST_MOAT},
            {SPRITE(CTL_FORTRESS_TAVERN), CTLCNST_TAVERN},
            {SPRITE(CTL_FORTRESS_TREEHOUSE), CTLCNST_DWELL_TREEHOUSE},
            {SPRITE(CTL_FORTRESS_HOMESTEAD), CTLCNST_DWELL_HOMESTEAD},
            {SPRITE(CTL_FORTRESS_TREASURY), CTLCNST_TREASURY},
            {SPRITE(CTL_FORTRESS_MINERS_GUILD), CTLCNST_MINERSGUILD},
            {SPRITE(CTL_FORTRESS_DWARF_COTTAGE), CTLCNST_DWELL_DWCOTTAGE},
            {SPRITE(CTL_FORTRESS_STONEHENGE), CTLCNST_DWELL_STONEHENGE},
            {SPRITE(CTL_FORTRESS_CITY_HALL), CTLCNST_CITYHALL},
            {SPRITE(CTL_FORTRESS_TOWN_HALL), CTLCNST_TOWNHALL},
            {SPRITE(CTL_FORTRESS_MARKETPLACE), CTLCNST_MARKETPLACE},
            {SPRITE(CTL_FORTRESS_TREE), -1},
            {SPRITE(CTL_FORTRESS_BACKGROUND), -1},
            {-1, -1}
        },
        
        // Necropolis
        {
            {SPRITE(CTL_NECROPOLIS_GRAVEYARD), CTLCNST_DWELL_GRAVEYARD},
            {SPRITE(CTL_NECROPOLIS_CURSED_TOWER), CTLCNST_DWELL_CURSEDTOWER},
            {SPRITE(CTL_NECROPOLIS_SARCOPHAGUS), CTLCNST_DWELL_SARCOPHAGUS},
            {SPRITE(CTL_NECROPOLIS_TAVERN), CTLCNST_TAVERN},
            {SPRITE(CTL_NECROPOLIS_TOWN_HALL), CTLCNST_TOWNHALL},
            {SPRITE(CTL_NECROPOLIS_CITY_HALL), CTLCNST_CITYHALL},
            {SPRITE(CTL_NECROPOLIS_MTURRET), CTLCNST_MTURRET},
            {SPRITE(CTL_NECROPOLIS_MAGE5), CTLCNST_MAGEGUILD_L5},
            {SPRITE(CTL_NECROPOLIS_MAGE4), CTLCNST_MAGEGUILD_L4},
            {SPRITE(CTL_NECROPOLIS_MAGE3), CTLCNST_MAGEGUILD_L3},
            {SPRITE(CTL_NECROPOLIS_MAGE2), CTLCNST_MAGEGUILD_L2},
            {SPRITE(CTL_NECROPOLIS_MAGE1), CTLCNST_MAGEGUILD_L1},
            {SPRITE(CTL_NECROPOLIS_ALCHLAB), CTLCNST_ALCHLAB},
            {SPRITE(CTL_NECROPOLIS_DARKTOMB), CTLCNST_DWELL_DARKTOMB},
            {SPRITE(CTL_NECROPOLIS_MAUSOLEUM), CTLCNST_DWELL_MAUSOLEUM},
            {SPRITE(CTL_NECROPOLIS_NECRAMPLIFIER), CTLCNST_NECRAMPLIFIER},
            {SPRITE(CTL_NECROPOLIS_MARKETPLACE), CTLCNST_MARKETPLACE},
            {SPRITE(CTL_NECROPOLIS_SAWMILL), CTLCNST_SAWMILL},
            {SPRITE(CTL_NECROPOLIS_COVEROFDARKNESS), CTLCNST_COVEROFDARKNESS},
            {SPRITE(CTL_NECROPOLIS_ORE_MINE), CTLCNST_OREMINE},
            {SPRITE(CTL_NECROPOLIS_UNEARTHED_GRAVES), CTLCNST_UNEARTHEDGRAVES},
            {SPRITE(CTL_NECROPOLIS_MOAT), CTLCNST_MOAT},
            {SPRITE(CTL_NECROPOLIS_EXCAVATION), CTLCNST_DWELL_EXCAVATION},
            {SPRITE(CTL_NECROPOLIS_BACKGROUND), -1},
            {-1, -1}
        },
        // Kremlin	
        {
            {SPRITE(CTL_KREMLIN_MAGE5), CTLCNST_MAGEGUILD_L5},  
            {SPRITE(CTL_KREMLIN_MAGE4), CTLCNST_MAGEGUILD_L4},  
            {SPRITE(CTL_KREMLIN_MAGE3), CTLCNST_MAGEGUILD_L3},  
            {SPRITE(CTL_KREMLIN_MAGE2), CTLCNST_MAGEGUILD_L2},  
            {SPRITE(CTL_KREMLIN_MAGE1), CTLCNST_MAGEGUILD_L1},  
            {SPRITE(CTL_KREMLIN_RESIDENCE), CTLCNST_DWELL_RESIDENCE},  
            {SPRITE(CTL_KREMLIN_TOWN_HALL), CTLCNST_TOWNHALL},  
            {SPRITE(CTL_KREMLIN_CITY_HALL), CTLCNST_CITYHALL},  
            {SPRITE(CTL_KREMLIN_MERCHANT_SHOP), CTLCNST_MERCHANT},  
            {SPRITE(CTL_KREMLIN_MERC_BARRACKS), CTLCNST_DWELL_MERC_BARRACKS},  
            {SPRITE(CTL_KREMLIN_MARKETPLACE), CTLCNST_MARKETPLACE},  
            {SPRITE(CTL_KREMLIN_SCOUT_TOWER), CTLCNST_SCOUT_TOWER},  
            {SPRITE(CTL_KREMLIN_MTURRET), CTLCNST_MTURRET},  
            {SPRITE(CTL_KREMLIN_MOAT), CTLCNST_MOAT},  
            {SPRITE(CTL_KREMLIN_CASTLE), -1},  
            {SPRITE(CTL_KREMLIN_FOREST_HUT), CTLCNST_DWELL_FOREST_HUT},  
            {SPRITE(CTL_KREMLIN_LODGE), CTLCNST_LODGE},  
            {SPRITE(CTL_KREMLIN_BRIDGE), -1},  
            {SPRITE(CTL_KREMLIN_SAWMILL), CTLCNST_SAWMILL},  
            {SPRITE(CTL_KREMLIN_SHRINE), CTLCNST_DWELL_SHRINE},  
            {SPRITE(CTL_KREMLIN_HUNTERS_CABIN), CTLCNST_DWELL_HUNTERS_CABIN},  
            {SPRITE(CTL_KREMLIN_GEMS_MINE), CTLCNST_GEMSMINE},  
            {SPRITE(CTL_KREMLIN_ORE_MINE), CTLCNST_OREMINE},  
            {SPRITE(CTL_KREMLIN_FORGE), CTLCNST_DWELL_FORGE},  		
            {SPRITE(CTL_KREMLIN_TAVERN), CTLCNST_TAVERN},  
            {SPRITE(CTL_KREMLIN_BACKGROUND), -1},  		
            {-1, -1}
        }
    };
    
    for(sint16 x = 0; x < CTLT_COUNT; x++)
        for(sint16 y = 0; y < 27; y++)
            for(sint16 z = 0; z < 2; z++)
                CTL_GFX_INDEX[x][y][z] = temp[x][y][z];

}

const uint8 HERO_ORIENT[CTLO_COUNT] = {
    1, 7
};

const sint32 CTL_INCOME = 500;

// Pricelist for castle constructions
const CTLCNST_DESC_STRUCT CTLCNSTS_DESC[CTLCNST_COUNT] = {
    {
        //
        // Peasants' hut
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	200,	0,		0,		0,		0,		0,		0		},
		{ {CTLCNST_TAVERN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_CITADEL },
		CREAT_PEASANT, 0
	},{
		//
		// Archers' tower
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	0,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_PEASNHUT, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_CITADEL},
		CREAT_ARCHER, 0
	},{
		//
		// Guardhouse
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1500,	5,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_ARCHTOWER, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_CITADEL},
		CREAT_PIKEMAN, 0
	},{
		//
		// Abbey
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2500,	4,		5,		0,		2,		0,		2		},	
		{ {CTLCNST_DWELL_GHOUSE, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_CITADEL},
		CREAT_MONK, 0
	},{
		//
		// Jousting arena
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	3000,	0,		20,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_ABBEY, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_CITADEL},
		CREAT_CAVALRY, 0
	},{
		//
		// Cathedral
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	8000,	0,		20,		0,		0,		20,		0		},	
		{ {CTLCNST_DWELL_ABBEY, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_CITADEL},
		CREAT_PALADIN, 0
	},{
		//
		// Goblin barracks
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	300,	0,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_TAVERN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_STRONGHOLD},
		CREAT_GOBLIN, 0
	},{
		//
		// Orc tower
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	800,	0,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_GBARRACKS, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_STRONGHOLD},
		CREAT_ORC, 0
	},{
		//
		// Warg cave
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1500,	0,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_ORCTOWER, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_STRONGHOLD},
		CREAT_WARG_RIDER, 0
	},{
		//
		// Ogre fort
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2000,	10,		10,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_ORCTOWER, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_STRONGHOLD},
		CREAT_OGRE, 0
	},{
		//
		// Troll bridge
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	4000,	20,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_OGREFORT, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_STRONGHOLD},
		CREAT_TROLL, 0
	},{
		//
		// Pyramid
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	10000,	20,		0,		0,		0,		20,		0		},	
		{ {CTLCNST_DWELL_WARGCAVE, CTLTM_ALL}, 		  
		  {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_STRONGHOLD},
		CREAT_CYCLOP, 0
	},{
		//
		// Magic school
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	400,	0,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_TAVERN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_TOWER},
		CREAT_YOUNG_MAGE, 0
	},{
		//
		// White wolf den
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	800,	0,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_MAGICSCHOOL, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_TOWER},
		CREAT_WHITE_WOLF, 0
	},{
		//
		// Factory
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1500,	5,		5,		3,		0,		0,		0		},	
		{ {CTLCNST_DWELL_MAGICSCHOOL, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_TOWER},
		CREAT_LIVING_ARMOR, 0
	},{
		//
		// Magic cloud/Cliff in the clouds/Magic Garden
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	3000,	0,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_WHITEWOLFDEN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_TOWER},
		CREAT_PEGASUS, 0
	},{
		//
		// Mage tower
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	3500,	5,		5,		5,		5,		5,		5		},	
		{ {CTLCNST_DWELL_FACTORY, CTLTM_ALL}, {CTLCNST_MAGEGUILD_L1, CTLTM_ALL} }, 
		{ CTLSM_SML, CTLTM_TOWER},
		CREAT_MAGE, 0
	},{
		//
		// Cloud palace
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	17500,	5,		5,		0,		20,		0,		0		},	
		{ {CTLCNST_DWELL_MAGICCLOUD, CTLTM_ALL} , {CTLCNST_DWELL_MAGETOWER, CTLTM_ALL}  },
		{ CTLSM_ML, CTLTM_TOWER},
		CREAT_THOR, 0
	},{
		//
		// Centaurs' cave
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	500,	0,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_TAVERN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_DUNGEON},
		CREAT_CENTAUR, 0
	},{
		//
		// Crypt
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	10,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_CENTCAVE, CTLTM_ALL} , {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_DUNGEON},
		CREAT_GARGOYLE, 0
	},{
		//
		// Griffin tower
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2000,	0,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_CRYPT, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_DUNGEON},
		CREAT_GRIFFIN, 0
	},{
		//
		// Labyrinth
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	3000,	0,		0,		0,		10,		0,		0		},	
		{ {CTLCNST_DWELL_GRIFTOWER, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_DUNGEON},
		CREAT_MINOTAUR, 0
	},{
		//
		// Hydra pond
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	4000,	0,		0,		0,		0,		0,		10		},	
		{ {CTLCNST_DWELL_LABYRINTH, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_DUNGEON},
		CREAT_HYDRA, 0
	},{
		//
		// Dragon cave
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	22500,	20,		0,		0,		0,		0,		20		},	
		{ {CTLCNST_DWELL_HYDRAPOND, CTLTM_ALL} , {CTLCNST_MAGEGUILD_L2, CTLTM_ALL}  },
		{ CTLSM_ML, CTLTM_DUNGEON},
		CREAT_RED_DRAGON, 0
	},{
		//
		// Treehouse
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	500,	0,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_TAVERN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_FORTRESS},
		CREAT_SPRITE, 0
	},{
		//
		// Dwarf cottage
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1500,	0,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_TREEHOUSE, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_FORTRESS},
		CREAT_DWARF, 0
	},{
		//
		// Homestead
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2000,	0,		10,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_DWCOTTAGE, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_FORTRESS},
		CREAT_ELF, 0
	},{
		//
		// Stonehenge
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2500,	10,		0,		5,		0,		0,		0		},	
		{ {CTLCNST_DWELL_HOMESTEAD, CTLTM_ALL} , {CTLCNST_MAGEGUILD_L1, CTLTM_ALL}  },
		{ CTLSM_SML, CTLTM_FORTRESS},
		CREAT_DRUID, 0
	},{
		//
		// Unicorn glade
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	3000,	0,		10,		0,		10,		0,		0		},	
		{ {CTLCNST_DWELL_STONEHENGE, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_FORTRESS},
		CREAT_UNICORN, 0
	},{
		//
		// Red tower
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	10000,	20,		0,		15,		0,		0,		0		},	
		{ {CTLCNST_DWELL_UNGLADE, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_FORTRESS},
		CREAT_FIREBIRD, 0
	},{
		//
		// Excavation
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	400,	0,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_TAVERN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_NECROPOLIS},
		CREAT_SKELETON, 0
	},{
		//
		// Graveyard
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1500,	0,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_EXCAVATION, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_NECROPOLIS},
		CREAT_ZOMBIE, 0
	},{
		//
		// Mausoleum
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2000,	10,		0,		0,		0,		5,		0		},	
		{ {CTLCNST_DWELL_GRAVEYARD, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_NECROPOLIS},
		CREAT_LICH, 0
	},{
		//
		// Sarcophagus
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	3000,	0,		10,		0,		5,		0,		0		},	
		{ {CTLCNST_DWELL_MAUSOLEUM, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_NECROPOLIS},
		CREAT_VAMPIRE, 0
	},{
		//
		// Dark Tomb
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	5000,	0,		10,		0,		0,		0,		10		},	
		{ {CTLCNST_DWELL_SARCOPHAGUS, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_NECROPOLIS},
		CREAT_BLACK_KNIGHT, 0
	},{
		//
		// Cursed Tower
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	12500,	10,		10,		20,		0,		0,		0		},	
		{ {CTLCNST_DWELL_MAUSOLEUM, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_NECROPOLIS},
		CREAT_PLAGUE, 0
	},
		
		//////////////////////////////////////////////////////////////////////////
// new variags units
//////////////////////////////////////////////////////////////////////////
	{
		//
		// Hunter Cabin
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	400,	0,		0,		0,		0,		0,		0		},
		{ {CTLCNST_TAVERN, CTLTM_ALL} , {CTLCNST_INVALID, CTLTM_ALL}  },
		{ CTLSM_SML, CTLTM_VARIAGS },
		CTLCNST_DWELL_HUNTERS_CABIN, 
	},{
		//
		// Forge
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1500,	0,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_HUNTERS_CABIN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_VARIAGS},
		CREAT_BLACKSMITH, 0
	},{
		//
		// Mercenary Barracks
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1800,	5,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_FORGE, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_VARIAGS},
		CREAT_MERCENARY, 0
	},{
		//
		// Forest Hut
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	3000,	0,		10,		0,		3,		0,		3		},	
		{ {CTLCNST_DWELL_MERC_BARRACKS, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_VARIAGS},
		CREAT_WITCH, 0
	},{
		//
		// Shrine
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	3500,	0,		5,		3,		3,		3,		3		},	
		{ {CTLCNST_DWELL_FOREST_HUT, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_VARIAGS},
		CREAT_VOLCHV, 0
	},{
		//
		// Residence
		//
		CCT_DWELLING,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	10000,	10,		20,		0,		20,		0,		0		},	
		{ {CTLCNST_DWELL_SHRINE, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_VARIAGS},
		CREAT_VITYAZ, 0
    },

		{
		//
		// Mage guild 1
		//
		CCT_MAGEGUILD,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2000,	5,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_INVALID, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_ALL},
		1, 0
	},{
		//
		// Mage guild 2
		//
		CCT_MAGEGUILD,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	5,		5,		4,		4,		4,		4		},	
		{ {CTLCNST_MAGEGUILD_L1, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_ALL},
		2, 0
	},{
		//
		// Mage guild 3
		//
		CCT_MAGEGUILD,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	5,		5,		6,		6,		6,		6		},	
		{ {CTLCNST_MAGEGUILD_L2, CTLTM_ALL} , {CTLCNST_INVALID, CTLTM_ALL } },
		{ CTLSM_SML, CTLTM_ALL},
		3, 0
	},{
		//
		// Mage guild 4
		//
		CCT_MAGEGUILD,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	5,		5,		8,		8,		8,		8		},	
		{ {CTLCNST_MAGEGUILD_L3, CTLTM_ALL} , {CTLCNST_INVALID, CTLTM_ALL}  },
		{ CTLSM_ML, CTLTM_MAGIC | CTLTM_MISC},
		4, 0
	},{
		//
		// Mage guild 5
		//
		CCT_MAGEGUILD,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	5,		5,		10,		10,		10,		10		},	
		{ {CTLCNST_MAGEGUILD_L4, CTLTM_ALL} , {CTLCNST_INVALID, CTLTM_ALL}  },
		{ CTLSM_L, CTLTM_MAGIC | CTLTM_MISC},
		5, 0
	},{
		//
		// Magic node
		//
		CCT_MAGICNODE,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	3000,	10,		0,		2,		2,		2,		2		},	
		{ {CTLCNST_MAGEGUILD_L2, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_L, CTLTM_MAGIC},
		0, 0
	},{
		//
		// Tavern
		//
		CCT_TAVERN,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	0,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_INVALID, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_ALL},
		0, 0
	},{
		//
		// Marketplace
		//
		CCT_GENERIC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	500,	0,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_TAVERN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_ALL},
		0, 0
	},{
		//
		// Town hall
		//
		CCT_MINE,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2500,	0,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_TAVERN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_M, CTLTM_ALL},
		MINERAL_GOLD, 500
	},{
		//
		// City hall
		//
		CCT_MINE,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	5000,	0,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_MAGEGUILD_L1, CTLTM_ALL}, {CTLCNST_MARKETPLACE, CTLTM_ALL}  },
		{ CTLSM_L, CTLTM_ALL},
		MINERAL_GOLD, 1500
	},{
		//
		// Ore mine
		//
		CCT_MINE,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	3000,	5,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_MARKETPLACE, CTLTM_ALL}, 
		  {CTLCNST_SAWMILL, CTLTM_CITADEL}, // only after sawmill for all except tower
		  {CTLCNST_SAWMILL, CTLTM_STRONGHOLD}, // only after sawmill for all except tower
		  {CTLCNST_SAWMILL, CTLTM_FORTRESS}, // only after sawmill for all except tower
		  {CTLCNST_SAWMILL, CTLTM_DUNGEON}, // only after sawmill for all except tower
		  {CTLCNST_SAWMILL, CTLTM_NECROPOLIS}, // only after sawmill for all except tower
		  {CTLCNST_SAWMILL, CTLTM_VARIAGS}, // only after sawmill for all except tower		  
		  {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_ALL},
		MINERAL_ORE, 1
	},{
		//
		// Sawmill
		//
		CCT_MINE,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	3000,	0,		5,		0,		0,		0,		0		},	
		{ 
			{CTLCNST_MARKETPLACE, CTLTM_ALL}, 
			{CTLCNST_OREMINE, CTLTM_TOWER}, // only after oremine for tower
			{CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_SML, CTLTM_ALL},
		MINERAL_WOOD, 1
	},{
		//
		// Alchemists' laboratory
		//
		CCT_MINE,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	5000,	5,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_MARKETPLACE, CTLTM_ALL}, 		  
		  {CTLCNST_OREMINE, CTLTM_ALL}, // after ore mine for all
		  {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_MISC},
		MINERAL_MERCURY, 1
	},{
		//
		// Gems mine
		//
		CCT_MINE,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	5000,	5,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_MARKETPLACE, CTLTM_ALL}, 
		  {CTLCNST_OREMINE, CTLTM_ALL}, // only after ore mine for all
		  {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_TOWER | CTLTM_VARIAGS},
		MINERAL_GEMS, 1
	},{
		//
		// Crystal mine
		//
		CCT_MINE,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	5000,	5,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_MARKETPLACE, CTLTM_ALL}, 
		  {CTLCNST_SAWMILL, CTLTM_MIGHT},  // only after sawmill for Might castles
		  {CTLCNST_OREMINE, CTLTM_MIGHT},  // only after ore mine for Might castles
		{CTLCNST_INVALID, CTLTM_ALL},		  
		},

		{ CTLSM_ML, CTLTM_MIGHT},
		MINERAL_CRYSTAL, 1
	},{
		//
		// Sulfur mine
		//
		CCT_MINE,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	5000,	5,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_MARKETPLACE, CTLTM_ALL}, 
		{CTLCNST_OREMINE, CTLTM_TOWER}, // only after ore mine for tower
		{CTLCNST_OREMINE, CTLTM_DUNGEON}, // only after ore mine for dungeon
		{CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_DUNGEON},
		MINERAL_SULFUR, 1
	},{
		//
		// Moat
		//
		CCT_GENERIC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	5,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_INVALID, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_ALL },
		0, 0
	},{
		//
		// Main turret
		//
		CCT_GENERIC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2500,	5,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_MOAT, CTLTM_ALL}, // only after moat for all
		  {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_ALL },
		0, 0
	},{
		//
		// Left turret
		//
		CCT_GENERIC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1500,	5,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_MTURRET, CTLTM_ALL},
		  {CTLCNST_RTURRET, CTLTM_MIGHT}, // only after right turret for Might Castles		
		  {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_MIGHT },
		0, 0
	},{
		//
		// Right turret
		//
		CCT_GENERIC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1500,	5,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_MTURRET, CTLTM_ALL}, 		  
		  {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_MIGHT },
		0, 0
	},{
		//
		// Shooting range
		//
		CCT_DWELL_ENC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	0,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_ARCHTOWER, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_CITADEL },
		CTLCNST_DWELL_ARCHTOWER, 10
	},{
		//
		// Mess hall
		//
		CCT_DWELL_ENC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	5,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_ORCTOWER, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_STRONGHOLD },
		CTLCNST_DWELL_ORCTOWER, 10
	},{
		//
		// Cavern
		//
		CCT_DWELL_ENC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1500,	0,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_WHITEWOLFDEN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_TOWER },
		CTLCNST_DWELL_WHITEWOLFDEN, 8
	},{
		//
		// Waterfall
		//
		CCT_DWELL_ENC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	5,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_CENTCAVE, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_DUNGEON },
		CTLCNST_DWELL_CENTCAVE, 18
	},{
		//
		// Miners guild
		//
		CCT_DWELL_ENC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	500,	5,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_DWCOTTAGE, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_FORTRESS },
		CTLCNST_DWELL_DWCOTTAGE, 8
	},{
		//
		// Unearthed graves
		//
		CCT_DWELL_ENC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1500,	0,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_EXCAVATION, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_NECROPOLIS },
		CTLCNST_DWELL_EXCAVATION, 18
	},{
        //
        //  Scout Tower
        //
        CCT_DWELL_ENC,
        //	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
        {	1000,	5,		5,		0,		0,		0,		0		},
        { 
			{CTLCNST_MOAT, CTLTM_ALL}, 
			{CTLCNST_DWELL_HUNTERS_CABIN, CTLTM_ALL}, 
			{CTLCNST_INVALID, CTLTM_ALL} },
        { CTLSM_ML, CTLTM_VARIAGS },
        CTLCNST_DWELL_HUNTERS_CABIN, 18
    },
		{		
        //
        //  Lodge
        //
        CCT_LODGE,
        //	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
        {	3000,	0,		5,		5,		0,		5,		0		},
        { {CTLCNST_INVALID, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
        { CTLSM_ML, CTLTM_VARIAGS },
        0, 0
    },{
        //
        //  Merchant
        //
        CCT_MERCHANT,
        //	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
        {	500,	0,		5,		0,		0,		0,		0		},	
        { {CTLCNST_MARKETPLACE, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
        { CTLSM_ML, CTLTM_VARIAGS },
        0, 0
    },
		{
		//
		// Observation post
		//
		CCT_OBSERVPOST,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2500,	5,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_TAVERN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_CITADEL },
		0, 0
	},{
		//
		// Fortification
		//
		CCT_GENERIC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	10,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_LTURRET, CTLTM_CITADEL},  // after LTURRET for citadel
		  {CTLCNST_MOAT, CTLTM_CITADEL},  // after MOAT for citadel
		  {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_CITADEL },
		0, 0
	},{
		//
		// Hall of Valhalla
		//
		CCT_PERM_FSK_MOD,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1000,	5,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_TAVERN, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_STRONGHOLD },
		FSK_ATTACK, +1
	},{
		//
		// Adobe
		//
		CCT_DWELL_ENC,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2500,	10,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_DWELL_OGREFORT, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_STRONGHOLD },
		CTLCNST_DWELL_OGREFORT, 6
	},{
		//
		// Wall of knowledge
		//
		CCT_PERM_FSK_MOD,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2000,	5,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_MAGEGUILD_L1, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_TOWER },
		FSK_KNOWLEDGE, +1
	},{
		//
		// Library
		//
		CCT_LIBRARY,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	5000,	5,		5,		5,		5,		5,		5		},	
		{ {CTLCNST_MAGEGUILD_L2, CTLTM_ALL}, 
		  {CTLCNST_WALLOFKNOWLEDGE, CTLTM_TOWER},  // only after wall of knowledge for tower
		  {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_TOWER },
		0, 0
	},{
		//
		// Altar
		//
		CCT_MINE,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	5000,	5,		0,		0,		0,		0,		0		},	
		{ {CTLCNST_TAVERN, CTLTM_ALL}, 
		{ CTLCNST_MTURRET, CTLTM_ALL},
		  {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_DUNGEON },
		MINERAL_GOLD, 1000
	},{
		//
		// Mana vortex
		//
		CCT_MANAVORTEX,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2500,	0,		0,		1,		1,		1,		1		},	
		{ {CTLCNST_MAGEGUILD_L1, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_DUNGEON },
		0, 0
	},{
		//
		// Treasury
		//
		CCT_MINE,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	5000,	5,		5,		0,		0,		0,		0		},	
		{ {CTLCNST_MINERSGUILD, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_FORTRESS },
		MINERAL_GOLD, 1000
	},{
		//
		// Mystic pound
		//
		CCT_MYSTICPOUND,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1500,	1,		1,		1,		1,		1,		1		},	
		{ {CTLCNST_MOAT, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_FORTRESS },
		0, 0
	},{
		//
		// Necromancy amplifier
		//
		CCT_NECRAMPLIFIER,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	2500,	0,		0,		0,		0,		0,		0		},	
		{  { CTLCNST_DWELL_GRAVEYARD, CTLTM_NECROPOLIS}, // only after graveyard for necropolis
		  {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_NECROPOLIS },
		0, 0
	},{
		//
		// Cover of darkness
		//
		CCT_COVEROFDARKNESS,
		//	Gold,	Ore,	Wood,	Merc,	Gems,	Cryst,	Sulfur
		{	1500,	5,		0,		0,		0,		0,		0		},
		{ { CTLCNST_INVALID, CTLTM_ALL}, {CTLCNST_INVALID, CTLTM_ALL} },
		{ CTLSM_ML, CTLTM_NECROPOLIS },
		0, 0		
    },
};




const iSpellFilter CTL_MAGE_GUILD_FILTER[CTLT_COUNT][5] = {
	{
		// Citadel
		{SPT_MASK_ALL, SPL_MASK_FIRST, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_SECOND, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_THIRD, MST_MASK_GOOD}
	},{
		// Stronghold
		{SPT_MASK_ALL, SPL_MASK_FIRST, MST_MASK_EVIL},
		{SPT_MASK_ALL, SPL_MASK_SECOND, MST_MASK_EVIL},
		{SPT_MASK_ALL, SPL_MASK_THIRD, MST_MASK_EVIL}
	},{
		// Tower
		{SPT_MASK_ALL, SPL_MASK_FIRST, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_SECOND, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_THIRD, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_FOURTH, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_FIFTH, MST_MASK_GOOD}
	},{
		// Dungeon
		{SPT_MASK_ALL, SPL_MASK_FIRST, MST_MASK_EVIL},
		{SPT_MASK_ALL, SPL_MASK_SECOND, MST_MASK_EVIL},
		{SPT_MASK_ALL, SPL_MASK_THIRD, MST_MASK_EVIL},
		{SPT_MASK_ALL, SPL_MASK_FOURTH, MST_MASK_EVIL},
		{SPT_MASK_ALL, SPL_MASK_FIFTH, MST_MASK_EVIL}
	},{
		// Fortress
		{SPT_MASK_ALL, SPL_MASK_FIRST, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_SECOND, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_THIRD, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_FOURTH, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_FIFTH, MST_MASK_GOOD}
	},{
		// Necropolis
		{SPT_MASK_ALL, SPL_MASK_FIRST, MST_MASK_EVIL},
		{SPT_MASK_ALL, SPL_MASK_SECOND, MST_MASK_EVIL},
		{SPT_MASK_ALL, SPL_MASK_THIRD, MST_MASK_EVIL},
		{SPT_MASK_ALL, SPL_MASK_FOURTH, MST_MASK_EVIL},
		{SPT_MASK_ALL, SPL_MASK_FIFTH, MST_MASK_EVIL}
    },{
        // Kremlin
		{SPT_MASK_ALL, SPL_MASK_FIRST, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_SECOND, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_THIRD, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_FOURTH, MST_MASK_GOOD},
		{SPT_MASK_ALL, SPL_MASK_FIFTH, MST_MASK_GOOD}
	}
};

const uint8 CTL_MAGE_GUILD_SPELLS[CTLT_COUNT][5] = 
{
	{
		// Citadel
		5, 4, 3
	},{
		// Stronghold
		5, 4, 3
	},{
		// Tower
		5, 4, 3, 2, 1
	},{
		// Dungeon
		5, 4, 3, 2, 1
	},{
		// Fortress
		5, 4, 3, 2, 1
	},{
		// Necropolis
		5, 4, 3, 2, 1
    },{
        // Kremlin
        5, 4, 3, 2, 1
    }
};

// END OF FILE
