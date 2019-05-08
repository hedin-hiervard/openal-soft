#ifndef PH_COMMON_CM_CONSTRUCTIONS_H_
#define PH_COMMON_CM_CONSTRUCTIONS_H_

/*
 *  Predefined construction types
 */

// Visitables
enum VIS_CNST_TYPE {
    VCNST_BASIC = 0,    // Generic Visitable construction
    VCNST_STABLE,       // Adds 'n' action points (end of week to each visitor)
    VCNST_GAZEBO,       // Adds 'n' exp points (once to each visitor)
    VCNST_MANASOURCE,   // Adds 'n' mana points (end of week to each visitor)
    VCNST_MLMODIFIER,   // Modifies morale and luck
    VCNST_PSMODIFIER,   // Modifies set of primary skills by mask
    VCNST_OBELISK,      // Opens one element of puzzle map
    VCNST_SIGN,         // Shows message
    VCNST_DWELLING,     // Dwelling
    VCNST_WITCHHUT,     // Witch Hut (learn random or specified secondary skill)
    VCNST_SHRINE,       // Magic shrine (learn random spell of specified (1-3) level)
    VCNST_TREASURY,     // Resource treasury
    VCNST_TELEPORT,     // Teleports hero   
    VCNST_KEYMASTER,    // Gives specified key
    VCNST_KNWLTREE,     // Tree of Knowledge
    VCNST_WINDMILL,     // Windmill (gives random ammount of random mineral one time per week)
    VCNST_WEEKLYMIN,    // Gives fixed ammount of specified mineral(s) one time per week
    VCNST_NEWTELEPORT,
    VCNST_HARBOR,       // Harbor (Dock): build / occupy ship + market
    VCNST_WARACADEMY,   // War academy
    VCNST_COUNT                     
};

// Ownerables
enum OWN_CNST_TYPE {
    OCNST_BASIC = 0,    // Generic Ownerable construction
    OCNST_FURTSKILL,    // Modifies one or several owner's further skills
    OCNST_COUNT
};


#endif //PH_COMMON_CM_CONSTRUCTIONS_H_
