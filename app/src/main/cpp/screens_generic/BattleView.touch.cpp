#include "stdafx.h"
#include "BattleView.h"
#include "BattleAI.h"
#include "Dlg_BattleResult.h"
#include "Dlg_BattleHero.h"
#include "Dlg_BattleCastle.h"
#include "Dlg_SurrenderCond.h"
#include "Dlg_TownList.h"
#include "Dlg_BattleLog.h"
#include "Dlg_Spellbook.h"
#include "Dlg_Settings.h"
#include "Dlg_ChooseBattleGroup.h"
#include "GameMenuDlg.h"
#include "Dlg_Save.h"
#include "Dlg_ScenProps.h"
//#include "xxc/xxc.security.h"
#include "OverlandView.h"

#include "BattleView.touch.h"
#include "Dlg_CreatInfo.h"


// from BattleAI.cpp
sint32 CalcMeleeValue(iBattleGroup* pGroup, const iBattleGroup::iMeleeEntry* se, iBattleGroup* pTarget, iBattleEngine& engine);
void CalcBestMeleeDir(iBattleGroup* pGroup, iBattleGroup* pTarget, const iBattleGroup::iMeleeEntry* me, iBattleEngine& engine, uint16& mdir, sint32 &cval, bool bForceFF);
#ifdef OS_IPHONE
void DoubleDib(const iDib& src, iDib &dst);
#endif

#define AWI_SIZE 60 //action wheel item size

iBattleView_touch::iBattleView_touch(): iBattleView()
, m_bCurSelFlag(false)
, m_bModeChangeFlag(false)
, m_bGestureComplete(false)
, m_SpellTrack(cInvalidPoint)
, m_bGesturesEnabled(true)
, m_moveDisapTimer(fix32::zero)
, m_bInfoMode(false)
#ifdef OS_IPHONE
, m_pZoomer(NULL)
, m_pActionWheel(NULL)
, m_bAlwaysReceiveMouse(false)
#endif
{	
	AddChild(m_pBtnCastSpell = new iImgBtn(&gApp.ViewMgr(), this, iRect(), BCI_CASTSPELL_BTN, PDGG(BTN_SPELLBOOK), PDGG(BTN_SPELLBOOK_PRESSED), iStringT(), Visible|Enabled));
	AddChild(m_pBtnWait = new iImgBtn(&gApp.ViewMgr(), this, iRect(), BCI_WAIT_BTN, PDGG(BTN_END_TURN), PDGG(BTN_END_TURN_PRESSED), iStringT(), Visible|Enabled));	
	AddChild(m_pBtnDefend = new iImgBtn(&gApp.ViewMgr(), this, iRect(), BCI_DEFEND_BTN, PDGG(BTN_DEFEND), PDGG(BTN_DEFEND_PRESSED), iStringT(), Visible|Enabled));	
//	AddChild(m_pBtnInfo = new iImgBtn(&gApp.ViewMgr(), this, iRect(), BCI_INFO_BTN, PDGG(BTN_INFOBAR), PDGG(BTN_INFOBAR_I_PRESSED)));
//	m_pBtnInfo->SetHoldable(true);

    if(!gUseIpadUI)
    {
        AddChild(m_pBtnInfo = new iImgBtn(&gApp.ViewMgr(), this, iRect(), BCI_INFO_BTN, PDGG(BTN_INFO), PDGG(BTN_INFOBAR_PRESSED)));
        m_pBtnInfo->SetHoldable(true);
    } else
    {
        AddChild(m_pBtnInfo = new iImgBtn(&gApp.ViewMgr(), this, iRect(), BCI_INFO_BTN, PDGG(BTN_INFOBAR), PDGG(BTN_INFOBAR_PRESSED)));
        m_pBtnInfo->SetHoldable(true);
    }
    
	AddChild(m_pCreatInfoView = new iCreatDescView(&gApp.ViewMgr(), this, iRect(), 50, CDV_BKND | CDV_TITLE | CDV_NOTITLE_PERKS | CDV_GLOW
#ifndef PC_VERSION
                                                   | CDV_STATS
#endif
                                                   , NULL));
	m_pCreatInfoView->SetVisible(false);
	
	AddChild(m_pBtnAssaulter = new iHeroPortBtn(&gApp.ViewMgr(), this, iRect(),  BCI_ASSAULTER_BTN, false, false, AlignBottomLeft));
	AddChild(m_pBtnDefenderHero = new iHeroPortBtn(&gApp.ViewMgr(), this, iRect(),  BCI_DEFENDER_BTN, false, false, AlignBottomRight));
	AddChild(m_pBtnDefenderCastle = new iCastleGlyphBtn(&gApp.ViewMgr(), this, iRect(),  BCI_DEFENDER_BTN, false));
	
}

void iBattleView_touch::UpdateSize()
{
#if defined PC_VERSION
    Recalc();
#endif    
	
	iPoint offset = iPoint(0,0); 
    iPoint offset2 = iPoint(0,0);
    if(!gUseIpadUI)
    {
        offset.y = 8;
        offset2.y = 3;
    }
    m_pAutoBattleToolBar->SetRect(iRect(0, m_Rect.y2()-DEF_BTN_HEIGHT - 5,m_Rect.w, DEF_BTN_HEIGHT + 5));
	if(m_pCastSpellToolBar)
        m_pCastSpellToolBar->SetRect(iRect(0, m_Rect.y2()-DEF_BTN_HEIGHT - 5, m_Rect.w, DEF_BTN_HEIGHT + 5));

	//SetRect(m_pMgr->Metrics());
	m_pBtnAssaulter->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottomLeft) + offset2);
	m_pBtnDefenderHero->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottomRight) + offset2);
	m_pBtnDefenderCastle->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottomRight) + offset2);

    iPoint devoff;
    if(!gUseIpadUI)
        devoff = iPoint(-(sint32)iIphoneBtn::sm_defSize.w * 0.5, 0);
    else
        devoff = iPoint(-(sint32)iIphoneBtn::sm_defSize.w * 2, 0);
    
    m_pBtnCastSpell->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottom) + offset + devoff);
	m_pBtnWait->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottom) + offset + 
#ifdef OS_IPHONE
		iPoint(iIphoneBtn::sm_defSize.w * 0.5, 0)						
#else
		iPoint()
#endif
						);
	
    iPoint devOffset;
    if(!gUseIpadUI)
        devOffset = iPoint(iIphoneBtn::sm_defSize.w * 1.5, 0);
    else
        devOffset = iPoint(iIphoneBtn::sm_defSize.w * 2, 0);
        
        
    m_pBtnDefend->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottom) + offset + devOffset);
    

