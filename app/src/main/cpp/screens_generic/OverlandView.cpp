#include "stdafx.h"
#include "Composer.h"
#include "OverlandView.h"
#include "GameMenuDlg.h"
#include "Dlg_Save.h"
#include "gxl.popupview.h"
#include "ai.scanner.h"
#include "Dlg_Spellbook.h"
#include "dbg.h"
#include "helpers.h"

//#include "xxc/xxc.security.h"

const iRect OVERLAND_POPUP_BOUND(14,28,292,176);

/*
 *	Main Game View
 */
iMainView::iMainView()
:iChildGameView(true, iChildGameView::CV_MENU) ,m_KeyTrackMask(0), m_MouseTrackMask(0)
, m_bForceTrack(false), m_bForceHelp(false), m_pPopupView(NULL), m_OLComposer(iRect(0,15,m_Rect.w,m_Rect.h-36), this), m_bSurveyMode(false), m_holdTimer(-1)
, m_pHighlightedObject(NULL)
, m_pLastHighlightedObject(NULL)
, m_objHighlightTimer(fix32::zero)
, m_highlightedCell(cInvalidPoint)
, m_lastHighlightedCell(cInvalidPoint)
#ifdef MULTIPLAYER
, m_bInactive(false)
, m_bShowMpActIcon(false)
#endif
{	
	m_dibSurveyBuff.Init(iSize(480, 320), iDib::RGB);		
	m_OLComposer.Init();	
}

iMainView::~iMainView()
{
	m_OLComposer.Cleanup();
}

iBaseMapObject*	iMainView::ObjectHitTest(const iPoint& scr_pos)
{
	return NULL;
}

void iMainView::OnSizeChanged(const iSize& nsiz)
{
	UpdateMetrics();
}

//
//
void iMainView::ShowHelpPopup(const iPoint& pos)
{
	iIsoMetric im(im_coef);
	iPoint npos = im.Screen2Map(m_OLComposer.ActAnchor(pos));
	if (gGame.Map().IsValidPos(npos.x,npos.y)){
		uint8 dcfg, fcfg;
		check(!m_pPopupView);
		if (!gGame.Map().ActPlayer()->FogMap().GetFogCfg(npos,dcfg,fcfg) && dcfg==15) {
			AddChild(m_pPopupView = new iTextPopupDlg(m_pMgr,gTextMgr[TRID_UNCHARTED_TERRITORY],PID_NEUTRAL));
		} else {
			iStringT name;
			if (iBaseMapObject* pObj = gGame.Map().GetCellObj(npos, name)) {
				VISION_LEVEL vl = VL_BASIC;
				iHero* pHero = gGame.Map().CurHero();
				sint32 radius=0;
				if (!gGame.Map().ActPlayer()->FogMap().IsHidden(pObj->Pos()) && pHero) vl = pHero->VisionLevel(radius);
				if (vl == VL_EXPERT && pHero->Pos().GetDelta(pObj->Pos()) > radius) vl = VL_BASIC;
				if (iVisCnst* pVisCnst = DynamicCast<iVisCnst*>(pObj)) {
					AddChild(m_pPopupView = new iPopupDlg_VisCnst(m_pMgr,pVisCnst,vl,pHero));
				} else if (iOwnCnst* pOwnCnst = DynamicCast<iOwnCnst*>(pObj)) {
					if (pOwnCnst->Owner() == gGame.Map().ActPlayer()->PlayerId()) vl = VL_EXPERT;
					AddChild(m_pPopupView = new iPopupDlg_OwnCnst(m_pMgr,pOwnCnst, vl));
				} else if (iCastle* pCastle = DynamicCast<iCastle*>(pObj)) {
					if (pCastle->Owner() == gGame.Map().ActPlayer()->PlayerId()) vl = VL_EXPERT;
					AddChild(m_pPopupView = new iPopupDlg_Castle(m_pMgr,pCastle,vl));
				} else if (DynamicCast<iMapItem_Mineral*>(pObj) || DynamicCast<iMapItem_ManaCrystal*>(pObj) || DynamicCast<iMapItem_CampFire*>(pObj) || DynamicCast<iMapItem_Artifact*>(pObj) || DynamicCast<iMapItem_Chest*>(pObj) || DynamicCast<iMapItem_SpellScroll*>(pObj) || DynamicCast<iMapItem_KeyGuard*>(pObj) || DynamicCast<iMapItem_Lamp*>(pObj)) {
					iMapItem* pMapItem = (iMapItem*)pObj;
					AddChild(m_pPopupView = new iTextPopupDlg(m_pMgr,gTextMgr[pMapItem->NameKey()],PID_NEUTRAL));
				} else if (iMapGuard* pGuard = DynamicCast<iMapGuard*>(pObj)) {
					AddChild(m_pPopupView = new iPopupDlg_Guard(m_pMgr, pGuard, vl, pHero));
				} else if (iHero* pHero = DynamicCast<iHero*>(pObj)) {
					if (pHero->Owner()->PlayerId() == gGame.Map().ActPlayer()->PlayerId()) vl = VL_EXPERT;
					AddChild(m_pPopupView = new iPopupDlg_Hero(m_pMgr,pHero, vl));
					//OutputDebugString(iFormat(_T("Mana: %d/%d\n"), pHero->ManaPts(), pHero->MaxManaPts()).CStr());
                }
                else if (iShip* pShip = DynamicCast<iShip*>(pObj)) 
                {
                    // CtllTODO: add proper string id
                    AddChild(m_pPopupView = new iTextPopupDlg(m_pMgr, gTextMgr[TRID_OBJ_SHIP],PID_NEUTRAL));
				} 
                else 
                {
					check(0);
				}
			} else {
				AddChild(m_pPopupView = new iTextPopupDlg(m_pMgr,name,PID_NEUTRAL));
			}
		}
		check( m_pPopupView != NULL );
		m_pPopupView->TrackPopup(pos,OVERLAND_POPUP_BOUND,AlignBottomRight);
	}
}

