#include "stdafx.h"
#include "BattleView.h"
#include "BattleAI.h"
#include "Dlg_BattleResult.h"
#include "Dlg_BattleHero.h"
#include "Dlg_BattleCastle.h"
#include "Dlg_SurrenderCond.h"
#include "Dlg_TownList.h"
#include "Dlg_BattleLog.h"
#include "Dlg_CastSpell.h"
#include "Dlg_Spellbook.h"
#include "Dlg_Settings.h"
#include "Dlg_ChooseBattleGroup.h"
#include "GameMenuDlg.h"
#include "Dlg_Save.h"
#include "Dlg_ScenProps.h"
#include "xxc/xxc.security.h"
#include "OverlandView.h"

#include "BattleView.stylus.h"

const iRect BATTLE_POPUP_BOUND(2,2,316,216);

iBattleView_stylus::iBattleView_stylus(): iBattleView(),
m_battleMode(BNM_MELEE)

{
	AddChild(m_pToolBar = new iBattleToolBar(&gApp.ViewMgr(), this, iRect(0, m_Rect.y2()-20,m_Rect.w, 21)));
	m_pToolBar->SetVisible(false);	
}



void iBattleView_stylus::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param) 
{
	uint32 uid = pView->GetUID();	

	if (uid == BCI_MODE_SWITCH) {
		if (cmd == CCI_TABCHANGED) {
			m_battleMode = (BattleNavMode)param;
		}
	}
	iBattleView::iCMDH_ControlCommand(pView, cmd, param);
}

void iBattleView_stylus::BeginAutobattle()
{
	m_pToolBar->SetVisible(false);	
	iBattleView::BeginAutobattle();	
}

void iBattleView_stylus::EndAutobattle()
{
	m_pToolBar->SetVisible(true);
	iBattleView::EndAutobattle();
}



void iBattleView_stylus::BeginBattle(iBattleWrapper* pBattle, SURF_TYPE st, BATTLE_BACK_TYPE background, BATTLE_SKY_TYPE sky)
{
	m_pBattle = pBattle;
	m_background = background;
	m_sky = sky;
	iBattleMember_Hero* pAssaulter = DynamicCast<iBattleMember_Hero*>(m_pBattle->Engine().GetBattleInfo().m_pAssaulter);
	check(pAssaulter);
	m_pToolBar->m_pBtnAssaulter->SetHero(pAssaulter->GetHero());
	if (iBattleMember_Hero* pHero = DynamicCast<iBattleMember_Hero*>(m_pBattle->Engine().GetBattleInfo().m_pDefender)){
		m_pToolBar->m_pBtnDefender->SetHero(pHero->GetHero());
	} else if (iBattleMember_Castle* pCastle = DynamicCast<iBattleMember_Castle*>(m_pBattle->Engine().GetBattleInfo().m_pDefender)){
		if (pCastle->GetVisitor()) m_pToolBar->m_pBtnDefender->SetHero(pCastle->GetVisitor());
	}

	PrepareSurface();

	// Prepare and distribute obstacles
	if (!pBattle->Engine().CastleFort() && !pBattle->Engine().OnShip()) {
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
			} while (pBattle->Engine().ObstaclesMap().GetAt(op.x, op.y) == CT_OBSTACLE || (size == 2 && pBattle->Engine().ObstaclesMap().GetAt(op.x+1, op.y) == CT_OBSTACLE));
			m_obstacles.Add(iBatObstEntry(olist[rval], op));
			pBattle->Engine().ObstaclesMap().GetAt(op.x, op.y) = CT_OBSTACLE;
			if (size == 2) pBattle->Engine().ObstaclesMap().GetAt(op.x+1, op.y) = CT_OBSTACLE;
			olist.RemoveAt(rval);
		}
	}
	// occupy catapult's cell with an obstacle
	if(pBattle->Engine().CastleFort()) {
		pBattle->Engine().ObstaclesMap().GetAt(0, 9) = CT_OBSTACLE;
		pBattle->Engine().ObstaclesMap().GetAt(1, 9) = CT_OBSTACLE;
	}

}

bool iBattleView_stylus::BeginSpellTrack(iCombatSpell* pSpell)
{
	m_pToolBar->SetVisible(false);	
	return iBattleView::BeginSpellTrack(pSpell);
}