#ifndef PC_VERSION
    if(!gUseIpadUI)
        m_pBtnInfo->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottom) - iPoint(iIphoneBtn::sm_defSize.w * 1.5, 0) + offset);
    else
        m_pBtnInfo->SetRect(AlignChild(iSize(40, 75), AlignLeft));

#endif
}

const iPoint iBattleView_touch::fingerOffset(0, -30);

void iBattleView_touch::BeginBattle(iBattleWrapper* pBattle, SURF_TYPE st, BATTLE_BACK_TYPE background, BATTLE_SKY_TYPE sky)
{
	m_pBattle = pBattle;
	m_background = background;
	m_sky = sky;
	iBattleMember_Hero* pAssaulter = DynamicCast<iBattleMember_Hero*>(m_pBattle->Engine().GetBattleInfo().m_pAssaulter);
	check(pAssaulter);
	
	m_pBtnAssaulter->SetHero(pAssaulter->GetHero());
	if (iBattleMember_Hero* pHero = DynamicCast<iBattleMember_Hero*>(m_pBattle->Engine().GetBattleInfo().m_pDefender)) {
		m_pBtnDefenderHero->SetHero(pHero->GetHero());		
		m_pBtnDefenderCastle->SetCastle(NULL);
	} else if (iBattleMember_Castle* pCastle = DynamicCast<iBattleMember_Castle*>(m_pBattle->Engine().GetBattleInfo().m_pDefender)){
		if (pCastle->GetVisitor()) {
			m_pBtnDefenderHero->SetHero(pCastle->GetVisitor());					
			m_pBtnDefenderCastle->SetCastle(NULL);
		}
		else {
			m_pBtnDefenderCastle->SetCastle(pCastle->GetCastle());					
			m_pBtnDefenderHero->SetHero(NULL);
		}
	} else {
		m_pBtnDefenderHero->SetHero(NULL);
		m_pBtnDefenderCastle->SetCastle(NULL);
	}

	UpdateSize();

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

bool iBattleView_touch::BeginSpellTrack(iCombatSpell* pSpell)
{	
	m_SpellTrack = cInvalidPoint;
	m_bCurSelFlag = false;	
	return iBattleView::BeginSpellTrack(pSpell);
}

void iBattleView_touch::EnableControls(uint32 flags)
{
	bool bModal = gApp.ViewMgr().HasModalDlg();

	iBattleUnit* pCurUnit = m_pBattle->Engine().TurnSeq().CurUnit();
	iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(pCurUnit);
	iBattleGroup* pGroup  = pCurCreatGroup ? pCurCreatGroup->GetCreatGroup() : NULL;
	iHero* pHero = pCurUnit ? pCurUnit->Owner()->SpellCaster() : NULL;

	PLAYER_ID pid = pGroup ? pGroup->Owner()->Owner() : PID_NEUTRAL;
	bool bEnemyTurn = (pid == PID_NEUTRAL || gGame.Map().FindPlayer(pid)->PlayerType() == PT_COMPUTER);

#ifdef MULTIPLAYER
TODO: add remote player
#endif

	bool bCanWait = m_pBattle->Engine().CanWait();	
	bool bAutobattle = IsAutobattle();
	bool bCanCast = pHero && pHero->ManaPts() > 0 &&  
		m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->CanCastSpell() &&
		pHero->SpellBook().SpellsCount() > 0;
	bool bActing = IsAni();
	bool bSpellTracking = SpellTracking();

	bool bCommon = !bEnemyTurn && !bActing && !bSpellTracking && !bModal && !m_bInfoMode && !bAutobattle;
		
	m_pBtnCastSpell->SetVisible(bCommon && bCanCast);
#ifdef OS_IPHONE
	m_pBtnInfo->SetVisible(bCommon || m_bInfoMode);
#endif
	m_pBtnWait->SetVisible(bCommon && bCanWait);
	m_pBtnDefend->SetVisible(bCommon);

	m_pBtnDefenderCastle->SetVisible(bCommon);
	m_pBtnDefenderHero->SetVisible(bCommon);
	m_pBtnAssaulter->SetVisible(bCommon);
		
	m_bGesturesEnabled = bCommon;
}

bool iBattleView_touch::OnGroupChanged()
{
	m_bCurSelFlag = false;
#ifdef PC_VERSION
    m_Entered = cInvalidPoint;
#endif
	return iBattleView::OnGroupChanged();
}

iBattleGroup* iBattleView_touch::FindNearestGroup(iSimpleArray<iBattleGroup*> &list, const iPoint &pos, sint32 &best)
{
	best = 1000;
	iBattleGroup *res = NULL;
	for(uint32 x=0; x<list.GetSize(); x++)
		if(list[x]->Pos().GetDelta(pos) < best) {
			best = list[x]->Pos().GetDelta(pos);
			res = list[x];
		}
		return res;
}

iPoint iBattleView_touch::FindNearestCell(iSimpleArray<iPoint> &list, const iPoint &pos, sint32 &best)
{
	best = 1000;
	iPoint res = cInvalidPoint;
	for(uint32 x=0; x<list.GetSize(); x++)
		if(Map2ScreenCenter(list[x]).GetDelta(pos) < best) {
			best = Map2ScreenCenter(list[x]).GetDelta(pos);
			res = list[x];
		}
		return res;
}



iSimpleArray<iBattleGroup*> iBattleView_touch::FindBattleGroupsWithAttraction(const iPoint &cell, sint32 attr)
{
	iSimpleArray<iBattleGroup*> res;
	iBattleGroup* pGroup = m_pBattle->Engine().FindGroup(cell);
	if(pGroup)
		res.Add(pGroup);
	if(!pGroup) {
		for(sint32 xx=-attr; xx<=attr; xx++) 
			for(sint32 yy=-attr; yy<=attr; yy++) 
				if(m_pBattle->Engine().FindGroup(cell + iPoint(xx, yy))) {
					pGroup = m_pBattle->Engine().FindGroup(cell + iPoint(xx, yy));
					if(pGroup)
						res.Add(pGroup);
				}
	}
	return res;
}

void iBattleView_touch::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	// AUTOBATTLE: end autobattle
	if (m_pAutoBattleToolBar && m_pAutoBattleToolBar->IsVisible())
	{
		EndAutobattle();
		return;
	}
#ifdef OS_IPHONE
    // we clicked through the info view to disable info mode, release the button
	if(m_pCreatInfoView->IsVisible() && m_pBtnInfo->GetScrRect().PtInRect(pos)) {
		m_pBtnInfo->MouseDown(pos, mID, mbID);
		return;
	}
#endif

	// not our turn or animation playing: return
	if (!m_bHumanTurn || m_bAni || m_pBattle->IsBlocked()) return;
	m_bGestureComplete = false;

	// tapped on a child control, skip
	if(iView* pView = GetChildByPos(pos)) {
#ifdef OS_IPHONE
        if(pView != m_pCreatInfoView)
#endif
			return;		
	}	
	
	// INFO MODE: show creat info and exit
	if(m_bInfoMode) {
		iBattleGroup* pGroup = m_pBattle->Engine().FindGroup(Screen2Map(pos));
		if(pGroup)
		{
			m_highlightedGroup = pGroup->Pos();
			m_pCreatInfoView->SetVisible(true);
#ifndef PC_VERSION
            m_bAlwaysReceiveMouse = true;
#endif
			iPoint corner = iPoint(pos.x > m_Rect.w / 2 ? 0 : m_Rect.w, pos.y > m_Rect.h / 2 ? 0 : m_Rect.h);
			iRect temprc = iRect(pos, corner);
			iSize sz = m_pCreatInfoView->GetSize(m_Rect.w / 4, pGroup->Type(), pGroup->Count(), CDV_TITLE | CDV_BKND | CDV_NOTITLE_PERKS | CDV_GLOW
#ifndef PC_VERSION
                                                 | CDV_STATS
#endif
                                                 , pGroup);			
			iRect cvrc = AlignRect(sz, temprc, AlignCenter);
#ifndef PC_VERSION
            cvrc.InflateRect(DLG_GLOW_FRAME_SIZE, 5, 0, 0);
#endif
			if(cvrc.x < 0) cvrc.x = 0;
			if(cvrc.y < 0) cvrc.y = 0;
			if(cvrc.x2() > m_Rect.w) cvrc.x -= (cvrc.x2() - m_Rect.w);
			if(cvrc.y2() > m_Rect.h) cvrc.y -= (cvrc.y2() - m_Rect.h);
			m_pCreatInfoView->SetRect(cvrc);
			m_pCreatInfoView->SetGroup(pGroup);			
			Invalidate();
		}
		return;
	}
	
	m_trackCell = Screen2Map(pos);
	m_MoveEntry = m_trackCell;
	Invalidate();
	m_trackPos = pos;
	m_initialTrackCell = m_trackCell;
	m_bModeChangeFlag = false;


	// SPELL CASTING: choose target
	if (SpellTracking()) {
		//m_SpellTrack = m_trackCell;
		//check(m_SpellTrack == Screen2Map(Map2Screen(m_SpellTrack)));
		iHero* pCaster = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
		check(pCaster);
		MAGIC_SCHOOL_LEVEL msl = m_pCastSpellToolBar->Spell()->GetSchoolLevel(pCaster);

		if (m_pCastSpellToolBar->Spell()->TargetMode(msl) == STM_CREAT_GROUP) {
			sint32 dist;
			iPoint targCell = FindNearestCell(m_spellTargets, pos, dist);
			if(targCell != cInvalidPoint && dist < sm_hexWidth) {
#ifndef PC_VERSION
				if(m_SpellTrack != targCell)
					m_bCurSelFlag = false;
#else
                m_bCurSelFlag = true;
#endif
				m_SpellTrack = targCell;
			}
			else
				m_SpellTrack = cInvalidPoint;
		} else {
			if(m_SpellTrack == cInvalidPoint || Screen2Map(pos) != m_SpellTrack) {
				if(m_SpellTrack != m_trackCell)
					m_bCurSelFlag = false;
				m_SpellTrack = m_trackCell;			
			}
		}

		if (m_pCastSpellToolBar->Spell()->TargetMode(msl) == STM_CREAT_GROUP) {
			sint32 res = m_spellTargets.Find(m_trackCell);
			if (res != -1) {
				iBattleGroup* pTarget = m_pBattle->Engine().FindGroup(m_trackCell, m_pCastSpellToolBar->Spell()->AffectsToDead());
				check(pTarget);
				m_pCastSpellToolBar->SetTarget(pTarget);
			} 
		}
		Invalidate();
		return;
	} 

	// BATTLE TARGET SELECTION
	iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattle->Engine().TurnSeq().CurUnit());
	iBattleGroup *pGroup = pCurCreatGroup->GetCreatGroup();

	int oldMode = m_pShootTrack ? 0 : (m_pMeleeTrack ? 1 : 2);

	if(!(m_pShootTrack = pGroup->GetShootEntry(m_trackCell))) {
		m_pMeleeTrack = pGroup->GetMeleeEntry(m_trackCell);
	}
	else {
		m_pMeleeTrack = NULL;
	}

