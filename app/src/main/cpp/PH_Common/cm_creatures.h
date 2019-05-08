#ifndef PH_COMMON_CM_CREATURES_H_
#define PH_COMMON_CM_CREATURES_H_

/*
 *  Creatures
 */

/*
 *	for conversion purposes
 */


enum CREATURE_TYPE {
    CREAT_UNKNOWN = -1,
//////////////////////////////////////////////////////////////////////////
// Usual (without upgrades)
//////////////////////////////////////////////////////////////////////////
    /* Knight */
    CREAT_PEASANT = 0,
    CREAT_ARCHER,
    CREAT_PIKEMAN,
    CREAT_MONK,
    CREAT_CAVALRY,
    CREAT_PALADIN,

    /* Barbarian */
    CREAT_GOBLIN,
    CREAT_ORC,
    CREAT_WARG_RIDER,
    CREAT_OGRE,
    CREAT_TROLL,
    CREAT_CYCLOP,

    /* Wizard */
    CREAT_YOUNG_MAGE,
    CREAT_WHITE_WOLF,
    CREAT_LIVING_ARMOR,
    CREAT_PEGASUS,
    CREAT_MAGE,
    CREAT_THOR,
    
    /* Warlock */
    CREAT_CENTAUR,
    CREAT_GARGOYLE,
    CREAT_GRIFFIN,
    CREAT_MINOTAUR,
    CREAT_HYDRA,
    CREAT_RED_DRAGON,
        
    /* Sorcer */
    CREAT_SPRITE,
    CREAT_DWARF,
    CREAT_ELF,
    CREAT_DRUID,
    CREAT_UNICORN,
    CREAT_FIREBIRD,
    
    /* Necromant */
    CREAT_SKELETON,
    CREAT_ZOMBIE,
    CREAT_LICH,
    CREAT_VAMPIRE,
    CREAT_BLACK_KNIGHT,
    CREAT_PLAGUE,

    /* Variags */
    CREAT_HUNTER,
    CREAT_BLACKSMITH,
    CREAT_MERCENARY,
    CREAT_WITCH,
    CREAT_VOLCHV,
    CREAT_VITYAZ,


/////////////////////////////////////////////////////////////////
// Upgrade1                 // Upgrade2                  
/////////////////////////////////////////////////////////////////
    /* Knight */                /* Knight */             
    CREAT_MILITIA,              CREAT_CITIZEN,           
    CREAT_CROSSBOWMAN,          CREAT_SHARPSHOOTER,      
    CREAT_HEAVY_PIKEMAN,        CREAT_HALBERDIER,        
    CREAT_EXORCIST,             CREAT_BISHOP,            
    CREAT_HEAVY_CAVALRY,        CREAT_KNIGHT,            
    CREAT_CRUSADER,             CREAT_HOLY_WARRIOR,      
                                                         
    /* Barbarian */             /* Barbarian */          
    CREAT_CONTAGIOUS_GOBLIN,    CREAT_GOBLIN_WARRIOR,    
    CREAT_ORC_CHIEF,            CREAT_ORC_WARRIOR,       
    CREAT_WILD_WARG,            CREAT_WARG_WARRIOR,      
    CREAT_OGRE_CHIEF,           CREAT_OGRE_SHAMAN,       
    CREAT_BALLISTA_TROLL,       CREAT_TROLL_SHAMAN,      
    CREAT_ANCIENT_CYCLOPS,      CREAT_DIRE_CYCLOPS,      
                                                         
    /* Wizard */                /* Wizard */             
    CREAT_NEOPHYTE,             CREAT_MAGE_APPRENTICE,   
    CREAT_RED_WOLF,             CREAT_TWO_HEADED_WOLF,   
    CREAT_STEEL_GUARD,          CREAT_WAR_MACHINE,       
    CREAT_ROYAL_PEGASUS,        CREAT_BLINDING_PEGASUS,  
    CREAT_AIR_MAGE,             CREAT_PROPHET,           
    CREAT_LEGEND,               CREAT_DEMIGOD,           
                                                         
    /* Warlock */               /* Warlock */            
    CREAT_CENTEUR_RANGER,       CREAT_CENTAUR_LORD,      
    CREAT_OBSIDIAN_GARGOYLE,    CREAT_STONE_GARGOYLE,    
    CREAT_SNAKE_GRIFFIN,        CREAT_ROYAL_GRIFFIN,     
    CREAT_MINOTAUR_REAPER,      CREAT_DIRE_MINOTAUR,     
    CREAT_POISONOUS_HYDRA,      CREAT_ANCIENT_HYDRA,     
    CREAT_FIRE_DRAGON,          CREAT_BLACK_DRAGON,      
                                                         