void iBattleView_stylus::EndSpellTrack(const iPoint& cell)
{
	iBattleView::EndSpellTrack(cell);
	m_pToolBar->SetVisible(true);
}

void iBattleToolBar::UpdateSizeHelper(uint32 index, iRect &rc)
{
	iView *p = GetChildByIndex(index);
	if (p)
	{
		p->SetRect(rc);
		p->UpdateSize();
		p->Invalidate();
	}
}


void iBattleToolBar::UpdateSize()
{
	sint32 xpos = iABS<sint32>(m_Rect.w - 320) / 2;

	// Assaulter
	UpdateSizeHelper(0, iRect(xpos,1,32,20));
	xpos += 32;

	// splitter
	xpos += 1;
	UpdateSizeHelper(1, iRect(xpos,1,5,20));    
	xpos += 5+1;

	UpdateSizeHelper(2, iRect(xpos,1,BNM_COUNT*25,20)); // 2
	xpos += BNM_COUNT * 25;

	// splitter
	xpos += 1;
	UpdateSizeHelper(3, iRect(xpos,1,5,20)); // 3
	xpos += 5+1;

	// Wait and Defend buttons
	UpdateSizeHelper(4, iRect(xpos,1,25,20));
	xpos += 25;
	UpdateSizeHelper(5, iRect(xpos,1,25,20));
	xpos += 25;

	// splitter
	xpos += 1;
	UpdateSizeHelper(6, iRect(xpos,1,5,20)); // 6
	xpos += 5+1;

	// Cast spell button
	UpdateSizeHelper(7, iRect(xpos,1,24,20));
	xpos += 24;

	// Autobattle button
	UpdateSizeHelper(8, iRect(xpos,1,24,20));
	xpos += 24;

	// splitter
	xpos += 1;
	UpdateSizeHelper(9, iRect(xpos,1,5,20)); // 9
	xpos += 5+1;

	// Battle message log
	UpdateSizeHelper(10, iRect(xpos,1,24,20));
	xpos += 24;

	// Settings button
	UpdateSizeHelper(11, iRect(xpos,1,24,20));
	xpos += 24;

	// splitter
	xpos += 1;
	UpdateSizeHelper(12, iRect(xpos,1,5,20)); // 12
	xpos += 5+1;

	// Defender
	UpdateSizeHelper(13, iRect(xpos,1,32,20)); // 13

#ifdef MULTIPLAYER
	if(gMPMgr.IsMultiplayer())
		UpdateSizeHelper(14, iRect(rect.w - 30, 1, 30, 20)); // 14
#endif
}


void iBattleToolBar::EnableControls(uint32 flags)
{
	bool bCommon = (flags&iBattleView::EnemyTurn) == 0 && (flags&iBattleView::Acting) == 0 
#ifdef MULTIPLAYER
		&& (flags&RemoteTurn) == 0;
#else
		;
#endif
	m_pBtnAssaulter->SetEnabled(bCommon);
	m_pModeSwitch->SetEnabled(bCommon);
	m_pBtnWait->SetEnabled(bCommon && (flags&iBattleView::CanWait) != 0);
	m_pBtnDefend->SetEnabled(bCommon);
	m_pBtnCastSpell->SetEnabled(bCommon && (flags&iBattleView::CanCast) != 0);
	m_pBtnAutoBattle->SetEnabled(bCommon);
	m_pBtnMsgLog->SetEnabled(bCommon);
	m_pBtnSettings->SetEnabled(true); // settings btn must be enabled all the time
	if (m_pBtnDefender->Hero()) m_pBtnDefender->SetEnabled(bCommon);

	m_pModeSwitch->EnableTab(BNM_MELEE, (flags&iBattleView::CanMelee) != 0);
	m_pModeSwitch->EnableTab(BNM_SHOOT, (flags&iBattleView::CanShoot) != 0);
	m_pModeSwitch->EnableTab(BNM_INFO, (flags&iBattleView::CanInfo) != 0);

	SetVisible(bCommon);
}

