#include "stdafx.h"
#include "cm_ships.h"

SHIP_TYPE_DESCRIPTOR *SHIP_TYPE_DESC = NULL;

void init_ships()
{
    static SHIP_TYPE_DESCRIPTOR temp[SHIP_TYPE_COUNT] =
    {
        {
            PDGG_SINT16(SHIPS),
            {10000,0,40,0,0,0,0},
            
        },
        {
            static_cast<sint16>(PDGG_SINT16(SHIPS) + 48),
            {10000,0,40,0,0,0,0},
        }
    };
    
    SHIP_TYPE_DESC = temp;
}
