#ifdef MULTIPLAYER

#include "stdafx.h"
#include "HiddenBattle.h"
#include "BattleAI.h"
#include "Dlg_BattleResult.h"

iHiddenBattle::iHiddenBattle(bool bShowResult)
: iBattleWrapper(true), m_bShowResult(bShowResult)
{
}

void iHiddenBattle::OnBeginBattle()
{
	gMPMgr.SetBattleWrapper(this);
	
	iPoint dpos = m_engine.GetBattleInfo().Defender()->Pos();
	iCell c = gGame.Map().GetAt(dpos.x,dpos.y);
		
	SURF_TYPE st = (SURF_TYPE)c.SurfNode(0);
	
	// do a random sky if needed to keep the RNG synced
	SURF_TYPE upSurf = (SURF_TYPE)c.SurfNode(0);
	BATTLE_BACK_TYPE back = BBTYPE_GRASS;
	BATTLE_SKY_TYPE sky = BSTYPE_GRASS_FOREST;
	bool bIsSea = false;
	for(sint32 dx = -3; dx <= 3; dx++) {
		for(sint32 dy = -3; dy <= 3; dy++)
			if(gGame.Map().IsValidPos(dpos.x + dx, dpos.y + dy)) {
				iCell c = gGame.Map().GetAt(dpos.x + dx,dpos.y + dy);	
				SURF_TYPE surf = (SURF_TYPE)c.SurfNode(0);
				if(surf == STYPE_WATER) {
						bIsSea = true;
						break;
				} else if(upSurf == STYPE_WATER)
					upSurf = surf;
			}
		if(bIsSea)
			break;
	}	
	if( DynamicCast<iBattleMember_Castle*>(m_engine.GetBattleInfo().m_pDefender)) {
		back = BBTYPE_BRICK;
		sky = BSTYPE_BRICK_CITY;
	} else if(m_engine.OnShip()) {
		back = BBTYPE_SHIP;
		sky = BSTYPE_SHIP;
	} else {
		switch(upSurf) {
			case STYPE_WATER:
				check(0);
				break;
			case STYPE_SAND:
				back = BBTYPE_BEACH;
				sky = BSTYPE_BEACH_SEA;
				break;
			case STYPE_DIRT:
				back = BBTYPE_DIRT;
				if(bIsSea)
					sky = BSTYPE_DIRT_SEA;
				else {
					sky = gGame.Map().Rand(1) == 0 ? BSTYPE_DIRT_MOUNT : BSTYPE_DIRT_FOREST;
				}			
				break;
			case STYPE_GRASS:
				back = BBTYPE_GRASS;
				if(bIsSea)
					sky = BSTYPE_GRASS_SEA;
				else
					sky = BSTYPE_GRASS_FOREST;
				break;
			case STYPE_SWAMP:
				back = BBTYPE_SWAMP;
				sky = BSTYPE_SWAMP_FOREST;
				break;
			case STYPE_LAVA:
				back = BBTYPE_LAVA;
				sky = BSTYPE_LAVA_MOUNT;
				break;
			case STYPE_WASTELAND:
				back = BBTYPE_WASTELAND;
				sky = BSTYPE_WASTELAND_MOUNT;
			break;
			case STYPE_DESERT:
				back = BBTYPE_GOLDEN_SAND;
				if(bIsSea)
					sky = BSTYPE_GOLDEN_SAND_SEA;
				else
					sky = BSTYPE_GOLDEN_SAND_MOUNT;
				break;
			case STYPE_SNOW:
				back = BBTYPE_SNOW;
				if(bIsSea)
					sky = BSTYPE_SNOW_SEA;
				else
					sky = BSTYPE_SNOW_MOUNT;
				break;
			case STYPE_NDESERT:
				back = BBTYPE_GOLDEN_SAND;
				if(bIsSea)
					sky = BSTYPE_GOLDEN_SAND_SEA;
				else
					sky = BSTYPE_GOLDEN_SAND_MOUNT;			
				break;
			case STYPE_PAVEMENT:
				back = BBTYPE_BRICK;
				sky = BSTYPE_BRICK_CITY;
				break;
			case STYPE_NWASTELAND:
				back = BBTYPE_BARREN;
				sky = BSTYPE_BARREN_MOUNT;
				break;
			default:
				check(0);
		}
	}

	// Prepare and distribute obstacles
	if (!m_engine.CastleFort() && !m_engine.OnShip()) {
		iSimpleArray<uint16> olist;
		gGame.ItemMgr().BatObstMgr().SelectItems(olist, st);
		uint32 max_count = 5;
		while (max_count-- && olist.GetSize()) {
			uint32 rval = gGame.Map().Rand(olist.GetSize());
			uint8 size = gGame.ItemMgr().BatObstMgr().Item(olist[rval]).size;
			iPoint op;
			do {
				op.x = 3+gGame.Map().Rand(7);
				op.y = 1+gGame.Map().Rand(9);
			} while (m_engine.ObstaclesMap().GetAt(op.x, op.y) == CT_OBSTACLE || (size == 2 && m_engine.ObstaclesMap().GetAt(op.x+1, op.y) == CT_OBSTACLE));
			
			m_engine.ObstaclesMap().GetAt(op.x, op.y) = CT_OBSTACLE;
			if (size == 2) m_engine.ObstaclesMap().GetAt(op.x+1, op.y) = CT_OBSTACLE;
			olist.RemoveAt(rval);
		}
	}	


}