void iBattleToolBar::OnCompose()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsMultiplayer()) {
		PLAYER_ID kickee = gGame.Map().RealCurPlayer();
		m_pKickBtn->SetVisible(gMPMgr.GetHostType() == iMPManager::HT_HOST && 
			gGame.Map().ActPlayer()->PlayerId() != kickee && 
			kickee != PID_NEUTRAL && 
			gGame.Map().FindPlayer(kickee)->PlayerType() == PT_REMOTE && 
			GetTickCount() - gMPMgr.LastActivity() > 5000);		
		if(m_pKickBtn->IsVisible()) {
			if(((iBattleWrapper*)gGame.BattleWrapper())->Engine().TurnSeq().CurUnit()->Owner()->GetSide() == iBattleMember::Assaulter)
			{
				m_pKickBtn->SetPos(m_pBtnAssaulter->GetPos());
				m_pBtnAssaulter->SetVisible(false);
			} else {
				m_pKickBtn->SetPos(m_pBtnDefender->GetPos());
				m_pBtnDefender->SetVisible(false);
			}
		} else {
			m_pBtnDefender->SetVisible(true);
			m_pBtnAssaulter->SetVisible(true);
		}
	}
#endif

	iRect rect = GetScrRect();
	gGfxMgr.BlitTile(PDGG_BKTILE, gApp.Surface(),rect);
	ButtonFrame(gApp.Surface(), rect, 0);
}


/*
*	Battle Toolbars
*/
iBattleToolBar::iBattleToolBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect)
: iView(pViewMgr, rect, GENERIC_VIEWPORT, 0, Enabled|Visible)
{
	sint32 xpos = iABS<sint32>(rect.w - 320) / 2;

	// Assaulter
	AddChild(m_pBtnAssaulter = new iHeroPortBtn(pViewMgr, pCmdHandler, iRect(xpos,1,32,20),BCI_ASSAULTER_BTN)); // 0
	xpos += 32;

	// splitter
	xpos += 1;
	AddChild(new iTBSplitter(pViewMgr, iRect(xpos,1,5,20)));  // 1
	xpos += 5+1;

	// Current creature mode tab
	m_pModeSwitch = new iBarTabSwitch(pViewMgr, pCmdHandler, iRect(xpos,1,BNM_COUNT*25,20), BNM_COUNT, BCI_MODE_SWITCH); 
	m_pModeSwitch->SetTabIcon(PDGG_BTN_ATTACK);
	m_pModeSwitch->SetTabIcon(PDGG_BTN_SHOOT);
	m_pModeSwitch->SetTabIcon(PDGG_BTN_INFO);
	m_pModeSwitch->SetCurrentTab(BNM_MELEE);
	AddChild(m_pModeSwitch); // 2
	xpos += BNM_COUNT * 25;

	// splitter
	xpos += 1;
	AddChild(new iTBSplitter(pViewMgr, iRect(xpos,1,5,20))); // 3
	xpos += 5+1;

	// Wait and Defend buttons
	AddChild(m_pBtnWait = new iIconButton(pViewMgr,pCmdHandler, iRect(xpos,1,25,20),PDGG_BTN_ENDTURN,BCI_WAIT_BTN));
	xpos += 25;
	AddChild(m_pBtnDefend = new iIconButton(pViewMgr,pCmdHandler, iRect(xpos,1,25,20),PDGG_BTN_DEFEND,BCI_DEFEND_BTN));
	xpos += 25;

	// splitter
	xpos += 1;
	AddChild(new iTBSplitter(pViewMgr, iRect(xpos,1,5,20))); // 6
	xpos += 5+1;

	// Cast spell button
	AddChild(m_pBtnCastSpell = new iIconButton(pViewMgr,pCmdHandler, iRect(xpos,1,24,20),PDGG_TAB_SPELLBOOK,BCI_CASTSPELL_BTN));
	xpos += 24;

	// Autobattle button
	AddChild(m_pBtnAutoBattle = new iIconButton(pViewMgr,pCmdHandler, iRect(xpos,1,24,20),PDGG_BTN_AUTOBATTLE,BCI_AUTOBATTLE_BTN));
	xpos += 24;

	// splitter
	xpos += 1;
	AddChild(new iTBSplitter(pViewMgr, iRect(xpos,1,5,20))); // 9
	xpos += 5+1;

	// Battle message log
	AddChild(m_pBtnMsgLog = new iIconButton(pViewMgr,pCmdHandler, iRect(xpos,1,24,20),PDGG_TAB_QUESTLOG,BCI_MSGLOG_BTN));
	xpos += 24;

	// Settings button
	AddChild(m_pBtnSettings = new iIconButton(pViewMgr,pCmdHandler, iRect(xpos,1,24,20),PDGG_BTN_MAINMENU,BCI_SETTINGS_BTN));
	xpos += 24;

	// splitter
	xpos += 1;
	AddChild(new iTBSplitter(pViewMgr, iRect(xpos,1,5,20))); // 12
	xpos += 5+1;

	// Defender
	AddChild(m_pBtnDefender = new iHeroPortBtn(pViewMgr, pCmdHandler, iRect(xpos,1,32,20),BCI_DEFENDER_BTN)); // 13

#ifdef MULTIPLAYER
	if(gMPMgr.IsMultiplayer())
		AddChild(m_pKickBtn = new iIconButton(&gApp.ViewMgr(), pCmdHandler, iRect(rect.w - 30, 1, 30, 20), PDGG_BTN_DISMISS, BCI_KICK_BTN, Visible | Enabled)); 
#endif
}

