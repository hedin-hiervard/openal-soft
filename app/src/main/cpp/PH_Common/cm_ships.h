#ifndef PH_COMMON_CM_SHIPS_H_
#define PH_COMMON_CM_SHIPS_H_

struct SHIP_TYPE_DESCRIPTOR {
	sint16 sprite;     // sprite to draw
	iMineralSet cost;  // build cost of the ship	
};

#define SHIP_TYPE_COUNT 2

extern SHIP_TYPE_DESCRIPTOR *SHIP_TYPE_DESC;

void init_ships();

#endif // PH_COMMON_CM_SHIPS_H_
