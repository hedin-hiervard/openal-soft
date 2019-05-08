#include "stdafx.h"

#ifdef _DEEP_DEBUG_
#	define new DEBUG_NEW
#endif 

#include "ai.scanner.h"

/*
 *	Danger map
 */
iDangerMap::iDangerMap(PLAYER_ID owner)
: m_owner(owner)
, m_dangerMap(gGame.Map().m_PassMap.GetWidth(),gGame.Map().m_PassMap.GetHeight())
, m_actorMap(gGame.Map().m_PassMap.GetWidth(),gGame.Map().m_PassMap.GetHeight())
{
	m_actorMap.ZeroMem(0xFF);
	m_dangerMap.ZeroMem(0);
}

void iDangerMap::UpdateDangerMap()
{
	iTimer timer;
	// Reset danger map
	m_actorMap.ZeroMem(0xFF);
	m_dangerMap.ZeroMem(0);

	// Prepare actors list
	m_enActors.RemoveAll();
	for (iGameWorld::iPLIt pit = gGame.Map().m_Players.First(); pit != gGame.Map().m_Players.End(); ++pit) {
		if ((*pit)->PlayerId() == m_owner) continue;
		for (iPlayer::iHLIt hit = (*pit)->HeroFirst(); hit != (*pit)->HeroEnd(); ++hit) {
			m_enActors.Add(*hit);
		}
	}

	// No enemy heroes, so no danger
	if (!m_enActors.GetSize()) return;

	// Make danger map
	for (uint32 actorId = 0; actorId < m_enActors.GetSize(); ++actorId) {
		// Prepare pass map
		iDistMapProcessor hdm(iSize(gGame.Map().m_PassMap.GetWidth(),gGame.Map().m_PassMap.GetHeight()));
		hdm.MakeDistMap(m_enActors[actorId]->Pos(), m_enActors[actorId]->Army().ArmyPower(), m_enActors[actorId]->MoveCostBonus(), m_enActors[actorId]->InShip(), m_enActors[actorId]->Owner()->PlayerId(), m_enActors[actorId]->TotalMoves());
		iDistMapProcessor::iMark* pMark = hdm.m_distMap.GetPtr();
		uint8* pActorId = m_actorMap.GetPtr();
		uint32* pDanger = m_dangerMap.GetPtr();
		sint32 count = m_actorMap.GetWidth() * m_actorMap.GetHeight();
		while (count--) {
			uint16 len = pMark->move;
			if(len == 0x3FFF) len = pMark->touch;
			if (len != 0x3FFF) {
				len = iMAX<uint16>(len, 1);
				if ( *pActorId == 0xFF || m_enActors[actorId]->Army().ArmyPower() * 3 / len > *pDanger) {
					*pActorId = (uint8)actorId;
					*pDanger = (uint32)(m_enActors[actorId]->Army().ArmyPower() * 3 / len);
					
				}
			}
			pActorId++;
			pDanger++;
			pMark++;
		}
	}

	// Prepare danger enemy heroes list
	iPlayer* pOwner = gGame.Map().FindPlayer(m_owner);
	check(pOwner);
	for (iPlayer::iCtLIt cit = pOwner->CastleFirst(); cit != pOwner->CastleEnd(); ++cit) {
		uint8 eidx = m_actorMap.GetAt((*cit)->Pos().x, (*cit)->Pos().y);
		if (eidx != 0xFF) {
			m_danActors.Add(m_enActors[eidx]);
		}
	}
}
