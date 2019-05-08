#include "stdafx.h"
#include "Dlg_Chest.h"
#include "Dlg_ArmyRoom.h"
#include "Dlg_FightGuard.h"
#include "Dlg_ArtInfo.h"
#include "Dlg_Recruit.touch.h"
#include "Dlg_Rewards.h"
#include "serialize.h"

//#include "xxc/wce.detects.h"

//////////////////////////////////////////////////////////////////////////
iMapItem::iMapItem(const iPoint& pos, MAP_ITEM_TYPE mit, const SpriteId& sid)
: iBaseMapObject(pos, true), m_mapItemType(mit), m_spriteId(sid) 
{
	
}

bool iMapItem::Activate(iHero* pHero, bool bActive, bool bShowMessage)
{	
    if(bActive && m_mapItemType != MAPITEM_KEYGUARD)
        gTutorial.Trigger(TUT_MAP_ITEMS);
    
	if (bActive && bShowMessage && !m_message.Empty()) {
		iTextDlg dlg(&gApp.ViewMgr(), _T(""), gGame.Map().GetText(TET_MAPITEM_MSG, this),pHero->Owner()->PlayerId());
		dlg.DoModal();
	}
	if (!Guard().Empty()) {
		if (bActive) {
			iDlg_FightGuard dlg(&gApp.ViewMgr(), gTextMgr[NameKey()], Guard(), PID_NEUTRAL, VL_EXPERT);
            uint32 res = dlg.DoModal();
			if (res == DRC_YES) {
				iBattleInfo bi(pHero, this);
				gGame.BeginBattle(bi);
			}
            else if( res == DRC_QUICK ){
                
                iBattleInfo bi(pHero, this);
                gGame.BeginBattle(bi, true);
            }
			return false;
		} else {
#ifdef MULTIPLAYER
			if(pHero->Owner()->PlayerType() == PT_REMOTE) return false;
#endif
			iBattleInfo bi(pHero, this);
			gGame.BeginBattle(bi);
			return false;
		}
	
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
iMapItem_Mineral::iMapItem_Mineral(const iPoint& pos, MINERAL_TYPE mt, sint32 count)
: iMapItem(pos, MAPITEM_MINERAL, PDGG(RES_ICONS)+mt), m_mineralType(mt), m_count(count) 
{ 
	if (mt == MINERAL_UNKNOWN) {
		mt = (MINERAL_TYPE)(MINERAL_GOLD + gGame.Map().Rand(MINERAL_COUNT));
		m_spriteId = PDGG(RES_ICONS)+mt;
		m_mineralType = mt;
	}

	if (m_count == RANDOM_QUANTITY) {
		if (mt == MINERAL_GOLD)	m_count = 500 + gGame.Map().Rand(501); 
		else if (mt == MINERAL_WOOD || mt == MINERAL_ORE) m_count = 5 + gGame.Map().Rand(6); 
		else m_count = 3 + gGame.Map().Rand(4); 
	}
}

bool iMapItem_Mineral::Activate(iHero* pHero, bool bActive, bool bShowMessage)
{
	if (!iMapItem::Activate(pHero, bActive, bShowMessage)) return false;
	if (bActive) {
		gSfxMgr.PlaySound(CSND_PICKUP01 + (m_Pos.x+m_Pos.y) % 5);
		gGame.AddCellMsg(iFormat(_T("#I%04X+%d"),PDGG(RES_ICONS) + m_mineralType, m_count), m_Pos);
	}
	pHero->Owner()->AddMineral(m_mineralType, m_count);

	return true;
}

void iMapItem_Mineral::Serialize(iDynamicBuffer& buff)
{
	buff.Write((sint8)m_mineralType);
	buff.Write((sint32)m_count);
}

//////////////////////////////////////////////////////////////////////////
iMapItem_ManaCrystal::iMapItem_ManaCrystal(const iPoint& pos, sint32 count)
: iMapItem(pos, MAPITEM_MANACRYSTAL, PDGG(RES_MCRYST)), m_count(count)
{ if (m_count == RANDOM_QUANTITY) m_count = 5 + gGame.Map().Rand(20); }

bool iMapItem_ManaCrystal::Activate(iHero* pHero, bool bActive, bool bShowMessage)
{
	if (!iMapItem::Activate(pHero, bActive, bShowMessage)) return false;
	pHero->ManaPts() += m_count;
#ifdef MULTIPLAYER
	gMPMgr.OnHeroUpdate(pHero);
#endif
	if (bActive) {
		gGame.AddCellMsg(iFormat(_T("#I%04X+%d"),PDGG(DGLYPH_MANA), m_count), m_Pos);
		gSfxMgr.PlaySound(CSND_PICKUP01 + (m_Pos.x+m_Pos.y) % 5);
	}
	return true;
}

void iMapItem_ManaCrystal::Serialize(iDynamicBuffer& buff)
{
	buff.Write((sint32)m_count);
}

//////////////////////////////////////////////////////////////////////////
iMapItem_CampFire::iMapItem_CampFire(const iPoint& pos, sint32 goldCount, MINERAL_TYPE addMinType, sint32 addMinCount)
: iMapItem(pos, MAPITEM_CAMPFIRE, PDGG(RES_CMFIRE))
, m_addMinType(addMinType) 
, m_addMinCount(addMinCount)
, m_goldCount(goldCount)
{
	if (m_goldCount == RAND_VAL) m_goldCount = 400 + (gGame.Map().Rand(3))*100;
	if (m_addMinType == MINERAL_UNKNOWN) m_addMinType = (MINERAL_TYPE)(MINERAL_ORE + gGame.Map().Rand(6)); 
	if (m_addMinCount == RAND_VAL) m_addMinCount = 4+gGame.Map().Rand(3);
}

bool iMapItem_CampFire::Activate(iHero* pHero, bool bActive, bool bShowMessage)
{
	if (!iMapItem::Activate(pHero, bActive, bShowMessage)) return false;

	pHero->Owner()->AddMineral(MINERAL_GOLD, m_goldCount);
	pHero->Owner()->AddMineral(m_addMinType, m_addMinCount);

	if (bActive) {
		gGame.AddCellMsg(iFormat(_T("#I%04X+%d #I%04X+%d"), PDGG(RES_ICONS), m_goldCount, PDGG(RES_ICONS) + m_addMinType, m_addMinCount), m_Pos);
		gSfxMgr.PlaySound(CSND_PICKUP01 + (m_Pos.x+m_Pos.y) % 5);
	}
	return true;
}

void iMapItem_CampFire::Serialize(iDynamicBuffer& buff)
{
	buff.Write(m_goldCount);
	buff.Write((sint8)m_addMinType);
	buff.Write(m_addMinCount);
}

//////////////////////////////////////////////////////////////////////////
iMapItem_Chest::iMapItem_Chest(const iPoint& pos, sint32 count)
: iMapItem(pos, MAPITEM_CHEST, PDGG(RES_CHEST)), m_count(count) 
{
	if (m_count == RAND_VAL) {
		m_count = 500 + (gGame.Map().Rand(10) * 100);
	}
}

bool iMapItem_Chest::Activate(iHero* pHero, bool bActive, bool bShowMessage)
{
	if (!iMapItem::Activate(pHero, bActive, bShowMessage)) return false;
	if (bActive) {
		iDlg_Chest dlg(&gApp.ViewMgr(), pHero, this);
		sint32 res;

		// if no level cap or not reached, choose, otherwise choose gold
		if(gGame.Map().LevelCap() == 0 || pHero->Level() < gGame.Map().LevelCap())
			res = dlg.DoModal();
		else
			res = iDlg_Chest::Gold;

		if (res == iDlg_Chest::Gold) {
			pHero->Owner()->AddMineral(MINERAL_GOLD, dlg.GoldCount(), true);
			if (bActive) gGame.AddCellMsg(iFormat(_T("#I%04X+%d"), PDGG(RES_ICONS) + MINERAL_GOLD, dlg.GoldCount()), m_Pos);
		} else if (res == iDlg_Chest::Experience) {
			pHero->ReceiveExperience(dlg.ExpCount());
		} else {
			//check(0);
		}
		gSfxMgr.PlaySound(CSND_PICKUP01 + (m_Pos.x+m_Pos.y) % 5);
	} else if(iAI_Player* pOwner = DynamicCast<iAI_Player*>(pHero->Owner())) {		
		check(pOwner);
		pOwner->ProcessChest(pHero, this);
	}
#ifdef MULTIPLAYER	
	gMPMgr.OnHeroUpdate(pHero);
#endif
	return true;
}

void iMapItem_Chest::Serialize(iDynamicBuffer& buff)
{
	buff.Write(m_count);
}

/////////////////////////////////////////////////////////////////////////
iMapItem_Artifact::iMapItem_Artifact(const iPoint& pos, uint16 aid)
: iMapItem(pos, MAPITEM_ARTIFACT, gGame.ItemMgr().m_artMgr[aid].Sprite()), m_artId(aid) {}

bool iMapItem_Artifact::Activate(iHero* pHero, bool bActive, bool bShowMessage)
{
	if (!iMapItem::Activate(pHero, bActive, bShowMessage)) return false;
	if (bActive) {
		iDlg_ArtInfo dlg(&gApp.ViewMgr(), m_artId, pHero);
		dlg.DoModal();
		gSfxMgr.PlaySound(CSND_PICKUP01 + (m_Pos.x+m_Pos.y) % 5);
		pHero->Artifacts().Push(m_artId);
	} else {
		pHero->Artifacts().SmartPush(m_artId);
	}
#ifdef MULTIPLAYER
	gMPMgr.OnHeroUpdate(pHero);
#endif

	return true;
}

void iMapItem_Artifact::Serialize(iDynamicBuffer& buff)
{
	buff.Write((uint16)m_artId);
}
//////////////////////////////////////////////////////////////////////////
iMapItem_Lamp::iMapItem_Lamp(const iPoint& pos, sint32 count)
: iMapItem(pos, MAPITEM_LAMP, PDGG(RES_LAMP)), m_count(count) 
{
	if (m_count == RAND_VAL) {
		m_count = 2 + gGame.Map().Rand(4);
	}
}

bool iMapItem_Lamp::Activate(iHero* pHero, bool bActive, bool bShowMessage)
{
	if (!iMapItem::Activate(pHero, bActive, bShowMessage)) return false;
	check(m_count > 0);
	if (bActive) {
		iCreatGroup cg(CREAT_GENIE, m_count);
		iDlg_Recruit rdlg(&gApp.ViewMgr(), cg, pHero->Army(), pHero->Owner()->PlayerId(), pHero);
#ifndef PC_VERSION
        rdlg.SetRect(iRect(iPoint(0,0), rdlg.ClientSize()));
#endif
		if (rdlg.DoModal() == DRC_OK) {
			m_count = cg.Count();
			if (!m_count) gSfxMgr.PlaySound(CSND_PICKUP01 + (m_Pos.x+m_Pos.y) % 5);
		}
	} else if(iAI_Player* pOwner = DynamicCast<iAI_Player*>(pHero->Owner())) {		
		check(pOwner);
		iCreatGroup cg(CREAT_GENIE, m_count);
		if (pOwner->ProcessRecruitCreatures(pHero, cg)) {
			m_count -= cg.Count();
		}
	}
#ifdef MULTIPLAYER		
	gMPMgr.OnMapItemUpdate(this);
#endif

	return m_count == 0;
}

void iMapItem_Lamp::Serialize(iDynamicBuffer& buff)
{
	buff.Write(m_count);
}
/////////////////////////////////////////////////////////////////////////
iMapItem_SpellScroll::iMapItem_SpellScroll(const iPoint& pos, uint16 spellId)
: iMapItem(pos, MAPITEM_SPELLSCROLL, PDGG(RES_SPELL_SCROLL)), m_spellId(spellId) {}

bool iMapItem_SpellScroll::Activate(iHero* pHero, bool bActive, bool bShowMessage)
{
	if (!iMapItem::Activate(pHero, bActive, bShowMessage)) return false;
	if (bActive) {
		iIconDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_MAPRES_SPELLSCROLL], iFormat(gTextMgr[TRID_MAPRES_SPELL_SCROLL_MSG], gTextMgr[TRID_SPNAME_MAGICARROW+m_spellId]), PDGG(SPELL_ICONS)+m_spellId,pHero->Owner()->PlayerId());
		dlg.DoModal();
		gSfxMgr.PlaySound(CSND_PICKUP01 + (m_Pos.x+m_Pos.y) % 5);
	}

	pHero->LearnSpell(m_spellId, true);
	return true;
}