//#ifdef MOVE_CONFIRMATION
	if(m_pMeleeTrack || m_pShootTrack || !pGroup->CanMove(m_trackCell.x, m_trackCell.y))
		m_MoveEntry = cInvalidPoint;
//#endif

	int newMode = m_pShootTrack ? 0 : (m_pMeleeTrack ? 1 : 2);

	if(oldMode != newMode)
		m_bCurSelFlag = false;


}
#ifdef PC_VERSION
void iBattleView_touch::OnMouseEntered(const iPoint& pos, MouseId mID){

    
    if( 
#ifdef PC_VERSION
       gGame.bRightDown || 
#endif
       m_bInfoMode ){
    
        iBattleGroup* pGroup = m_pBattle->Engine().FindGroup(Screen2Map(pos));
		if(pGroup)

		{
			m_highlightedGroup = pGroup->Pos();
			m_pCreatInfoView->SetVisible(true);
			iPoint corner = iPoint(pos.x > m_Rect.w / 2 ? 0 : m_Rect.w, pos.y > m_Rect.h / 2 ? 0 : m_Rect.h);
			iRect temprc = iRect(pos, corner);
			iSize sz = m_pCreatInfoView->GetSize(m_Rect.w / 4, pGroup->Type(), pGroup->Count(), CDV_TITLE | CDV_BKND | CDV_LINEAR_PERKS | CDV_GLOW, pGroup);			
			iRect cvrc = AlignRect(sz, temprc, AlignCenter);
			if(cvrc.x < 0) cvrc.x = 0;
			if(cvrc.y < 0) cvrc.y = 0;
			if(cvrc.x2() > m_Rect.w) cvrc.x -= (cvrc.x2() - m_Rect.w);
			if(cvrc.y2() > m_Rect.h) cvrc.y -= (cvrc.y2() - m_Rect.h);
			m_pCreatInfoView->SetRect(cvrc);
			m_pCreatInfoView->SetGroup(pGroup);			
			Invalidate();
		}        
        return;
    }
    else{
    
        m_pCreatInfoView->SetVisible(false);
        m_highlightedGroup = cInvalidPoint;
    }
    
    iPoint cell = Screen2Map(pos);
    iPoint op = anchor;
#ifdef PC_VERSION
    m_Entered = cInvalidPoint;
#endif
    
    if( !SpellTracking() ){
    
        if (iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattle->Engine().TurnSeq().CurUnit())){
            if (pCurCreatGroup->GetCreatGroup()->CanMove(cell.x,cell.y)) {
#ifdef PC_VERSION
                m_Entered = cell;
#endif
                //gGfxMgr.Blit( PDGG(HEX_TARGET_CELL), gApp.Surface(), cell);
                //Invalidate();
            }
        }
    }
    else{
    
        iHero* pCaster = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
		check(pCaster);
		MAGIC_SCHOOL_LEVEL msl = m_pCastSpellToolBar->Spell()->GetSchoolLevel(pCaster);
        
		if (m_pCastSpellToolBar->Spell()->TargetMode(msl) == STM_CREAT_GROUP) {
			sint32 dist;
			iPoint targCell = FindNearestCell(m_spellTargets, pos, dist);
			if(targCell != cInvalidPoint && dist < sm_hexWidth){
#ifdef PC_VERSION
                m_Entered = targCell;
#endif
                m_SpellTrack = targCell;
            }
                
		} else {
#ifdef PC_VERSION
			if(m_SpellTrack == cInvalidPoint || Screen2Map(pos) != m_SpellTrack) 
                m_Entered = m_SpellTrack;
#endif
		}
        
        if (m_pCastSpellToolBar->Spell()->TargetMode(msl) == STM_CREAT_GROUP) {
//			sint32 res = m_spellTargets.Find(cell);
//			if (res != -1) {
				iBattleGroup* pTarget = m_pBattle->Engine().FindGroup(cell, m_pCastSpellToolBar->Spell()->AffectsToDead());
				if (!pTarget) return;
				m_pCastSpellToolBar->SetTarget(pTarget);
//			} 
		}
		Invalidate();
    }
}