void iBattleView_stylus::UpdateToolbarMode()
{
	iBattleUnit* pCurUnit = m_pBattle->Engine().TurnSeq().CurUnit();
	iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(pCurUnit);
	iBattleGroup* pGroup  = pCurCreatGroup->GetCreatGroup();
	bool bCanShot = pGroup->Shots() && pGroup->ShootListCount();
	if (m_battleMode != BNM_SHOOT && bCanShot) {
		m_battleMode = BNM_SHOOT;
		m_pToolBar->m_pModeSwitch->SetCurrentTab(BNM_SHOOT);
	} else if (!bCanShot && m_battleMode != BNM_MELEE) {
		m_battleMode = BNM_MELEE;
		m_pToolBar->m_pModeSwitch->SetCurrentTab(BNM_MELEE);
	}
}

void iBattleView_stylus::EnableControls(uint32 flags) 
{ 
	m_pToolBar->EnableControls(flags);	
}

void iBattleView_stylus::OnMouseDown(const iPoint& pos, MouseId mID)
{

	if (!m_bHumanTurn || m_bAni) return;

	m_trackCell = Screen2Map(pos);


	if (SpellTracking()) {
		//if(m_SpellTrack != m_trackCell)
		//	m_bCurSelFlag = false;
		//m_SpellTrack = m_trackCell;
		iHero* pCaster = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
		check(pCaster);
		MAGIC_SCHOOL_LEVEL msl = m_pCastSpellToolBar->Spell()->GetSchoolLevel(pCaster);
		if (m_pCastSpellToolBar->Spell()->TargetMode(msl) == STM_CREAT_GROUP) {
			sint32 res = m_spellTargets.Find(m_trackCell);
			if (res != -1) {
				iBattleGroup* pTarget = m_pBattle->Engine().FindGroup(m_trackCell, m_pCastSpellToolBar->Spell()->AffectsToDead());
				check(pTarget);
				m_toolTip.Setf(gTextMgr[TRID_MSG_BAT_CAST_SPELL_TO], gTextMgr[m_pCastSpellToolBar->Spell()->NameKey()], gTextMgr[TRID_CREATURE_PEASANT_F3+pTarget->Type()*3]);
			} else {
				m_toolTip= _T("");
			}
		} else if (m_trackCell != cInvalidPoint) {
			m_toolTip.Setf(gTextMgr[TRID_MSG_BAT_CAST_SPELL], gTextMgr[m_pCastSpellToolBar->Spell()->NameKey()]);
		} else {
			m_toolTip= _T("");
		}
		Invalidate();

	} else if (m_battleMode == BNM_INFO || m_bForceInfo) {
		const iBattleGroup* pGroup = m_pBattle->Engine().FindGroup(m_trackCell);
		if (pGroup) {
			AddChild(m_pCreatInfoPopup = new iCreatInfoPopup(m_pMgr, pGroup->Owner()->Owner(), pGroup));
			if (pGroup->Pos().x > 5) m_pCreatInfoPopup->TrackPopup(iPoint(BATTLE_POPUP_BOUND.x,BATTLE_POPUP_BOUND.y),BATTLE_POPUP_BOUND,AlignLeft);
			else m_pCreatInfoPopup->TrackPopup(iPoint(BATTLE_POPUP_BOUND.x2(),BATTLE_POPUP_BOUND.y),BATTLE_POPUP_BOUND,AlignRight);
		} else {
			iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattle->Engine().TurnSeq().CurUnit());
			check(pCurCreatGroup);
			//iBattleGroup* pCreatGroup = pCurCreatGroup->GetCreatGroup();
			//OutputDebugString(iFormat(_T("Cells delta (%d,%d) -> (%d,%d) is %d\n"),pCreatGroup->Pos().x,pCreatGroup->Pos().y,m_trackCell.x,m_trackCell.y,CellsDelta2(pCreatGroup->Pos().x,pCreatGroup->Pos().y,m_trackCell.x,m_trackCell.y)).CStr());
		}
	} else if (m_battleMode == BNM_MELEE) {
		iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattle->Engine().TurnSeq().CurUnit());
		check(pCurCreatGroup);
		m_pMeleeTrack = pCurCreatGroup->GetCreatGroup()->GetMeleeEntry(m_trackCell);
		m_meleeDir = -1;
		const iBattleGroup* pGroup = m_pBattle->Engine().FindGroup(m_trackCell);
		if (pGroup && m_pMeleeTrack) {
			/*m_toolTip.Setf(gTextMgr[TRID_MSG_BAT_ATTACK], gTextMgr[TRID_CREATURE_PEASANT_F3+pGroup->Type()*3]);
			sint32 min_dmg = pCurCreatGroup->GetCreatGroup()->CalcDamage(pGroup, pCurCreatGroup->GetCreatGroup()->ActMinDamage(), false, 1, pCurCreatGroup->GetCreatGroup()->CalcJoustingBonus(m_pMeleeTrack->m_pos));
			sint32 max_dmg = pCurCreatGroup->GetCreatGroup()->CalcDamage(pGroup, pCurCreatGroup->GetCreatGroup()->ActMaxDamage(), false, 1, pCurCreatGroup->GetCreatGroup()->CalcJoustingBonus(m_pMeleeTrack->m_pos));
			if(max_dmg == min_dmg) m_toolTip.Addf(gTextMgr[TRID_MSG_BAT_DAMAGE1], min_dmg);
			else m_toolTip.Addf(gTextMgr[TRID_MSG_BAT_DAMAGE2], min_dmg, max_dmg);
			*/
		}
		Invalidate();
	} else if (m_battleMode == BNM_SHOOT) {
		iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattle->Engine().TurnSeq().CurUnit());
		check(pCurCreatGroup);
		m_pShootTrack = pCurCreatGroup->GetCreatGroup()->GetShootEntry(m_trackCell);
		const iBattleGroup* pGroup = m_pBattle->Engine().FindGroup(m_trackCell);
		if (pGroup && m_pShootTrack) {
			m_toolTip.Setf(gTextMgr[TRID_MSG_BAT_SHOOT], gTextMgr[TRID_CREATURE_PEASANT_F3+pGroup->Type()*3]);
			sint32 min_dmg = pCurCreatGroup->GetCreatGroup()->CalcDamage(pGroup, pCurCreatGroup->GetCreatGroup()->ActMinDamage(), true, m_pShootTrack->m_penalty, 0);
			sint32 max_dmg = pCurCreatGroup->GetCreatGroup()->CalcDamage(pGroup, pCurCreatGroup->GetCreatGroup()->ActMaxDamage(), true, m_pShootTrack->m_penalty, 0);
			if (max_dmg == min_dmg) m_toolTip.Addf(gTextMgr[TRID_MSG_BAT_DAMAGE1], min_dmg);
			else m_toolTip.Addf(gTextMgr[TRID_MSG_BAT_DAMAGE2], min_dmg, max_dmg);
		}
		Invalidate();
	}
}