    /* Sorcer */                /* Sorcer */             
    CREAT_ELUSIVE_SPRITE,       CREAT_POISONOUS_SPRITE,  
    CREAT_AXE_HURLER,           CREAT_BERZERKER,         
    CREAT_ELF_RANGER,           CREAT_ELF_HUNTER,        
    CREAT_GRAND_DRUID,          CREAT_OUTCAST,           
    CREAT_ROYAL_UNICORN,        CREAT_GHOST_UNICORN,     
    CREAT_PHOENIX,              CREAT_MAGIC_FIREBIRD,    
                                                         
    /* Necromant */             /* Necromant */          
    CREAT_SKELETON_ARCHER,      CREAT_SKELETON_WARRIOR,  
    CREAT_CONTAGIOUS_ZOMBIE,    CREAT_TOUGH_ZOMBIE,      
    CREAT_ARCHLICH,             CREAT_POWERFUL_LICH,     
    CREAT_NOSFERATU,            CREAT_VAMPIRE_LORD,      
    CREAT_DEATH_KNIGHT,         CREAT_BLACK_GRAIL_KNIGHT,
    CREAT_DEATH,                CREAT_FEAR,              
                                                         
    /* Variags */               /* Variags */            
    CREAT_OLD_HUNTER,           CREAT_MATURE_HUNTER,     
    CREAT_NOBLE_BLACKSMITH,     CREAT_WEREWOLF,          
    CREAT_BOYARIN,              CREAT_ROYAL_GUARD,       
    CREAT_ENCHANTER,            CREAT_YAGA,              
    CREAT_LESHIY,               CREAT_GREAT_VOLCHV,      
    CREAT_BOGATYR,              CREAT_VOLOT,             

//////////////////////////////////////////////////////////////////////////
    /* Neutral = 11 */
    CREAT_ROGUE,
    CREAT_NOMAD,
    CREAT_GHOST,
    CREAT_GENIE,
    CREAT_MEDUSA,
    CREAT_EARTH_ELEMENTAL,
    CREAT_AIR_ELEMENTAL,
    CREAT_FIRE_ELEMENTAL,
    CREAT_WATER_ELEMENTAL,
    CREAT_TATAR_ARCHER,
    CREAT_GORYNYCH,

    CREAT_COUNT,

    /* Random values */
    CREAT_RANDOM        = 0x0F00,
    CREAT_RANDOM_L1     = CREAT_RANDOM | 1,
    CREAT_RANDOM_L2     = CREAT_RANDOM | 2,
    CREAT_RANDOM_L3     = CREAT_RANDOM | 3,
    CREAT_RANDOM_L4     = CREAT_RANDOM | 4,
    CREAT_RANDOM_L5     = CREAT_RANDOM | 5,
    CREAT_RANDOM_L6     = CREAT_RANDOM | 6,
};
//////////////////////////////////////////////////////////////////////////
const uint32 CPERKS_COUNT = 21 + 7 + 1;
enum CREAT_PERK {
    CPERK_NONE              = 0,
    CPERK_DOUBLESHOT        = 0x00000001,   // Shots twice (rangers, grand elves, etc)
    CPERK_DOUBLEATTACK      = 0x00000002,   // Attack the target twice (paladins, wolves, etc)
    CPERK_NOMELEEPENALTY    = 0x00000004,   // scores full damage even at melee
    CPERK_NONRETALATTACK    = 0x00000008,   // Non retaliated attack (vampires, rogues, sprites, hydras, etc)
    CPERK_RETALTOALL        = 0x00000010,   // Retaliates against every attack (griffins)
    CPERK_TWOHEXATTACK      = 0x00000020,   // Two-hex attack (dragons)
    CPERK_ADJACENTATTACK    = 0x00000040,   // Attacks all adjacent enemies (hydras)
    CPERK_LICHSHOOT         = 0x00000080,   // Range attack affects adjacent hexes except undeads (Liches)
    CPERK_UNDEAD            = 0x00000100,   // All necropolis creatures + ghosts
    CPERK_LIFELESS          = 0x00000200,   // Golems, gargoyles, elementals
    CPERK_REGENERATES       = 0x00000400,   // Trolls
    CPERK_JOUSTING          = 0x00000800,   // Cavalry
    CPERK_AIRMAGICIMM       = 0x00001000,   // Air elementals
    CPERK_EARTHMAGICIMM     = 0x00002000,   // Earth elementals
    CPERK_FIREMAGICIMM      = 0x00004000,   // Fire elementals, Phoenix
    CPERK_WATERMAGICIMM     = 0x00008000,   // Water elementals
    CPERK_40PROCRESIST      = 0x00010000,   // 40% magic resistance (dwrves)
    CPERK_QUARTERDMG        = 0x00020000,   // receives only qurter damage from damage spells (golems)
    CPERK_GHOST             = 0x00040000,   // Ghost perk
    CPERK_DOHALF            = 0x00080000,   // Genie perk
    CPERK_DRAINLIFES        = 0x00100000,   // Drains life (vampires)
    CPERK_ALLMAGICIMM       = CPERK_AIRMAGICIMM | CPERK_EARTHMAGICIMM | CPERK_FIREMAGICIMM | CPERK_WATERMAGICIMM,   // Black draggons
    CPERK_FEAR              = 0x00200000,   // Decreases enemy morale
    CPERK_POISON            = 0x00400000,	// adds poison for 3 turns
    CPERK_HOLY              = 0x00800000,	// gives 2x bonus against undead
    CPERK_FURY              = 0x01000000,	// casts bloodlust if initial group quantity falls below 50%
    CPERK_CURSE             = 0x02000000,	// 100% chance of cursing the target
    CPERK_BLIND             = 0x04000000,	// 20% of blinding the target
    CPERK_MISER             = 0x08000000,	// gives 1 gold per creature per day
    CPERK_RETURN            = 0x10000000	// creature returns to its initial positions
};