void iMapItem_SpellScroll::Serialize(iDynamicBuffer& buff)
{
	buff.Write((uint16)m_spellId);
}
/////////////////////////////////////////////////////////////////////////
iMapItem_KeyGuard::iMapItem_KeyGuard(const iPoint& pos, uint8 key)
: iMapItem(pos, MAPITEM_KEYGUARD, PDGG(KEY_GUARDS)+key), m_key(key) {}

bool iMapItem_KeyGuard::Activate(iHero* pHero, bool bActive, bool bShowMessage)
{
	if (!iMapItem::Activate(pHero, bActive, false)) return false;
	if (bActive) {
		if (pHero->Owner()->HasKey(m_key)) {
			iQuestDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_MAPRES_KEYGUARD], !m_message.Empty() ?
				gGame.Map().GetText(TET_MAPITEM_MSG, this) : gTextMgr[TRID_MAPRES_KEYGUARD_OK_MSG], 
				PID_NEUTRAL);
			dlg.DoModal();		
			if (dlg.DoModal() == DRC_YES) {
				gSfxMgr.PlaySound(CSND_DEL_GUARD);
				return true;
			}
		} else {
			iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_MAPRES_KEYGUARD], gTextMgr[TRID_MAPRES_KEYGUARD_NOPASS_MSG], pHero->Owner()->PlayerId());
			dlg.DoModal();
			return false;
		}
	} else {
		check(pHero->Owner()->HasKey(m_key));
		if (pHero->Owner()->HasKey(m_key)) {
			return true;
		}
	}
	
	return false;
}

