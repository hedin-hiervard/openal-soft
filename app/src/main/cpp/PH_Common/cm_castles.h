#ifndef PH_COMMON_CM_CASTLES_H_
#define PH_COMMON_CM_CASTLES_H_

// Castle types
enum CTL_TYPE {
	CTLT_CITADEL = 0,	// Done
	CTLT_STRONGHOLD,	// Done
	CTLT_TOWER,			// Done
	CTLT_DUNGEON,		// Done
	CTLT_FORTRESS,		// Done
	CTLT_NECROPOLIS,	// Done    	
    CTLT_KREMLIN,	    // Done
	CTLT_RANDOM,        // Random
	CTLT_COUNT,
	CTLT_INVALID	= 0xFF,
};
extern const uint32 CTL_HEROES[CTLT_COUNT];

// Castle dimensions
enum CTL_SIZE {
	CTLS_SMALL = 1,	// Village
	CTLS_MEDIUM,	// Town
	CTLS_LARGE,		// City
	CTLS_COUNT
};

// Castle orientation
enum CTL_ORIENT {
	CTLO_LEFT,		// Left 
	CTLO_RIGHT,		// Right	
	CTLO_COUNT
};

extern const uint8 HERO_ORIENT[CTLO_COUNT];

extern const sint32 CTL_INCOME;

extern sint16 CTL_GFX_INDEX[CTLT_COUNT][27][2];
void init_castle_cnsts();
/*
 *	Castle constructions
 */
enum CTL_CONSTR {
	CTLCNST_INVALID = -1,
	
	// Dwellings
	CTLCNST_DWELLINGS = 0,

	CTLCNST_DWELL_PEASNHUT = CTLCNST_DWELLINGS,
	CTLCNST_DWELL_ARCHTOWER,
	CTLCNST_DWELL_GHOUSE,
	CTLCNST_DWELL_ABBEY,
	CTLCNST_DWELL_JARENA,
	CTLCNST_DWELL_CATHEDRAL,
	CTLCNST_DWELL_GBARRACKS,
	CTLCNST_DWELL_ORCTOWER,
	CTLCNST_DWELL_WARGCAVE,
	CTLCNST_DWELL_OGREFORT,
	CTLCNST_DWELL_TRBRIDGE,
	CTLCNST_DWELL_PYRAMID,
	CTLCNST_DWELL_MAGICSCHOOL,
	CTLCNST_DWELL_WHITEWOLFDEN,
	CTLCNST_DWELL_FACTORY,
	CTLCNST_DWELL_MAGICCLOUD,
	CTLCNST_DWELL_MAGETOWER,
	CTLCNST_DWELL_CLDPALACE,
	CTLCNST_DWELL_CENTCAVE,
	CTLCNST_DWELL_CRYPT,
	CTLCNST_DWELL_GRIFTOWER,
	CTLCNST_DWELL_LABYRINTH,
	CTLCNST_DWELL_HYDRAPOND,
	CTLCNST_DWELL_DRAGCAVE,
	CTLCNST_DWELL_TREEHOUSE,
	CTLCNST_DWELL_DWCOTTAGE,
	CTLCNST_DWELL_HOMESTEAD,
	CTLCNST_DWELL_STONEHENGE,
	CTLCNST_DWELL_UNGLADE,
	CTLCNST_DWELL_REDTOWER,
	CTLCNST_DWELL_EXCAVATION,
	CTLCNST_DWELL_GRAVEYARD,
	CTLCNST_DWELL_MAUSOLEUM,
	CTLCNST_DWELL_SARCOPHAGUS,
	CTLCNST_DWELL_DARKTOMB,
	CTLCNST_DWELL_CURSEDTOWER,

	// kremlin
	CTLCNST_DWELL_HUNTERS_CABIN,
    CTLCNST_DWELL_FORGE,
    CTLCNST_DWELL_MERC_BARRACKS,
    CTLCNST_DWELL_FOREST_HUT,
    CTLCNST_DWELL_SHRINE,
    CTLCNST_DWELL_RESIDENCE,
	
	// Common constructions
	CTLCNST_MAGEGUILDS,
	CTLCNST_MAGEGUILD_L1 = CTLCNST_MAGEGUILDS,
	CTLCNST_MAGEGUILD_L2,
	CTLCNST_MAGEGUILD_L3,
	CTLCNST_MAGEGUILD_L4,
	CTLCNST_MAGEGUILD_L5,
	CTLCNST_MAGICNODE,
	CTLCNST_TAVERN,
	CTLCNST_MARKETPLACE,

	// Common mines
	CTLCNST_MINES,
	CTLCNST_TOWNHALL = CTLCNST_MINES,
	CTLCNST_CITYHALL,
	CTLCNST_OREMINE,
	CTLCNST_SAWMILL,
	CTLCNST_ALCHLAB,
	CTLCNST_GEMSMINE,
	CTLCNST_CRYSTALMINE,
	CTLCNST_SULFURMINE,

