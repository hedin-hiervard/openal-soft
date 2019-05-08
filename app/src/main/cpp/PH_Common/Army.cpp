#include "stdafx.h"
#include "common.h"
#include "cm_creatures.h"

#include "Army.h"

//////////////////////////////////////////////////////////////////////////
iCreatGroup::iCreatGroup(CREATURE_TYPE ct, sint32 count)
: m_creatType(ct), m_count(count) 
{
}

const sint32 CREAT_GROWTH_DIVIDER[6] = {
	9, 9, 10, 10, 11, 12
};

const sint32 CREAT_GROWTH_DIVIDER_DEC[6] = {
    30,	30,	40,	40,	60,	70
};

CREATURE_TYPE iCreatGroup::GetUpgradedType(UpgradeWay way)  const
{
	if (m_creatType >= CREAT_PEASANT && m_creatType <= CREAT_VITYAZ)    
        return (CREATURE_TYPE)((m_creatType - CREAT_PEASANT) * 2 + CREAT_MILITIA + (uint32)way);        
	else
		return CREAT_UNKNOWN;
}

void iCreatGroup::Upgrade(UpgradeWay way)	
{
    CREATURE_TYPE ct = GetUpgradedType(way);	

    if (m_creatType == CREAT_UNKNOWN)    
        return;    

    m_creatType = ct;
}



void iCreatGroup::Grow(uint32 weeks)
{
	check(m_count != RAND_VAL);
	while (weeks) {
		sint32 div = (gGame.Map().GameDay() < 8 * 4 * 7) ? CREAT_GROWTH_DIVIDER[CREAT_DESC[m_creatType].level-1] : 
            CREAT_GROWTH_DIVIDER_DEC[CREAT_DESC[m_creatType].level-1];
		if (m_count < CREAT_DESC[m_creatType].growth*2) div /= 2;
		
		m_count = m_count + (m_count+(div-1)) / div;

		weeks--;
	}
}


//////////////////////////////////////////////////////////////////////////
bool iArmy::CanAddGroup(CREATURE_TYPE ct) const
{
	check(ct != CREAT_UNKNOWN);
	for (uint32 xx=0; xx<7; ++xx){
		if (!m_creatGroups[xx].IsValid() ||  m_creatGroups[xx].Type() == ct) return true;
	}
	return false;
}

sint32 iArmy::CreatureCount(CREATURE_TYPE ct) const
{
	sint32 res = 0;
	for (uint32 xx=0; xx<7; ++xx)
		if (m_creatGroups[xx].Type() == ct)
			res += m_creatGroups[xx].Count();
	return res;
}

sint32 iArmy::AddGroup(CREATURE_TYPE ct, sint32 count)
{
	sint32 fEmpty = -1;

	check(ct != CREAT_UNKNOWN);
		
	if(count > 0) {
		for (uint32 xx=0; xx<7; ++xx){
			if (m_creatGroups[xx].Type() == ct){
				m_creatGroups[xx].Count() += count;							
				return xx;
			} else if (fEmpty == -1 && !m_creatGroups[xx].IsValid()) {
				fEmpty = xx;
			}
		}

		if (fEmpty != -1){
			m_creatGroups[fEmpty].Type() = ct;
			m_creatGroups[fEmpty].Count() = count; 
			return fEmpty;
		}

		return -1; //no free cell
	} else  {
		for (uint32 xx=0; xx<7; ++xx){
			if (m_creatGroups[xx].Type() == ct) {
				if(m_creatGroups[xx].Count() + count >= 0) {
					m_creatGroups[xx].Count() += count;							
				} else {
					count += m_creatGroups[xx].Count();
					m_creatGroups[xx].Count() = 0;
				}
				
				if(m_creatGroups[xx].Count() <= 0)
					m_creatGroups[xx].Type() = CREAT_UNKNOWN;

				if(count >= 0) // no substractions left
					return xx;
			}
		}	
		return -1; 
	}
}

sint32 iArmy::SlowestSpeed() const
{ 
	sint32 res=SPEED_MAX; 
	for (uint32 xx=0; xx<7; ++xx) 
		if (m_creatGroups[xx].IsValid() && CREAT_DESC[m_creatGroups[xx].Type()].speed < res) 
			res = CREAT_DESC[m_creatGroups[xx].Type()].speed; 
	return res; 
}

const iCreatGroup& iArmy::WeakestCreatures() const
{
	sint32 idx=-1;
	for (sint32 xx=0; xx<7; ++xx) {
		if (m_creatGroups[xx].Type() != CREAT_UNKNOWN && (idx == -1 || CREAT_DESC[m_creatGroups[idx].Type()].pidx > CREAT_DESC[m_creatGroups[xx].Type()].pidx)) idx=xx;
	}
	check(idx != -1);
	return m_creatGroups[idx];
}

sint32 iArmy::WeakestCreaturesIdx() const
{
	sint32 idx=-1;
	for (sint32 xx=0; xx<7; ++xx) {
		if (m_creatGroups[xx].Type() != CREAT_UNKNOWN && (idx == -1 || CREAT_DESC[m_creatGroups[idx].Type()].pidx > CREAT_DESC[m_creatGroups[xx].Type()].pidx)) idx=xx;
	}
	check(idx != -1);
	return idx;
}