void iMapItem_KeyGuard::Serialize(iDynamicBuffer& buff)
{
	buff.Write((uint8)m_key);
}
//////////////////////////////////////////////////////////////////////////
iMapEvent::iMapEvent(const iPoint& pos)
: iBaseMapObject(pos, false)
{
}

bool iMapEvent::Activate(iHero* pHero, bool bActive, bool bShowMessage, bool& stopMoving)
{

	// check the requirements for the event to trigger		
	if(!pHero->MeetsReqs(m_reqs)) {
		if (bActive && bShowMessage && !m_rfailMessage.Empty()) {
			iTextDlg dlg(&gApp.ViewMgr(), iStringT(), gGame.Map().GetText(TET_MEVENT_RFAILMSG, this), pHero->Owner()->PlayerId());
			dlg.DoModal();
			stopMoving = true;
		}
		return false;
	}		

	// Guards
	if (!Guard().Empty()) {
		stopMoving = true;
		// Show Attack message
		if (bActive && bShowMessage) {
			iTextDlg dlg(&gApp.ViewMgr(), iStringT(), (m_attackMessage.Empty())?gTextMgr[TRID_MSG_MAPEVENT_ATTACK]:gGame.Map().GetText(TET_MEVENT_ATTMSG, this), pHero->Owner()->PlayerId());
			dlg.DoModal();
		}
#ifdef MULTIPLAYER
		if(pHero->Owner()->PlayerType() == PT_REMOTE) return false;
#endif

		// Here is battle
		iBattleInfo bi(pHero, this);
		gGame.BeginBattle(bi);
		return false;
	} else if (bActive && bShowMessage && !m_attackMessage.Empty()) {
		iTextDlg dlg(&gApp.ViewMgr(), iStringT(), gGame.Map().GetText(TET_MEVENT_ATTMSG, this), pHero->Owner()->PlayerId());
		dlg.DoModal();
	}

	// Rewards
	if (m_rewards.GetSize()) {		
		if(pHero->ReceiveRewards(m_rewards, bActive, _T(""), gGame.Map().GetText(TET_MEVENT_REWMSG, this).CStr()) == 0)
			return false; // don't stop movement if no rewards received
		else
			stopMoving = true;
	} else if (bActive && !m_rewardMessage.Empty()) {
		iTextDlg dlg(&gApp.ViewMgr(), iStringT(), gGame.Map().GetText(TET_MEVENT_REWMSG, this), pHero->Owner()->PlayerId());
		dlg.DoModal();
		stopMoving = true;
	}
#ifdef MULTIPLAYER
	gMPMgr.OnHeroUpdate(pHero);	
#endif

	return true;
}