void iBattleView_stylus::OnMouseUp(const iPoint& pos, MouseId mID)
{

	if (!m_bHumanTurn || m_bAni) return;

	iPoint curCell = Screen2Map(pos);
	//m_trackPos = pos;
	//m_trackCell = Screen2Map(pos);

	iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattle->Engine().TurnSeq().CurUnit());

	if(!pCurCreatGroup) return; //this is a small fix for crash when pressing 'autocombat' button (Hedin)

	iBattleGroup *pGroup = pCurCreatGroup->GetCreatGroup();

	if (SpellTracking()) {
		EndSpellTrack(m_trackCell );
	}
	else if (m_pCreatInfoPopup || m_battleMode == BNM_INFO || m_bForceInfo) {
		if (m_pCreatInfoPopup) {
			RemoveChild(m_pCreatInfoPopup);
			m_pCreatInfoPopup->HidePopup();
			delete m_pCreatInfoPopup;
			m_pCreatInfoPopup = NULL;
		}
	} else if (m_battleMode == BNM_MELEE && m_pMeleeTrack) {
		if ( m_meleeDir != -1) {
			check (m_meleeDir < MDIR_COUNT);
			m_pBattle->Engine().Melee(m_pMeleeTrack->m_pos, (uint16)m_meleeDir);
			BeginAni();
			m_meleeDir = -1;
		}
		m_pMeleeTrack = NULL;
	} else if (m_battleMode == BNM_SHOOT) {
		if (m_pShootTrack) {
			m_pBattle->Engine().Shot(m_pShootTrack->m_pos, m_pShootTrack->m_penalty);
			BeginAni();
			m_pShootTrack = NULL;
		}
	} else if (iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattle->Engine().TurnSeq().CurUnit())) {
		iPoint nCell = Screen2Map(pos);
		if (nCell == m_trackCell && pCurCreatGroup->GetCreatGroup()->CanMove(nCell.x,nCell.y) && !pCurCreatGroup->GetCreatGroup()->IsGroupCell(nCell)) {
			m_pBattle->Engine().Move(nCell, pCurCreatGroup->GetCreatGroup()->Orient());
			BeginAni();
		}
	}
	m_trackCell = cInvalidPoint;
	m_trackPos = cInvalidPoint;
	if (!m_toolTip.Empty()) m_toolTip = _T("");
	Invalidate();
}