const iCreatGroup& iArmy::WeakestGroup() const
{
	sint32 idx=-1;
	for (sint32 xx=0; xx<7; ++xx) {
		if (m_creatGroups[xx].Type() != CREAT_UNKNOWN && (idx == -1 || m_creatGroups[idx].GroupPower()>m_creatGroups[xx].GroupPower())) idx=xx;
	}
	check(idx != -1);
	return m_creatGroups[idx];
}

//////////////////////////////////////////////////////////////////////////
struct RAND_QUANTITY_ITEM {
	sint32	minVal;
	sint32	maxVal;
};

const RAND_QUANTITY_ITEM	INITIAL_QUNTITY[6] = {
	{750,1500}, {1000,2000}, {1500,3000}, {2000,4000}, {3000,6000}, {10000,25000}
};



sint32 InitialTroopQuantity(CREATURE_TYPE ct)
{
#ifdef _HMM_GAME_
	sint32 minVal = INITIAL_QUNTITY[CREAT_DESC[ct].level-1].minVal / CREAT_DESC[ct].pidx;
	sint32 maxVal = INITIAL_QUNTITY[CREAT_DESC[ct].level-1].maxVal / CREAT_DESC[ct].pidx;
	return  minVal + gGame.Map().Rand((maxVal-minVal));
#else
	return 0;
#endif
}


void iCreatGroup::Init()
{
#ifdef _HMM_GAME_
	if (IsValid() && Count() == RANDOM_QUANTITY) 
		Count() = InitialTroopQuantity(Type());
#endif
}

uint32 RandCreaturesCount(CREATURE_TYPE type);

void iCreatGroup::InitRandoms()
{
#ifdef _HMM_GAME_
	if (IsValid() && Count() == RANDOM_QUANTITY) 
		Count() = RandCreaturesCount(Type());
#endif
}


sint32 iArmy::WeakestGroupIdx() const
{
	sint32 idx=-1;
	for (sint32 xx=0; xx<7; ++xx) {
		if (m_creatGroups[xx].Type() != CREAT_UNKNOWN && (idx == -1 || m_creatGroups[idx].GroupPower()>m_creatGroups[xx].GroupPower())) idx=xx;
	}
	return idx;
}


bool iArmy::JoinGroups()
{
	bool bJoined = false;
	for (uint32 xx=0; xx<7; ++xx) {
		for (uint32 yy=(xx+1); yy<7; ++yy) {
			if (m_creatGroups[xx].Type() != CREAT_UNKNOWN && m_creatGroups[xx].Type() == m_creatGroups[yy].Type()) {
				m_creatGroups[xx].Count() += m_creatGroups[yy].Count();
				m_creatGroups[yy].Reset();
				bJoined = true;
			}
		}
	}
	return bJoined;
}

void iArmy::ResetGroup(uint32 idx)
{ 
	m_creatGroups[idx].Reset();
}

void iArmy::ResetArmy()
{
	for (uint32 xx=0; xx<7; ++xx) m_creatGroups[xx].Reset();
}

void iArmy::SetGroup(uint32 idx, CREATURE_TYPE ct, sint32 count)
{
	check(idx<7);
	m_creatGroups[idx].Type() = ct;
	m_creatGroups[idx].Count() = count;
}

CREATURE_TYPE iArmy::StrongestCreature() const
{
	CREATURE_TYPE ct = CREAT_UNKNOWN;
	sint32 pidx = 0;
	for (uint32 xx=0; xx<7; ++xx) {
		if (m_creatGroups[xx].Type() != CREAT_UNKNOWN && CREAT_DESC[m_creatGroups[xx].Type()].pidx > pidx) {
			pidx = CREAT_DESC[m_creatGroups[xx].Type()].pidx;
			ct = m_creatGroups[xx].Type();
		}
	}
	check(ct != CREAT_UNKNOWN);
	return ct;
}

sint32 iArmy::FearFactor() const
{
	sint32 res = 0;
	iSimpleArray<sint32> types;
	for (uint32 xx=0; xx<7; ++xx) {
		if (m_creatGroups[xx].Type() != CREAT_UNKNOWN &&
			types.Find(m_creatGroups[xx].Type()) == -1 && 
			CREAT_DESC[m_creatGroups[xx].Type()].perks & CPERK_FEAR) {
			types.Add(m_creatGroups[xx].Type());
			res += 1;
		}
	}
	return res;
}

sint32 iArmy::MoraleModifier() const
{
	sint32 res = 0;
	bool bUndeads = false;
	uint32 alignments = 0;
	uint32 nat_mask = 0;
	for (uint32 xx=0; xx<7; ++xx) {
		if (m_creatGroups[xx].Type() != CREAT_UNKNOWN && (nat_mask & (1<<CREAT_DESC[m_creatGroups[xx].Type()].nation)) == 0 ) {
			alignments ++;
			nat_mask |= (1<<CREAT_DESC[m_creatGroups[xx].Type()].nation);
			if (CREAT_DESC[m_creatGroups[xx].Type()].nation == NATION_UNDEADS) bUndeads = true;
		}
	}

	// Troops of >=5 alignment -3
	if (alignments >= 5) res -= 3;
	// Troops of 4 alignment -2
	else if (alignments == 4) res -= 2;
	// Troops of 3 alignment -1
	else if (alignments == 3) res -= 1;
	// All troops of one alignment +1
	else if (alignments <= 1) res += 1;
	// Undead in army -1
	if (bUndeads) res -= 1;
	
	return res;
}