//////////
iMapMarker::iMapMarker(const iPoint& pos)
: iBaseMapObject(pos, false)
{
}


//////////////////////////////////////////////////////////////////////////
CREATURE_TYPE NormalizeCreatType(CREATURE_TYPE ct)
{
	if (ct < CREAT_RANDOM) return ct;
	else if (ct == CREAT_RANDOM) 
    {
        // loop until random doesn't upgrade
		do 
        {
#ifdef MULTIPLAYER
			FIXME: random valid ?
#endif
            ct = (CREATURE_TYPE)gGame.Map().Rand(CREAT_COUNT);

        } while(ct >= CREAT_MILITIA && ct <= CREAT_VOLOT);
      
        return ct;
    }
	else 
    {
		sint32 lvl = ct - CREAT_RANDOM;
		iSimpleArray<sint32> cid;
		for (sint32 xx=0; xx<CREAT_COUNT; ++xx) 
        {
			if (CREAT_DESC[xx].level == lvl) cid.Add(xx);
		}
		check(cid.GetSize()>0);
		//return (CREATURE_TYPE)cid[gGame.Map().Rand(cid.GetSize())];

        // loop until random doesn't upgrade

        do 
        {
            ct = (CREATURE_TYPE)cid[gGame.Map().Rand(cid.GetSize())];

        } while(ct >= CREAT_MILITIA && ct <= CREAT_VOLOT);

        return ct;
	}
}