bool iHiddenBattle::Step(iBattleResult &br) 
{
	bool bCasted = ProcessSpellCastAI(m_engine);	

TODO: check
	if (!CheckBattleStateAI(m_engine, br, false)) {
		return true;
	}

	if (!bCasted && !ProcessBattleStepAI(m_engine)) {
		// waited		
		return false;
	}

	while (m_engine.ActionCount()) {
		iBattleAct* pAct = m_engine.StepAction();
		check(pAct);
		delete pAct;
	}
	if (m_engine.CheckBattleState() != BR_NA) {
		br.m_result = m_engine.CheckBattleState();
		br.m_defCause = DC_DEFEAT;
		return true;
	}	
	m_engine.NextGroup();	
	return false;	
}

void iHiddenBattle::OnNextGroup() {
	if(gMPMgr.IsReplaying())
		return;
	iBattleResult br;
	
	while(true) {
		if (m_engine.CheckBattleState() != BR_NA) {
			br.m_result = m_engine.CheckBattleState();
			br.m_defCause = DC_DEFEAT;
			m_engine.PrepareBattleResult(br);	
			gMPMgr.OnCombatEnd(br);
			EndBattle(br);			
			return;
		}		

		iBattleUnit* pCurUnit = m_engine.TurnSeq().CurUnit();
			
		if (iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(pCurUnit)) {
			iBattleGroup* pGroup  = pCurCreatGroup->GetCreatGroup();
			
			// First check morale
			if (pGroup->RoundMorale() == iBattleGroup::MLM_NEGATIVE) {
				m_engine.BadMorale();
				continue;
			}	
			if(pCurCreatGroup->Owner()->Owner() != PID_NEUTRAL && gGame.Map().FindPlayer(pCurCreatGroup->Owner()->Owner())->PlayerType() != PT_COMPUTER)
				return; // wait for remote side

			// the next action is from AI. so disable PRA now and further...
			gMPMgr.DisablePRA();

			if(Step(br)) {			
				// battle ended
				m_engine.PrepareBattleResult(br);	
				gMPMgr.OnCombatEnd(br);
				EndBattle(br);			
				return;
			} 
		} else if (DynamicCast<iBattleUnit_Catapult*>(pCurUnit)) {
			iCastleFort::ElementType target = m_engine.CastleFort()->GetTarget();
			if (target != iCastleFort::InvalidElement) {
				m_engine.CatapultShot(target);							
				while (m_engine.ActionCount()) {
					iBattleAct* pAct = m_engine.StepAction();
					check(pAct);
					delete pAct;
				}						
				m_engine.NextGroup();	
			}
		} else if (DynamicCast<iBattleUnit_Turret*>(pCurUnit)) {
			iBattleGroup* pTarget = SelectTurretTarget(m_engine);		
			m_engine.TurretShot(pTarget);		
			while (m_engine.ActionCount()) {
				iBattleAct* pAct = m_engine.StepAction();
				check(pAct);
				delete pAct;
			}						
			m_engine.NextGroup();	
		}
	}
}

void iHiddenBattle::PlayAnim()
{	
	while(true) {
		iBattleUnit* pCurUnit = m_engine.TurnSeq().CurUnit();
			
		if (iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(pCurUnit)) {
			if(m_engine.ActionCount()) {
				while (m_engine.ActionCount()) {
					iBattleAct* pAct = m_engine.StepAction();
					check(pAct);
					delete pAct;
				}							
				m_engine.NextGroup();	
			} else
				return;
		} else if (DynamicCast<iBattleUnit_Catapult*>(pCurUnit)) {
			iCastleFort::ElementType target = m_engine.CastleFort()->GetTarget();
			if (target != iCastleFort::InvalidElement) {
				m_engine.CatapultShot(target);							
				while (m_engine.ActionCount()) {
					iBattleAct* pAct = m_engine.StepAction();
					check(pAct);
					delete pAct;
				}						
				m_engine.NextGroup();	
			}
		} else if (DynamicCast<iBattleUnit_Turret*>(pCurUnit)) {
			iBattleGroup* pTarget = SelectTurretTarget(m_engine);		
			m_engine.TurretShot(pTarget);		
			while (m_engine.ActionCount()) {
				iBattleAct* pAct = m_engine.StepAction();
				check(pAct);
				delete pAct;
			}						
			m_engine.NextGroup();	
		} else {
			if(m_engine.ActionCount()) {
				while (m_engine.ActionCount()) {
					iBattleAct* pAct = m_engine.StepAction();
					check(pAct);
					delete pAct;
				}		
				m_engine.NextGroup();	
			} else
				return;
		}
	}	
}

void iHiddenBattle::OnStart()
{
	OnNextGroup();
}

void iHiddenBattle::OnEndBattle()
{
	/*
	int a = 0;
	const iArmy& aArmy = m_engine.GetBattleInfo().m_pAssaulter->Army();
	for (uint32 aidx=0; aidx<m_engine.AArmy().Count(); ++aidx) {
	}

	const iArmy& dArmy = m_engine.GetBattleInfo().m_pDefender->Army();
	for (uint32 didx=0; didx<m_engine.DArmy().Count(); ++didx) {
	}
	*/
}

void iHiddenBattle::AddLogEvent(const iStringT& msg) 
{
}

void iHiddenBattle::AddCellEvent(const iStringT& msg, const iPoint& pos)
{
}


#endif //MULTIPLAYER