void iBattleView_stylus::OnMouseTrack(const iPoint& pos, MouseId mID)
{
	if (!m_bHumanTurn || m_bAni) return;
	m_trackPos = pos;
	m_trackCell = Screen2Map(pos);

	if (SpellTracking()) {
		m_trackCell = Screen2Map(pos);

		iHero* pCaster = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
		check(pCaster);
		MAGIC_SCHOOL_LEVEL msl = m_pCastSpellToolBar->Spell()->GetSchoolLevel(pCaster);

		if (m_pCastSpellToolBar->Spell()->TargetMode(msl) == STM_CREAT_GROUP) {
			sint32 res = m_spellTargets.Find(m_trackCell);
			if (res != -1) {
				iBattleGroup* pTarget = m_pBattle->Engine().FindGroup(m_trackCell, m_pCastSpellToolBar->Spell()->AffectsToDead());
				check(pTarget);
				m_toolTip.Setf(gTextMgr[TRID_MSG_BAT_CAST_SPELL_TO], gTextMgr[m_pCastSpellToolBar->Spell()->NameKey()], gTextMgr[TRID_CREATURE_PEASANT_F3+pTarget->Type()*3]);
			} else {
				m_toolTip= _T("");
			}
		} else if (m_trackCell != cInvalidPoint) {
			m_toolTip.Setf(gTextMgr[TRID_MSG_BAT_CAST_SPELL], gTextMgr[m_pCastSpellToolBar->Spell()->NameKey()]);
		} else {
			m_toolTip= _T("");
		}
		Invalidate();
	} else if (m_battleMode == BNM_MELEE && m_pMeleeTrack) {
		iPoint center = Map2Screen(m_pMeleeTrack->m_pos);
		center.Move(10,9);
		sint32 xdiff = pos.x - center.x;
		sint32 ydiff = pos.y - center.y;
		if ( iABS(xdiff) > 30 || iABS(ydiff) > 30) {
			m_meleeDir = -1;
		} else {
			sint32 ndir = -1;

			if ( ((m_pMeleeTrack->m_cfg & (1<<MDIR_LSPTOP)) || (m_pMeleeTrack->m_cfg & (1<<MDIR_LSPBOTTOM))) && xdiff<=0 && iABS(xdiff) > iABS(ydiff)) {
				if (ydiff<0) ndir = MDIR_LSPTOP;
				else ndir = MDIR_LSPBOTTOM;
			} else if (((m_pMeleeTrack->m_cfg & (1<<MDIR_RSPTOP)) || (m_pMeleeTrack->m_cfg & (1<<MDIR_RSPBOTTOM))) && xdiff>0 && iABS(xdiff) > iABS(ydiff)) {
				if (ydiff<0) ndir = MDIR_RSPTOP;
				else ndir = MDIR_RSPBOTTOM;
			} else if ( (m_pMeleeTrack->m_cfg & (1<<MDIR_NORTH)) && ydiff <= 0 && iABS(ydiff) > (iABS(xdiff)*3)) {
				ndir = MDIR_NORTH;
			} else if (xdiff <= 0 && ydiff <= 0 && iABS(ydiff) > iABS(xdiff)) {
				ndir = MDIR_NORTHWEST;
			} else if (xdiff > 0 && ydiff <= 0 && iABS(ydiff) > iABS(xdiff)) {
				ndir = MDIR_NORTHEAST;
			} else if ((m_pMeleeTrack->m_cfg & (1<<MDIR_SOUTH)) && ydiff > 0 && iABS(ydiff) > (iABS(xdiff)*3)) {
				ndir = MDIR_SOUTH;
			} else if (xdiff <= 0 && ydiff > 0 && iABS(ydiff) > iABS(xdiff)) {
				ndir = MDIR_SOUTHWEST;
			} else if (xdiff > 0 && ydiff > 0 && iABS(ydiff) > iABS(xdiff)) {
				ndir = MDIR_SOUTHEAST;
			} else if (xdiff <= 0  && iABS(ydiff) <= iABS(xdiff)) {
				ndir = MDIR_WEST;
			} else if (xdiff > 0  && iABS(ydiff) <= iABS(xdiff)) {
				ndir = MDIR_EAST;
			}

			if (m_pMeleeTrack->m_cfg & (1<<ndir)) m_meleeDir = ndir;
			else m_meleeDir = -1;
		}		
		if(iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattle->Engine().TurnSeq().CurUnit()))
			if (m_pMeleeTrack && m_meleeDir != -1) {
				iBattleGroup *pGroup = m_pBattle->Engine().FindGroup(m_pMeleeTrack->m_pos);			

				iPoint mp;
				uint8 aor;
				pCurCreatGroup->GetCreatGroup()->GetMeleePosition(pGroup, m_pMeleeTrack->m_pos, m_meleeDir, mp, aor);	

				m_toolTip.Setf(gTextMgr[TRID_MSG_BAT_ATTACK], gTextMgr[TRID_CREATURE_PEASANT_F3+pGroup->Type()*3]);
				sint32 min_dmg = pCurCreatGroup->GetCreatGroup()->CalcDamage(pGroup, pCurCreatGroup->GetCreatGroup()->ActMinDamage(), false, 1, pCurCreatGroup->GetCreatGroup()->CalcJoustingBonus(mp));
				sint32 max_dmg = pCurCreatGroup->GetCreatGroup()->CalcDamage(pGroup, pCurCreatGroup->GetCreatGroup()->ActMaxDamage(), false, 1, pCurCreatGroup->GetCreatGroup()->CalcJoustingBonus(mp));
				if(max_dmg == min_dmg) m_toolTip.Addf(gTextMgr[TRID_MSG_BAT_DAMAGE1], min_dmg);
				else m_toolTip.Addf(gTextMgr[TRID_MSG_BAT_DAMAGE2], min_dmg, max_dmg);
			} else {
				m_toolTip.Setf(_T(""));
			}

			Invalidate();
	} else if (m_battleMode == BNM_SHOOT) {
		m_trackCell = Screen2Map(pos);
		iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattle->Engine().TurnSeq().CurUnit());
		check(pCurCreatGroup);
		iPoint ncell = Screen2Map(pos);
		if (!m_pShootTrack || ncell != m_pShootTrack->m_pos) m_pShootTrack = pCurCreatGroup->GetCreatGroup()->GetShootEntry(ncell);

		const iBattleGroup* pGroup = m_pBattle->Engine().FindGroup(ncell);
		if (m_pShootTrack && pGroup) {
			m_toolTip.Setf(gTextMgr[TRID_MSG_BAT_SHOOT], gTextMgr[TRID_CREATURE_PEASANT_F3+pGroup->Type()*3]);
			sint32 min_dmg = pCurCreatGroup->GetCreatGroup()->CalcDamage(pGroup, pCurCreatGroup->GetCreatGroup()->ActMinDamage(), true, m_pShootTrack->m_penalty, 0);
			sint32 max_dmg = pCurCreatGroup->GetCreatGroup()->CalcDamage(pGroup, pCurCreatGroup->GetCreatGroup()->ActMaxDamage(), true, m_pShootTrack->m_penalty, 0);
			if (max_dmg == min_dmg) m_toolTip.Addf(gTextMgr[TRID_MSG_BAT_DAMAGE1], min_dmg);
			else m_toolTip.Addf(gTextMgr[TRID_MSG_BAT_DAMAGE2], min_dmg, max_dmg);
		} else {
			m_toolTip = _T("");
		}

		Invalidate();
	}
}

