#include "stdafx.h"
#include "serialize.h"

void iArmyOwner::CheckVictory()
{
	if(GetArmyOwnerPlayer())
		gGame.CheckPlayerWinAndLose(GetArmyOwnerPlayer());
}

bool iArmyOwner::AddGroupToArmy(CREATURE_TYPE ct, sint32 count)
{
	sint32 res = m_Army.AddGroup(ct, count);
#ifdef MULTIPLAYER
	if(m_pOwner->PlayerType() != PT_REMOTE)
		gMPMgr.OniHeroUpdate(pHero);
#endif
	if(ct != CREAT_UNKNOWN && count != 0)
		m_changedCell = res;
	 
	CheckVictory();
	return res != -1 || count <= 0;
}

bool iArmyOwner::JoinArmyGroups()
{
	bool res = m_Army.JoinGroups();
#ifdef MULTIPLAYER
	if(m_pOwner->PlayerType() != PT_REMOTE)
		gMPMgr.OnHeroUpdate(pHero);
#endif	
	CheckVictory();
	
	return res;
}


void iArmyOwner::UpgradeArmyGroup(uint32 idx, iCreatGroup::UpgradeWay uw)
{ 
	m_Army.At(idx).Upgrade(uw); 
#ifdef MULTIPLAYER
	if(m_pOwner->PlayerType() != PT_REMOTE)
		gMPMgr.OnHeroUpdate(pHero);
#endif
	m_changedCell = idx;
	CheckVictory();
}

void iArmyOwner::ResetArmy()
{
	m_Army.ResetArmy();
#ifdef MULTIPLAYER
	if(m_pOwner->PlayerType() != PT_REMOTE)
		gMPMgr.OnHeroUpdate(pHero);
#endif
	CheckVictory();
}

void iArmyOwner::SetArmyGroup(sint32 idx, const iCreatGroup& cg)
{
	m_Army[idx] = cg;
#ifdef MULTIPLAYER
	if(m_pOwner->PlayerType() != PT_REMOTE)
		gMPMgr.OnHeroUpdate(pHero);
#endif
	m_changedCell = idx;
	CheckVictory();	
}

void iArmyOwner::ResetArmyGroup(sint32 idx)
{
	m_Army[idx].Reset();
#ifdef MULTIPLAYER
	if(m_pOwner->PlayerType() != PT_REMOTE)
		gMPMgr.OnHeroUpdate(pHero);
#endif
//	m_changedCell = idx;
	CheckVictory();
	
}

void iArmyOwner::SetArmyGroupCount(sint32 idx, sint32 count)
{
	m_Army[idx].Count() = count;
#ifdef MULTIPLAYER
	if(m_pOwner->PlayerType() != PT_REMOTE)
		gMPMgr.OnHeroUpdate(pHero);
#endif
	m_changedCell = idx;
	CheckVictory();
}

void iArmyOwner::AddArmyGroupCount(sint32 idx, sint32 add)
{
	m_Army[idx].Count() += add;
	m_changedCell = idx;
	CheckVictory();
}

void iArmyOwner::SetArmyGroupType(sint32 idx, CREATURE_TYPE ct)
{
	m_Army[idx].Type() = ct;
	m_changedCell = idx;
	CheckVictory();
}

void iArmyOwner::InitArmy()
{
	for (uint32 xx=0; xx<7; ++xx) m_Army[xx].Init();
	m_changedCell = -1;
}

void iArmyOwner::InitArmyRandoms()
{
	for (uint32 xx=0; xx<7; ++xx) m_Army[xx].InitRandoms();	
	m_changedCell = -1;
}


void iArmyOwner::GrowArmyGroup(sint32 idx, uint32 weeks)
{
	m_Army[idx].Grow(weeks);
	m_changedCell = idx;
	CheckVictory();
}

void iArmyOwner::UnserializeArmy(iDynamicBuffer& dbuff)
{
	::Unserialize(dbuff, m_Army);
	m_changedCell = -1;
}

void iArmyOwner::SetArmy(const iArmy& anotherArmy)
{
	m_Army = anotherArmy;
	m_changedCell = -1;
	CheckVictory();
}

void iArmyOwner::SwapArmyGroups(sint32 idx1, sint32 idx2, iArmyOwner* other)
{
	iSwap(m_Army.At(idx1), (other ? (other->m_Army) : m_Army).At(idx2)) ;
	CheckVictory();
}

uint32 iArmyOwner::TotalCreatures() const
{
	uint32 res = 0;
	for(uint32 xx=0; xx<7; xx++)
		res += m_Army.At(xx).Count();
	return res;
}