void iMainView::AddMsg(const iStringT& text)
{
#if 0
    // disabled. look ugly. Hedin
	m_EventList.Add(new iGameEvent(fix32(4.0f),text));
#endif
	Invalidate();
}

void iMainView::AddCellMsg(const iStringT& msg, const iPoint& pos)
{
	m_OLComposer.AddCellMsg(msg,pos);
}

const sint32 SCROLL_SPEED[5] = { 100, 175, 250, 400, 600 };

bool iMainView::Process(fix32 t)
{
	if (gApp.ViewMgr().HasModalDlg()) return false;
	
#ifdef MULTIPLAYER
	if(gMPMgr.IsMultiplayer() && GetTickCount() - gMPMgr.LastActivity() <= 5000) {
		if(!m_bShowMpActIcon) Invalidate();
		m_bShowMpActIcon = true;
	} else {
		if(m_bShowMpActIcon) Invalidate();
		m_bShowMpActIcon = false;
	}
	
	
#endif
	
	// Keyboard and mouse state
	uint8 mtm = GetMouseTrackMask();
	uint8 ktm = GetKeyTrackMask();
	if (ktm || mtm) {
		sint32 val = (t * SCROLL_SPEED[gSettings.GetEntryValue(CET_MAPSCROLLSPEED)]).ceil();
		iPoint npos = m_OLComposer.GetAnchor();
		if (ktm & 0x01 || mtm & 0x01) 
			npos.y -= val;
		if (ktm & 0x02 || mtm & 0x02) 
			npos.y += val;
		if (ktm & 0x04 || mtm & 0x04) 
			npos.x -= val;
		if (ktm & 0x08 || mtm & 0x08) 
			npos.x += val;
		m_OLComposer.SetAnchor(npos);
		Invalidate();
	}

    bool bMsgRedraw = false;
    
	// Process events
	fix32 hD(0.5f);
	fix32 fani(18.0f);
	iEvtIt it=m_EventList.First();

	while(it != m_EventList.End()) {
		(*it)->m_time -= t;
		if ( (*it)->m_time < fix32::zero){
			it = m_EventList.Delete(it);
			bMsgRedraw = true;
			continue;
		} else if ((*it)->m_time < hD) {
			(*it)->dstate = (uint8)((hD-(*it)->m_time) * fani).ceil();
			bMsgRedraw = true;
		}
		++it;
	}
	
	// Animation
	if (m_OLComposer.StepAni(t)) {
		Invalidate();
		return false;
	}
	
	// check if highlighted object not highlighted anymore
	if((m_pHighlightedObject || m_pLastHighlightedObject) && !IsMouseDown(0)) {
		m_objHighlightTimer += t;
		if(m_objHighlightTimer >= fix32(1.0f)) {
			m_objHighlightTimer = fix32::zero;
			m_pHighlightedObject = NULL;
			m_pLastHighlightedObject = NULL;
		}
	}

	if (bMsgRedraw) Invalidate();
	
	
	
	return true;
}


// Game notifications
void iMainView::OnPlayerChanged(iPlayer* pPlayer, bool bAct)
{
#ifndef MULTIPLAYER
	SetEnabled(bAct);
#else
	m_bInactive = !bAct;
#endif
	m_pOpTurnBar->SetVisible(!bAct);
}


