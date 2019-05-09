#include "stdafx.h"

#ifdef _DEEP_DEBUG_
#	define new DEBUG_NEW
#endif 

#include "BattleAI.h"
#include "BattlePF.h"
#include "Dlg_TownList.h"

/*
 *	Debug
 */

#define DEBUG_BATTLE_AI
#ifdef DEBUG_BATTLE_AI
#define _LOG(x) ODS(x);
#else
#define _LOG(x)
#endif //DEBUG_BATTLE


/*
	if (shoot list is not empty) {
		for each shoot list item as sitem {
			calculate target index for sitem {
				+ Damaging to sitem
				+ Max possible damage from sitem (check shoot and melee list of sitem)
			}
		}
		select target with bigger index and return;
	}

	if (melee list is not empty) {
		for each melee list item as mitem {
			calculate target index for mitem {
				+ Max Damaging to mitem
				+ Max possible damage from mitem (check shoot and melee list of sitem)
				- Retalation damage from mitem
			}
			for each mitem direction {
				find most safe and productive direction {
					- Max damaging from all enemy troops to specified cell
					+ Suitable position for next turn melee or shoot attack
				}
			}
		}
		select target and direction with bigger index and return;
	}

	for each enemy group as egroup {
		calculate distance to egroup {
		}
	}

	find path to closest enemy group {
		select cell and return;
	}
	
 */

const sint16 FORT_CELLS[11] = { 8, 8, 7 , 7, 6, 6, 6, 7, 7, 8, 8 };
//////////////////////////////////////////////////////////////////////////
inline bool InCastle(const iPoint& pos) 
{
	check(pos.y<11);
	return pos.x >= FORT_CELLS[pos.y];
}

//////////////////////////////////////////////////////////////////////////
iBattleAI::iBattleAI(iBattleEngine& engine)
: m_engine(engine)
{
}

//////////////////////////////////////////////////////////////////////////
iBattleGroup* SelectTurretTarget(iBattleEngine& engine)
{
	sint32 sidx = -1;
	sint32 sval = 0;

	for (uint32 xx=0; xx<engine.AArmy().Groups().GetSize(); ++xx) {
		iBattleGroup* pGroup = engine.AArmy().Groups()[xx];
		if (pGroup->IsAlive()) {
			sint32 nval = 1;
			if (pGroup->TransType() == TRANS_FLY) nval += 2;
			if (pGroup->CanShoot() && pGroup->Shots()) nval += 3;
			if (InCastle(pGroup->Pos())) nval += 4;
			if (nval > sval) {
				sidx = xx;
				sval = nval;
			}
		}
	}
	check(sidx != -1);
	return engine.AArmy().Groups()[sidx];
}

sint32 CalcShootValue(iBattleGroup* pGroup, const iBattleGroup::iShootEntry* se, iBattleGroup* pTarget)
{
	sint32 val = 0;
	sint32 dist = BattleCellsDelta(pGroup->Pos().x, pGroup->Pos().y, pTarget->Pos().x, pTarget->Pos().y);
	
	// Speed - Distance
	if (pTarget->Speed() > dist) val += 1;

	// Shoot without penalty ?
	if (se->m_penalty == 1) val += 5;

	// Can target shoot ?
	if (pTarget->CanShoot() && pTarget->Shots()) val += 3;

	// Can target flight ?
	if (pTarget->TransType() == TRANS_FLY) val += 1;

	return val * pTarget->Count();
}