void iBattleView_touch::OnMouseRightDown(const iPoint& pos, MouseId mID, MouseButtonId mbID){

    iBattleGroup* pGroup = m_pBattle->Engine().FindGroup(Screen2Map(pos));
    if(pGroup)
    {
        m_highlightedGroup = pGroup->Pos();
        m_pCreatInfoView->SetVisible(true);
        iPoint corner = iPoint(pos.x > m_Rect.w / 2 ? 0 : m_Rect.w, pos.y > m_Rect.h / 2 ? 0 : m_Rect.h);
        iRect temprc = iRect(pos, corner);
        iSize sz = m_pCreatInfoView->GetSize(m_Rect.w / 4, pGroup->Type(), pGroup->Count(), CDV_TITLE | CDV_BKND | CDV_LINEAR_PERKS | CDV_GLOW, pGroup);			
        iRect cvrc = AlignRect(sz, temprc, AlignCenter);
        if(cvrc.x < 0) cvrc.x = 0;
        if(cvrc.y < 0) cvrc.y = 0;
        if(cvrc.x2() > m_Rect.w) cvrc.x -= (cvrc.x2() - m_Rect.w);
        if(cvrc.y2() > m_Rect.h) cvrc.y -= (cvrc.y2() - m_Rect.h);
        m_pCreatInfoView->SetRect(cvrc);
        m_pCreatInfoView->SetGroup(pGroup);			
        Invalidate();
    }        
}

void iBattleView_touch::OnMouseRightUp(const iPoint& pos, MouseId mID, MouseButtonId mbID){

    m_pCreatInfoView->SetVisible(false);
    m_highlightedGroup = cInvalidPoint;
}
#else
bool iBattleView_touch::IsInterestingCell(iBattleGroup* pGroup, const iPoint& cell, sint32 radius) const
{
	if(SpellTracking()) {
		iCombatSpell* pSpell = m_pCastSpellToolBar->Spell();
		iHero* pCaster = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
		MAGIC_SCHOOL_LEVEL msl = pSpell->GetSchoolLevel(pCaster);
		
		iSimpleArray<iPoint> targets;
		m_pBattle->Engine().GetValidSpellTargets(pSpell, msl, targets);
		
		for (sint32 xx=-radius; xx<radius; xx++)
			for (sint32 yy=-radius; yy<radius; yy++)
			{
				iPoint t = cell + iPoint(xx, yy);
				for(uint32 i=0; i<targets.GetSize(); i++)
					if(targets[i] == t)
						return true;
			}		
		return false;
	} 
	
	iSimpleArray<iPoint> m_targets;
	iSimpleArray<bool> m_targetsType;
	bool target_is_shot = false;
	bool allow_melee = true;
    
	for (sint32 xx=-radius; xx<radius; xx++)
		for (sint32 yy=-radius; yy<radius; yy++)
		{
			iPoint t = cell + iPoint(xx, yy);
			if (pGroup->GetShootEntry(t))
				return true;
			if (pGroup->GetMeleeEntry(t))
				return true;
			if(pGroup->CanMove(t.x, t.y) && !pGroup->IsGroupCell(t))
				return true;
		}
	return false;
}

sint32 GetGroupPossibilitiesCount(const iBattleGroup* pGroup, const iPoint& cell)
{
	sint32 res = 0;
	if(pGroup->GetShootEntry(cell))
		res++;
	
	if(const iBattleGroup::iMeleeEntry* pMeleeTrack = pGroup->GetMeleeEntry(cell)) {
		for(uint32 x=0; x<MDIR_COUNT/*mlist.GetSize()*/; x++)
			if (pMeleeTrack->m_cfg & (1<<x)) 
				res++;
	}
	return res;
}
#endif


