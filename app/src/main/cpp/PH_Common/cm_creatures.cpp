#include "stdafx.h"
#include "common.h"
#include "cm_creatures.h"

#ifdef _HMM_GAME_
#include "Common/iphone/cnst_gfx.h"
#include "Common/ipad/cnst_gfx.h"
#include "cnst_sfx.h"
#endif

#ifndef _HMM_GAME_
#define SPRITE(name) 0
#define SOUND(name) 0
#else
#define SPRITE(name) PDGG_SINT16(name)
#define SOUND(name) CSND_##name
#endif 


//#ifdef DO_INCLUDE_CREAT_DESC
CREAT_DESCRIPTOR *CREAT_DESC = NULL;

void init_creat_desc()
{
    //////////////////////////////////////////////////////////////////////////////////////////
    // base creatures (without upgrades)
    //////////////////////////////////////////////////////////////////////////////////////////
    /* Knight */
    static CREAT_DESCRIPTOR temp[CREAT_COUNT]
    {
    {
        // Peasant
        1,NATION_HIGHMEN,
            1,2,                                        // Attack, defence
            4,1,TRANS_WALK,0,                           // Speed, size, transportation, range attack
            2,                                          // Hit points
            1,1,                                        // Min, max damage
        {25,0,0,0,0,0,0},                           // Cost to buy : Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{15,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        22, 15, CPERK_NONE,                         // growth rate, power index, perks	
        SPRITE(CREAT_PEASANT), -1,                          // animation sprites beginning, ammo sprite
        ANIM_6, SA_FLY,                             // moving animation type, shooting animation type
        iPoint(0, 0), // the starting  offset of arrow flight
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT4), SOUND(DEATH3) } 
    },{
        // Archer
        2,NATION_HIGHMEN,
            5,4,
            5,1,TRANS_WALK,24,
            10,
            2,3,
        {200,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{100,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        10, 140, CPERK_DOUBLESHOT,
        SPRITE(CREAT_ARCHER), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(12, 1),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT3), SOUND(DEATH4) } 
    },{
        // Pikeman
        3,NATION_HIGHMEN,
            5,9,
            4,1,TRANS_WALK,0,
            25,
            3,4,
        {250,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{120,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        7, 250, CPERK_NONE,
        SPRITE(CREAT_PIKEMAN), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(SWORD_ATTACK1), SOUND(GETHIT4), SOUND(DEATH5) }   
    },{
        // MONK
        4,NATION_HIGHMEN,
            7,9,
            7,1,TRANS_WALK,15,
            30,
            4,6,
        {325,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{160,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 409, CPERK_NOMELEEPENALTY,
        SPRITE(CREAT_MONK), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 4),
        ANIM_6, SA_FLY,
        iPoint(28, 7),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT2), SOUND(DEATH4) }    
    },

    {
        // Cavalry
        5,NATION_HIGHMEN,
            10,9,
            9,1,TRANS_WALK,0,
            40,
            5,10,
        {450,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{280,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 689, CPERK_JOUSTING,
        SPRITE(CREAT_CAVALRY), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(SWORD_ATTACK2), SOUND(GETHIT5), SOUND(DEATH10) }   
    },{
        // Paladin
        6,NATION_HIGHMEN,
            11,13,
            8,1,TRANS_WALK,0,
            80,
            10,20,
        {1200,0,0,0,0,1,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{600,0,0,0,0,1,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 1764, CPERK_DOUBLEATTACK,
        SPRITE(CREAT_PALADIN), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(STEEL_GUARD_WALK), SOUND(SWORD_ATTACK3), SOUND(GETHIT5), SOUND(DEATH11) }   
    },

        /* Barbarian */
    {
        // Goblin
        1,NATION_BARBARIANS,
            4,1,
            5,1,TRANS_WALK,0,
            3,
            1,2,
        {40,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{10,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        20, 33, CPERK_NONE,
        SPRITE(CREAT_GOBLIN), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(BITING_ATTACK3), SOUND(GETHIT6), SOUND(DEATH6) }  
    },

    {
        // Orc
        2,NATION_BARBARIANS,
            3,4,
            4,1,TRANS_WALK,5,
            15,
            3,4,
        {185,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{90,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        10, 155, CPERK_NONE,
        SPRITE(CREAT_ORC), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(15, 11),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT_ORC), SOUND(DEATH5) }  
    },

    {
        // Warg Rider
        3,NATION_BARBARIANS,
            6,4,
            8,1,TRANS_WALK,0,
            20,
            3,5,
        {250,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{120,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 262, CPERK_DOUBLEATTACK,
        SPRITE(CREAT_WARG_RIDER), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(HAND_ATTACK2), SOUND(GETHIT6), SOUND(DEATH7) } 
    },

    {
        // Ogre
        4,NATION_BARBARIANS,
            9,7,
            5,1,TRANS_WALK,0,
            55,
            5,7,
        {500,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{250,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 627, CPERK_NONE,
        SPRITE(CREAT_OGRE), SA_FLY,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT1), SOUND(DEATH7) } 
    },

    {
        // Troll
        5,NATION_BARBARIANS,
            10,6,
            8,1,TRANS_WALK,15,
            40,
            7,12,
        {700,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{300,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 807, CPERK_REGENERATES,
        SPRITE(CREAT_TROLL), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 2),
        ANIM_6, SA_FLY,
        iPoint(26, -3),
        { SOUND(FOOTSTEPS), SOUND(THROW_ATTACK), SOUND(GETHIT_TROLL), SOUND(DEATH8) }   
    },

    {
        // Cyclop
        6,NATION_BARBARIANS,
            12,9,
            8,1,TRANS_WALK,0,
            100,
            12,24,
        {1500,0,0,0,0,1,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{700,0,0,0,0,1,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 2036, CPERK_TWOHEXATTACK,
        SPRITE(CREAT_CYCLOP), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT_GRIFFIN), SOUND(DEATH9) } 
    },

        /* Wizard */
    { 
        // Young mage
        1,NATION_WIZARDS,
            2,1,
            3,1,TRANS_WALK,5,
            4,
            1,3,
        {60,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{20,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        18, 41, CPERK_NONE,
        SPRITE(CREAT_YOUNG_MAGE), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 6),
        ANIM_6, SA_FLY,
        iPoint(17, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT6), SOUND(DEATH4) }    
    },{
        // White wolf
        2,NATION_WIZARDS,
            5,4,
            7,1,TRANS_WALK,0,
            15,
            2,3,
        {150,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{80,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 149, CPERK_NONE,
        SPRITE(CREAT_WHITE_WOLF), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(BITING_ATTACK3), SOUND(GETHIT_GRIFFIN), SOUND(DEATH7) }   
    },{
        // Living armor
        3,NATION_WIZARDS,
            6,9,
            4,1,TRANS_WALK,0,
            35,
            4,5,
        {300,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{150,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 401, CPERK_LIFELESS | CPERK_QUARTERDMG,
        SPRITE(CREAT_LIVING_ARMOR), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(STEEL_GUARD_WALK), SOUND(SWORD_ATTACK3), SOUND(GETHIT_STEELGUARD), SOUND(DEATH8) }  
    },{
        // Pegasus
        4,NATION_WIZARDS,
            7,7,
            8,1,TRANS_FLY,0,
            40,
            5,8,
        {400,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{200,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 527, CPERK_NONE,
        SPRITE(CREAT_PEGASUS), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(PEGASUS_ATTACK), SOUND(GETHIT_PEGASUS), SOUND(DEATH_PEGASUS)} 
    },{
        // Mage
        5,NATION_WIZARDS,
            12,8,
            9,1,TRANS_WALK,15,
            35,
            7,10,
        {700,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{350,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 748, CPERK_NOMELEEPENALTY, 
        SPRITE(CREAT_MAGE), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 7),
        ANIM_6, SA_FLY,
        iPoint(36, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT3), SOUND(DEATH4) }    
    },{
        // Thor
        6,NATION_WIZARDS,
            13,13,
            14,1,TRANS_WALK,15,
            300,
            15,30,
        {3500,0,0,0,1,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{2000,0,0,0,1,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 6950, CPERK_NOMELEEPENALTY,
        SPRITE(CREAT_THOR), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 9),
        ANIM_6, SA_INPLACE_2,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(LIGHTNING_STRIKE), SOUND(GETHIT_THOR), SOUND(DEATH11) }   
    },

        /* Warlock */
    {   
        // Centaur
        1,NATION_BEASTMEN,
            3,1,
            5,1,TRANS_WALK,5,
            5,
            1,2,
        {60,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{20,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        18, 47, CPERK_NONE,
        SPRITE(CREAT_CENTAUR), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(4, 1),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT1), SOUND(DEATH7) } 
    },{
        // Gargoyle
        2,NATION_BEASTMEN,
            4,7,
            9,1,TRANS_FLY,0,
            15,
            2,3,
        {200,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{100,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 175, CPERK_LIFELESS,
        SPRITE(CREAT_GARGOYLE), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(HAND_ATTACK), SOUND(GETHIT5), SOUND(DEATH10) }    
    },{
        // Griffin
        3,NATION_BEASTMEN,
            6,6,
            8,1,TRANS_FLY,0,
            25,
            3,5,
        {300,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{150,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 334, CPERK_RETALTOALL,
        SPRITE(CREAT_GRIFFIN), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(BITING_ATTACK2), SOUND(GETHIT_GRIFFIN), SOUND(DEATH9) }   
    },{
        // Minotaur
        4,NATION_BEASTMEN,
            9,8,
            7,1,TRANS_WALK,0,
            50,
            5,10,
        {500,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{250,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 682, CPERK_NONE,
        SPRITE(CREAT_MINOTAUR), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK2), SOUND(GETHIT5), SOUND(DEATH8) }    
    },{
        // Hydra
        5,NATION_BEASTMEN,
            8,9,
            4,1,TRANS_WALK,0,
            60,
            6,12,
        {750,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{350,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 872, CPERK_NONRETALATTACK | CPERK_ADJACENTATTACK,
        SPRITE(CREAT_HYDRA), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(CRAWL), SOUND(HYDRA_ATTACK), SOUND(GETHIT_TROLL), SOUND(DEATH1) }   
    },{
        // Red dragon
        6,NATION_BEASTMEN,
            13,12,
            13,1,TRANS_FLY,0,
            250,
            20,40,
        {4000,0,0,0,0,0,1},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{2000,0,0,0,0,0,1},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 8528, CPERK_ALLMAGICIMM | CPERK_TWOHEXATTACK,
        SPRITE(CREAT_RED_DRAGON), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(FIREBREATH_ATTACK), SOUND(GETHIT_DRAGON), SOUND(DEATH2) } 
    },

        /* Sorcer */
    {
        // Sprite
        1,NATION_ELVES,
            4,2,
            7,1,TRANS_FLY,0,
            2,
            1,2,
        {60,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{20,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        20, 39, CPERK_NONRETALATTACK,
        SPRITE(CREAT_SPRITE), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(FAIRY_ATTACK), SOUND(GETHIT_FAIRY), SOUND(DEATH_MEDUSA) } 
    },{
        // Dwarf
        2,NATION_ELVES,
            6,6,
            5,1,TRANS_WALK,0,
            20,
            2,4,
        {250,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{110,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 190, CPERK_40PROCRESIST,
        SPRITE(CREAT_DWARF), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK2), SOUND(GETHIT1), SOUND(DEATH5) }    
    },{
        // Elf
        3,NATION_ELVES,
            5,5,
            8,1,TRANS_WALK,20,
            15,
            2,4,
        {300,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{150,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        7, 194, CPERK_DOUBLESHOT,
        SPRITE(CREAT_ELF), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(12, 5),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT6), SOUND(DEATH5) } 
    },{
        // Druid
        4,NATION_ELVES,
            8,7,
            8,1,TRANS_WALK,15,
            25,
            5,8,
        {400,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{200,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 433, CPERK_NOMELEEPENALTY,
        SPRITE(CREAT_DRUID), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 5),
        ANIM_6, SA_FLY,
        iPoint(19, 7),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT2), SOUND(DEATH10) }   
    },{
        // Unicorn
        5,NATION_ELVES,
            10,9,
            8,1,TRANS_WALK,0,
            60,
            7,14,
        {650,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{300,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 819, CPERK_NONE,
        SPRITE(CREAT_UNICORN), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(HAND_ATTACK), SOUND(GETHIT_PEGASUS), SOUND(DEATH_PEGASUS) }    
    },{
        // Firebird
        6,NATION_ELVES,
            12,11,
            15,1,TRANS_FLY,0,
            150,
            20,40,
        {1700,0,0,1,0,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{800,0,0,1,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 3064, CPERK_FIREMAGICIMM | CPERK_TWOHEXATTACK,
        SPRITE(CREAT_FIREBIRD), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(FIREBREATH_ATTACK), SOUND(GETHIT_TROLL), SOUND(DEATH8) }  
    },


        /* Necromant */
    {
        // Skeleton
        1,NATION_UNDEADS,
            3,3,
            5,1,TRANS_WALK,0,
            4,
            1,3,
        {65,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{25,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        18, 62, CPERK_UNDEAD,
        SPRITE(CREAT_SKELETON), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(SWORD_ATTACK2), SOUND(GETHIT_SKELETON), SOUND(DEATH5) }   
    },{
        // Zombie
        2,NATION_UNDEADS,
            5,2,
            4,1,TRANS_WALK,0,
            20,
            2,3,
        {200,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{100,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 153, CPERK_UNDEAD,
        SPRITE(CREAT_ZOMBIE), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT_ORC), SOUND(DEATH6) }  
    },{
        // Lich
        3,NATION_UNDEADS,
            7,6,
            7,1,TRANS_WALK,6,
            25,
            2,3,
        {330,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{160,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 320, CPERK_UNDEAD | CPERK_NOMELEEPENALTY | CPERK_LICHSHOOT,
        SPRITE(CREAT_LICH), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 11),
        ANIM_6, SA_INPLACE_3,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK2), SOUND(GETHIT6), SOUND(DEATH7) }   
    },{
        // Vampire
        4,NATION_UNDEADS,
            8,6,
            8,1,TRANS_FLY,0,
            40,
            5,7,
        {650,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{300,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 746, CPERK_UNDEAD | CPERK_NONRETALATTACK | CPERK_DRAINLIFES,
        SPRITE(CREAT_VAMPIRE), -1,
        ANIM_18_STILLSTART, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(BITING_ATTACK), SOUND(GETHIT4), SOUND(DEATH9) }   
    },{
        // Black Knight
        5,NATION_UNDEADS,
            8,12,
            8,1,TRANS_WALK,0,
            50,
            8,14,
        {850,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{400,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 795, CPERK_UNDEAD,
        SPRITE(CREAT_BLACK_KNIGHT), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(SWORD_ATTACK3), SOUND(GETHIT5), SOUND(DEATH10) }   
    },{
        // Plague
        6,NATION_UNDEADS,
            11,9,
            8,1,TRANS_FLY,15,
            150,
            20,35,
        {2500,0,0,1,0,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{1200,0,0,1,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 3559, CPERK_UNDEAD | CPERK_FEAR,
        SPRITE(CREAT_PLAGUE), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 8),
        ANIM_18_TELEPORT, SA_FLY,
        iPoint(4, 0),
        { SOUND(PLAGUE_WALK), SOUND(MAGIC_ATTACK3), SOUND(GETHIT_PLAGUE), SOUND(DEATH_PLAGUE) } 
    },

        /* Variags */
    {
        // hunter
        1,NATION_VARIAGS,
            3,1,
            4,1,TRANS_WALK,5,
            3,
            1,3,
        {40,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{10,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        18, 62, 0,
        SPRITE(CREAT_HUNTER), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT6), SOUND(DEATH5) }   
    },{
        // blacksmith
        2,NATION_VARIAGS,
            5,2,
            4,1,TRANS_WALK,0,
            20,
            2,3,
        {250,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{100,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 153, 0,
        SPRITE(CREAT_BLACKSMITH), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT_THOR), SOUND(DEATH11) }  
    },{
        // mercenary
        3,NATION_VARIAGS,
            7,6,
            4,1,TRANS_WALK,0,
            35,
            4,5,
        {330,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{150,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 320, CPERK_NONE,
        SPRITE(CREAT_MERCENARY), -1,
        ANIM_6, SA_INPLACE_3,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(SWORD_ATTACK3), SOUND(GETHIT6), SOUND(DEATH4) }   
    },{
        // witch
        4,NATION_VARIAGS,
            8,6,
            8,1,TRANS_FLY,0,
            40,
            5,10,
        {500,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{250,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 746, CPERK_RETURN,
        SPRITE(CREAT_WITCH), -1,
        ANIM_18_STILLSTART, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(BITING_ATTACK), SOUND(GETHIT_FAIRY), SOUND(DEATH6) }   
    },{
        // volchv
        5,NATION_VARIAGS,
            11,6,
            7,1,TRANS_WALK,15,
            35,
            7,10,
        {600,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{300,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 795, CPERK_NONE,
        SPRITE(CREAT_VOLCHV), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 9),
        ANIM_6, SA_INPLACE_2,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK3), SOUND(GETHIT4), SOUND(DEATH10) }   
    },{
        // vityaz
        6,NATION_VARIAGS,
            12,11,
            9,1,TRANS_WALK,0,
            100,
            15,20,
        {1500,0,0,0,1,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{700,0,0,0,1,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 3559, CPERK_EARTHMAGICIMM | CPERK_RETALTOALL,
        SPRITE(CREAT_VITYAZ), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 8),
        ANIM_6, SA_FLY,
        iPoint(4, 0),
        { SOUND(HORSE_GALLOP), SOUND(SWORD_ATTACK3), SOUND(GETHIT6), SOUND(DEATH4) } 
    },
        // CtllTODO: add for AI PID 3559

        //////////////////////////////////////////////////////////////////////////////////////////
        // upgrade 1 & update 2
        //////////////////////////////////////////////////////////////////////////////////////////

        /* Knight */
    {
        // militia
        1,NATION_HIGHMEN,
            2,4,                                        // Attack, defence
            4,1,TRANS_WALK,0,                           // Speed, size, transportation, range attack
            4,                                          // Hit points
            1,3,                                        // Min, max damage
        {15,0,0,0,0,0,0},                           // Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        10, 15, CPERK_NONE,                         // growth rate, power index, perks
        SPRITE(CREAT_MILITIA), -1,                  // animation sprites beginning, ammo sprite
        ANIM_6, SA_FLY,                             // moving animation type, shooting animation type
        iPoint(0, 0), // the starting  offset of arrow flight
        { SOUND(FOOTSTEPS), SOUND(SWORD_ATTACK1), SOUND(GETHIT4), SOUND(DEATH3) } 
    },{    
        // citizen
        1,NATION_HIGHMEN,
            1,1,                                        // Attack, defence
            6,1,TRANS_WALK,0,                           // Speed, size, transportation, range attack
            2,                                          // Hit points
            1,1,                                        // Min, max damage
        {15,0,0,0,0,0,0},                           // Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        10, 15, CPERK_MISER,                         // growth rate, power index, perks
        SPRITE(CREAT_CITIZEN), -1,                  // animation sprites beginning, ammo sprite
        ANIM_6, SA_FLY,                             // moving animation type, shooting animation type
        iPoint(0, 0), // the starting  offset of arrow flight
        { SOUND(FOOTSTEPS), SOUND(SWORD_ATTACK2), SOUND(GETHIT4), SOUND(DEATH3) } 
    },

    {
        // crossbowman
        2,NATION_HIGHMEN,
            5,6,
            4,1,TRANS_WALK,24,
            15,
            4,5,
        {100,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        10, 140, CPERK_NONE,
        SPRITE(CREAT_CROSSBOWMAN), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(12, 1),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT3), SOUND(DEATH4) } 
    },

    {
        // sharpshooter
        2,NATION_HIGHMEN,
            7,3,
            7,1,TRANS_WALK,24,
            10,
            2,4,
        {100,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        10, 140, CPERK_DOUBLESHOT,
        SPRITE(CREAT_SHARPSHOOTER), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(12, 1),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT3), SOUND(DEATH4) } 
    },

    {
        // heavy_pikeman
        3,NATION_HIGHMEN,
            5,11,
            3,1,TRANS_WALK,0,
            35,
            3,4,
        {120,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        7, 250, CPERK_NONE,
        SPRITE(CREAT_HEAVY_PIKEMAN), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(SWORD_ATTACK1), SOUND(GETHIT5), SOUND(DEATH5) }   
    },{
        // halberdier
        3,NATION_HIGHMEN,
            8,9,
            4,1,TRANS_WALK,0,
            25,
            4,5,
        {120,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        7, 250, CPERK_NONE,
        SPRITE(CREAT_HALBERDIER), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(SWORD_ATTACK1), SOUND(GETHIT4), SOUND(DEATH5) }   
    },


    {
        // exorcist
        4,NATION_HIGHMEN,
            9,9,
            7,1,TRANS_WALK,15,
            30,
            4,8,
        {160,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 409, CPERK_NOMELEEPENALTY | CPERK_HOLY,
        SPRITE(CREAT_EXORCIST), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 4),
        ANIM_6, SA_FLY,
        iPoint(28, 7),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT2), SOUND(DEATH4) }    
    },{
        // bishop
        4,NATION_HIGHMEN,
            7,10,
            7,1,TRANS_WALK,15,
            40,
            4,6,
        {160,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 409, CPERK_NOMELEEPENALTY,
        SPRITE(CREAT_BISHOP), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 24),
        ANIM_6, SA_FLY,
        iPoint(28, 7),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK3), SOUND(GETHIT2), SOUND(DEATH4) }    
    },


    {

        // heavy_cavalry
        5,NATION_HIGHMEN,
            10,13,
            8,1,TRANS_WALK,0,
            65,
            5,10,
        {280,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 689, CPERK_JOUSTING,
        SPRITE(CREAT_HEAVY_CAVALRY), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(SWORD_ATTACK2), SOUND(GETHIT5), SOUND(DEATH10) }   
    },{
        // knight
        5,NATION_HIGHMEN,
            12,9,
            9,1,TRANS_WALK,0,
            45,
            8,10,
        {280,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 689, CPERK_JOUSTING,
        SPRITE(CREAT_KNIGHT), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(SWORD_ATTACK1), SOUND(GETHIT5), SOUND(DEATH10) }   
    },


    {
        // crusader
        6,NATION_HIGHMEN,
            11,13,
            8,1,TRANS_WALK,0,
            160,
            15,20,
        {600,0,0,0,0,1,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 1764, CPERK_NONRETALATTACK,
        SPRITE(CREAT_CRUSADER), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(STEEL_GUARD_WALK), SOUND(SWORD_ATTACK3), SOUND(GETHIT5), SOUND(DEATH11) }   
    }, {
        // holy_warrior
        6,NATION_HIGHMEN,
            13,13,
            8,1,TRANS_WALK,0,
            120,
            10,20,
        {600,0,0,0,0,1,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 1764, CPERK_DOUBLEATTACK | CPERK_HOLY,
        SPRITE(CREAT_HOLY_WARRIOR), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(STEEL_GUARD_WALK), SOUND(SWORD_ATTACK3), SOUND(GETHIT5), SOUND(DEATH11) }   
    },

        /////////////////////////////////////////////////////

        /* Barbarian */
    {
        // contagious_goblin
        1,NATION_BARBARIANS,
            5,1,
            6,1,TRANS_WALK,0,
            3,
            1,2,
        {10,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        20, 33, CPERK_NONRETALATTACK,
        SPRITE(CREAT_CONTAGIOUS_GOBLIN), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(BITING_ATTACK3), SOUND(GETHIT6), SOUND(DEATH6) }  
    },{
        // goblin_warrior
        1,NATION_BARBARIANS,
            4,4,
            4,1,TRANS_WALK,0,
            4,
            1,3,
        {10,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        20, 33, CPERK_NONE,
        SPRITE(CREAT_GOBLIN_WARRIOR), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(BITING_ATTACK3), SOUND(GETHIT6), SOUND(DEATH6) }  
    },



    {
        // orc_chief
        2,NATION_BARBARIANS,
            4,4,
            5,1,TRANS_WALK,10, // Cotulla: Jim req 5 -> 10
            15,
            3,4,
        {90,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        10, 155, CPERK_DOUBLESHOT,
        SPRITE(CREAT_ORC_CHIEF), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(15, 11),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT_ORC), SOUND(DEATH5) }  
    },{
        // orc_warrior
        2,NATION_BARBARIANS,
            3,6,
            4,1,TRANS_WALK,5,
            20,
            3,4,
        {90,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        10, 155, CPERK_NONE,
        SPRITE(CREAT_ORC_WARRIOR), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(15, 11),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT_ORC), SOUND(DEATH5) }  
    },




    {
        // wild_warg
        3,NATION_BARBARIANS,
            6,4,
            8,1,TRANS_WALK,0,
            20,
            3,5,
        {120,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 262, CPERK_DOUBLEATTACK | CPERK_NONRETALATTACK,
        SPRITE(CREAT_WILD_WARG), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(SWORD_ATTACK1), SOUND(GETHIT_GRIFFIN), SOUND(DEATH7) } 
    },{
        // warg_warrior
        3,NATION_BARBARIANS,
            8,8,
            8,1,TRANS_WALK,0,
            25,
            3,5,
        {120,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 262, CPERK_DOUBLEATTACK,
        SPRITE(CREAT_WARG_WARRIOR), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(SWORD_ATTACK2), SOUND(GETHIT_GRIFFIN), SOUND(DEATH7) } 
    },




    {
        // ogre_chief
        4,NATION_BARBARIANS,
            11,7,
            5,1,TRANS_WALK,0,
            65,
            5,9,
        {250,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 627, CPERK_NONE,
        SPRITE(CREAT_OGRE_CHIEF), SA_FLY,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT_TROLL), SOUND(DEATH9) } 
    },{
        // ogre_shaman
        4,NATION_BARBARIANS,
            9,7,
            7,1,TRANS_WALK,0,
            55,
            6,7,
        {250,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 627, CPERK_CURSE,
        SPRITE(CREAT_OGRE_SHAMAN), SA_FLY,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT1), SOUND(DEATH10) } 
    },



    {
        // ballista_troll
        5,NATION_BARBARIANS,
            10,8,
            8,1,TRANS_WALK,15,
            55,
            7,12,
        {300,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 807, CPERK_REGENERATES,
        SPRITE(CREAT_BALLISTA_TROLL), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 2),
        ANIM_6, SA_FLY,
        iPoint(26, -3),
        { SOUND(FOOTSTEPS), SOUND(THROW_ATTACK), SOUND(GETHIT_TROLL), SOUND(DEATH8) }   
    },{
        // troll_shaman
        5,NATION_BARBARIANS,
            10,6,
            8,1,TRANS_WALK,15,
            40,
            7,15,
        {300,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 807, CPERK_REGENERATES | CPERK_AIRMAGICIMM,
        SPRITE(CREAT_TROLL_SHAMAN), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 7),
        ANIM_6, SA_FLY,
        iPoint(26, -3),
        { SOUND(FOOTSTEPS), SOUND(THROW_ATTACK), SOUND(GETHIT_TROLL), SOUND(DEATH8) }   
    },


    {
        // ancient_cyclops
        6,NATION_BARBARIANS,
            12,13,
            8,1,TRANS_WALK,0,
            150,
            12,24,
        {700,0,0,0,0,1,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 2036, CPERK_TWOHEXATTACK,
        SPRITE(CREAT_ANCIENT_CYCLOPS), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK2), SOUND(GETHIT_GRIFFIN), SOUND(DEATH9) } 
    },{
        // dire_cyclops
        6,NATION_BARBARIANS,
            14,9,
            8,1,TRANS_WALK,0,
            100,
            15,24,
        {700,0,0,0,0,1,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 2036, CPERK_TWOHEXATTACK | CPERK_FURY,
        SPRITE(CREAT_DIRE_CYCLOPS), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT_GRIFFIN), SOUND(DEATH9) } 
    },


        //////////////////////////////////////////

        /* Wizard */
    { 
        // neophyte
        1,NATION_WIZARDS,
            2,3,
            3,1,TRANS_WALK,5,
            4,
            2,3,
        {20,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        18, 41, CPERK_FIREMAGICIMM,
        SPRITE(CREAT_NEOPHYTE), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 22),
        ANIM_6, SA_FLY,
        iPoint(17, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT6), SOUND(DEATH4) }    
    }, { 
        // mage_apprentice
        1,NATION_WIZARDS,
            2,1,
            5,1,TRANS_WALK,5,
            5,
            1,3,
        {20,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        18, 41, CPERK_AIRMAGICIMM,
        SPRITE(CREAT_MAGE_APPRENTICE), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 20),
        ANIM_6, SA_FLY,
        iPoint(17, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT6), SOUND(DEATH4) }    
    },


    {
        // red_wolf
        2,NATION_WIZARDS,
            6,4,
            8,1,TRANS_WALK,0,
            15,
            2,3,
        {80,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 149, CPERK_POISON,
        SPRITE(CREAT_RED_WOLF), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(BITING_ATTACK3), SOUND(GETHIT_GRIFFIN), SOUND(DEATH7) }   
    },{
        // two-headed_wolf
        2,NATION_WIZARDS,
            5,5,
            7,1,TRANS_WALK,0,
            15,
            2,3,
        {80,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 149, CPERK_ADJACENTATTACK,
        SPRITE(CREAT_TWO_HEADED_WOLF), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(BITING_ATTACK3), SOUND(GETHIT_GRIFFIN), SOUND(DEATH7) }   
    },


    {
        // steel_guard
        3,NATION_WIZARDS,
            6,11,
            4,1,TRANS_WALK,0,
            45,
            4,5,
        {150,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 401, CPERK_LIFELESS | CPERK_QUARTERDMG,
        SPRITE(CREAT_STEEL_GUARD), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(STEEL_GUARD_WALK), SOUND(SWORD_ATTACK3), SOUND(GETHIT_STEELGUARD), SOUND(DEATH8) }  
    },{
        // war_machine
        3,NATION_WIZARDS,
            9,9,
            5,1,TRANS_WALK,0,
            35,
            4,5,
        {150,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 401, CPERK_LIFELESS | CPERK_QUARTERDMG,
        SPRITE(CREAT_WAR_MACHINE), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(STEEL_GUARD_WALK), SOUND(SWORD_ATTACK3), SOUND(GETHIT_STEELGUARD), SOUND(DEATH8) }  
    },{
        // royal_pegasus
        4,NATION_WIZARDS,
            9,7,
            8,1,TRANS_FLY,0,
            40,
            5,8,
        {200,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 527, CPERK_RETALTOALL,
        SPRITE(CREAT_ROYAL_PEGASUS), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(PEGASUS_ATTACK), SOUND(GETHIT_PEGASUS), SOUND(DEATH_PEGASUS)} 
    },{
        // blinding_pegasus
        4,NATION_WIZARDS,
            7,9,
            8,1,TRANS_FLY,0,
            40,
            6,8,
        {200,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 527, CPERK_BLIND,
        SPRITE(CREAT_BLINDING_PEGASUS), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(PEGASUS_ATTACK), SOUND(GETHIT_PEGASUS), SOUND(DEATH_PEGASUS)} 
    },{
        // air_mage
        5,NATION_WIZARDS,
            13,8,
            9,1,TRANS_WALK,15,
            35,
            10,15,
        {350,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 748, CPERK_NOMELEEPENALTY | CPERK_AIRMAGICIMM,
        SPRITE(CREAT_AIR_MAGE), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 23),
        ANIM_6, SA_FLY,
        iPoint(36, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT3), SOUND(DEATH4) }    
    },{
        // prophet
        5,NATION_WIZARDS,
            12,10,
            10,1,TRANS_WALK,15,
            40,
            7,10,
        {350,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 748, CPERK_NOMELEEPENALTY | CPERK_FIREMAGICIMM,
        SPRITE(CREAT_PROPHET), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 20),
        ANIM_6, SA_FLY,
        iPoint(36, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT3), SOUND(DEATH4) }    
    },{ 
        // legend
        6,NATION_WIZARDS,
            13,15,
            14,1,TRANS_WALK,15,
            350,
            15,30,
        {2000,0,0,0,1,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 6950, CPERK_NOMELEEPENALTY,
        SPRITE(CREAT_LEGEND), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 9),
        ANIM_6, SA_INPLACE_2,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(LIGHTNING_STRIKE), SOUND(GETHIT_THOR), SOUND(DEATH11) }   
    },{
        // demigod
        6,NATION_WIZARDS,
            15,13,
            14,1,TRANS_WALK,15,
            300,
            20,40,
        {2000,0,0,0,1,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 6950, CPERK_NOMELEEPENALTY,
        SPRITE(CREAT_DEMIGOD), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 9),
        ANIM_6, SA_INPLACE_2,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(LIGHTNING_STRIKE), SOUND(GETHIT_THOR), SOUND(DEATH11) }   
    },

        //////////////////////////////////////////////////////////
        /* Warlock */
    {   
        // centeur_ranger
        1,NATION_BEASTMEN,
            4,1,
            6,1,TRANS_WALK,10,
            5,
            2,3,
        {20,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        18, 47, CPERK_NONE,
        SPRITE(CREAT_CENTEUR_RANGER), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(4, 1),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT6), SOUND(DEATH7) } 
    },{
        // centaur_lord
        1,NATION_BEASTMEN,
            3,3,
            5,1,TRANS_WALK,5,
            6,
            1,2,
        {20,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        18, 47, CPERK_NONE,
        SPRITE(CREAT_CENTAUR_LORD), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(4, 1),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT6), SOUND(DEATH7) } 
    },{
        // obsidian_gargoyle
        2,NATION_BEASTMEN,
            4,8,
            9,1,TRANS_FLY,0,
            20,
            2,3,
        {100,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 175, CPERK_LIFELESS,
        SPRITE(CREAT_OBSIDIAN_GARGOYLE), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(HAND_ATTACK), SOUND(GETHIT5), SOUND(DEATH10) }    
    },{
        // stone_gargoyle
        2,NATION_BEASTMEN,
            6,7,
            9,1,TRANS_FLY,0,
            15,
            3,5,
        {100,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 175, CPERK_LIFELESS,
        SPRITE(CREAT_STONE_GARGOYLE), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(HAND_ATTACK), SOUND(GETHIT5), SOUND(DEATH10) }    
    },{ 
        // snake_griffin
        3,NATION_BEASTMEN,
            6,8,
            8,1,TRANS_FLY,0,
            30,
            3,5,
        {150,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 334, CPERK_RETALTOALL | CPERK_NONRETALATTACK,
        SPRITE(CREAT_SNAKE_GRIFFIN), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(HYDRA_ATTACK), SOUND(GETHIT_GRIFFIN), SOUND(DEATH9) }   
    },{
        // royal_griffin
        3,NATION_BEASTMEN,
            7,6,
            8,1,TRANS_FLY,0,
            20,
            4,5,
        {150,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 334, CPERK_RETALTOALL | CPERK_ADJACENTATTACK,
        SPRITE(CREAT_ROYAL_GRIFFIN), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(BITING_ATTACK2), SOUND(GETHIT_GRIFFIN), SOUND(DEATH9) }   
    },{  
        // minotaur_reaper
        4,NATION_BEASTMEN,
            11,8,
            7,1,TRANS_WALK,0,
            50,
            5,12,
        {250,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 682, CPERK_FURY,
        SPRITE(CREAT_MINOTAUR_REAPER), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK2), SOUND(GETHIT5), SOUND(DEATH8) }    
    },{
        // dire_minotaur
        4,NATION_BEASTMEN,
            9,9,
            7,1,TRANS_WALK,0,
            60,
            7,10,
        {250,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 682, CPERK_NONE,
        SPRITE(CREAT_DIRE_MINOTAUR), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK2), SOUND(GETHIT5), SOUND(DEATH8) }    
    },{ 
        // poisonous_hydra
        5,NATION_BEASTMEN,
            8,9,
            4,1,TRANS_WALK,0,
            60,
            6,12,
        {350,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 872, CPERK_NONRETALATTACK | CPERK_ADJACENTATTACK | CPERK_POISON,
        SPRITE(CREAT_POISONOUS_HYDRA), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(CRAWL), SOUND(HYDRA_ATTACK), SOUND(GETHIT_TROLL), SOUND(DEATH1) }   
    },{
        // ancient_hydra
        5,NATION_BEASTMEN,
            8,9,
            4,1,TRANS_WALK,0,
            80,
            7,12,
        {350,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 872, CPERK_NONRETALATTACK | CPERK_ADJACENTATTACK,
        SPRITE(CREAT_ANCIENT_HYDRA), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(CRAWL), SOUND(HYDRA_ATTACK), SOUND(GETHIT_TROLL), SOUND(DEATH1) }   
    },{

        // fire_dragon
        6,NATION_BEASTMEN,
            15,13,
            13,1,TRANS_FLY,0,
            250,
            20,50,
        {2000,0,0,0,0,0,1},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 8528, CPERK_ALLMAGICIMM | CPERK_TWOHEXATTACK,
        SPRITE(CREAT_FIRE_DRAGON), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(FIREBREATH_ATTACK), SOUND(GETHIT_DRAGON), SOUND(DEATH2) } 
    },{
        // black_dragon
        6,NATION_BEASTMEN,
            13,12,
            13,1,TRANS_FLY,0,
            300,
            30,40,
        {2000,0,0,0,0,0,1},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 8528, CPERK_ALLMAGICIMM | CPERK_TWOHEXATTACK,
        SPRITE(CREAT_BLACK_DRAGON), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(FIREBREATH_ATTACK), SOUND(GETHIT_DRAGON), SOUND(DEATH2) } 
    },

        ////////////////////////////////////////////////

        /* Sorcer */
    {
        // elusive_sprite
        1,NATION_ELVES,
            6,2,
            8,1,TRANS_FLY,0,
            3,
            1,3,
        {20,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        20, 39, CPERK_NONRETALATTACK,
        SPRITE(CREAT_ELUSIVE_SPRITE), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(FAIRY_ATTACK), SOUND(GETHIT_FAIRY), SOUND(DEATH_MEDUSA) } 
    },{
        // poisonous_sprite
        1,NATION_ELVES,
            4,2,
            7,1,TRANS_FLY,0,
            2,
            1,2,
        {20,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        20, 39, CPERK_NONRETALATTACK | CPERK_POISON,
        SPRITE(CREAT_POISONOUS_SPRITE), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(FAIRY_ATTACK), SOUND(GETHIT_FAIRY), SOUND(DEATH_MEDUSA) } 
    },{

        // axe_hurler
        2,NATION_ELVES,
            6,6,
            5,1,TRANS_WALK,5,
            20,
            2,4,
        {110,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 190, CPERK_40PROCRESIST,
        SPRITE(CREAT_AXE_HURLER), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 16),
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT1), SOUND(DEATH5) }    
    },{
        // berzerker
        2,NATION_ELVES,
            12,3,
            7,1,TRANS_WALK,0,
            20,
            2,4,
        {110,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 190, CPERK_40PROCRESIST,
        SPRITE(CREAT_BERZERKER), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK2), SOUND(GETHIT1), SOUND(DEATH5) }    
    },{

        // elf_ranger
        3,NATION_ELVES,
            6,5,
            9,1,TRANS_WALK,20,
            15,
            4,5,
        {150,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        7, 194, CPERK_DOUBLESHOT,
        SPRITE(CREAT_ELF_RANGER), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(12, 5),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT6), SOUND(DEATH5) } 
    },{
        // elf_hunter
        3,NATION_ELVES,
            5,5,
            8,1,TRANS_WALK,20,
            20,
            3,4,
        {150,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        7, 194, CPERK_DOUBLESHOT,
        SPRITE(CREAT_ELF_HUNTER), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(12, 5),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT6), SOUND(DEATH5) } 
    },{

        // grand_druid
        4,NATION_ELVES,
            9,7,
            8,1,TRANS_WALK,15,
            25,
            8,10,
        {200,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 433, CPERK_NOMELEEPENALTY | CPERK_AIRMAGICIMM,
        SPRITE(CREAT_GRAND_DRUID), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 14),
        ANIM_6, SA_FLY,
        iPoint(19, 7),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT2), SOUND(DEATH10) }   
    },{
        // outcast
        4,NATION_ELVES,
            8,7,
            8,1,TRANS_WALK,15,
            35,
            6,12,
        {200,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 433, CPERK_NOMELEEPENALTY,
        SPRITE(CREAT_OUTCAST), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 15),
        ANIM_6, SA_FLY,
        iPoint(19, 7),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK), SOUND(GETHIT2), SOUND(DEATH10) }   
    },{

        // royal_unicorn
        5,NATION_ELVES,
            12,9,
            9,1,TRANS_WALK,0,
            70,
            7,14,
        {300,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 819, CPERK_BLIND,
        SPRITE(CREAT_ROYAL_UNICORN), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(HAND_ATTACK), SOUND(GETHIT_PEGASUS), SOUND(DEATH_PEGASUS) }    
    },{
        // ghost_unicorn
        5,NATION_ELVES,
            10,9,
            9,1,TRANS_WALK,0,
            60,
            7,14,
        {300,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 819, CPERK_NONRETALATTACK,
        SPRITE(CREAT_GHOST_UNICORN), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { -1, SOUND(MAGIC_ATTACK), SOUND(GETHIT_PEGASUS), SOUND(DEATH_PEGASUS) }    
    },{

        // phoenix
        6,NATION_ELVES,
            13,12,
            15,1,TRANS_FLY,0,
            150,
            30,40,
        {800,0,0,1,0,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 3064, CPERK_FIREMAGICIMM | CPERK_TWOHEXATTACK,
        SPRITE(CREAT_PHOENIX), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(FIREBREATH_ATTACK), SOUND(GETHIT_TROLL), SOUND(DEATH8) }  
    },{
        // magic_firebird
        6,NATION_ELVES,
            12,11,
            15,1,TRANS_FLY,0,
            150,
            20,40,
        {800,0,0,1,0,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 3064, CPERK_TWOHEXATTACK | CPERK_ALLMAGICIMM,
        SPRITE(CREAT_MAGIC_FIREBIRD), -1,
        ANIM_18, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(FIREBREATH_ATTACK), SOUND(GETHIT_TROLL), SOUND(DEATH8) }  
    },


        ///////////////////////////////////////////////////////////////
        /* Necromant */
    {
        // skeleton_archer
        1,NATION_UNDEADS,
            4,3,
            5,1,TRANS_WALK,5,
            4,
            1,3,
        {25,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        18, 62, CPERK_UNDEAD,
        SPRITE(CREAT_SKELETON_ARCHER), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT_SKELETON), SOUND(DEATH5) }   
    },{
        // skeleton_warrior
        1,NATION_UNDEADS,
            3,4,
            5,1,TRANS_WALK,0,
            5,
            2,3,
        {25,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        18, 62, CPERK_UNDEAD,
        SPRITE(CREAT_SKELETON_WARRIOR), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(SWORD_ATTACK2), SOUND(GETHIT_SKELETON), SOUND(DEATH5) }   
    },{

        // contagious_zombie
        2,NATION_UNDEADS,
            6,1,
            4,1,TRANS_WALK,0,
            25,
            2,3,
        {100,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 153, CPERK_NONRETALATTACK | CPERK_UNDEAD,
        SPRITE(CREAT_CONTAGIOUS_ZOMBIE), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT_ORC), SOUND(DEATH6) }  
    },{
        // tough_zombie
        2,NATION_UNDEADS,
            5,6,
            4,1,TRANS_WALK,0,
            30,
            2,3,
        {100,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 153, CPERK_UNDEAD,
        SPRITE(CREAT_TOUGH_ZOMBIE), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT_ORC), SOUND(DEATH6) }  
    },{

        // archlich
        3,NATION_UNDEADS,
            7,8,
            7,1,TRANS_WALK,6,
            35,
            2,3,
        {160,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 320, CPERK_UNDEAD | CPERK_NOMELEEPENALTY | CPERK_LICHSHOOT,
        SPRITE(CREAT_ARCHLICH), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 11),
        ANIM_6, SA_INPLACE_3,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK2), SOUND(GETHIT6), SOUND(DEATH7) }   
    },{
        // powerful_lich
        3,NATION_UNDEADS,
            9,6,
            8,1,TRANS_WALK,6,
            25,
            4,5,
        {160,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 320, CPERK_UNDEAD | CPERK_NOMELEEPENALTY | CPERK_LICHSHOOT,
        SPRITE(CREAT_POWERFUL_LICH), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 11),
        ANIM_6, SA_INPLACE_3,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK2), SOUND(GETHIT6), SOUND(DEATH7) }   
    },{

        // nosferatu
        4,NATION_UNDEADS,
            8,6,
            8,1,TRANS_FLY,0,
            30,
            5,7,
        {300,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 746, CPERK_UNDEAD | CPERK_NONRETALATTACK | CPERK_DRAINLIFES | CPERK_RETALTOALL,
        SPRITE(CREAT_NOSFERATU), -1,
        ANIM_18_TELEPORT, SA_FLY,
        iPoint(0, 0),
        { SOUND(PLAGUE_WALK), SOUND(BITING_ATTACK), SOUND(GETHIT4), SOUND(DEATH9) }   
    },{
        // vampire_lord
        4,NATION_UNDEADS,
            9,8,
            8,1,TRANS_FLY,0,
            40,
            6,7,
        {300,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 746, CPERK_UNDEAD | CPERK_NONRETALATTACK | CPERK_DRAINLIFES,
        SPRITE(CREAT_VAMPIRE_LORD), -1,
        ANIM_18_TELEPORT, SA_FLY,
        iPoint(0, 0),
        { SOUND(PLAGUE_WALK), SOUND(BITING_ATTACK), SOUND(GETHIT4), SOUND(DEATH9) }   
    },{

        // death_knight
        5,NATION_UNDEADS,
            12,12,
            8,1,TRANS_WALK,0,
            50,
            9,16,
        {400,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 795, CPERK_UNDEAD | CPERK_CURSE,
        SPRITE(CREAT_DEATH_KNIGHT), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(SWORD_ATTACK3), SOUND(GETHIT5), SOUND(DEATH10) }   
    },{
        // black_grail_knight
        5,NATION_UNDEADS,
            10,13,
            8,1,TRANS_WALK,0,
            70,
            8,14,
        {400,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 795, CPERK_UNDEAD | CPERK_BLIND,
        SPRITE(CREAT_BLACK_GRAIL_KNIGHT), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(SWORD_ATTACK3), SOUND(GETHIT5), SOUND(DEATH10) }   
    },{     

        // death
        6,NATION_UNDEADS,
            13,10,
            8,1,TRANS_FLY,0,
            180,
            20,35,
        {1200,0,0,1,0,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 3559, CPERK_UNDEAD | CPERK_FEAR | CPERK_POISON,
        SPRITE(CREAT_DEATH), -1,
        ANIM_6, SA_FLY,
        iPoint(4, 0),
        { -1, SOUND(MAGIC_ATTACK3), SOUND(GETHIT_PLAGUE), SOUND(DEATH_PLAGUE) } 
    },{
        // fear
        6,NATION_UNDEADS,
            11,9,
            8,1,TRANS_FLY,0,
            200,
            20,40,
        {1200,0,0,1,0,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 3559, CPERK_UNDEAD | CPERK_FEAR | CPERK_CURSE,
        SPRITE(CREAT_FEAR), -1,
        ANIM_18_TELEPORT, SA_FLY,
        iPoint(4, 0),
        { SOUND(PLAGUE_WALK), SOUND(MAGIC_ATTACK3), SOUND(GETHIT_PLAGUE), SOUND(DEATH_PLAGUE) } 
    },

        /////////////////////////////////////////////////
        /* Variags */
    {
        // old_hunter
        1,NATION_VARIAGS,
            4,2,
            3,1,TRANS_WALK,5,
            3,
            1,5,
        {10,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        18, 62, CPERK_NONE,
        SPRITE(CREAT_OLD_HUNTER), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT1), SOUND(DEATH5) }   
    },{
        // mature_hunter
        1,NATION_VARIAGS,
            3,4,
            5,1,TRANS_WALK,5,
            5,
            2,3,
        {10,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        18, 62, CPERK_NONE,
        SPRITE(CREAT_MATURE_HUNTER), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(ARROW_SHOOT), SOUND(GETHIT1), SOUND(DEATH5) }   
    },{

        // noble_blacksmith
        2,NATION_VARIAGS,
            7,3,
            6,1,TRANS_WALK,0,
            20,
            3,5,
        {100,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 153, CPERK_NONE,
        SPRITE(CREAT_NOBLE_BLACKSMITH), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT_THOR), SOUND(DEATH4) }  
    },{
        // werewolf
        2,NATION_VARIAGS,
            7,6,
            4,1,TRANS_WALK,0,
            30,
            2,4,
        {100,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 153, CPERK_FURY,
        SPRITE(CREAT_WEREWOLF), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(BITING_ATTACK3), SOUND(GETHIT_ORC), SOUND(DEATH7) }  
    },{

        // boyarin
        3,NATION_VARIAGS,
            8,8,
            9,1,TRANS_FLY,0,
            35,
            4,5,
        {150,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 320, CPERK_NONE,
        SPRITE(CREAT_BOYARIN), -1,
        ANIM_6, SA_INPLACE_3,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(SWORD_ATTACK3), SOUND(GETHIT6), SOUND(DEATH4) }   
    },{
        // royal_guard
        3,NATION_VARIAGS,
            8,9,
            7,1,TRANS_WALK,0,
            40,
            4,5,
        {150,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6, 320, CPERK_NONE,
        SPRITE(CREAT_ROYAL_GUARD), -1,
        ANIM_6, SA_INPLACE_3,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(SWORD_ATTACK1), SOUND(GETHIT6), SOUND(DEATH4) }   
    },{

        // enchanter
        4,NATION_VARIAGS,
            8,7,
            9,1,TRANS_FLY,0,
            45,
            5,10,
        {250,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 746, CPERK_RETURN,
        SPRITE(CREAT_ENCHANTER), -1,
        ANIM_18_STILLSTART, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(BITING_ATTACK), SOUND(GETHIT_FAIRY), SOUND(DEATH9) }   
    },{
        // yaga
        4,NATION_VARIAGS,
            8,5,
            9,1,TRANS_FLY,0,
            35,
            5,10,
        {250,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        5, 746, CPERK_NONRETALATTACK | CPERK_RETURN,
        SPRITE(CREAT_YAGA), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(HAND_ATTACK2), SOUND(GETHIT1), SOUND(DEATH9) }   
    },{

        // leshiy
        5,NATION_VARIAGS,
            11,6,
            7,1,TRANS_WALK,15,
            50,
            7,10,
        {300,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 795, CPERK_FIREMAGICIMM,
        SPRITE(CREAT_LESHIY), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 9),
        ANIM_6, SA_INPLACE_2,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(LIGHTNING_STRIKE), SOUND(GETHIT4), SOUND(DEATH10) }   
    },{
        // great_volchv
        5,NATION_VARIAGS,
            12,10,
            9,1,TRANS_WALK,15,
            35,
            8,14,
        {300,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 795, CPERK_NONE,
        SPRITE(CREAT_GREAT_VOLCHV), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 15),
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(MAGIC_ATTACK3), SOUND(GETHIT4), SOUND(DEATH10) }   
    },{

        // bogatyr
        6,NATION_VARIAGS,
            12,11,
            9,1,TRANS_WALK,24,
            150,
            15,20,
        {700,0,0,0,1,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 3559, CPERK_EARTHMAGICIMM,
        SPRITE(CREAT_BOGATYR), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(4, 0),
        { SOUND(HORSE_GALLOP), SOUND(ARROW_SHOOT), SOUND(GETHIT5), SOUND(DEATH3) } 
    },{
        // volot
        6,NATION_VARIAGS,
            12,11,
            8,1,TRANS_WALK,0,
            200,
            15,20,
        {700,0,0,0,1,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 3559, CPERK_EARTHMAGICIMM | CPERK_RETALTOALL,
        SPRITE(CREAT_VOLOT), -1,
        ANIM_6, SA_FLY,
        iPoint(4, 0),
        { SOUND(FOOTSTEPS), SOUND(SWORD_ATTACK3), SOUND(GETHIT5), SOUND(DEATH11) } 
    },

        //////////////////////////////////////////////////////////////////////////////////////////
        // neutral creatures
        //////////////////////////////////////////////////////////////////////////////////////////
        /* Neutral = 11 */
    {
        // Rogue
        1,NATION_NEUTRAL,
            6,1,
            6,1,TRANS_WALK,0,
            4,
            1,2,
        {50,0,0,0,0,0,0},                           //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        8, 54, CPERK_NONRETALATTACK,
        SPRITE(CREAT_ROGUE), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(KNIFE_ATTACK), SOUND(GETHIT3), SOUND(DEATH3) }    
    },{
        // Nomad
        3,NATION_NEUTRAL,
            7,5,
            9,1,TRANS_WALK,0,
            20,
            2,5,
        {200,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        6,244, CPERK_NONE,
        SPRITE(CREAT_NOMAD), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(SWORD_ATTACK1), SOUND(GETHIT7),  SOUND(DEATH3) }   
    },{
        // Ghost
        5,NATION_NEUTRAL,
            8,6,
            7,1,TRANS_FLY,0,
            20,
            4,6,
        {1000,0,0,0,0,0,0},                         //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 468, CPERK_UNDEAD | CPERK_GHOST,
        SPRITE(CREAT_GHOST), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { -1, SOUND(GHOST_ATTACK), SOUND(GETHIT_SKELETON),  SOUND(DEATH_GHOST) }  
    },{
        // Genie
        6,NATION_NEUTRAL,
            10,9,
            10,1,TRANS_FLY,0,
            50,
            20,30,
        {750,0,0,0,1,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        3, 1725, CPERK_DOHALF,
        SPRITE(CREAT_GENIE), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        {-1, SOUND(MAGIC_ATTACK), SOUND(GETHIT6), SOUND(DEATH8)}
    },{
        // Medusa
        4,NATION_NEUTRAL,
            8,9,
            5,1,TRANS_WALK,0,
            35,
            6,10,
        {350,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 600, CPERK_NONE,
        SPRITE(CREAT_MEDUSA), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        {SOUND(CRAWL), SOUND(MEDUSA_ATTACK), SOUND(GETHIT_FAIRY), SOUND(DEATH_MEDUSA)}
    },{
        // Earth elemental
        4,NATION_NEUTRAL,
            8,8,
            4,1,TRANS_WALK,0,
            50,
            4,5,
        {500,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 525, CPERK_EARTHMAGICIMM | CPERK_LIFELESS,
        SPRITE(CREAT_EARTH_ELEMENTAL), SA_FLY,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT5), SOUND(DEATH2) } 
    },{
        // Air elemental
        4,NATION_NEUTRAL,
            7,7,
            7,1,TRANS_WALK,0,
            35,
            2,8,
        {500,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 428, CPERK_AIRMAGICIMM | CPERK_LIFELESS,
        SPRITE(CREAT_AIR_ELEMENTAL), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT5), SOUND(DEATH2) } 
    },{
        // Fire elemental
        4,NATION_NEUTRAL,
            8,6,
            6,1,TRANS_WALK,0,
            40,
            4,5,
        {500,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 455, CPERK_FIREMAGICIMM | CPERK_LIFELESS,
        SPRITE(CREAT_FIRE_ELEMENTAL), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT5), SOUND(DEATH2) } 
    },{
        // Water elemental
        4,NATION_NEUTRAL,
            6,8,
            5,1,TRANS_WALK,0,
            45,
            3,7,
        {500,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 512, CPERK_WATERMAGICIMM | CPERK_LIFELESS,
        SPRITE(CREAT_WATER_ELEMENTAL), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(FOOTSTEPS), SOUND(HAND_ATTACK), SOUND(GETHIT5), SOUND(DEATH2) } 
    },{
        // Tatar archer
        4,NATION_NEUTRAL,
            8,6,
            6,1,TRANS_WALK,15,
            45,
            4,5,
        {500,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 455, CPERK_NONE,
        SPRITE(CREAT_TATAR_ARCHER), static_cast<sint16>(SPRITE(SHOOT_AMMO) + 3),
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(HORSE_GALLOP), SOUND(ARROW_SHOOT), SOUND(GETHIT2), SOUND(DEATH4) } 
    },{
        // Gorynych
        6,NATION_NEUTRAL,
            16,18,
            6,1,TRANS_FLY,0,
            400,
            30,70,
        {8000,0,0,0,0,0,0},                          //  Gold,   Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
		{0,0,0,0,0,0,0},                           // Upgrade cost: Gold,    Ore,    Wood,   Merc,   Gems,   Cryst,  Sulfur
        4, 6000, CPERK_ADJACENTATTACK | CPERK_REGENERATES,
        SPRITE(CREAT_GORYNYCH), -1,
        ANIM_6, SA_FLY,
        iPoint(0, 0),
        { SOUND(WING_FLAP), SOUND(FIREBREATH_ATTACK), SOUND(GETHIT5), SOUND(DEATH2) } 
    }
    };
    
    CREAT_DESC = temp;
};