//////////////////////////////////////////////////////////////////////////
enum CREATURE_SPEED {
    SPEED_SUPERSLOW = 1,
    SPEED_ULTRASLOW = 2,
    SPEED_VERYSLOW = 3,
    SPEED_EXTRASLOW = 4,
    SPEED_SLOW = 5,
    SPEED_SWIFT = 6,
    SPEED_EXTRASWIFT = 7,
    SPEED_VERYSWIFT = 8,  
    SPEED_ULTRASWIFT = 9,   // (2 to 2)
    SPEED_SUPERSWIFT = 10,  // (1 to 2 or 2 to 1)
    SPEED_QUICK = 11,       // (1 to 1)
    SPEED_EXTRAQUICK = 12,
    SPEED_VERYQUICK = 13,
    SPEED_ULTRAQUICK = 14,
    SPEED_SUPERQUICK = 15,
    SPEED_FAST = 16,
    SPEED_EXTRAFAST = 17,
    SPEED_VERYFAST = 18,
    SPEED_ULTRAFAST = 19,
    SPEED_SUPERFAST = 20,
    SPEED_MAX
};
//////////////////////////////////////////////////////////////////////////
enum TRANS_TYPE {
    TRANS_WALK = 0,
    TRANS_FLY
};

enum ANIM_TYPE {
    ANIM_6 = 0,
    ANIM_18,
    ANIM_18_TELEPORT,
    ANIM_18_STILLSTART
};

enum SHOOT_ANIM_TYPE {
    SA_FLY = 0,
	SA_FLY4,
    SA_INPLACE_2,
    SA_INPLACE_3
};
//////////////////////////////////////////////////////////////////////////
struct CREAT_DESCRIPTOR {
    uint8   level;      // unit level (1-6)
    uint8   nation;     // unit alignment

    uint8   attack;     // attack skill
    uint8   defence;    // deffence skill
    uint8   speed;      // unit speed (1-20)
    uint8   size;       // unit size (1 or 2 - used in battle)

    uint8   transType;  // transportation method
    uint8   shots;      // 0 - means no range attack
    uint16  hits;       // hit points (health)
    uint16  damage_min; // minimum damage
    uint16  damage_max; // maximum damage

    iMineralSet cost, upgradecost;   // cost per unit
    uint16      growth; // growth rate
    uint16      pidx;   // power index (used for AI)
    uint32      perks;  // creature perks
    sint16      sprite;  // starting sprite of an animation sequence
    sint16      ammoSprite; //sprite of the ammo if shooting
    ANIM_TYPE   movAnim;   // moving animation type
    SHOOT_ANIM_TYPE shootAnim; // shooting animation type
    iPoint      shootStartPoint;
    struct {
        sint16 walk, attack, gethit, die;
    } snds;
};


// enum creature bonus type
enum CREAT_BONUS_TYPE {
    CBT_ATTACK = 0,
    CBT_DEFENCE,
    CBT_SPEED,
    CBT_COUNT
};

// creature bonus struct
struct iCreatBonus {
    CREATURE_TYPE ct;
    CREAT_BONUS_TYPE bt;
    sint32 bonus;
};  

typedef iSimpleArray<iCreatBonus> iCreatBonusList;
extern CREAT_DESCRIPTOR *CREAT_DESC;
void init_creat_desc();
//#include "cm_creatures.cpp"

extern const sint32 INITIAL_HERO_ARMY[HERO_TYPE_COUNT][2][3];


#endif //PH_COMMON_CM_CREATURES_H_