void iBattleView_touch::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{	
#ifdef OS_IPHONE
	if(m_bInfoMode && mID == 0) {
		m_pBtnInfo->MouseUp(m_pBtnInfo->GetPos(), 0, mbID);
		if(!(m_pBtnInfo->GetButtonState() && iButton::Pressed))
			SetInfoMode(false);
		return;
	}
#endif
	if(iView* pView = GetChildByPos(pos)) {
		if(pView != m_pAutoBattleToolBar && pView != m_pCastSpellToolBar)
			return;		
	}	
	
	// MULTI-FINGERED: return
	if(ActiveFingers() > 1 || PerformedFingers() > 1) return;

	// Not our turn or animation playing: return
	if (!m_bHumanTurn || m_bAni || m_pBattle->IsBlocked()) return;

#ifdef PC_VERSION
	// INFO MODE: hide creat info
	if(m_bInfoMode) {		
		m_pCreatInfoView->SetVisible(false);
		m_highlightedGroup = cInvalidPoint;
		return;
	}
#endif
    	
	iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattle->Engine().TurnSeq().CurUnit());
	if(!pCurCreatGroup) return; //this is a small fix for crash when pressing 'autocombat' button (Hedin)
	iBattleGroup *pGroup = pCurCreatGroup->GetCreatGroup();
	
#if defined(OS_IPHONE)
    if(!gUseIpadUI)
    {
        iPoint realPos = pos;
        // show zoomer
        if(IsInterestingCell(pGroup, Screen2Map(pos), 1))
        {
            const uint32 zmsz = 100;
            iRect rc(pos.x - zmsz / 2, pos.y - zmsz / 2, zmsz, zmsz);
            m_pZoomer = new iZoomer(&gApp.ViewMgr(), this, gApp.Surface(), rc);
            m_pZoomer->UpdateSurface();
            m_pZoomer->DoModal();
            if(m_pZoomer->GetRes() == cInvalidPoint) {
                delete m_pZoomer;
                m_pZoomer = NULL;
                return;
            }
            realPos = m_pZoomer->GetRes();
            //		m_trackCell = Screen2Map(realPos);
            //		if(SpellTracking()) m_SpellTrack = Screen2Map(realPos);
            
            delete m_pZoomer;
            m_pZoomer = NULL;
        } else
            return;
    }
#endif

	m_MoveEntry = cInvalidPoint;
	Invalidate();
	
	iPoint curCell = Screen2Map(pos);

	// SPELL CASTING: decide to cast or not
	if (SpellTracking()) {		
		if(m_SpellTrack != cInvalidPoint && Screen2Map(pos) == m_SpellTrack)  // fixed for different resolutions, Hedin, 15.01.2011
		{
			if( 
#ifdef OS_IPHONE
               1 || 
#endif
               m_bCurSelFlag ) {
				EndSpellTrack(m_SpellTrack);
				m_SpellTrack = cInvalidPoint;
			} else
				m_bCurSelFlag = true;
			//m_SpellTrack = m_trackCell;
		} else {
			EndSpellTrack(cInvalidPoint);
		}
		Invalidate();
		return;
	} 

	// BATTLE TARGET SELECTION

	// this block is for movement confirmation mode (iPhone)
#ifdef MOVE_CONFIRMATION
	if(m_MoveEntry != cInvalidPoint && Screen2Map(pos) == m_MoveEntry && 
		pGroup->CanMove(m_MoveEntry.x,m_MoveEntry.y) &&
		!pGroup->IsGroupCell(m_trackCell))
	{
		if(m_bCurSelFlag) { 
			m_pBattle->Engine().Move(m_MoveEntry, pCurCreatGroup->GetCreatGroup()->Orient());
			m_MoveEntry = cInvalidPoint;
			BeginAni();
			m_pShootTrack = NULL;
			m_pMeleeTrack = NULL;
		} else
			m_bCurSelFlag = true;
	} 
	else 
#endif
		// this is a block for Move Entry selection or (if no confirmation) direct action
	{
		iSimpleArray<iPoint> m_targets;
		iSimpleArray<bool> m_targetsType;
		bool target_is_shot = false;
		bool allow_melee = true;

		sint32 xx=0, yy=0;

		// if this is enabled, targets (melee and shoot) are looked in +-1 range
#ifdef TARGET_ATTRACTION
		for (sint32 xx=-1; xx<1; xx++)
			for (sint32 yy=-1; yy<1; yy++)
#endif
			{
				if (pGroup->GetShootEntry(curCell + iPoint(xx, yy)))
				{
					m_targets.Add(curCell + iPoint(xx, yy));            
					m_targetsType.Add(true);
					// if we can shot, we will shot
					allow_melee = false;
				}
				if (pGroup->GetMeleeEntry(curCell + iPoint(xx, yy)))
				{
					m_targets.Add(curCell + iPoint(xx, yy));            
					m_targetsType.Add(false);
				}
			}
			uint32 best = 1000;

			// find the nearest target
			iPoint bestCell = cInvalidPoint;
			for (uint32 x = 0; x < m_targets.GetSize(); x++)
				if (Map2Screen(m_targets[x]).GetDelta(pos) < best && (allow_melee || m_targetsType[x])
#ifdef MOVE_CONFIRMATION
					&& m_MoveEntry != m_trackCell
#endif
					) 
				{
					best = Map2Screen(m_targets[x]).GetDelta(pos);
					bestCell = m_targets[x];
					target_is_shot = m_targetsType[x];
				}

				// if we clicked the possible target
				if (bestCell == m_trackCell
					&& (!gUseIpadUI || m_initialTrackCell == m_trackCell)
					)
				{
					// Cotulla: new target select code (EasyAttack, advanced combat)               

					// easy attack: ai determines the direction of attack
					if (!gSettings.GetEntryValue(CET_ADVANCED_COMBAT))
					{
						const sint32 INV_CVAL = -65536;
						sint32 cval = INV_CVAL;
						iPoint mCell;
						uint16 mDir = 0xffff;

						if (target_is_shot)
						{
							m_pBattle->Engine().Shot(bestCell, pGroup->GetShootEntry(bestCell)->m_penalty);
						}
						else
						{
							const iBattleGroup::iMeleeEntry* me = pGroup->GetMeleeEntry(bestCell);
							if(!me) return;
							iBattleGroup* pTarget = m_pBattle->Engine().FindGroup(me->m_pos);
							if(!pTarget) return;
							sint32 val = CalcMeleeValue(pGroup, me, pTarget, m_pBattle->Engine());
							CalcBestMeleeDir(pGroup, pTarget, me, m_pBattle->Engine(), mDir, cval, true);
							mCell = me->m_pos;
							check (mDir != 0xffff);
							m_pBattle->Engine().Melee(mCell, mDir);
						}
						BeginAni();
					}

					// advanced combat: show direction select dialog
                    // SHL: plus, calc and show possible damage infobar
					else if(bestCell != cInvalidPoint)
					{
#ifdef OS_IPHONE
                        const uint32 zmsz = AWI_SIZE * 3 / 2;
						iPoint scrpos = Map2Screen(bestCell);
						iRect rc(scrpos.x + sm_hexHalfWidth - zmsz / 2, scrpos.y + sm_hexHeight / 2 - zmsz / 2, zmsz, zmsz); 
						m_pActionWheel = new iActionWheel(&gApp.ViewMgr(), pGroup, bestCell, gApp.Surface(), this, rc);
						m_highlightedTarget = bestCell;
						sint32 res = m_pActionWheel->DoModal();
						delete m_pActionWheel;
						m_pActionWheel = NULL;
#else
                        iActionWheel wheel(&gApp.ViewMgr(), pGroup, bestCell);
						m_highlightedTarget = bestCell;
                        sint32 res = wheel.DoModal();
#endif
						m_highlightedTarget = cInvalidPoint;
						if(res == 100) {
							m_pBattle->Engine().Shot(bestCell, pGroup->GetShootEntry(bestCell)->m_penalty);
							BeginAni();
						} else if(res != 1000 && res != -1) {
							m_pBattle->Engine().Melee(bestCell, (uint16)res);
							BeginAni();
						}
					}
				}

				// we clicked free cell and we can move there
				else if(!m_pMeleeTrack && !m_pShootTrack && pGroup->CanMove(m_trackCell.x, m_trackCell.y) &&
					!pGroup->IsGroupCell(m_trackCell)) {
#ifdef MOVE_CONFIRMATION
						if(!m_bDontMove)
							m_MoveEntry = m_trackCell;
						else
							m_bDontMove = false;
						m_moveDisapTimer = fix32::zero;
						m_bCurSelFlag = true;
#else
						m_pBattle->Engine().Move(m_trackCell, pCurCreatGroup->GetCreatGroup()->Orient());						
						BeginAni();
						m_pShootTrack = NULL;
						m_pMeleeTrack = NULL;
#endif
				} 				
	}

	Invalidate();
}