uint32 RandCreaturesCount(CREATURE_TYPE type)
{
	return (150 + gGame.Map().Rand(80) + CREAT_DESC[type].level * 10 ) / int_sqrt(CREAT_DESC[type].pidx);
}

//////////////////////////////////////////////////////////////////////////
iMapGuard::iMapGuard(const iPoint& pos, CREATURE_TYPE ct, sint32 cnt, sint32 joinVal, GUARD_DISPOS disp, bool bNotGrow, bool bDisabled)
: iBaseMapObject(pos, true), m_creatures(NormalizeCreatType(ct),cnt), m_disp(disp), m_bNotGrow(bNotGrow), m_bDisabled(bDisabled), m_seenIntro(0),
m_okRewBeh(GRB_USUAL), m_permFailRewBeh(GRB_USUAL)
{
	m_spriteId = PDGG(MINIMON) + m_creatures.Type();

	// Init quantity
	if (cnt == RANDOM_QUANTITY) {
		m_creatures.Count() = RandCreaturesCount(m_creatures.Type());
	}

	m_joinVal = joinVal;
}

iMapGuard::MEET_RESULT iMapGuard::CheckMeetResult(iHero* pHero)
{
	bool bFlee = m_disp != GDISP_SAVAGE && pHero->Army().ArmyPower() > (m_creatures.GroupPower()*2);

	// join ?
	sint32 dip = iMIN<sint32>(100, 5 + pHero->FurtSkill(FSK_DIPLOMACY) / 2);
	bool bJoin = (m_disp == GDISP_COMPLIANT || (m_disp == GDISP_AGGRESSIVE && pHero->Army().ArmyPower() > m_creatures.GroupPower() && (dip > m_joinVal)));

	if (bFlee && bJoin) return MR_JOIN_OR_FLEE;
	else if (bFlee) return MR_FLEE;
	else if (bJoin) return MR_JOIN_OR_ATTACK;
	return MR_ATTACK;
}

