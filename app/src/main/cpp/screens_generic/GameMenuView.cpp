//
// GameMenuView.cpp
// 

#include "stdafx.h"

//#include "xxc/xxc.security.h"
#include "MainMenuView.h"
#include "GameMenuView.h"
#include "Dlg_KingdomOverview.touch.h"
#include "Dlg_PuzzleMap.touch.h"
#include "Dlg_Settings.h"
#include "Dlg_Spellbook.h"
#include "Dlg_Save.h"
#include "Dlg_ScenProps.h"
#include "Dlg_ScenList.h"
#include "Dlg_GameLanguage.h"
#include "helpers.h"
#include "Ctl_Settings.h"


#include "Credits.h"
#include "MenuView.h"

#include "TransitionView.h"

#include "GameMenuDlg.h"
#include "OverlandView.h"

#include "screens_ipad/MainMenuView.h"
#include "screens_iphone/MainMenuView.h"

void iGameMenuView::ShowView(CHILD_MENU cm)
{
	for (uint32 x = 0; x < CM_COUNT; x++)
		m_pChildren[x]->SetVisible(false);

	m_pChildren[cm]->SetVisible(true);
}

iGameMenuView::iGameMenuView():iChildGameView(false, CV_OVERLAND), m_bGestureComplete(false), m_pos(0), m_timer(fix32::zero), m_frame(0)
{
    iRect src = GetScrRect();

    // at start of game, main menu located on centered dialog with size (1000,700)
    // so we emulate this behavior here
    //iRect clRect((src.w - 1000) / 2, (src.h - 700) / 2, 1000, 700);
    // Cotulla: fixed values specified, they match new main menu dlg layout at start up
    //iRect	clRect(32, 32, 954, 698);
    //iRect	clRect(32+3, 32+3, 954, 698);
    //clRect.InflateRect(3,3, 0,0);

    //clRect.h -= 20;    
    
	iRect clRect(GetScrRect());

    //AddChild(m_pChildren[0] = new iNewMenuView(m_pMgr, this, clRect, iNewMenuView::TypeMain, true, 109) );
    if(gUseIpadUI)
    {
        AddChild(m_pChildren[0] =
                 new UI_IPAD::iMainMenuView(
#ifdef PC_VERSION
                                            m_pMgr, this, iRect(iPoint(), iSize(1024, 768)),
#else
                                            m_pMgr, this, clRect,
#endif
                                            
#ifdef	 ROYAL_BOUNTY
                                            iMainMenuView::TypeSingleMapInGame,
#else
                                            IMainMenuView::TypeInGame,
#endif
                                            true, 110) );
    } else
    {
        clRect.w *= 2;
        AddChild(m_pChildren[0] =
                 new UI_IPHONE::iMainMenuView(
#ifdef PC_VERSION
                                            m_pMgr, this, iRect(iPoint(), iSize(1024, 768)),
#else
                                            m_pMgr, this, clRect,
#endif
                                            
#ifdef	 ROYAL_BOUNTY
                                            iMainMenuView::TypeSingleMapInGame,
#else
                                            IMainMenuView::TypeInGame,
#endif
                                            true, 110) );
        
    }
}

iGameMenuView::~iGameMenuView()
{

}
#if defined( PC_VERSION )
void iGameMenuView::SetLoadFlagTrue(){

	m_pChildren[CM_SINGLE]->m_bLoad = true;
}

void iGameMenuView::SetSaveFlagTrue(){

	m_pChildren[CM_SINGLE]->m_bSave = true;
}
#endif

void iGameMenuView::OnActivate(bool bActivate)
{
	ShowView(CM_SINGLE);

    m_pChildren[0]->OnActivate(bActivate);
    iRect rc = m_pChildren[0]->GetRect();
    if(!gUseIpadUI)
        m_pChildren[0]->SetPos(iPoint((rc.w / 2 - 480) / 2, rc.y));
	
	if(bActivate){
        
		FallDown();
//#ifndef PC_VERSION
//        iFingerHelp fh( &gApp.ViewMgr(), FH_UP );
//        fh.DoModal();
//s#endif
	}
	
	/*m_pChildren[1]->OnActivate(bActivate);
	m_pChildren[2]->OnActivate(bActivate);
	*/
	
}

void iGameMenuView::OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	m_bGestureComplete = false;
    m_bAllowGestures = true;
#ifdef PC_VERSION
    // disallow close window during list drags
    iView* v = GetChildByPos(pos);
    if (v && (v->GetUID() == 109 || v->GetUID() == 110 || v->GetUID() == 111))
    {       
        v = v->GetChildByPos(pos - v->GetPos());
        if (v && (v->GetUID() == 302 || v->GetUID() == 303 || v->GetUID() == 304 || v->GetUID() == 305))
        {
            m_bAllowGestures = false;
        }
    }
