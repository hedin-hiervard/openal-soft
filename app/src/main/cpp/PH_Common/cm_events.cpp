#include "stdafx.h"
#include "common.h"
#include "cm_creatures.h"
#include "cm_magic.h"
#include "cm_events.h"
#include "Army.h"
#include "serialize.h"

static sint32 EventFreqDays[iTimeEvent::EFREQ_COUNT] = {
	0, 1, 2, 3, 4, 5, 6, 7, 14, 21, 28
};

bool iTimeEvent::IsConform(PLAYER_ID pid, uint32 curDay) const
{
	// player
	if ((m_playerMask & (1<<pid)) == 0) return false;	

	if (curDay < m_fTime) {
		return false;
	} else if ( curDay == m_fTime ) {
		return true;
	} else if ( m_repTime != EFREQ_NEVER ) {
		sint32 repeat = EventFreqDays[m_repTime];
		check( repeat != 0 );
		return 0 == ( (curDay-m_fTime) % repeat );
	} else {
		return false;
	}
}