//////////////////////////////////////////////////////////////////////////
const sint32 INITIAL_HERO_ARMY[HERO_TYPE_COUNT][2][3] = {
    {
        // Knight
        {
            CREAT_PEASANT, 25, 50
        },{
            CREAT_ARCHER, 0, 7
        }
        
    },{
        // Barbarian
        {
            CREAT_GOBLIN, 15, 30
        },{
            CREAT_ORC, 0, 8
        }
    },{
        // Wizard
        {
            CREAT_YOUNG_MAGE, 10, 20
        },{
            CREAT_WHITE_WOLF, 0, 6
        }
    },{
        // Warlock
        {
            CREAT_CENTAUR, 11, 22
        },{
            CREAT_GARGOYLE, 0, 6
        }
    },{
        // Sorceress
        {
            CREAT_SPRITE, 14, 28
        },{
            CREAT_DWARF, 0, 8
        }
    },{
        // Necromancer
        {
            CREAT_SKELETON, 20, 40
        },{
            CREAT_ZOMBIE, 0, 9
        }
    },{
        // Variags
        {
            CREAT_HUNTER, 20, 40
        },{
            CREAT_BLACKSMITH, 0, 9
        }
    }

};

// new helper functions here

//#endif // DO_INCLUDE_CREAT_DESC