void iMainView::UpdateSize()
{
    iRect Rect = m_pMgr->Metrics();  
    
    m_pOpTurnBar->SetRect(iRect(0, Rect.h - 21, Rect.w, 21));
    m_pOpTurnBar->Invalidate();
    m_pOpTurnBar->UpdateSize();

    SetRect(Rect);
    m_OLComposer.SetRect(iRect(0, 15, Rect.w, Rect.h - 36));
}

iRect iMainView::GetVisibleMapRect() const
{
	iIsoMetric im(im_coef);
	iPoint fpos = im.Screen2Map(m_OLComposer.GetAnchor());
	return iRect(fpos, iSize(m_Rect.w / cell_w + 3, m_Rect.h / cell_h + 5));
}


iStringT FormatTraffic(uint32 count) 
{
	if(count > 1024 * 1024 * 1024) 
		return iFormat(_T("%dGB"), count / (1024 * 1024 * 1024));
	else if(count > 1024 * 1024)
		return iFormat(_T("%dMB"), count / (1024 * 1024));
	else if(count > 1024)
		return iFormat(_T("%dKB"), count / (1024));
	else
		return iFormat(_T("%dB"), count);
}


/*
 *	Opponent turn information bar
 */
iOpTurnBar::iOpTurnBar(iViewMgr* pViewMgr, const iRect& rect) : iView(pViewMgr,rect,GENERIC_VIEWPORT,0, Visible|Enabled)
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsMultiplayer())
		AddChild(m_pKickBtn = new iIconButton(&gApp.ViewMgr(), this, iRect(rect.w - 30, 1, 30, 20), PDGG(BTN_DISMISS), 100, Visible | Enabled));
#endif
}

#ifdef MULTIPLAYER
void iOpTurnBar::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if(uid == 100) { // kick
		PLAYER_ID pid = gGame.Map().RealCurPlayer();
		iQuestDlg qdlg(m_pMgr, _T(""), iFormat(gTextMgr[TRID_KICK_CNFRM], gMPMgr.GetColorNickname(pid).CStr())
					   , PID_NEUTRAL);
		if (qdlg.DoModal() == DRC_YES) {
			gMPMgr.DoKick(pid);
		}				
	};	
}
#endif

void iOpTurnBar::OnCompose()
{
    if(!gGame.Map().CurPlayer())
        return;
#ifdef MULTIPLAYER
	if(gMPMgr.IsMultiplayer()) {
		PLAYER_ID kickee = gGame.Map().RealCurPlayer();
		m_pKickBtn->SetVisible(gMPMgr.GetHostType() == iMPManager::HT_HOST &&
							   kickee != gGame.Map().ActPlayer()->PlayerId() && 
							   kickee != PID_NEUTRAL && 
							   gGame.Map().FindPlayer(kickee)->PlayerType() == PT_REMOTE && 
							   !gGame.MainView()->ShowMPActivity());
	}
#endif
	uint8 pid = gGame.Map().CurPlayer()->PlayerId();
	gGfxMgr.BlitTile(PDGG(PLTILES) + pid, gApp.Surface() ,m_Rect);
	ButtonFrame(gApp.Surface(), m_Rect, 0);
#ifdef MULTIPLAYER
	if(!gMPMgr.IsMultiplayer())
#endif
		iStringT txt = iStringT(_T("")) + PLAYER_TEXT_COLORS[pid]+gTextMgr[TRID_PLAYER_RED+pid] + _T(" ") + gTextMgr[TRID_PLAYER];
		iSize sz = gTextComposer.GetTextSize(txt, dlgfc_hdr);
		gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), txt, AlignRect(sz, m_Rect, AlignCenter));	
#ifdef MULTIPLAYER
	else {		
		iStringT nick = gMPMgr.GetColorNickname(gGame.Map().CurPlayer()->PlayerId());
		if(nick == _T("")) nick = _T("AI");		
		if(gGame.Map().CurPlayer()->PlayerType() == PT_REMOTE && !gGame.MainView()->ShowMPActivity())
			gGfxMgr.Blit( PDGG(ZZZ), gApp.Surface(), m_Rect.point() + iPoint(25, 1));
		else {
			if(gMPMgr.IsCombat())
				gGfxMgr.Blit( PDGG(PATH) + 2, gApp.Surface(), m_Rect.point() + iPoint(20, 8));
			else
				gGfxMgr.Blit( PDGG(PATH) + 3, gApp.Surface(), m_Rect.point() + iPoint(20, 8));
		}
		gTextComposer.TextOut(gApp.Surface(),m_Rect.point(), iStringT(_T("#S2"))+PLAYER_TEXT_COLORS[pid]+gTextMgr[TRID_PLAYER_RED+pid] + _T(" ") + gTextMgr[TRID_PLAYER] + _T(" (") + nick + _T(")"),m_Rect,AlignCenter);	
		
	}
#endif
	
}