void iBattleView_stylus::ComposeSpellCursor()
{
	iHero* pCaster = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
	check(pCaster);

	MAGIC_SCHOOL_LEVEL msl = m_pCastSpellToolBar->Spell()->GetSchoolLevel(pCaster);
	if (m_pCastSpellToolBar->Spell()->TargetMode(msl) == STM_CREAT_GROUP) {
		//			if (m_SpellTrack != cInvalidPoint && m_spellTargets.Find(m_SpellTrack) != -1) {
		iPoint cop(m_trackPos.x - (sint32)(gGfxMgr.Dimension(PDGG_SPELL_CURSOR).w/2), m_trackPos.y - (sint32)(gGfxMgr.Dimension(PDGG_SPELL_CURSOR).h/2));
		gGfxMgr.BlitEffect( PDGG_SPELL_CURSOR, gApp.Surface(), cop,iGfxManager::EfxShadow2D);
		gGfxMgr.Blit( PDGG_SPELL_CURSOR, gApp.Surface(), cop);
	}
}

void iBattleView_stylus::ComposeBattleCursors()
{
	if (m_battleMode == BNM_MELEE) {
		if (m_pMeleeTrack && m_meleeDir != -1) {
			iPoint cop = Map2Screen(m_pMeleeTrack->m_pos);
			gGfxMgr.BlitEffect( PDGG_MELEE_CURSOR+m_meleeDir, gApp.Surface(), cop,iGfxManager::EfxShadow2D);
			gGfxMgr.Blit( PDGG_MELEE_CURSOR+m_meleeDir, gApp.Surface(), cop);
		}
	} else if (m_battleMode == BNM_SHOOT) {
		if (m_pShootTrack) {
			SpriteId sid = PDGG_SHOOT_CURSOR;
			iPoint cell = Map2Screen(m_trackCell);
			if (m_pShootTrack->m_penalty > 1) sid += m_pShootTrack->m_penalty/2;
			iPoint cop(cell.x, cell.y - (sint32)(gGfxMgr.Dimension(sid).h/2));
			gGfxMgr.BlitEffect( sid, gApp.Surface(), cop,iGfxManager::EfxShadow2D);
			gGfxMgr.Blit( sid, gApp.Surface(), cop);
		}
	}
}

void iBattleView_stylus::UpdateSize()
{
	iBattleView::UpdateSize();
	m_pToolBar->SetRect(iRect(0, m_Rect.y2()-20,m_Rect.w, 21));
	m_pToolBar->UpdateSize();
	m_pToolBar->Invalidate();
}

bool iBattleView_stylus::OnKeyDown(sint32 key)
{
	if (!m_bHumanTurn) return false;
	if (key == gSettings.ActionKey(BAT_HELP_MODE)) {
		m_bForceInfo = true;
		m_pToolBar->SetEnabled(false);
	} else {
		return false;
	}
	return true;
}

bool iBattleView_stylus::OnKeyUp(sint32 key)
{
	if (!m_bHumanTurn) return false;
	if (key == gSettings.ActionKey(BAT_HELP_MODE)) {
		m_bForceInfo = false;
		m_pToolBar->SetEnabled(true);
	} else {
		return false;
	}
	return true;
}