#endif
}

void iGameMenuView::OnSwipeUp(sint32 length)
{
#ifdef OS_IPHONE
	if(
	dynamic_cast<iListBox*>(gApp.ViewMgr().CapturedView()) ||
	dynamic_cast<iAmountChooser*>(gApp.ViewMgr().CapturedView()))
		return;

	
	if (m_bAllowGestures && !m_bGestureComplete && length > 40) 
    {
		m_bGestureComplete = true;
		iDib menu(gApp.Surface().GetSize(), iDib::RGB);
		gApp.Surface().CopyToDibXY(menu, iPoint());
		iTransitionView* trView = new iTransitionView(&gApp.ViewMgr(), 
													  iTransitionView::TVM_VRT_OVERLAY,
													  iTransitionView::MM_MANUAL, 						
													  menu, gGame.PeekScreen());
		trView->TransferMouseState(this);
//		gApp.ViewMgr().ReleaseViewCapture();
		gApp.ViewMgr().SetViewCapture(trView);
		iTransitionView::TV_RESULT res = (iTransitionView::TV_RESULT)trView->DoModal();
		delete trView;
		if(res == iTransitionView::TVR_UP) 
        {
			gGame.HideViewDelayed(CV_GAMEMENU);
		}
		//m_bGestureComplete = true;
		//Invalidate();
	}
#endif
}

bool iGameMenuView::MouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
    return false;
} 

void iGameMenuView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
    if (uid == 109 || uid == 110 || uid == 111)
    {
		if (param == IMainMenuView::ResExit) {
			iQuestDlg qdlg(&gApp.ViewMgr(), iStringT(), gTextMgr[TRID_CONFIRM_EXITMM], PID_NEUTRAL);
			if (qdlg.DoModal() == DRC_YES)
				gGame.ExitGame(true);
		}
        else if (param == IMainMenuView::ResMain)
            ShowView(CM_SINGLE);
        /*else if (param == iNewMenuView::ResSingle)
            ShowView(CM_SINGLE);
        else if (param == iNewMenuView::ResMulti)
            ShowView(CM_MULTI);
			*/
        else if (param == IMainMenuView::ResContinue)
            gGame.HideView(CV_GAMEMENU);
		// ResContinueNoHide doesn't need anything
    }
}

void iGameMenuView::OnCompose()
{
	uint16 sid;
#if defined(ROYAL_BOUNTY)
	sid = PDGG(BTN_MENU);
#else
	sid = PDGG(MAINMENU_DELUXE);
    
#endif
	iRect rc = AlignRect(gGfxMgr.Dimension(sid), iRect(iPoint(), gApp.Surface().GetSize()), 
#ifdef PC_VERSION
                         AlignCenter);
#else
                         AlignCenter);
#endif
    gApp.Surface().Fill(cColor_Black);
    
	gGfxMgr.Blit(sid, gApp.Surface(), rc);
#ifdef PC_VERSION
    rc.x = iMAX(rc.x, 0);
    rc.y = iMAX(rc.y, 0);
	gApp.Surface().Fill(gApp.Surface().GetPixel(rc.point()));
	gGfxMgr.Blit(sid, gApp.Surface(), rc);
#endif
	
#if !defined(ROYAL_BOUNTY)
	uint32 alpha = 128 + 128 * sin(((float)m_frame));
	gGfxMgr.BlitAlpha(PDGG(MAINMENU_DELUXE_OVERLAY), gApp.Surface(), rc, alpha);	
	
#endif

#ifdef PLAYCITY
    iStringT text = _T("Copyright 2013 MyPlayCity, Inc., iosoftware LLC");
    iSize sz = gTextComposer.GetTextSize(text, dlgfc_plain);
    iRect copyrightrc = AlignRect(sz, gApp.Surface().GetSize(), AlignBottomRight);
    gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), text, copyrightrc);
#endif
}

void iGameMenuView::UpdateSize()
{  
#ifdef PC_VERSION
    iRect rc = AlignRect(iSize(1024, 768), m_Rect, AlignCenter);

    rc -= iPoint(0, 30);

    m_pChildren[0]->SetRect(rc);
#endif
}

bool iGameMenuView::Process(fix32 t)
{
#ifdef PC_VERSION
    m_timer += t;
    if(m_timer >= fix32(0.1f)) {
        m_timer = fix32::zero;
        m_frame++;
    }
#endif
    m_pChildren[0]->Process(t);
    /*m_pChildren[1]->Process(t);
    m_pChildren[2]->Process(t);
	*/
	Invalidate();
    return true;
}

// END OF FILE