void iBattleView_touch::OnDualSwipeDown(sint32 length)
{
	/*if(m_bGesturesEnabled && length > 40 && !m_bGestureComplete) {
		m_pBattle->Engine().Defend();
		OnGroupChanged();
		m_bGestureComplete = true;
	}*/
}

void iBattleView_touch::OnDualSwipeLeft(sint32 length)
{
	/*if(m_bGesturesEnabled && length > 40 && !m_bGestureComplete) {
		m_pBattle->Engine().Wait();
		OnGroupChanged();
		m_bGestureComplete = true;
	}
	*/
}

void iBattleView_touch::OnDualSwipeUp(sint32 length)
{
	/*if(m_bGesturesEnabled && length > 40 && !m_bGestureComplete) {
		m_bGestureComplete = true;
		iHero* pHero = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
#ifndef IPHONE_DEBUG
		if(!m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->CanCastSpell())
			return;
#endif
		check(pHero);
		// Check for Spere of Negation
		if (iHero* pArtOwner = m_pBattle->Engine().GetBattleInfo().HasArtInBattle(ARTT_NEGSPHERE)) {
			iTextDlg dlg(m_pMgr, _T(""), iFormat(gTextMgr[TRID_MSG_BAT_NEGSPHERE], pArtOwner->Name().CStr()), pHero->Owner()->PlayerId());
			dlg.DoModal();
			return;
		}

		// Show spell list dialog
		// fix for double cast bug: remove the previous spelltrack if any
		EndSpellTrack(cInvalidPoint);
		iDlg_Spellbook csDlg(m_pMgr, pHero, SPT_MASK_COMBAT);
		if (csDlg.DoModal() == DRC_YES) {
			iBaseSpell* pSpell = gGame.ItemMgr().m_spellMgr.Spell(csDlg.SelSpell());
			check(pSpell && pSpell->Type() == SPT_COMBAT);
			iCombatSpell* pCombatSpell = DynamicCast<iCombatSpell*>(pSpell);
			check(pCombatSpell);
			MAGIC_SCHOOL_LEVEL msl = pSpell->GetSchoolLevel(pHero);
			if (pCombatSpell->NeedTarget(msl)) {
				if (!BeginSpellTrack(pCombatSpell)) {
					iTextDlg tdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_SPELL_AFFECT_NO_ONE], pHero->Owner()->PlayerId());
					tdlg.DoModal();
				}
			} else {
				if (!m_pBattle->Engine().CastSpell(pCombatSpell, cInvalidPoint, NULL)) {
					iTextDlg tdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_SPELL_AFFECT_NO_ONE], pHero->Owner()->PlayerId());
					tdlg.DoModal();
				} else BeginAni();
			}
		}
	}*/
}

void iBattleView_touch::OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	// not our turn, animation playing or info mode: return
	if (!m_bHumanTurn || m_bAni || m_bInfoMode || m_pBattle->IsBlocked()) return;

	// gesture performed with more than 1 finger: return
	if(ActiveFingers() > 1 || PerformedFingers() > 1) return;

	m_trackPos = pos;
	m_trackCell = Screen2Map(pos);	
	m_bCurSelFlag = false;

	m_MoveEntry = m_trackCell;
	Invalidate();

	// SPELL CASTING: move target selection
	if (SpellTracking()) {
		m_trackCell = Screen2Map(pos);
		if(m_SpellTrack != m_trackCell)
			m_bCurSelFlag = false;

		iHero* pCaster = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
		check(pCaster);
		MAGIC_SCHOOL_LEVEL msl = m_pCastSpellToolBar->Spell()->GetSchoolLevel(pCaster);

		if (m_pCastSpellToolBar->Spell()->TargetMode(msl) == STM_CREAT_GROUP) {
			sint32 dist;
			iPoint targCell = FindNearestCell(m_spellTargets, pos, dist);
			if(targCell != cInvalidPoint && dist < sm_hexWidth) {
				if(m_SpellTrack != targCell)
					m_bCurSelFlag = false;
				m_SpellTrack = targCell;
			}
			else
				m_SpellTrack = cInvalidPoint;
		} else {
			m_SpellTrack = m_trackCell;			
		}
		Invalidate();
		return;
	} 
#ifdef OS_IPHONE
    iBattleUnit* pCurUnit = m_pBattle->Engine().TurnSeq().CurUnit();
	iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(pCurUnit);
	iBattleGroup *pGroup = pCurCreatGroup->GetCreatGroup();
	if(m_pMeleeTrack || m_pShootTrack || !pGroup->CanMove(m_trackCell.x, m_trackCell.y))
		m_MoveEntry = cInvalidPoint;