void CalcBestMeleeDir(iBattleGroup* pGroup, iBattleGroup* pTarget, const iBattleGroup::iMeleeEntry* me, iBattleEngine& engine, uint16& mdir, sint32 &cval, bool bForceFF)
{
	const sint32 INV_CVAL = -65536;	
	mdir = 0xffff;
	cval = INV_CVAL;

	for (uint16 dd=0; dd<12; ++dd) {
		if (me->m_cfg & (1<<dd)) {			
			iPoint mp;
			uint8 aor;
			pGroup->GetMeleePosition(pTarget, me->m_pos, dd, mp, aor);
			sint32 dval = -(sint32)pGroup->DistMap().GetAt(mp.x,mp.y).dirs[aor].val;

			bool bDamagesOurs = false;
			if (pGroup->HasPerk(CPERK_TWOHEXATTACK)) {
				iBattleGroup* pNebTarget = engine.FindGroup(engine.GetThroughHex(pTarget->Pos(), dd));
				if(pNebTarget) {
					if (pNebTarget != pGroup && pNebTarget->Owner() == pGroup->Owner()) {
						bDamagesOurs = true;
						dval -= 1000;
					}
					else if(pNebTarget != pGroup && pNebTarget->Owner() == pTarget->Owner()) {
						dval += 1000;
					}
				}
			}							

			if ((cval == INV_CVAL || cval < dval) && (!bDamagesOurs || bForceFF)) {
				cval = dval;				
				mdir = dd;
			}
		}
	}
}

sint32 CalcMeleeValue(iBattleGroup* pGroup, const iBattleGroup::iMeleeEntry* se, iBattleGroup* pTarget, iBattleEngine& engine)
{
	sint32 val = 0;
	if (pTarget->CanShoot() && pTarget->Shots()) val += 20;
	if (pTarget->TransType() == TRANS_FLY) val += 10;
	if ( !pGroup->HasPerk(CPERK_NONRETALATTACK) && !pTarget->CanRetail()) val += 15;

	// Vampires
	if ( pGroup->HasPerk(CPERK_DRAINLIFES) && !pTarget->HasPerk(CPERK_UNDEAD) && !pTarget->HasPerk(CPERK_LIFELESS)) val += 20;

	// Ghosts
	if ( pGroup->HasPerk(CPERK_GHOST) && !pTarget->HasPerk(CPERK_UNDEAD) && !pTarget->HasPerk(CPERK_LIFELESS)) 
    {
// CotullaComment
		val += 20 + (7 - CREAT_DESC[pTarget->Type()].level) * 6;
        // calculate firsts damage 
#if 0
        val = 0;
        // primary
        sint32 penalty = 0;
        if (pGroup->CanShoot() && pGroup->Shots() > 0)
        {
            penalty = engine.CalcShotPenalty(pGroup, pTarget->Pos());
        }

        bool bRange = false;
        sint32 joustBonus = pGroup->CalcJoustingBonus(pTarget->Pos());
        sint32 damage = pGroup->CalcDamage(pTarget, pGroup->ActMinDamage(), bRange, penalty, joustBonus);
        if (pGroup->ActMinDamage() != pGroup->ActMaxDamage()) 
        {
            sint32 max_dmg = pGroup->CalcDamage(pTarget, pGroup->ActMaxDamage(), bRange, penalty, joustBonus);
            damage += gGame.Map().Rand((max_dmg - damage + 1));
        }

        bool full_die = false;
        sint32 killed_num = 0;
        sint32 back_killed_num = 0;
        if (damage >= pTarget->TotalHitPoints())
        {
            full_die = true;
            killed_num = pTarget->Count();
        }
        else
        {
            sint32 hitPoints = pTarget->HitPointsLeft();            
            hitPoints -= damage;
            while(hitPoints <= 0) 
            {
                killed_num++;                
                hitPoints += pTarget->HitPoints();
            }
        }
              
        // back-attack (protection)
        if (!full_die)
        {               
            penalty = 0;
            if (pTarget->CanShoot() && pTarget->Shots() > 0)
            {
                penalty = engine.CalcShotPenalty(pTarget, pGroup->Pos());
            }

            bool bRange = false;
            sint32 joustBonus = pTarget->CalcJoustingBonus(pGroup->Pos());
            sint32 damage = pTarget->CalcDamage(pTarget, pTarget->ActMinDamage(), bRange, penalty, joustBonus);
            if (pTarget->ActMinDamage() != pTarget->ActMaxDamage()) 
            {
                sint32 max_dmg = pTarget->CalcDamage(pGroup, pTarget->ActMaxDamage(), bRange, penalty, joustBonus);
                damage += gGame.Map().Rand((max_dmg - damage + 1));
            }

            bool full_die = false;
            back_killed_num = 0;
            if (damage >= pGroup->TotalHitPoints())
            {
                full_die = true;
                back_killed_num = pGroup->Count();
            }
            else
            {
                sint32 hitPoints = pGroup->HitPointsLeft();            
                hitPoints -= damage;
                while(hitPoints <= 0) 
                {
                    back_killed_num++;                
                    hitPoints += pGroup->HitPoints();
                }
            }

        } // if (!full_die)

        ODS(iFormat(_T("#### kill=%-4d backkill=%-4d diff=%-4d [%s->%s] ###\n"), killed_num, back_killed_num, killed_num - back_killed_num, 
            gTextMgr[TRID_CREATURE_PEASANT_F1 + pGroup->Type() * 3], gTextMgr[TRID_CREATURE_PEASANT_F1 + pTarget->Type() * 3]).CStr());

        if (killed_num > back_killed_num)
        {
            return killed_num - back_killed_num;                        
        }
        return 0;
#endif
	}
	
	// Cavalry
	if (pGroup->HasPerk(CPERK_JOUSTING)) {
		val += pGroup->CalcJoustingBonus(se->m_pos) / 3;
	}
	
	return val * pTarget->Count();
}