bool iMapGuard::Activate(iHero* pHero, bool bActive)
{
	if(m_bDisabled) return false;
	MEET_RESULT mr = CheckMeetResult(pHero);
	if (bActive) {
		if(!(m_seenIntro & (1<<pHero->Owner()->PlayerId()))) {// Show message if have not seen yet
			if (!m_message.Empty()) {
				iTextDlg dlg(&gApp.ViewMgr(), _T(""), gGame.Map().GetText(TET_MAPGUARD_INTROMSG, this), pHero->Owner()->PlayerId());
				dlg.DoModal();
			}
			m_seenIntro |= (1<<pHero->Owner()->PlayerId());
		}
		
		if(pHero->MeetsReqs(m_reqs)) {
			if(!m_okRewards.IsEmpty())
				pHero->ReceiveRewards(m_okRewards, bActive, _T(""), gGame.Map().GetText(TET_MAPGUARD_OKREWMSG, this).CStr());
			else {				
				if(!gGame.Map().GetText(TET_MAPGUARD_OKREWMSG, this).Empty()) {
					iTextDlg dlg(&gApp.ViewMgr(), _T(""), gGame.Map().GetText(TET_MAPGUARD_OKREWMSG, this), PID_NEUTRAL);
					dlg.DoModal();
				}
				m_seenIntro |= (1 << (8 + pHero->Owner()->PlayerId()));
			}
			
			m_okRewards.RemoveAll();
			m_reqs.RemoveAll();
			m_permFailRewards.RemoveAll();
			m_message.SetEmpty();
			switch(m_okRewBeh) {
				case GRB_USUAL:
					break;
				case GRB_DISAPPEAR:
					gSfxMgr.PlaySound(CSND_DEL_GUARD);
					return true;
				case GRB_DISABLE:						
					m_bDisabled = true;
					return false;					
			}				
		} else if(pHero->PermanentlyFailedReqs(m_reqs)) {
			if(!m_permFailRewards.IsEmpty())
				pHero->ReceiveRewards(m_permFailRewards, bActive, _T(""), gGame.Map().GetText(TET_MAPGUARD_PERMFAILMSG, this).CStr());
			else {
				if(!gGame.Map().GetText(TET_MAPGUARD_PERMFAILMSG, this).Empty()) {
					iTextDlg dlg(&gApp.ViewMgr(), _T(""), gGame.Map().GetText(TET_MAPGUARD_PERMFAILMSG, this), PID_NEUTRAL);
					dlg.DoModal();
				}
				m_seenIntro |= (1 << (8 + pHero->Owner()->PlayerId()));
			}
			m_okRewards.RemoveAll();
			m_reqs.RemoveAll();
			m_permFailRewards.RemoveAll();
			m_message.SetEmpty();
			switch(m_permFailRewBeh) {
					case GRB_USUAL:
						break;
					case GRB_DISAPPEAR:
						gSfxMgr.PlaySound(CSND_DEL_GUARD);
						return true;
					case GRB_DISABLE:						
						m_bDisabled = true;
						return false;					
			}
		} else if(m_reqs.GetSize() > 0 && !(m_seenIntro & (1 << (8 + pHero->Owner()->PlayerId())))) {
			if (!m_curFailMessage.Empty()) {
				iTextDlg dlg(&gApp.ViewMgr(), _T(""), gGame.Map().GetText(TET_MAPGUARD_CURFAILMSG, this), pHero->Owner()->PlayerId());
				dlg.DoModal();
			}
			return false;
		}		
				
		if (mr == MR_JOIN_OR_FLEE || mr == MR_JOIN_OR_ATTACK) {
			// Join Army
			iQuestDlg dj(&gApp.ViewMgr(), _T(""), iFormat(gTextMgr[TRID_CREAT_JOIN_MESSAGE], gTextMgr[TRID_CREATURE_PEASANT_F3+Creatures().Type()*3]), pHero->Owner()->PlayerId());
			if (dj.DoModal() == DRC_YES) {
				if (!pHero->AddGroupToArmy(m_creatures.Type(),m_creatures.Count())) {
					iDlg_ArmyRoom arDlg(&gApp.ViewMgr(), *pHero, m_creatures);
					arDlg.DoModal();
				}
#ifdef MULTIPLAYER
				gMPMgr.OnHeroUpdate(pHero);
#endif	

				gSfxMgr.PlaySound(CSND_DEL_GUARD);
				return true;
			}
		}

		bool quick = false;
		if (mr == MR_JOIN_OR_FLEE || mr == MR_FLEE) {
			iPursueDlg de(&gApp.ViewMgr(), _T(""), iFormat(gTextMgr[TRID_CREAT_ESCAPE_MESSAGE], gTextMgr[TRID_CREATURE_PEASANT_F2+Creatures().Type()*3]),pHero->Owner()->PlayerId());
			uint32 res = de.DoModal();
			if (res == DRC_NO) {
				gSfxMgr.PlaySound(CSND_DEL_GUARD);
				return true;
			}
			if(res == DRC_QUICK)
				quick = true;
		}
#ifdef MULTIPLAYER
			if(pHero->Owner()->PlayerType() == PT_REMOTE) return false;
#endif
		// Here is battle
		iBattleInfo bi(pHero, this);
#ifdef MULTIPLAYER
		gMPMgr.OnAttackMapGuard(pHero, this);
#endif				
		gGame.BeginBattle(bi, quick);
		return false;

	} else if(iAI_Player* pOwner = DynamicCast<iAI_Player*>(pHero->Owner())) {		
		check(pOwner);
#if defined(NEED_REGISTRATION) && defined(UNDER_CE)
		// TRICK: #2 - join all encountered creatures
		if ( pOwner->m_HackedTricks & 0x0c != 0 ) {
			pOwner->ProcessJoinCreatures(pHero->Army(), m_creatures);
			mr = MR_JOIN_OR_FLEE;
		}
#endif
		// ------------------------------
		if (mr == MR_FLEE || mr == MR_ATTACK || !pOwner->ProcessJoinCreatures(pHero, m_creatures)) {
			// Here is battle
			iBattleInfo bi(pHero, this);
			gGame.BeginBattle(bi);
			return false;
		}
	}
	return true;
}

