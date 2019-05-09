#include "stdafx.h"
#include "Observer.h"

iObserver_Player::iObserver_Player()
: iPlayer(PTM_HUMAN_ONLY, PID_NEUTRAL, NATION_NEUTRAL, iSize(gGame.Map().GetWidth(), gGame.Map().GetHeight()))
{		
	m_FogMap.DiscoverMap(0, 0, gGame.Map().GetWidth() + gGame.Map().GetHeight(), false);
}


iObserver_Player::~iObserver_Player()
{	
}

void iObserver_Player::OnSelect(bool bNewWeek, bool bAfterLoad)
{

}

void iObserver_Player::Process(fix32 t) {

}

bool iObserver_Player::NeedStopHero() const {
	return false;
}