//////////////////////////////////////////////////////////////////////////
bool ProcessBattleStepAI(iBattleEngine& engine)
{	
	const sint32 INV_CVAL = -65536;
	_LOG(_T(">> Begin of Battle AI <	<\r\n"));

	iBattleUnit* pCurUnit = engine.TurnSeq().CurUnit();

	if (iBattleUnit_CreatGroup* pCreatUnit = DynamicCast<iBattleUnit_CreatGroup*>(pCurUnit)) {
		iBattleGroup* pGroup = pCreatUnit->GetCreatGroup();

		// Shoot list
		if (pGroup->ShootListCount()) {
	#ifdef DEBUG_BATTLE_AI
			_LOG(iFormat(_T("Shot targets found:%d\r\n"),pGroup->ShootListCount()).CStr());
			for (uint32 ll=0; ll<pGroup->ShootListCount(); ++ll) 
				_LOG(iFormat(_T("%d,%d\r\n"),pGroup->GetShootEntry(ll)->m_pos.x,pGroup->GetShootEntry(ll)->m_pos.y).CStr());
	#endif // DEBUG_BATTLE_AI

			// select appropriate target
			sint32 cval = INV_CVAL;
			sint32 penalty = 0;
			iPoint mCell;
			for (uint32 xx=0; xx<pGroup->ShootListCount(); ++xx) {
				const iBattleGroup::iShootEntry* se = pGroup->GetShootEntry(xx);
				iBattleGroup* pTarget = engine.FindGroup(se->m_pos);
				check(pTarget);
				sint32 val = CalcShootValue(pGroup, se, pTarget);
				if ( cval == INV_CVAL || cval < val) {
					cval = val;
					mCell = se->m_pos;
					penalty = se->m_penalty;
				}
			}
			engine.Shot(mCell, penalty);			
			return true;
		} 
		
		if (pGroup->MeleeListCount()) {
	#ifdef DEBUG_BATTLE_AI
			_LOG(iFormat(_T("Melee targets found:%d\r\n"),pGroup->MeleeListCount()).CStr());
			for (uint32 ll=0; ll<pGroup->MeleeListCount(); ++ll) 
				_LOG(iFormat(_T("%d,%d\r\n"),pGroup->GetMeleeEntry(ll)->m_pos.x,pGroup->GetMeleeEntry(ll)->m_pos.y).CStr());
	#endif // DEBUG_BATTLE_AI

			// select appropriate target and direction
			sint32 cval = INV_CVAL;
			iPoint mCell;
			uint16 mDir = 0xffff;
			for (uint32 xx=0; xx<pGroup->MeleeListCount(); ++xx) {
				const iBattleGroup::iMeleeEntry* me = pGroup->GetMeleeEntry(xx);
				iBattleGroup* pTarget = engine.FindGroup(me->m_pos);
				check(pTarget);
				sint32 tempval, dval; uint16 tempdir;												

				tempval = CalcMeleeValue(pGroup, me, pTarget, engine);
				CalcBestMeleeDir(pGroup, pTarget, me, engine, tempdir, dval, false);
				if(cval == INV_CVAL || tempval + dval > cval) {
					cval = tempval + dval;
					mDir = tempdir;
					mCell = me->m_pos;
				}
	
			}
			if( mDir != 0xffff ) {
				engine.Melee(mCell, mDir);
				return true;
			}
		} 
		
		if (pGroup->PotTargetsCount()) {
			// first try to wait
			if (pCurUnit->CanWait()) {
				_LOG(_T("Found potential melee targets. Wait.\r\n"));
				engine.Wait();
				return false;
			} else {
				#ifdef DEBUG_BATTLE_AI
						_LOG(iFormat(_T("Potential melee targets found:%d\r\n"),pGroup->PotTargetsCount()).CStr());
						for (uint32 ll=0; ll<pGroup->PotTargetsCount(); ++ll) 
							_LOG(iFormat(_T("%d,%d\r\n"),pGroup->GetPotTargetEntry(ll)->m_pos.x,pGroup->GetPotTargetEntry(ll)->m_pos.y).CStr());
				#endif // DEBUG_BATTLE_AI

				// select appropriate target and direction
				sint32 cval = INV_CVAL;
				uint8 mOrient;
				iPoint mPos;
				iPoint mCell;
				uint16 mDir;

				for (uint32 xx=0; xx<pGroup->PotTargetsCount(); ++xx) {
					const iBattleGroup::iMeleeEntry* me = pGroup->GetPotTargetEntry(xx);
					iBattleGroup* pTarget = engine.FindGroup(me->m_pos);
					check(pTarget);
					sint32 val = CalcMeleeValue(pGroup, me, pTarget, engine);
					for (uint16 dd=0; dd<12; ++dd) {
						if (me->m_cfg & (1<<dd)) {
							sint32 dval = val;
							iPoint mp;
							uint8 aor;
							pGroup->GetMeleePosition(pTarget, me->m_pos, dd, mp, aor);
							dval -= (sint32)pGroup->DistMap().GetAt(mp.x,mp.y).dirs[aor].val;
							if ( cval == INV_CVAL || cval < dval) {
								cval = dval;
								mCell = me->m_pos;
								mDir = dd;
								mOrient = aor;
								mPos = mp;
							}
						}
					}
				}

				CalculatePath(pGroup, mPos, mOrient);
				_LOG(iFormat(_T("Group at (%d,%d) move to (%d,%d)\r\n"),pGroup->Pos().x,pGroup->Pos().y,mPos.x,mPos.y).CStr());
				engine.Move(mPos, mOrient);
				return true;
			}
		} 
		if (pCurUnit->CanWait()) {
			_LOG(_T("Nothing to do. Wait.\r\n"));
			engine.Wait();
			return false;
		} else {
			_LOG(_T("Nothing to do. Skip the turn.\r\n"));
			return engine.Defend();
		}		
	} else if (DynamicCast<iBattleUnit_Catapult*>(pCurUnit)) {
		engine.CatapultShot(engine.CastleFort()->GetTarget());
		return true;
	} else if (DynamicCast<iBattleUnit_Turret*>(pCurUnit)) {
		iBattleGroup* pTarget = SelectTurretTarget(engine);
		check(pTarget);
		engine.TurretShot(pTarget);
		return true;
	} else {
		check(0);
	}

	//_LOG(_T(">> End of Battle AI <<\r\n"));
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool CheckBattleStateAI( iBattleEngine& engine, bool bUserCanCancel, bool& bTriedToEscape, iBattleResult& br )
{
	iBattleUnit* pCurUnit = engine.TurnSeq().CurUnit();
	bTriedToEscape = false;
	if (iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(pCurUnit)) {
		iBattleGroup* pGroup = pCurCreatGroup->GetCreatGroup();
		if (iBattleMember_Hero* pHeroMember = DynamicCast<iBattleMember_Hero*>(pGroup->Owner())) {
			// We can't retreat from castle
			if (DynamicCast<iCastle*>(pHeroMember->GetHero()->GetLocation())) return true;
			// There is a shackles of war present
			if (engine.GetBattleInfo().HasArtInBattle(ARTT_SHOFWAR)) return true;

			// we are on ship, can't retreat
			if(pHeroMember->GetHero()->InShip()) return true;
			
			// check for free castles
			iPlayer* pPlayer = pHeroMember->GetHero()->Owner();
			bool bHasFree = false;
			for(uint32 xx=0; xx<pPlayer->CastleCount(); xx++)
				if(!(pPlayer->CastleFirst() + xx)->Visitor())
				{
					bHasFree = true;
					break;
				}
			if(!bHasFree)
				return true;

			iBattleMember::Side fSide = pHeroMember->GetSide();
			uint32 fPwr = (fSide == iBattleMember::Assaulter)?engine.AArmy().ArmyPower():engine.DArmy().ArmyPower();
			uint32 ePwr = (fSide == iBattleMember::Assaulter)?engine.DArmy().ArmyPower():engine.AArmy().ArmyPower();
			//_LOG(iFormat(_T("Compare army strength: %d / %d : "), fPwr, ePwr).CStr());
			if (ePwr / 5 >= fPwr) {
				bTriedToEscape = true;
				
				// for AI player autoselect best castle to escape
				if(iAI_Player* pPlayer = DynamicCast<iAI_Player*>(pHeroMember->GetHero()->Owner())) {
					iCastle* pDst = pPlayer->SelectBestCastleForEscape(pHeroMember->GetHero());
					if (pDst) {
						br.m_result = (fSide == iBattleMember::Assaulter)?BR_DEFENDER_WIN:BR_ASSAULTER_WIN;
						br.m_defCause = DC_RETREAT;
						br.m_pDest = pDst;
						return false;
					}
				} else if(iPlayer* pPlayer = pHeroMember->GetHero()->Owner()) {
					// only a human (not remote) player can select a castle for escape
					if(pPlayer->PlayerType() == PT_HUMAN) {
						iDlg_TownList tldlg(&gApp.ViewMgr(), pPlayer,gTextMgr[TRID_SELECT_CASTLE_TO_RETREAT], TRID_RETREAT, bUserCanCancel);
						tldlg.DoModal();
						iCastle* pDst = tldlg.Destination();
						if (pDst) {
							br.m_result = (fSide == iBattleMember::Assaulter)?BR_DEFENDER_WIN:BR_ASSAULTER_WIN;
							br.m_defCause = DC_RETREAT;
							br.m_pDest = pDst;
							return false;
						}
					}
				} 
			}
		}
	}

	return true;
}

/*
 *	Magic
 */

struct BattleStateDesc {
	iHero*			pCaster;
	iCombatSpell*	pSpell;
	iBattleMember*	pOwner;
	sint32			ePwr;
	sint32			fPwr;
};

//////////////////////////////////////////////////////////////////////////
inline sint32 CalcTargetDmgValue(iBattleGroup* pTarget, sint32 damage)
{
	if (pTarget->CanShoot() && pTarget->Shots()) damage += (damage>>1);
	if (pTarget->TransType() == TRANS_FLY) damage += (damage>>2);
	if (!pTarget->IsAlive() || pTarget->IsBlinded()) damage = damage >> 2;
	return damage;
}

typedef iSimpleArray<iBattleGroup*> iTargetList;
typedef sint32 (*ProcessSpellValueFunc)(const iTargetList& targets, const BattleStateDesc& bsd);

sint32 ProcessSpellValue_Damage(const iTargetList& targets, const BattleStateDesc& bsd)
{
	iSpell_Damage* pDmgSpell = (iSpell_Damage*)bsd.pSpell;
	sint32 totRes = 0;

	for (uint32 nn=0; nn<targets.GetSize(); ++nn) {
		iBattleGroup* pTarget = targets[nn];
		sint32 nres = CalcTargetDmgValue(pTarget, iMIN<sint32>(pDmgSpell->EstimateDamage(bsd.pCaster, pTarget), pTarget->TotalHitPoints()));
		if (bsd.pOwner->GetSide() == pTarget->Owner()->GetSide()) {
			totRes -= nres * 10;
		} else {
			totRes += nres;
		}
	}

	return (totRes > 0) ? totRes : 0;
}

sint32 ProcessSpellValue_Resurrect(const iTargetList& targets, const BattleStateDesc& bsd)
{
	iSpell_Resurrect* pResSpell = (iSpell_Resurrect*)bsd.pSpell;
	sint32 totRes = 0;
	
	for (uint32 nn=0; nn<targets.GetSize(); ++nn) {
		iBattleGroup* pTarget = targets[nn];
		if (pTarget->CanResurrect()) {
			totRes += iCLAMP<sint32>(0, pTarget->TotalCasHitPoints(), pResSpell->EstimateHitPoints(bsd.pCaster));
		}
	}

	return totRes;
}

sint32 ProcessSpellValue_Summon(const iTargetList& targets, const BattleStateDesc& bsd)
{
	// Our army is too strong to summon any creature
	if (bsd.ePwr * 2 < bsd.fPwr) return 0;

	//
	iSpell_Summon* pSumSpell = (iSpell_Summon*)bsd.pSpell;
	sint32 val = pSumSpell->EstimateQuant(bsd.pCaster) * CREAT_DESC[pSumSpell->CreatType()].hits;

	if (bsd.ePwr < bsd.fPwr) {
		// our army is stronger than enemies
		//sint32 pdif = (bsd.fPwr * 10) / bsd.ePwr;
		val = val >> 2;
	}

	return val;
}

sint32 ProcessSpellValue_Dispel(const iTargetList& targets, const BattleStateDesc& bsd)
{
	sint32 totRes = 0;
	for (uint32 nn=0; nn<targets.GetSize(); ++nn) {
		iBattleGroup* pTarget = targets[nn];
		if (pTarget->IsBlinded()) totRes += pTarget->TotalHitPoints();
		else if (pTarget->SpellsDispos() == SPD_NEGATIVE) totRes += pTarget->TotalHitPoints() / 3;
	}
	
	return totRes;
}

sint32 ProcessSpellValue_Cure(const iTargetList& targets, const BattleStateDesc& bsd)
{
	iSpell_Cure* pCureSpell = (iSpell_Cure*)bsd.pSpell;
	sint32 totRes = 0;
	
	for (uint32 nn=0; nn<targets.GetSize(); ++nn) {
		iBattleGroup* pTarget = targets[nn];
		totRes += iMIN<sint32>(pCureSpell->EstimateHitPoints(bsd.pCaster), pTarget->HitPoints() - pTarget->HitPointsLeft()) * 2;
		if (pTarget->IsBlinded()) totRes += pTarget->TotalHitPoints();
	}
	
	return totRes;
}

sint32 ProcessSpellValue_Blind(const iTargetList& targets, const BattleStateDesc& bsd)
{
	sint32 totRes = 0;
	for (uint32 nn=0; nn<targets.GetSize(); ++nn) {
		iBattleGroup* pTarget = targets[nn];
		if (!pTarget->IsBlinded()) {
			totRes += pTarget->TotalHitPoints() / 2;
		}
	}
	
	return totRes;
}

ProcessSpellValueFunc GetProcessSpellValueFunc(iCombatSpell* pSpell)
{
	if (pSpell->SpellClass() == SPC_DAMAGE) return ProcessSpellValue_Damage;
	else if (pSpell->SpellClass() == SPC_RESURRECT) return ProcessSpellValue_Resurrect;
	else if (pSpell->SpellClass() == SPC_SUMMON) return ProcessSpellValue_Summon;
	else if (pSpell->SpellClass() == SPC_DISPEL) return ProcessSpellValue_Dispel;
	else if (pSpell->SpellClass() == SPC_CURE) return ProcessSpellValue_Cure;
	else if (pSpell->SpellClass() == SPC_BLIND) return ProcessSpellValue_Blind;
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
bool ProcessSpellCastAI(iBattleEngine& engine)
{
	_LOG(_T("-----Process AI Spell cast-----\r\n"));
	BattleStateDesc bsd;
	bsd.pOwner = engine.TurnSeq().CurUnit()->Owner();
	bsd.pCaster = bsd.pOwner->SpellCaster();

	// Check general conditions
	if (!bsd.pCaster) {
		_LOG(_T("No spell caster available!\r\n"));
		return false;
	} else if (!bsd.pOwner->CanCastSpell()) {
		_LOG(iFormat(_T("%s already casts the spell in this round.\r\n"), bsd.pCaster->Name().CStr()).CStr());
		return false;
	} else if (engine.GetBattleInfo().HasArtInBattle(ARTT_NEGSPHERE)) {
		_LOG(iFormat(_T("Sphere of Negation\r\n")).CStr());
		return false;
	} else if (!DynamicCast<iBattleUnit_CreatGroup*>(engine.TurnSeq().CurUnit())) {
		return false;
	}

	// Prepare battle field information
	const iBattleArmy& fa =  (bsd.pOwner->GetSide() == iBattleMember::Assaulter)?engine.AArmy():engine.DArmy();
	const iBattleArmy& ea =  (bsd.pOwner->GetSide() == iBattleMember::Assaulter)?engine.DArmy():engine.AArmy();
	bsd.fPwr = fa.ArmyPower();
	bsd.ePwr = ea.ArmyPower();

	// Select spells
	iCombatSpell* pSpellToCast = NULL;
	iPoint spellTarget = cInvalidPoint;
	iBattleGroup *spellTargetGroup = NULL;
	sint32 spellValue = 0;

	const iSpellBook& book = bsd.pCaster->SpellBook();
	for (uint32 xx=0; xx<book.SpellsCount(); ++xx) {
		MAGIC_SPELL spellId = (MAGIC_SPELL)book.Spell(xx);
		if (bsd.pSpell = DynamicCast<iCombatSpell*>(gGame.ItemMgr().m_spellMgr.Spell(spellId))) {
			_LOG(iFormat(_T("Processing %s: "), gTextMgr[bsd.pSpell->NameKey()]).CStr());
			MAGIC_SCHOOL_LEVEL msl = bsd.pSpell->GetSchoolLevel(bsd.pCaster);
			uint8 cost = bsd.pSpell->Cost(msl);
			uint16 tgtMode = bsd.pSpell->TargetMode(msl);
			if (bsd.pCaster->ManaPts() < cost) {
				_LOG(_T("Not enough mana points!\r\n"));
				continue;
			}

			ProcessSpellValueFunc ProcessSpellValue = GetProcessSpellValueFunc(bsd.pSpell);
			if (!ProcessSpellValue) {
				_LOG(_T("This spell isn't supported by AI!\r\n"));
				continue;
			}

			_LOG(L"\n");

			//
			// Process spells
			//
			if (tgtMode == STM_CREAT_GROUP) {
				// One creature group
				iBattleGroup* pSelTarget = NULL;
				sint32 selValue = 0;

				// acquire all units which might be affected by spell
				iTargetList targetList;
				engine.SelectSpellTargets(bsd.pSpell, targetList);
				iTargetList ctarget;
				ctarget.Add(NULL);

				for (uint32 yy=0; yy<targetList.GetSize(); ++yy) {
					ctarget[0] = targetList[yy];
					sint32 tval = ProcessSpellValue(ctarget, bsd);
					if (tval && (!pSelTarget || selValue < tval)) {
						pSelTarget = targetList[yy];
						selValue = tval;
					}
				}
				if (pSelTarget)	{
					_LOG(iFormat(_T("\tBest target is %s: %d value.\r\n"), gTextMgr[TRID_CREATURE_PEASANT_F2 + pSelTarget->Type()*3], selValue).CStr());
					if (selValue > spellValue) {
						pSpellToCast = bsd.pSpell;
						spellTarget = pSelTarget->Pos();
						spellTargetGroup = pSelTarget;
						spellValue = selValue;
					}
				} else {
					_LOG(_T("No taget!\r\n"));
				}
			} else if (tgtMode == STM_BALL_SET || tgtMode == STM_BLAST_SET || tgtMode == STM_RING_SET) {
				// Ball/Blast/Ring set
				const iBattleMap& passMap = DynamicCast<iBattleUnit_CreatGroup*>(engine.TurnSeq().CurUnit())->GetCreatGroup()->PassMap();
				iSimpleArray<iPoint> covers;
				iTargetList targetList;
				iPoint selTarget = cInvalidPoint;
				sint32 selValue = 0;
				for (sint32 cy=0; cy<11; ++cy) {
					for (sint32 cx=0; cx<13; ++cx) {
						covers.RemoveAll();
						targetList.RemoveAll();
						engine.GetSpellCovers(bsd.pSpell, msl, iPoint(cx,cy), covers);
						for (uint32 yy=0; yy<covers.GetSize(); ++yy) {
							if ( (passMap.GetAt(covers[yy].x, covers[yy].y) & 0x7F) == CT_FRIENDLYC || (passMap.GetAt(covers[yy].x, covers[yy].y) & 0x7F) == CT_ENEMYC) {
								iBattleGroup* pTarget = engine.FindGroup(covers[yy]);
								check(pTarget);
								iBattleUnit_Hero tmpHero(bsd.pOwner);
								if (bsd.pSpell->CanCast( &tmpHero, pTarget)) targetList.AddUnique(pTarget);
							} 
						}

						sint32 totValue = ProcessSpellValue(targetList, bsd);
						if ( (selTarget == cInvalidPoint && totValue) || totValue > selValue) {
							selTarget.x = cx;
							selTarget.y = cy;
							selValue = totValue;
						}
					}
				}
				if (selTarget != cInvalidPoint)	{
					if (selValue > spellValue) {
						pSpellToCast = bsd.pSpell;
						spellTarget = selTarget;
						spellValue = selValue;
					}
					_LOG(iFormat(_T("\tBest target cell is (%d,%d): %d value.\r\n"), selTarget.x, selTarget.y, selValue).CStr());
				} else {
					_LOG(_T("No taget!\r\n"));
				}
			} else if (tgtMode == STM_ALL) {
				// All units
				iTargetList targetList;
				engine.SelectSpellTargets(bsd.pSpell, targetList);
				sint32 totValue = ProcessSpellValue(targetList, bsd);;
				if (totValue > spellValue) {
					pSpellToCast = bsd.pSpell;
					spellTarget = cInvalidPoint;
					spellValue = totValue;
				}
				_LOG(iFormat(_T("\t%d targets: %d value.\r\n"), targetList.GetSize(), totValue).CStr());
			} else if (tgtMode == STM_SUMMON) {
				iTargetList targetList;
				sint32 totValue = ProcessSpellValue(targetList, bsd);
				if (totValue > spellValue) {
					pSpellToCast = bsd.pSpell;
					spellTarget = cInvalidPoint;
					spellValue = totValue;
				}
				_LOG(iFormat(_T("\t%d value.\r\n"), totValue).CStr());
			} else if (tgtMode == STM_EARTHQUAKE) {
			} else {
				check("Unknown spell target mode" == 0);
			}
		}
	}

	if (pSpellToCast) {
		engine.CastSpell(pSpellToCast, spellTarget, spellTargetGroup);
		_LOG(iFormat(_T("Cast %s to (%d,%d) with value %d\r\n"), gTextMgr[pSpellToCast->NameKey()], spellTarget.x, spellTarget.y, spellValue).CStr());
		_LOG(_T("----------------------------\r\n"));
		return true;
	}
	
	_LOG(_T("Nothing to cast!"));
	_LOG(_T("----------------------------\r\n"));
	return false;
}