void iMapGuard::NewWeek(const iWeekDesc& weekDesk)
{
	if (!m_bNotGrow) m_creatures.Grow();
}

void iMapGuard::Unserialize(iDynamicBuffer& dbuff)
{	
	::Unserialize(dbuff, Message());
	::Unserialize(dbuff, CurFailMessage());
	::Unserialize(dbuff, OKRewardMessage());
	::Unserialize(dbuff, PermFailRewardMessage());
	::Unserialize(dbuff, Reqs());
	::Unserialize(dbuff, OKRewards());
	::Unserialize(dbuff, PermFailRewards());
	uint8 beh;
	dbuff.Read(beh); OkRewBehaviour() = (GUARD_REW_BEH)beh;
	dbuff.Read(beh); PermFailRewBehaviour() = (GUARD_REW_BEH)beh;
	dbuff.Read(m_seenIntro);	
}

void iMapGuard::Serialize(iDynamicBuffer& dbuff)
{	
	// reqs, rewards and messages		
	::Serialize(dbuff, Message());
	::Serialize(dbuff, CurFailMessage());
	::Serialize(dbuff, OKRewardMessage());
	::Serialize(dbuff, PermFailRewardMessage());
	::Serialize(dbuff, Reqs());
	::Serialize(dbuff, OKRewards());
	::Serialize(dbuff, PermFailRewards());		
	dbuff.Write((uint8)OkRewBehaviour());
	dbuff.Write((uint8)PermFailRewBehaviour());
	dbuff.Write(m_seenIntro);
}