#endif
#ifdef MOVE_CONFIRMATION
	iPoint foCell = Screen2Map(m_trackPos + fingerOffset);
	if(!m_pMeleeTrack && !m_pShootTrack && pGroup->CanMove(foCell.x, foCell.y) &&
		!pGroup->IsGroupCell(m_trackCell)) {
			m_MoveEntry = foCell;//m_trackCell;
			if(oldMelee || oldShoot)
				m_bCurSelFlag = false;
			Invalidate();
	}
#endif
}

bool iBattleView_touch::Process(fix32 t)
{
	// move entry disappearance
	if(!IsMouseDown(0) && m_MoveEntry != cInvalidPoint)
		m_moveDisapTimer += t;
	if(!IsMouseDown(0) && m_moveDisapTimer >= fix32(1.5f)) {
		m_moveDisapTimer = fix32::zero;
		m_MoveEntry = cInvalidPoint;
		Invalidate();
	}

	/*	for(uint32 x=0; x<m_deleteList.GetSize(); x++)
	delete m_deleteList[x];
	m_deleteList.RemoveAll();
	*/	

	return iBattleView::Process(t);
}

void iBattleView_touch::ComposeSpellCursor()
{
	if(m_SpellTrack != cInvalidPoint) {
		iPoint cop = Map2Screen(m_SpellTrack);
        
        iRect rc = iRect();
        iGfxManager::FlipType flip;

        if( cop.x < gApp.Surface().GetWidth() / 2 ){
        
            cop += iPoint(25, -20);      
            flip = iGfxManager::FlipNone;            
            rc = iRect(cop + iPoint(75, 0), iSize(50, 50));
        }
        else{
            
            cop += iPoint(-30, -20);        
            rc = iRect(cop - iPoint(50, 0), iSize(50, 50));            
            flip = iGfxManager::FlipVert;
        }
        
        gGfxMgr.BlitUniversal( PDGG(SPELL_CURSOR), gApp.Surface(), cop, cInvalidRect, iGfxManager::EfxShadow2D, false, -1, 255, flip);
        gGfxMgr.BlitUniversal( PDGG(SPELL_CURSOR), gApp.Surface(), cop, cInvalidRect, iGfxManager::EfxNone, false, -1, 255, flip);
        
        BlitIcon(gApp.Surface(), PDGG(SPELLSCROLL_UNROLLED), rc);
        BlitIcon(gApp.Surface(), m_pCastSpellToolBar->Spell()->Icon(), rc);
        
	}
}

bool iBattleView_touch::IsAcceptingCmds()
{
	return m_MoveEntry == cInvalidPoint;
}


void iBattleView_touch::SetInfoMode(bool bInfoMode) {
	m_bInfoMode = bInfoMode;
	
	EnableControls(Nothing);
	
	if(!m_bInfoMode) {
		m_pCreatInfoView->SetVisible(false);
		m_highlightedGroup = cInvalidPoint;
#ifdef OS_IPHONE
        m_bAlwaysReceiveMouse = false;
#endif
	}
}

void iBattleView_touch::RemoveCreaturePopup()
{
	RemoveChild(m_pCreatInfoPopup);
	m_pCreatInfoPopup->HidePopup();
	delete m_pCreatInfoPopup;		
	gApp.ViewMgr().ReleaseViewCapture();
	m_pCreatInfoPopup = NULL;		
	m_bGesturesEnabled = true;
	Invalidate();
}


void iBattleView_touch::HighlightMemberButton( uint32 type, bool bHighlight )
{
	if(type == BCI_ASSAULTER_BTN)
		m_pBtnAssaulter->SetHighlighted(bHighlight);
	else if(type == BCI_DEFENDER_BTN) {
		m_pBtnDefenderHero->SetHighlighted(bHighlight);
		m_pBtnDefenderCastle->SetHighlighted(bHighlight);
	}

}
#ifdef OS_IPHONE
void iBattleView_touch::AfterCompose()
{
	if(m_pZoomer) {
		m_pZoomer->UpdateSurface();
		iRect rc(iPoint(), gApp.Surface().GetSize());
		gApp.Surface().Darken50Rect(rc);
		gApp.Surface().Darken50Rect(rc);
	}
	if(m_pActionWheel) {
		m_pActionWheel->UpdateSurface();
		iRect rc(iPoint(), gApp.Surface().GetSize());
		gApp.Surface().Darken50Rect(rc);
		gApp.Surface().Darken50Rect(rc);
	}
}
#endif
#ifdef OS_IPHONE
iActionWheel::iActionWheel(iViewMgr* pViewMgr, iBattleGroup *pGroup, iPoint cell, iDib& srf, iView *pUnderView, iRect src_rect)
#else
iActionWheel::iActionWheel(iViewMgr* pViewMgr, iBattleGroup *pGroup, iPoint cell)
#endif
: iDialog(pViewMgr)
, m_pGroup(pGroup)
, m_cell(cell)
#ifdef OS_IPHONE
, m_srf(srf)
, m_pUnderView(pUnderView)
, m_src_rect(src_rect)
#endif
{
#ifdef OS_IPHONE
    m_buf.Resize(m_src_rect.size() * 2);
#endif
	if(const iBattleGroup::iShootEntry *pShootTrack = pGroup->GetShootEntry(cell)) {
		sint16 sid = PDGG(BATTLE_CURSORS) + 8, pressed = PDGG(BATTLE_CURSORS_PRESSED) + 8;

		if (pShootTrack->m_penalty > 1) {
			sid += pShootTrack->m_penalty/2;
			pressed += pShootTrack->m_penalty/2;
		}
		AddChild(new iImgBtn(pViewMgr, this, iRect(iPoint(AWI_SIZE, AWI_SIZE) + iPoint(10, 10), iSize(AWI_SIZE, AWI_SIZE)), 100, sid, pressed, iStringT(), Visible | Enabled));
	}
	if(const iBattleGroup::iMeleeEntry* pMeleeTrack = pGroup->GetMeleeEntry(cell)) {
		const uint8 meleeCursorConv[12] = {
			0, 7, 1, 7, 2, 6, 3, 6, 4, 6, 5
		};

		const iPoint meleeOffsets[8] = {
			iPoint(-(AWI_SIZE), -(AWI_SIZE)),
			iPoint((AWI_SIZE), -(AWI_SIZE)),
			iPoint((AWI_SIZE), 0),
			iPoint((AWI_SIZE), (AWI_SIZE)),
			iPoint(-(AWI_SIZE), (AWI_SIZE)),
			iPoint(-(AWI_SIZE), 0),
			iPoint(0, (AWI_SIZE)),
			iPoint(0, -(AWI_SIZE))
		};

		for(uint32 x=0; x<MDIR_COUNT/*mlist.GetSize()*/; x++)
			if (pMeleeTrack->m_cfg & (1<<x)) 
				AddChild(new iImgBtn(pViewMgr, this, iRect(iPoint(AWI_SIZE, AWI_SIZE) + iPoint(10, 10) + meleeOffsets[meleeCursorConv[x]], iSize(AWI_SIZE, AWI_SIZE)),
				x, 
				PDGG(BATTLE_CURSORS) + meleeCursorConv[x], 
				PDGG(BATTLE_CURSORS_PRESSED) + meleeCursorConv[x],
				iStringT(),
				Visible | Enabled));

	}
}