	CTLCNST_MOAT,
	CTLCNST_MTURRET,
	CTLCNST_LTURRET,
	CTLCNST_RTURRET,

	// Dwelling enchancers
	CTLCNST_SHOOTINGRANGE,
	CTLCNST_MESSHALL,
	CTLCNST_CAVERN,
	CTLCNST_WATERFALL,
	CTLCNST_MINERSGUILD,
	CTLCNST_UNEARTHEDGRAVES,
	CTLCNST_SCOUT_TOWER,
	
	// Other special constructions
	CTLCNST_LODGE,
    CTLCNST_MERCHANT,		
	CTLCNST_OBSERVPOST,
	CTLCNST_FORTIFICATION,
	CTLCNST_HALLOFVALHALLA,
	CTLCNST_ADOBE,
	CTLCNST_WALLOFKNOWLEDGE,
	CTLCNST_LIBRARY,
	CTLCNST_ALTAR,
	CTLCNST_MANAVORTEX,
	CTLCNST_TREASURY,
	CTLCNST_MYSTICPOUND,
	CTLCNST_NECRAMPLIFIER,
	CTLCNST_COVEROFDARKNESS,	   

	CTLCNST_COUNT
};


// Types of castle constructions
enum CTL_CNST_TYPE {
	CCT_GENERIC = 0,
	CCT_DWELLING,		// fparam = Creature type
	CCT_MAGEGUILD,		// fparam = guild level
	CCT_TAVERN,			// -
	CCT_MAGICNODE,		// -
	CCT_MINE,			// fparam = mineral type, sparam = quantity
	CCT_PERM_FSK_MOD,	// fparam = skill, sparam = modifier
	CCT_DWELL_ENC,		// fparam = dwelling id, sparam = modifier
	CCT_OBSERVPOST,		// -
	CCT_LIBRARY,		// -
	CCT_MANAVORTEX,		// -
	CCT_TREASURY,		// -
	CCT_MYSTICPOUND,	// -
	CCT_NECRAMPLIFIER,	// -
	CCT_COVEROFDARKNESS,// -
    CCT_LODGE,    	    // -
    CCT_MERCHANT, 	    // -    
	CCT_COUNT
};

enum CTL_TYPE_MASK {
	CTLTM_CITADEL		= 1 << CTLT_CITADEL,
	CTLTM_STRONGHOLD	= 1 << CTLT_STRONGHOLD,
	CTLTM_TOWER			= 1 << CTLT_TOWER,
	CTLTM_DUNGEON		= 1 << CTLT_DUNGEON,
	CTLTM_FORTRESS		= 1 << CTLT_FORTRESS,
	CTLTM_NECROPOLIS	= 1 << CTLT_NECROPOLIS,
    CTLTM_VARIAGS       = 1 << CTLT_KREMLIN,
	CTLTM_MIGHT			= CTLTM_CITADEL | CTLTM_STRONGHOLD,
	CTLTM_MAGIC			= CTLTM_TOWER | CTLTM_DUNGEON,
	CTLTM_MISC			= CTLTM_FORTRESS | CTLTM_NECROPOLIS,
	CTLTM_ALL			= CTLTM_MIGHT | CTLTM_MAGIC | CTLTM_MISC | CTLTM_VARIAGS
};

enum CTL_SIZE_MASK {
	CTLSM_S		= 1 << CTLS_SMALL,
	CTLSM_M		= 1 << CTLS_MEDIUM,
	CTLSM_L		= 1 << CTLS_LARGE,
	CTLSM_ML	= CTLSM_M | CTLSM_L,
	CTLSM_SML	= CTLSM_S | CTLSM_M | CTLSM_L
};

struct CTCNSTCAP {
	uint16	siz;
	uint16	type;

	inline bool Support(CTL_TYPE _type, CTL_SIZE _siz) const
	{
		return (type & (1<<_type)) && (siz & (1<<_siz));
	}

};

struct DEPEND_RECORD {
	CTL_CONSTR depend;
	CTL_TYPE_MASK castles;
	inline bool Support(CTL_TYPE _type) const
	{
		return (castles & (1 << _type)) != 0;
	}
};

struct CTLCNST_DESC_STRUCT {
	CTL_CNST_TYPE	type;
	iMineralSet		price;
	DEPEND_RECORD	depends[10];
	CTCNSTCAP		caps;
	sint32			fparam;
	sint32			sparam;
};

extern const CTLCNST_DESC_STRUCT CTLCNSTS_DESC[CTLCNST_COUNT];


extern const iSpellFilter CTL_MAGE_GUILD_FILTER[CTLT_COUNT][5];
extern const uint8 CTL_MAGE_GUILD_SPELLS[CTLT_COUNT][5];


#endif //PH_COMMON_CM_CASTLES_H_