#ifdef OS_IPHONE
void iActionWheel::UpdateSurface()
{
	iDib tmp(m_src_rect.size(), iDib::RGB);
	m_srf.CopyRectToDibXY(tmp, m_src_rect, iPoint());
	DoubleDib(tmp, m_buf);	
}
#endif

sint32 iActionWheel::DoModal()
{
	iPoint p = iBattleView::Map2Screen(m_cell);
#ifdef OS_IPHONE
    iRect r = AlignRect(m_src_rect.size() * 2, iRect(iPoint(), gApp.Surface().GetSize()), AlignCenter);
#else
	iRect r = iRect(p + iPoint(iBattleView::sm_hexHalfWidth, iBattleView::sm_hexHeight / 2) - iPoint(AWI_SIZE * 3 / 2, AWI_SIZE * 3 / 2), iSize(AWI_SIZE * 3, AWI_SIZE * 3));
#endif
	SetRect(r);
	OnCreateDlg();
	SetVisible();
	m_pMgr->PushModalDlg(this);
	while (m_pMgr->App()->Cycle() && m_retCode == DRC_UNDEFINED) {}
	iDialog* pDlg = m_pMgr->PopModalDlg();
	check(pDlg == this);

	return m_retCode;
}

void iActionWheel::OnMouseUp(const iPoint &p, MouseId mID, MouseButtonId mbID)
{
	for(uint32 x=0; x<m_Childs.GetSize(); x++)
		if(m_Childs[x]->GetScrRect().PtInRect(p))
			return;
	EndDialog(1000);
}

void iActionWheel::OnCreateDlg()
{

}
iSize iActionWheel::GetDlgMetrics() const
{
	return iSize(AWI_SIZE * 3, AWI_SIZE * 3);
}
void iActionWheel::OnCompose()
{
	//iRect rc = GetScrRect();
	//gApp.Surface().FrameRect(rc, 0);
#ifdef OS_IPHONE
    iRect rc = GetScrRect();
	iRect bkrc = rc;
	bkrc.InflateRect(10);
	ComposeDlgBkgnd(gApp.Surface(), bkrc);	
	m_buf.CopyToDibXY(m_srf, rc.point());
#endif
}

void iActionWheel::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	EndDialog(uid);
}

#ifdef OS_IPHONE
// ============================

iZoomer::iZoomer(iViewMgr* pViewMgr, iView* underView, iDib& srf, iRect src_rect): 
iDialog(pViewMgr), m_srf(srf), m_src_rect(src_rect), m_res(cInvalidPoint),
m_pUnderView(underView)
{
	m_buf.Resize(m_src_rect.size() * 2);
}

void iZoomer::UpdateSurface()
{
	iDib tmp(m_src_rect.size(), iDib::RGB);
	m_srf.CopyRectToDibXY(tmp, m_src_rect, iPoint());
	DoubleDib(tmp, m_buf);
}

sint32 iZoomer::DoModal()
{
	iPoint corner;
	iPoint c = m_src_rect.Center();
	iSize sz = m_srf.GetSize();
	corner.x = c.x > sz.w / 2 ? 0 : sz.w;
	corner.y = c.y > sz.h / 2 ? 0 : sz.h;
	iPoint vect = (corner - c);
	double norm = (sqrt(m_src_rect.h * m_src_rect.h + m_src_rect.w * m_src_rect.w) + 40) / sqrt(vect.x * vect.x + vect.y * vect.y);
	vect.x *= norm;
	vect.y *= norm;
	c += vect;
	iRect r = iRect(c.x - m_src_rect.w, c.y - m_src_rect.h, m_src_rect.w * 2, m_src_rect.h * 2);
	SetRect(r);
	OnCreateDlg();
	SetVisible();
	m_pMgr->PushModalDlg(this);
	while (m_pMgr->App()->Cycle() && m_retCode == DRC_UNDEFINED) {}
	iDialog* pDlg = m_pMgr->PopModalDlg();
	check(pDlg == this);
	
	return m_retCode;
}

iPoint iZoomer::ZoomedToReal(const iPoint& pos) const
{
	iRect rc = GetScrRect();
	iPoint c = rc.Center();
	iPoint dif = (pos - c);
	dif.x /= 2; dif.y /= 2;
	return m_src_rect.Center() + dif;
}

void iZoomer::OnMouseDown(const iPoint &p, MouseId mID, MouseButtonId mbID)
{
	iRect rc = GetScrRect();
	if(rc.PtInRect(p))
		m_pUnderView->OnMouseDown(ZoomedToReal(p), mID, mbID);
}

void iZoomer::OnMouseTrack(const iPoint &p, MouseId mID, MouseButtonId mbID)
{
	iRect rc = GetScrRect();
	if(rc.PtInRect(p))
		m_pUnderView->OnMouseTrack(ZoomedToReal(p), mID, mbID);	
}

void iZoomer::OnMouseUp(const iPoint &p, MouseId mID, MouseButtonId mbID)
{
	iRect rc = GetScrRect();
	if(!rc.PtInRect(p)) {
		m_res = cInvalidPoint;
		EndDialog(0);
	} else {
		m_res = ZoomedToReal(p);
		EndDialog(0);
	}
}

void iZoomer::OnCreateDlg()
{
	
}
iSize iZoomer::GetDlgMetrics() const
{
	return iSize(m_src_rect.w * 2, m_src_rect.h * 2);
}

void iZoomer::OnCompose()
{
	iRect rc = GetScrRect();
	iRect bkrc = rc;
	bkrc.InflateRect(10);
	ComposeDlgBkgnd(m_srf, bkrc);	
	m_buf.CopyToDibXY(m_srf, rc.point());
}

void iZoomer::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	EndDialog(uid);
}
#endif
