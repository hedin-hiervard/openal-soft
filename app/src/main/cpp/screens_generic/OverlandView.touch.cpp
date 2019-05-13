#include "stdafx.h"
#include "Composer.h"
#include "OverlandView.h"
#include "OverlandView.touch.h"
#include "GameMenuDlg.h"
#include "Dlg_Save.h"
#include "gxl.popupview.h"
#include "ai.scanner.h"
#include "Dlg_Spellbook.h"
#include "dbg.h"
#include "helpers.h"

//#include "xxc/xxc.security.h"
#include "ArtifactList.h"
#include "ArtifactList.touch.h"
#include "HeroView.h"
#include "CastleView.h"
#include "MinimapView.h"

#include "screens_ipad/OverlandUI.h"
#include "screens_iphone/OverlandUI.h"

bool openAllMap = false;

// set of behaviour defines
#undef  SCROLL_ON_DOUBLE_TRACK
#define SCROLL_ON_TRACK
#undef  SCROLL_ON_BORDERS
#undef  SCROLL_ON_BORDERS_ZOOMED_ONLY
#define SCROLL_AUTO_ZOOMED
#undef  ZOOM_ON_PINCH
#undef  ZOOM_ON_TOUCH
#undef ZOOM_ON_HOLD
#undef MINIMAP_ON_PINCH_IN

const iPoint fingerOffset(0, -35);
const iPoint zoomedFingerOffset(0, -35);
iPoint lastAnchor;

void DoubleDib(const iDib& src, iDib &dst)
{
    iDib::pixel* pDst = dst.GetPtr();
    for (uint32 yy=0; yy<src.GetHeight(); ++yy){
        const iDib::pixel* pSrc = src.GetPtr(iPoint(0,yy));
        for (uint32 xx=0; xx<src.GetWidth(); ++xx) {
            *pDst = *pSrc;
            *(pDst+1) = *pSrc;
            *(pDst + dst.GetWidth()) = *pSrc;
            *(pDst + dst.GetWidth() + 1) = *pSrc;
            pDst +=  2;
            pSrc ++;
        }
        pDst += dst.GetWidth();
    }
}

iMainView_touch::iMainView_touch()
: iMainView()
, m_bTracked(false)
, m_bGestureReset(false)
, m_bWillScroll(false)
, m_pathDisapTimer(fix32::zero)
, m_turnBlinkTimer(fix32::zero)
, m_bPathTapSelected(true)
, m_bPathConfirmed(false)
, m_bPreciseMode(false)
, m_ScrollAnchor(cInvalidPoint)
, m_bManualZoomed(false)
, m_globalAniTimer(fix32::zero)
, m_bHorseBtnBlinking(false)
, m_bEndTurnBtnBlinking(false)
, m_bVoidNextClick(false)
, m_ncell(iPoint(0,0))
#if defined( PC_VERSION )
, m_bEndTurn(false)
, m_bMinimap(false)
#endif
{
    AddChild(m_pOpTurnBar = new iOpTurnBar(&gApp.ViewMgr(), iRect()));
    
#ifndef PC_VERSION
    AddChild(m_pInfoBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), VPUID_BTN_INFO, PDGG(BTN_INFOBAR_BIG), PDGG(BTN_INFOBAR_BIG_PRESSED), iStringT(), Visible | Enabled, AlignLeft));
    m_pInfoBtn->SetHoldable(true);
    AddChild(m_pHorseBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), VPUID_BTN_HORSE, PDGG(BTN_HORSE_BIG), PDGG(BTN_HORSE_BIG_PRESSED), iStringT(), Visible | Enabled, AlignLeft));
#endif
    AddChild(m_pInfoBar = new iInfoBar_touch(&gApp.ViewMgr(), this, iRect()));
    
    AddChild(m_pCastleBtn = new iCastleGlyphBtn(&gApp.ViewMgr(), this, iRect(), VPUID_BTN_CASTLE));
    m_pCastleBtn->SetVisible(gGame.Map().CurCastle());
    AddChild(m_pHeroBtn = new iHeroPortBtn(&gApp.ViewMgr(), this, iRect(),VPUID_BTN_HERO, false, true, AlignTopLeft));
    m_pHeroBtn->SetVisible(gGame.Map().CurHero());
    
    m_pInfoBar->SetCell(cInvalidPoint);
    
    AddChild(m_pMenuBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), VPUID_BTN_MENU, PDGG(BTN_SETTINGS), PDGG(BTN_SETTINGS_PRESSED),
#ifdef PC_VERSION
                                      gTextMgr[TRID_HOVER_ENTER_MENU] /*iStringT(_T("Enter the game menu (Esc)"))*/,
#else
                                      iStringT(),
#endif
                                      Visible | Enabled, AlignTopRight));
    AddChild(m_pEndTurnBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), VPUID_BTN_NTURN, PDGG(BTN_END_TURN), PDGG(BTN_END_TURN_PRESSED),
#ifdef PC_VERSION
                                         gTextMgr[TRID_HOVER_END_TURN] /*iStringT(_T("End turn (E)"))*/,
#else
                                         iStringT(),
#endif
                                         Visible | Enabled, AlignRight));
    AddChild(m_pNextHeroCastleBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), VPUID_BTN_NHERO,
#ifdef PC_VERSION
                                                PDGG(BTN_SWITCH_HERO_BIG_RIGHT), PDGG(BTN_SWITCH_HERO_BIG_PRESSED), gTextMgr[TRID_HOVER_NXT_HERO] /*iStringT(_T("Next hero (Tab)"))*/,
#else
                                                PDGG(BTN_SWITCH_HERO_BIG), PDGG(BTN_SWITCH_HERO_BIG_PRESSED), iStringT(),
#endif
                                                Visible | Enabled, AlignRight));
    
    m_pNextHeroCastleBtn->SetHoldable(true);
    
    AddChild(m_pHorseBtnR = new iImgBtn(&gApp.ViewMgr(), this, iRect(), VPUID_BTN_HORSE2, PDGG(BTN_HORSE_BIG_R), PDGG(BTN_HORSE_BIG_R_PRESSED),
#ifdef PC_VERSION
                                        gTextMgr[TRID_HOVER_CNFRM_HERO_PATH] /*iStringT(_T("Confirm hero path (Space)"))*/,
#else
                                        iStringT(),
#endif
                                        Visible | Enabled, AlignRight));
    
    AddChild(m_pCastBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), VPUID_BTN_CAST, PDGG(BTN_SPELLBOOK), PDGG(BTN_SPELLBOOK_PRESSED), iStringT(), Visible | Enabled, AlignBottom));
    //AddChild(m_pMinimapBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), VPUID_BTN_MINIMAP, PDGG(BTN_MINIMAP), PDGG(BTN_MINIMAP_PRESSED), Visible|Enabled, AlignRight));
    //	new iIphoneBtn(m_pMgr, this, trc, 100 + xx, Visible | Enabled, lang_icons[xx]));
    AddChild(m_pMinimapBtn = new iIphoneBtn(&gApp.ViewMgr(), this, iRect(), VPUID_BTN_MINIMAP, Visible | Enabled, PDGG(ICN_MINIMAP)));
    
    m_pMinimapBtn->SetHoldable(true);
    m_pMinimapBtn->SetSwitch(true);
    m_tmpDib.Init(iSize(480 >> 1, 320 >> 1), iDib::RGB);
    
    AddChild(m_pHCSelect = new iHeroCastleSelectView(&gApp.ViewMgr(), iRect(), VPUID_HCSELECT));
    
    m_pHCSelect->SetVisible(false);
    
    AddChild(m_pMinimapView = new iMinimapRawView(&gApp.ViewMgr(), this, iRect(), VPUID_MINIMAPVIEW));
    m_pMinimapView->SetVisible(false);
    
    if(gUseIpadUI)
    {
        AddChild(m_pOverlandToolbar = new UI_IPAD::iOverlandToolbar(&gApp.ViewMgr(), VPUID_OVERLAND_TOOLBAR));
        AddChild(m_pResourceBar = new UI_IPAD::iResourceBar(&gApp.ViewMgr(), VPUID_RESOURCEBAR));
    } else
    {
        AddChild(m_pOverlandToolbar = new UI_IPHONE::iOverlandToolbar(&gApp.ViewMgr(), VPUID_OVERLAND_TOOLBAR));
        AddChild(m_pResourceBar = new UI_IPHONE::iResourceBar(&gApp.ViewMgr(), VPUID_RESOURCEBAR));
    }
    
    if(gUseIpadUI)
    {
        AddChild(m_pQuickSaveBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), VPUID_BTN_QSAVE, PDGG_IPAD(OVLND_BUTTON_SAVE), PDGG_IPAD(OVLND_BUTTON_SAVE_PRESSED),
#ifdef PC_VERSION
                                               gTextMgr[TRID_HOVER_SAVE] /*iStringT(_T("Quick save (Cmd-S)"))*/,
#else
                                               iStringT(),
#endif
                                               Visible | Enabled, AlignTopLeft));
        AddChild(m_pQuickLoadBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), VPUID_BTN_QLOAD, PDGG_IPAD(OVLND_BUTTON_LOAD), PDGG_IPAD(OVLND_BUTTON_LOAD_PRESSED),
                                               
#ifdef PC_VERSION
                                               gTextMgr[TRID_HOVER_LOAD] /*iStringT(_T("Quick load (Cmd-L)"))*/,
#else
                                               iStringT(),
#endif
                                               Visible | Enabled, AlignTopLeft));
    }
    SwitchInterface(IT_BARLESS);
    
    UpdateMetrics();
}

void iMainView_touch::OnGameStarted()
{
    m_pOverlandToolbar->UpdateMinimap();
}

void iMainView_touch::OnComposerAnchorChanged()
{
    m_pOverlandToolbar->CenterMinimap();
}

void iMainView_touch::UpdateSize()
{
    
    bool bBarless = (m_it == IT_BARLESS);
    bool bToolbar = (m_it == IT_TOOLBAR);
    
    iSize sidebtnsize = gGfxMgr.Dimension(PDGG(BTN_HORSE_BIG));
#ifndef PC_VERSION
    if(bToolbar)
#endif
        sidebtnsize.w += 15;
    
    iPoint  sidebtnoffset(0, 134);
#ifndef PC_VERSION
    if(m_Rect.w < 1024) {
        sidebtnoffset.y = 40;
    }
    
    if(bBarless)
        sidebtnoffset.x = 6;
#endif
    iMainView::UpdateSize();
    m_pOpTurnBar->SetRect(AlignRect(iSize(m_Rect.w, gTextComposer.GetFontHeight(dlgfc_hdr.fontSize)), m_Rect, AlignBottom));
    
#ifndef PC_VERSION
    m_pHorseBtn->SetRect(AlignRect(sidebtnsize, m_Rect, AlignLeft) - iPoint(sidebtnoffset.x, 0));
    if(!gUseIpadUI)
        m_pNextHeroCastleBtn->SetRect(AlignRect(sidebtnsize, m_Rect, AlignLeft) - iPoint(sidebtnoffset.x, sidebtnoffset.y));
    else
        m_pNextHeroCastleBtn->SetRect(AlignRect(sidebtnsize, m_Rect, AlignLeft) - iPoint(-13, sidebtnoffset.y));
    
    m_pInfoBtn->SetRect(AlignRect(sidebtnsize, m_Rect, AlignLeft) + iPoint(-sidebtnoffset.x, sidebtnoffset.y));
    
    if(bToolbar)
        m_pHorseBtnR->SetRect(AlignRect(sidebtnsize, m_Rect, AlignRight) + iPoint(sidebtnoffset.x, -sidebtnoffset.y / 2));
    else
        m_pHorseBtnR->SetRect(AlignRect(sidebtnsize, m_Rect, AlignRight) + iPoint(sidebtnoffset.x, 0));
    
    if(bToolbar)
        m_pEndTurnBtn->SetRect(AlignRect(sidebtnsize, m_Rect, AlignRight) + iPoint(0, sidebtnoffset.y / 2));
    else if(bBarless)
        m_pEndTurnBtn->SetRect(AlignRect(iIphoneBtn::sm_defSize, m_Rect, AlignBottomRight) );
#else
    m_pNextHeroCastleBtn->SetRect(AlignRect(sidebtnsize, m_Rect, AlignRight) + iPoint(0, -sidebtnoffset.y));
    m_pHorseBtnR->SetRect(AlignRect(sidebtnsize, m_Rect, AlignRight) - iPoint(0, 0));
    m_pEndTurnBtn->SetRect(AlignRect(sidebtnsize, m_Rect, AlignRight) + iPoint(-0, sidebtnoffset.y));
#endif
    
    m_pInfoBar->SetRect(AlignRect(iSize(480, 79), m_Rect, AlignTop));
    m_pCastleBtn->SetRect(AlignRect(iIphoneBtn::sm_defSize, m_Rect, AlignTopLeft));
    m_pHeroBtn->SetRect(AlignRect(iIphoneBtn::sm_defSize, m_Rect, AlignTopLeft));
    m_pMenuBtn->SetRect(AlignRect(iIphoneBtn::sm_defSize, m_Rect, AlignTopRight));
    //   m_pHorseBtn->SetRect(m_pNextHeroCastleBtn->GetRect() + iPoint(iIphoneBtn::sm_defSize.w * 2, 0));
    m_pCastBtn->SetRect(m_pEndTurnBtn->GetRect() + iPoint(iIphoneBtn::sm_defSize.w * 2, 0));
    m_pMinimapBtn->SetRect(AlignRect(iIphoneBtn::sm_defSize, m_Rect, AlignBottomLeft) + iPoint(0, 3));
    
    if(gGame.Map().CurPlayer()) {
        m_pHCSelect->SetRect(AlignChild(m_pHCSelect->GetSize(gGame.Map().CurPlayer()->HeroCount(), gGame.Map().CurPlayer()->CastleCount()), AlignCenter) + iPoint(0, 20));
        m_pHCSelect->UpdateSize();
    }
    m_pMinimapView->SetRect(AlignRect(iSize(m_Rect.w / 2, m_Rect.h / 2), m_Rect, AlignCenter));
    
    m_pOverlandToolbar->SetRect(AlignRect(m_pOverlandToolbar->GetDefSize(), m_Rect, AlignBottomRight));
    m_pOverlandToolbar->UpdateSize();
    
    m_pResourceBar->SetRect(AlignRect(m_pResourceBar->GetDefSize(), m_Rect, AlignTop));
    
    if(gUseIpadUI)
    {
        iSize sz = gGfxMgr.Dimension(PDGG_IPAD(OVLND_BUTTON_SAVE));
        m_pQuickSaveBtn->SetRect(AlignRect(sz, m_Rect, AlignTopLeft));
        m_pQuickLoadBtn->SetRect(AlignRect(sz, m_Rect, AlignTopLeft) + iPoint(sz.w, 0));
    }
}

void iMainView_touch::UpdateMetrics()
{
    uint32 finger = 40;
    rcScrLeft = rcScrRight = rcScrTop = rcScrBottom = iRect(0, 0, finger, finger);
    rcScrLeft.h = m_Rect.h;
    rcScrRight.x = m_Rect.w - finger;
    rcScrRight.h = m_Rect.h;
    rcScrTop.w  = m_Rect.w;
    rcScrBottom.y = m_Rect.h - finger;
    rcScrBottom.w = m_Rect.w;
}


void iMainView_touch::ShowInfobar(bool bShow)
{
    if(m_it == IT_TOOLBAR)
        m_pInfoBar->SetVisible(false);
    else if(m_it == IT_BARLESS)
        m_pInfoBar->SetVisible(bShow);
}

void iMainView_touch::SwitchInterface(InterfaceType it)
{
    m_it = it;
    if(it == IT_BARLESS) {
#ifndef PC_VERSION
        m_pHorseBtn->SetSprite(PDGG(BTN_HORSE_BIG), PDGG(BTN_HORSE_BIG_PRESSED));
        m_pInfoBtn->SetSprite(PDGG(BTN_INFOBAR_BIG), PDGG(BTN_INFOBAR_BIG_PRESSED));
#endif
        m_pHorseBtnR->SetSprite(PDGG(BTN_HORSE_BIG_R), PDGG(BTN_HORSE_BIG_R_PRESSED));
        m_pEndTurnBtn->SetSprite(PDGG(BTN_END_TURN), PDGG(BTN_END_TURN_PRESSED));
        m_pNextHeroCastleBtn->SetSprite(PDGG(BTN_SWITCH_HERO_BIG), PDGG(BTN_SWITCH_HERO_BIG_PRESSED));
        m_pMenuBtn->SetSprite(PDGG(BTN_SETTINGS), PDGG(BTN_SETTINGS_PRESSED));
    } else if(it == IT_TOOLBAR) {
        if(gUseIpadUI)
        {
            m_pHorseBtnR->SetSprite(PDGG(OVLND_BUTTON_RIGHT_HORSE), PDGG(OVLND_BUTTON_RIGHT_HORSE_PRESSED));
            m_pMenuBtn->SetSprite(PDGG_IPAD(OVLND_BUTTON_MENU), PDGG_IPAD(OVLND_BUTTON_MENU_PRESSED));
            m_pEndTurnBtn->SetSprite(PDGG(OVLND_BUTTON_RIGHT_ENDTURN), PDGG(OVLND_BUTTON_RIGHT_ENDTURN_PRESSED));
        }
#ifndef PC_VERSION
        m_pHorseBtn->SetSprite(PDGG(OVLND_BUTTON_LEFT_HORSE), PDGG(OVLND_BUTTON_LEFT_HORSE_PRESSED));
        m_pInfoBtn->SetSprite(PDGG(OVLND_BUTTON_LEFT_INFOBAR), PDGG(OVLND_BUTTON_LEFT_INFOBAR_PRESSED));
        m_pNextHeroCastleBtn->SetSprite(PDGG(OVLND_BUTTON_LEFT_SWITCHER), PDGG(OVLND_BUTTON_LEFT_SWITCHER_PRESSED));
#else
        m_pNextHeroCastleBtn->SetSprite(PDGG(OVLND_BUTTON_RIGHT_SWITCHER), PDGG(OVLND_BUTTON_RIGHT_SWITCHER_PRESSED));
        //m_pNextHeroCastleBtn->SetSprite(PDGG(OVLND_BUTTON_LEFT_SWITCHER), PDGG(OVLND_BUTTON_LEFT_SWITCHER_PRESSED));
#endif
    }
    UpdateSize();
}

void iMainView_touch::UpdateButtons()
{
    bool bActPlayer = gGame.Map().CurPlayer() == gGame.Map().ActPlayer();
    bool bCommon = (m_OLComposer.GetZoom() == 1 || !gSettings.GetEntryValue(CET_AUTOZOOM) || m_bManualZoomed) &&
    bActPlayer;
    
    bool bSmall = (m_Rect.w < 1024);
    
    bool bHCSelect = m_pHCSelect->IsVisible();
    bool bMinimap = m_pMinimapView->IsVisible();
    
    bool bInfoMode =
#ifndef PC_VERSION
    m_pInfoBtn->GetButtonState() & iButton::Pressed;
#else
    false;
#endif
    bool bSwitcher = gGame.Map().CurPlayer() &&
    (gGame.Map().CurPlayer()->CastleCount() > 0 ||
     gGame.Map().CurPlayer()->HeroCount() > 1);
    
    bool bBarless = (m_it == IT_BARLESS);
    bool bToolbar = (m_it == IT_TOOLBAR);
    
    ShowInfobar(bActPlayer
                && (gUseIpadUI ||  bInfoMode)
                );
    
    m_pOverlandToolbar->SetVisible(bToolbar && bCommon);
    m_pResourceBar->SetVisible(bToolbar && bCommon);
    
    m_pOverlandToolbar->SetHero(gGame.Map().CurHero());
    m_pOverlandToolbar->SetCastle(gGame.Map().CurCastle());
    
    
    if(bCommon && !bHCSelect && !bMinimap) {
#ifdef ROYAL_BOUNTY
        m_pEndTurnBtn->SetVisible(false);
#ifndef PC_VERSION
        m_pInfoBtn->SetVisible(true);
#endif
#else
        m_pEndTurnBtn->SetVisible(!bInfoMode);
#ifndef PC_VERSION
        m_pInfoBtn->SetVisible(true);
#endif
#endif
        m_pMenuBtn->SetVisible(!bInfoMode);
        m_pHeroBtn->SetHero(gGame.Map().CurHero());
        m_pCastleBtn->SetCastle(gGame.Map().CurCastle());
        m_pHeroBtn->SetVisible(!bInfoMode && gGame.Map().CurHero() && bBarless);
        m_pCastleBtn->SetVisible(!bInfoMode && gGame.Map().CurCastle() && bBarless);
#ifndef PC_VERSION
        m_pHorseBtn->SetVisible(!bSmall && gGame.Map().CurHero() && !bInfoMode);
#endif
        //m_pHorseBtn->SetVisible(false);
        m_pHorseBtnR->SetVisible(gGame.Map().CurHero() && !bInfoMode);
        
        //			m_pCastBtn->SetVisible(gGame.Map().CurHero() && gGame.Map().CurHero()->SpellBook().HasOverlandSpells() && !bInfoMode && bBarless);
        m_pCastBtn->SetVisible(false);
        
        if(gUseIpadUI)
        {
            m_pQuickSaveBtn->SetVisible(!bInfoMode && bToolbar);
            m_pQuickLoadBtn->SetVisible(!bInfoMode && bToolbar);
        }
        /*
         viva la horse button!
         m_pHorseBtn->SetVisible(gGame.Map().CurHero() && !gGame.Map().CurHero()->Path().IsEmpty() &&
         gGame.Map().CurHero()->Moves() >= gGame.Map().CurHero()->Path().GetStepCost());
         */
    } else {
        m_pHorseBtnR->SetVisible(false);
        m_pCastBtn->SetVisible(false);
        m_pEndTurnBtn->SetVisible(false);
        
        m_pMenuBtn->SetVisible(false);
        m_pHeroBtn->SetVisible(false);
        m_pCastleBtn->SetVisible(false);
        m_pMinimapBtn->SetVisible(false);
        if(gUseIpadUI)
        {
            m_pQuickSaveBtn->SetVisible(false);
            m_pQuickLoadBtn->SetVisible(false);
        }
#ifndef PC_VERSION
        m_pHorseBtn->SetVisible(false);
        m_pInfoBtn->SetVisible(false);
#endif
    }
    
    
    m_pMinimapBtn->SetVisible(bCommon && !bInfoMode && !bHCSelect && bBarless);
#ifdef ROYAL_BOUNTY
    m_pNextHeroCastleBtn->SetVisible(false);
#else
    //m_pNextHeroCastleBtn->SetVisible(true);
    m_pNextHeroCastleBtn->SetVisible(bCommon && !bMinimap && !bInfoMode);
#endif
    
    /*
     m_pHorseBtn->SetVisible(false);
     m_pHorseBtnR->SetVisible(false);
     m_pMenuBtn->SetVisible(false);
     m_pMinimapBtn->SetVisible(false);
     */
}

void iMainView_touch::OnSandGlass()
{
    /* iPlayer* pPlayer = gGame.Map().CurPlayer();
     // Are you sure you want to end your turn?
     if (gSettings.GetEntryValue(CET_ENDTURNCONF)) {
     for (iPlayer::iHLIt hit = pPlayer->HeroFirst(); hit != pPlayer->HeroEnd(); ++hit) {
     if ( !(*hit)->Exhausted() ) {
     iQuestDlg qdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_END_TURN], pPlayer->PlayerId());
     if (qdlg.DoModal() != DRC_YES) {
     return;
     }
     break;
     }
     }
     }
     // Autosave
     if (gSettings.GetEntryValue(CET_AUTOSAVE) > 0) {
     iStringT fname;
     iSaveDlg::GetSaveFileName(fname,SAVE_GAME_SLOTS-1);
     SaveGameFile( fname, gGame.Map() );
     }
     
     // Next turn
     gGame.Map().NextPlayer();
     */
}


void iMainView_touch::OnPinchIn(sint32 length)
{
#ifdef IPHONE_DEBUG
    /*if(length > 20 && !m_bGestureReset) {
     gGame.Map().CurPlayer()->NextHeroCastle();
     UpdateButtons();
     m_bGestureReset = true;
     return;
     }*/
#endif
    
#ifndef PC_VERSION
    if(m_pInfoBtn->GetButtonState() & iButton::Pressed)
        return;
#endif
    /*#ifdef MINIMAP_ON_PINCH_IN
     if(m_OLComposer.GetZoom() == 1 && length > 60 && !m_bGestureReset) {
     gGame.ShowViewDelayed(CV_MINIMAP);
     m_bGestureReset = true;
     }
     #endif
     */
#ifdef ZOOM_ON_PINCH
    //if(gSettings.GetEntryValue(CET_AUTOZOOM))
    //  return;
    if(length < 15 && length > 0)
        lastAnchor = m_OLComposer.GetAnchor();
    if(length > 90  && !m_bGestureReset) {
        //SetSurveyMode(true);
        //      gGame.ShowView(CV_MINIMAP);
        m_OLComposer.SetAnchor(lastAnchor - ((m_OLComposer.GetZoom() == 1) ? iPoint() : iPoint(120, 80)));
        m_OLComposer.SetZoom(1);
        UpdateButtons();
        m_bGestureReset = true;
    }
#endif
}

void iMainView_touch::OnPinchOut(sint32 length)
{
#ifndef PC_VERSION
    if(m_pInfoBtn->GetButtonState() & iButton::Pressed)
        return;
#endif
#ifdef ZOOM_ON_PINCH
    //if(gSettings.GetEntryValue(CET_AUTOZOOM))
    //  return;
    
    if(length < 15 && length > 0)
        lastAnchor = m_OLComposer.GetAnchor();
    
    if(length > 90 && !m_bGestureReset) {
        //      SetSurveyMode(false);
        m_OLComposer.SetAnchor(lastAnchor + ((m_OLComposer.GetZoom() == 2) ? iPoint() : iPoint(120, 80)));
        m_OLComposer.SetZoom(2);
        m_bManualZoomed = true;
        
        UpdateButtons();
        m_bGestureReset = true;
    }
#endif
}


void iMainView_touch::OnDualSwipeLeft(sint32 length)
{
#ifndef PC_VERSION
    if(m_pInfoBtn->GetButtonState() & iButton::Pressed)
        return;
#endif
    if(length > 20 && !m_bGestureReset) {
        gGame.Map().CurPlayer()->NextHero();
        //UpdateButtons();
        m_pInfoBar->SetMode(gGame.Map().CurHero() ? iInfoBar_touch::Default : iInfoBar_touch::Castle);
        m_bGestureReset = true;
    }
}

void iMainView_touch::OnDualSwipeRight(sint32 length)
{
#ifndef PC_VERSION
    if(m_pInfoBtn->GetButtonState() & iButton::Pressed)
        return;
#endif
    if(length > 20 && !m_bGestureReset) {
        gGame.Map().CurPlayer()->PrevHero();
        //UpdateButtons();
        m_pInfoBar->SetMode(gGame.Map().CurHero() ? iInfoBar_touch::Default : iInfoBar_touch::Castle);
        
        m_bGestureReset = true;
    }
}

void iMainView_touch::OnTripleSwipeLeft(sint32 length)
{
#ifndef PC_VERSION
    if(m_pInfoBtn->GetButtonState() & iButton::Pressed)
        return;
#endif
    if(length > 20 && !m_bGestureReset) {
        gGame.Map().CurPlayer()->NextCastle();
        //UpdateButtons();
        m_pInfoBar->SetMode(gGame.Map().CurHero() ? iInfoBar_touch::Default : iInfoBar_touch::Castle);
        m_bGestureReset = true;
    }
}

void iMainView_touch::OnTripleSwipeRight(sint32 length)
{
#ifndef PC_VERSION
    if(m_pInfoBtn->GetButtonState() & iButton::Pressed)
        return;
#endif
    if(length > 20 && !m_bGestureReset) {
        gGame.Map().CurPlayer()->PrevCastle();
        //UpdateButtons();
        m_pInfoBar->SetMode(gGame.Map().CurHero() ? iInfoBar_touch::Default : iInfoBar_touch::Castle);
        
        m_bGestureReset = true;
    }
}

void iMainView_touch::OnTimer(uint32 tid)
{
    if (tid == 1) {
        if (m_holdTimer == -1) {
            m_holdPos = cInvalidPoint;
            return;
        }
        m_holdTimer += 100;
#ifdef ZOOM_ON_HOLD
        iHero *pHero = gGame.Map().CurHero();
        if (
#ifndef PC_VERSION
            !(m_pInfoBtn->GetButtonState() & iButton::Pressed) &&
#endif
            m_holdTimer >= 500 &&  !IsMouseDown(1) && (!pHero || !pHero->Moving())) {
            if(gSettings.GetEntryValue(CET_AUTOZOOM) && m_OLComposer.GetZoom() == 1) {
                m_holdTimer = -1;
                m_downTime = GetTickCount();
                m_UnzoomedAnchor = m_OLComposer.GetAnchor();
                m_OLComposer.SetZoom(2);
                m_bManualZoomed = false;
                UpdateButtons();
                m_OLComposer.SetAnchor(m_OLComposer.GetAnchor() + iPoint(m_holdPos.x / 2, m_holdPos.y / 2));
#ifdef SCROLL_AUTO_ZOOMED
                m_OldAnchor = m_OLComposer.GetAnchor();
#endif
            } else {
                m_bPreciseMode = true;
            }
        } else m_pMgr->SetTimer(100,1,this);
#endif
    }
}

bool IsOneHanded();

void iMainView_touch::OnMouseEntered(const iPoint& pos, MouseId mID){
    
    iIsoMetric im(im_coef);
    iPoint actPos = Composer().ActAnchor(pos);
    
    m_ncell = im.Screen2Map(actPos);
    
    m_pInfoBar->SetCell(m_ncell);
    m_pOverlandToolbar->SetMapPos(m_ncell);
#ifdef PC_VERSION
    m_pOverlandToolbar->SetHighlighted(false);
#endif
}

void iMainView_touch::OnMouseLeave(const iPoint& pos, MouseId mID){
    
    
}

void iMainView_touch::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
    MouseId testId;
    if(gUseIpadUI) testId == 10;
    else testId = 4;
    
    if(mID == testId) {
        openAllMap = !openAllMap;
        if(openAllMap)
            gGame.Map().ActPlayer()->DiscoverFullMap();
        return;
    }
    
    //if(gAniHost.IsPlaying()) return;
    iIsoMetric im(im_coef);
    
    iPoint actPos = m_OLComposer.ActAnchor(pos + (m_OLComposer.GetZoom() == 1 ? fingerOffset: zoomedFingerOffset));
    
    if(m_pHCSelect->IsVisible()) {
        if (mID == 0)
            m_pNextHeroCastleBtn->MouseDown(pos, mID, mbID);
        else if(mID == 1)
            m_pHCSelect->MouseDown(pos, mID, mbID);
        return;
    }
    
    if(m_pMinimapView->IsVisible()) {
        if (mID == 0)
            m_pMinimapBtn->MouseDown(pos, mID, mbID);
        else if(mID == 1)
            m_pMinimapView->MouseDown(pos, mID, mbID);
        return;
    }
    
    m_bGestureReset = false;
#ifdef SCROLL_ON_DOUBLE_TRACK
    if(mID == 1) {
#endif
        if(!m_pHCSelect->IsVisible() && !m_pMinimapView->IsVisible()) {
            m_ScrollAnchor = m_OLComposer.GetAnchor();
            //      UpdateButtons();
            m_bWillScroll =  true;//m_TrackAnchor[0].GetDelta(m_TrackAnchor[1]) < 110;
        }
#ifdef SCROLL_ON_DOUBLE_TRACK
    }
#endif
    
#if defined(ZOOM_ON_HOLD)  || defined(ZOOM_ON_TOUCH)
    m_downTime = GetTickCount();
#endif
    
#ifdef ZOOM_ON_TOUCH
    if(mID == 0) {
        m_UnzoomedAnchor = m_OLComposer.GetAnchor();
        m_OLComposer.SetZoom(2);
        m_bManualZoomed = false;
        UpdateButtons();
        m_OLComposer.SetAnchor(m_OLComposer.GetAnchor() + iPoint(pos.x / 2, pos.y / 2));
    } else {
        m_OLComposer.SetAnchor(m_UnzoomedAnchor);
        m_OLComposer.SetZoom(1);
        UpdateButtons();
    }
#endif
    
#ifdef ZOOM_ON_HOLD
    m_holdTimer = 0;
    m_holdPos = pos;
    m_pMgr->SetTimer(100, 1, this);
    
#endif
    
#ifdef SCROLL_AUTO_ZOOMED
    if(mID == 0) {
        m_OldAnchor = m_OLComposer.GetAnchor();
    }
#endif
    
    
    iHero *pHero = gGame.Map().CurHero();
    
    if (pHero && pHero->Moving()) {
        pHero->Stop();
        gGame.OnHeroStopMoving(pHero);
    }
    
#ifdef SCROLL_ON_BORDERS
#ifdef SCROLL_ON_BORDERS_ZOOMED_ONLY
    if(m_OLComposer.GetZoom() == 2) {
#endif
        if (rcScrTop.PtInRect(pos)) m_MouseTrackMask |= 0x01;
        if (rcScrBottom.PtInRect(pos)) m_MouseTrackMask |= 0x02;
        if (rcScrLeft.PtInRect(pos)) m_MouseTrackMask |= 0x04;
        if (rcScrRight.PtInRect(pos)) m_MouseTrackMask |= 0x08;
#ifdef SCROLL_ON_BORDERS_ZOOMED_ONLY
    }
#endif
#endif
    
    // highlight tapped object
    actPos = m_OLComposer.ActAnchor(pos);
    iPoint ncell = im.Screen2Map(actPos);
    
    m_highlightedCell = ncell;
    
#ifndef PC_VERSION
    if(m_pInfoBtn->GetButtonState() & iButton::Pressed) {
#ifdef OS_IPHONE
        //if(mID == IsOneHanded() ? 0 : 1)
#endif
        {
            m_pInfoBar->SetCell(m_highlightedCell);
            m_pOverlandToolbar->SetMapPos(m_highlightedCell);
            m_pInfoBar->Invalidate();
        }
        return;
    }
#endif
    
    
    iBaseMapObject* pMapObj = gGame.Map().GetCellObj(ncell);
    
    if(pMapObj) {
        m_pInfoBar->SetCell(ncell);
        m_pOverlandToolbar->SetMapPos(ncell);
    }
    
    
    if(pHero = DynamicCast<iHero*>(pMapObj)) {
        // this is our hero - highlight
        if(pHero->Owner() == gGame.Map().CurPlayer()) {
            m_pHighlightedObject = pMapObj;
            m_objHighlightType = HLT_AVAILABLE;
        }
    }
    else if(pMapObj) {
        sint8 oval = 0;
        if(pHero = gGame.Map().CurHero()) {
            oval = pHero->Owner()->PassMap().GetAt(pMapObj->Pos().x,pMapObj->Pos().y);
        }
        // if this is a structure we can visit - highlight
        m_pHighlightedObject = pMapObj;
        m_objHighlightType = HLT_NOT_AVAILABLE;
        iCastle* pCastle = DynamicCast<iCastle*>(pMapObj);
        if(pCastle && pCastle->Owner() == gGame.Map().CurPlayer()->PlayerId()) {
            m_objHighlightType = HLT_AVAILABLE;
        } else if(pHero) {
            iPath path(pHero->Pos());
            m_objHighlightType = (!pHero->MakePath(path, pMapObj->Pos().x, pMapObj->Pos().y)) ? HLT_NOT_AVAILABLE : HLT_AVAILABLE;
        }
    } else {
        m_highlightedCell = ncell;
    }
    
}

void iMainView_touch::OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
    //if(gAniHost.IsPlaying()) return;
    if(PerformedFingers() > 1) return;
    if(m_bSurveyMode) return;
#ifndef PC_VERSION
    if(m_pInfoBtn->GetButtonState() & iButton::Pressed) return;
#endif
    //if(m_bTracked) { m_bTracked = false; return; }
    if(gGame.Map().CurHero() && gGame.Map().CurHero()->Moving()) return;
    
    iPoint actPos = m_OLComposer.ActAnchor(pos);
    iIsoMetric im(im_coef);
    iPoint ncell = im.Screen2Map(actPos);
    iHero *pHero;
    iBaseMapObject* pMapObj = gGame.Map().GetCellObj(ncell);
    if(DynamicCast<iHero*>(pMapObj)) {
        pHero = DynamicCast<iHero*>(pMapObj);
        if((!gGame.Map().CurHero() || pHero == gGame.Map().CurHero()) && pHero->Owner() == gGame.Map().CurPlayer()) {
            gGame.Map().CurPlayer()->SetCurHero(pHero);
            gGame.ShowView(CV_HERO);
        }
    }
    
    else if (/*ncell == m_OLComposer.GetTargetPoint() && */DynamicCast<iCastle*>(pMapObj)/* && pMapObj->Pos() != ncell*/) {
        iCastle* pCastle = DynamicCast<iCastle*>(pMapObj);
        if (pCastle->Owner() == gGame.Map().CurPlayer()->PlayerId()) {
            gGame.CastleView()->SetCastle(pCastle);
            gGame.ShowView(CV_CASTLE);
        }
    }
}

void iMainView_touch::SetHeroPath( iPoint ncell ){
    
    iHero *pHero = gGame.Map().CurHero();
    
    
    uint8 sticklevel = (m_OLComposer.GetZoom() == 1 ? 3: 0);
    sticklevel = 0;
    
    iBaseMapObject* pMapObj = gGame.Map().GetCellObj(ncell, sticklevel);
    
    sint8 oval = 0;
    if(pHero && pMapObj) {
        pHero->Owner()->UpdatePassMap();
        oval = pHero->Owner()->PassMap().GetAt(pMapObj->Pos().x,pMapObj->Pos().y);
    }
    
    //if(pMapObj == pHero) {
    //gApp.Surface().CopyToDibXY(sm_last, iPoint());
    //  gGame.ShowView(CV_HERO);
    //}
    //#if !defined(ZOOM_ON_HOLD) && !defined(ZOOM_ON_TOUCH)
#if defined(ZOOM_ON_HOLD) || defined(ZOOM_ON_TOUCH)
    if(GetTickCount() - m_downTime < 300) {
#endif
        if(pHero) {
            // we have clicked in radius of selected path and not on another object - CONFIRM
            sint32 radius = 1;
            
#ifndef PC_VERSION
            if(gSettings.GetEntryValue(CET_STICKY_PATH) > 0)
                radius = 2;
#endif
            
            // if we clicked on object with a path set, ...
            if(pMapObj && !pHero->Path().IsEmpty()) {
                // it's another object than our target, change target precisely
                if(pMapObj->Pos() != pHero->Path().DestPoint())
                    radius = 1;
                // otherwise, it's 100% confirmation
                else if(pMapObj && (!dynamic_cast<iCastle*>(pMapObj)))
                    radius = 1000;
            }
            // we need to click precisely on hero to enter the hero screen
            if(pMapObj == pHero)
                radius = 1;
            
            // if clicked first time on a hero or castle, show tutorial help
            if(DynamicCast<iHero*>(pMapObj) || DynamicCast<iCastle*>(pMapObj))
            {
                gTutorial.Trigger(TUT_DOUBLE_CLICK_TO_ENTER);
                gTutorial.MarkDontShow(TUT_DOUBLE_CLICK_TO_ENTER);
            }
            
            // confirm the path
            if(!pHero->Path().IsEmpty() && pHero->Path().DestPoint().GetDelta(ncell) < radius) {
                pHero->SetPath(ncell, true, radius);
                m_bHorseBtnBlinking = false;
                UpdateButtons();
                m_OLComposer.SetRedCross(cInvalidPoint);
                m_bPathTapSelected = true;
                m_bPathConfirmed = true;
            }
            // click on object
            else if (pMapObj && pMapObj != pHero &&
                     (!DynamicCast<iCastle*>(pMapObj) || pMapObj->Pos().GetDelta(ncell) < 2) &&
                     pHero->Owner()->PassMap().GetAt(pMapObj->Pos().x, pMapObj->Pos().y) != -1)
            {
                // stick path to object
                iPoint oldpos = pHero->Path().IsEmpty() ? cInvalidPoint : pHero->Path().DestPoint();
                
                pHero->SetPath(pMapObj->Pos(), true, 0);
                
                m_bHorseBtnBlinking = false;
                UpdateButtons();
                m_bPathTapSelected = true;
                m_bPathConfirmed = false;
                m_pathDisapTimer = fix32::zero;
                oval = pHero->Owner()->PassMap().GetAt(pMapObj->Pos().x, pMapObj->Pos().y);
                m_OLComposer.SetRedCross(oval == -1 ? ncell : cInvalidPoint);
                bool bEnterable = DynamicCast<iCastle*>(pMapObj) || DynamicCast<iHero*>(pMapObj);
                if(!bEnterable && (pHero->Path().IsEmpty() || oldpos == pHero->Path().DestPoint())) {
                    gTutorial.Trigger(TUT_NO_PATH);
                } else if(!bEnterable) {
                    gTutorial.Trigger(TUT_TAP_TO_CONFIRM);
                }
            } else if(1 || m_bPathTapSelected) {
                // set path to the tapped point
                
                //m_bPathTapSelected = m_OLComposer.GetZoom() == 1;
                //m_pathDisapTimer = fix32::zero;
                iPoint oldpos = pHero->Path().IsEmpty() ? cInvalidPoint : pHero->Path().DestPoint();
                
                pHero->SetPath(ncell, true, 0);
                m_bHorseBtnBlinking = false;
                UpdateButtons();
                m_bPathTapSelected = true;
                m_bPathConfirmed = false;
                m_pathDisapTimer = fix32::zero;
                if(pHero->Owner()->PassMap().IsValidPos(ncell.x, ncell.y)) {
                    oval = pHero->Owner()->PassMap().GetAt(ncell.x, ncell.y);
                    m_OLComposer.SetRedCross((oval == -1 && !pMapObj)? ncell : cInvalidPoint);
                }
                if(!pMapObj && (pHero->Path().IsEmpty() || oldpos == pHero->Path().DestPoint())) {
                    gTutorial.Trigger(TUT_NO_PATH);
                } else if(!pMapObj) {
                    gTutorial.Trigger(TUT_TAP_TO_CONFIRM);
                }
            }
            //pHero->SetPath();
        }
        
#if defined(ZOOM_ON_HOLD) || defined(ZOOM_ON_TOUCH)
    }
#endif
    
    //#if !defined(ZOOM_ON_HOLD) && !defined(ZOOM_ON_TOUCH)
    //}
    //#endif
    
    pHero = gGame.Map().CurHero();
    if(pHero && !pHero->Path().IsEmpty()) {
        m_pInfoBar->SetCell(pHero->Path().DestPoint());
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pHero->Path().DestPoint());
        m_pOverlandToolbar->SetMapPos(pHero->Path().DestPoint());
        
    }
    
    m_OLComposer.SetTarget(cInvalidPoint, 0);
    Invalidate();
}

void iMainView_touch::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
    //if(gAniHost.IsPlaying()) return;
    if(m_bVoidNextClick) {
        m_bVoidNextClick = false;
        return;
    }
#ifndef PC_VERSION
    if(m_pInfoBtn->GetButtonState() & iButton::Pressed)
        return;
#endif
    m_pLastHighlightedObject = NULL;
    
    
    /*if(m_pHCSelect->IsVisible() && mID == 1) {
     m_pHCSelect->MouseClick(pos, mID);
     return;
     }*/
    
    if(PerformedFingers() > 1) return;
    //    if(m_bIsDoubleClicking) return;
    if(m_bSurveyMode) return;
    if(m_bTracked) { m_bTracked = false; return; }
    
    iPoint ncell;
    if(m_pLastHighlightedObject != NULL &&
       (!dynamic_cast<iCastle*>(m_pLastHighlightedObject) || m_pLastHighlightedObject->Pos().GetDelta(pos) <= 2)) {
        ncell = m_pLastHighlightedObject->Pos();
    } else if(m_lastHighlightedCell != cInvalidPoint) {
        ncell = m_lastHighlightedCell;
    } else {
        iPoint actPos = m_OLComposer.ActAnchor(pos);
        iIsoMetric im(im_coef);
        ncell = im.Screen2Map(actPos);
    }
    
    SetHeroPath(ncell);
}

void iMainView_touch::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
    m_pLastHighlightedObject = m_pHighlightedObject;
    m_pHighlightedObject = NULL;
    m_holdTimer = -1;
    m_lastHighlightedCell = m_highlightedCell;
    m_highlightedCell = cInvalidPoint;
    
    if(m_pHCSelect->IsVisible()) {
        if (mID == 0) {
            m_pNextHeroCastleBtn->MouseUp(pos, mID, mbID);
            m_bVoidNextClick = true;
        }
        else if(mID == 1)
            m_pHCSelect->MouseUp(pos, mID, mbID);
        return;
    }
    
#ifndef PC_VERSION
    if(m_pInfoBtn->GetButtonState() & iButton::Pressed) {
        if (mID == 0) {
            //m_pInfoBtn->MouseDown(pos, 0);
            m_pInfoBtn->MouseUp(pos, 0, mbID);
        }
        /*if(mID == 1) {
         //m_pInfoBar->SetCell(cInvalidPoint);
         //m_pInfoBar->Invalidate();
         }*/
        return;
    }
#endif
    if(m_pMinimapView->IsVisible()) {
        if (mID == 0)
            m_pMinimapBtn->MouseUp(pos, mID, mbID);
        else if(mID == 1)
            m_pMinimapView->MouseUp(pos, mID, mbID);
        return;
    }
    
    
    m_ScrollAnchor = cInvalidPoint;
    m_bPreciseMode = false;
    
    UpdateButtons();
    
#if defined(ZOOM_ON_HOLD) || defined(ZOOM_ON_TOUCH)
    if(!m_bManualZoomed && m_OLComposer.GetZoom() == 2 && gSettings.GetEntryValue(CET_AUTOZOOM)) {
        m_OLComposer.SetZoom(1);
        UpdateButtons();
        m_OLComposer.SetAnchor(m_UnzoomedAnchor);
    }
#endif
    
    iHero* pHero = gGame.Map().CurHero();
    if(pHero && !pHero->Path().IsEmpty()) {
        m_pInfoBar->SetCell(pHero->Path().DestPoint());
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pHero->Path().DestPoint());
        m_pOverlandToolbar->SetMapPos(pHero->Path().DestPoint());
    }
}


void iMainView_touch::OnDualMouseTrack(const iPoint& pos)
{
#ifdef SCROLL_ON_DOUBLE_TRACK
    if(m_bWillScroll) {
        iPoint npos = m_ScrollAnchor + (m_BeginningTrackAnchor[0] - pos);
        m_OLComposer.SetAnchor(npos);
    }
#endif
}


void iMainView_touch::OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
    //if(gAniHost.IsPlaying()) return;
    iIsoMetric im(im_coef);
    iPoint actPos = m_OLComposer.ActAnchor(pos +  (m_OLComposer.GetZoom() == 1 ? fingerOffset: zoomedFingerOffset));
#ifndef PC_VERSION
    if(m_pInfoBtn->GetButtonState() & iButton::Pressed) {
#ifdef OS_IPHONE
        if(mID == IsOneHanded() ? 0 : 1)
#endif
        {
            actPos = m_OLComposer.ActAnchor(pos);
            m_pInfoBar->SetCell(im.Screen2Map(actPos));
            m_pOverlandToolbar->SetMapPos(im.Screen2Map(actPos));
            
            m_pInfoBar->Invalidate();
        }
        return;
    }
#endif
    m_highlightedCell = cInvalidPoint;
    
    if(m_pHCSelect->IsVisible() || m_pMinimapView->IsVisible())
        return;
    
    if(PerformedFingers() > 1) return;
    iHero *pHero = gGame.Map().CurHero();
    if(pHero && pHero->Moving())
        return;
    
#ifdef SCROLL_ON_TRACK
    iPoint offset =  (m_BeginningTrackAnchor[0] - pos);
    if((m_OLComposer.GetZoom() == 1 || !gSettings.GetEntryValue(CET_AUTOZOOM) || m_bManualZoomed) && !m_bPreciseMode && !m_pHCSelect->IsVisible() && !m_pMinimapView->IsVisible() &&
       offset.GetLength() > 5) {
        UpdateButtons();
        
        offset.x /= (sint32)m_OLComposer.GetZoom();
        offset.y /= (sint32)m_OLComposer.GetZoom();
        iPoint npos = m_ScrollAnchor + offset;
        m_OLComposer.SetAnchor(npos);
        m_holdTimer = -1;
        m_bTracked = true;
        return;
    }
#endif
    
    m_OLComposer.SetRedCross(cInvalidPoint);
    
#ifdef SCROLL_AUTO_ZOOMED
    if(!m_bManualZoomed && gSettings.GetEntryValue(CET_AUTOZOOM) && gSettings.GetEntryValue(CET_SCROLL_AUTOZOOMED) && m_OLComposer.GetZoom() == 2 && !m_MouseTrackMask) {
        m_OLComposer.SetAnchor(m_OldAnchor + (pos - m_BeginningTrackAnchor[0]));
    }
#endif
    
#ifdef SCROLL_ON_BORDERS
#ifdef SCROLL_ON_BORDERS_ZOOMED_ONLY
    if(m_OLComposer.GetZoom() == 2) {
#endif
        uint8 trackMask=0;
        if (rcScrTop.PtInRect(pos)) trackMask |= 0x01;
        if (rcScrBottom.PtInRect(pos)) trackMask |= 0x02;
        if (rcScrLeft.PtInRect(pos)) trackMask |= 0x04;
        if (rcScrRight.PtInRect(pos)) trackMask |= 0x08;
        
        if (trackMask != m_MouseTrackMask) {
            Invalidate();
            m_MouseTrackMask = trackMask;
        }
#ifdef SCROLL_ON_BORDERS_ZOOMED_ONLY
    }
#endif
#endif
    
    
    if(m_bSurveyMode) {
        iPoint npos = m_OldAnchor + (m_BeginningTrackAnchor[0] - pos);
        m_OLComposer.SetAnchor(npos);
        Invalidate();
        return;
    }
#ifdef TRACK_PATH
    if (!m_MouseTrackMask) {
        iPoint ncell = im.Screen2Map(m_OLComposer.ActAnchor(pos +  (m_OLComposer.GetZoom() == 1 ? fingerOffset: zoomedFingerOffset)));// - iPoint(0, 50));
        if (ncell != m_OLComposer.GetTargetPoint()) {
            //#ifndef ZOOM_ON_HOLD
            m_holdTimer = -1;
            //#endif
            if (iHero* pCurHero = gGame.Map().CurHero()) {
                iPath path(pCurHero->Pos());
                SpriteId cr = PDGG(TRACK);
                if (gGame.Map().IsValidPos(ncell.x,ncell.y) && pCurHero->Pos() != ncell && pCurHero->MakePath(path, ncell.x, ncell.y)) {
                    if (path.PathCost() > pCurHero->Moves()) cr += 1;
                } else {
                    cr += 2;
                }
                m_OLComposer.SetTarget(ncell, cr);
            } else {
                m_OLComposer.SetTarget(ncell, PDGG(TRACK) + 2);
            }
        }
        actPos = m_OLComposer.ActAnchor(pos +  (m_OLComposer.GetZoom() == 1 ? fingerOffset: zoomedFingerOffset));
        //  iIsoMetric im(im_coef);
        ncell = im.Screen2Map(actPos);
        
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(ncell);
        //if(pMapObj)
        // ShowOniInfoBar_touch_touch(ncell);
        m_pInfoBar->Invalidate();
        if (ncell == m_OLComposer.GetTargetPoint() && DynamicCast<iCastle*>(pMapObj) && pMapObj->Pos() != ncell) {
            iCastle* pCastle = DynamicCast<iCastle*>(pMapObj);
            if (pCastle->Owner() == gGame.Map().CurPlayer()->PlayerId()) {
                if (pCastle != gGame.Map().CurCastle()) {
                    //gGame.Map().CurPlayer()->SetCurCastle(pCastle);
                } /*else {
                   gGame.ShowView(CV_CASTLE);
                   }*/
            }
        } else if (iHero* pHero = gGame.Map().CurHero()) {
            if (ncell == m_OLComposer.GetTargetPoint()  && (DynamicCast<iOwnCnst*>(pMapObj) || DynamicCast<iVisCnst*>(pMapObj))) {
                pHero->SetPath(pMapObj->Pos());
                m_bHorseBtnBlinking = false;
                UpdateButtons();
                m_bPathTapSelected = false;
                m_bPathConfirmed = false;
                m_pathDisapTimer = 0;
            } else if (ncell == m_OLComposer.GetTargetPoint() || (!pHero->Path().IsEmpty() && pHero->Path().DestPoint() == ncell)) {
                pHero->SetPath(ncell);
                m_bHorseBtnBlinking = false;
                UpdateButtons();
                m_pathDisapTimer = 0;
                m_bPathConfirmed = false;
                m_bPathTapSelected = false;
            }
        }
    }
#endif
    Invalidate();
    //      m_holdTimer = -1;
}

void iMainView_touch::SetSurveyMode(bool bSurveyMode)
{
    if (bSurveyMode == m_bSurveyMode) return;
    m_bSurveyMode = bSurveyMode;
    if (m_bSurveyMode) {
        EnableToolBar(false);
        uint32 scale = gSettings.GetEntryValue(CET_SURVMAPSCALE)?2:1;
        
        m_tmpDib.Init(iSize(480 >> 1, 320 >> 1), iDib::RGB);
        m_dibSurveyBuff.Init(iSize(480, 320), iDib::RGB);
        iPoint anchor = m_OLComposer.GetAnchor();
    } else {
        m_dibSurveyBuff.Cleanup();
    }
}

void iMainView_touch::OnCompose()
{
    //gApp.Surface().Fill(0);
    //return;
    iRect rect = GetScrRect();
    
    //rect.DeflateRect(0,14,0,21);
    
    if(m_bSurveyMode) {
        m_OLComposer.Compose(m_tmpDib, m_OLComposer.GetAnchor());
        uint32 scale = gSettings.GetEntryValue(CET_SURVMAPSCALE)?2:1;
        
        if (gSettings.GetEntryValue(CET_SURVMAPFILTR)) iDibTransform::PyramidalStretch(m_tmpDib, m_dibSurveyBuff,scale);
        else iDibTransform::FastStretch(m_tmpDib, m_dibSurveyBuff,scale);
        m_dibSurveyBuff.CopyToDibXY(gApp.Surface(), iPoint());
        //  m_OLComposer.Compose();
    } else {
        //  m_OLComposer.Compose();
        if(m_OLComposer.GetZoom() == 2) {
            m_OLComposer.Compose(&m_tmpDib, m_globalAniTimer);
            DoubleDib(m_tmpDib, m_dibSurveyBuff);
            m_dibSurveyBuff.CopyToDibXY(gApp.Surface(), iPoint());
        } else
            m_OLComposer.Compose(NULL, m_globalAniTimer);
    }
    ComposeEventList();
    if(m_pHCSelect->IsVisible())
        gApp.Surface().Darken50Rect(m_Rect);
    if(m_pMinimapView->IsVisible()) {
        gApp.Surface().Darken50Rect(m_Rect);
        iRect rc = m_pMinimapView->GetRect();
        rc.InflateRect(2);
        //rc.InflateRect(0, 0, 1, 0);
        ComposeDlgBkgndPanel(gApp.Surface(), rc, false);
    }
    
    if(gUseIpadUI)
    {
        if(m_it == IT_TOOLBAR) {
            /* compose button underlays */
            sint16 ls = PDGG_IPAD(OVLND_BUTTONS_LEFT1),
            rs = PDGG_IPAD(OVLND_BUTTONS_RIGHT1);
            iSize sz = gGfxMgr.Dimension(ls);
            iPoint offset(15, (sz.h - m_pNextHeroCastleBtn->GetScrRect().h) / 2);
            
            bool
#ifndef PC_VERSION
            bInfo = m_pInfoBtn->IsVisible(),
            bLHorse = m_pHorseBtn->IsVisible(),
#endif
            bHC = m_pNextHeroCastleBtn->IsVisible(),
            bRHorse = m_pHorseBtnR->IsVisible(),
            bEndTurn = m_pEndTurnBtn->IsVisible();
#ifndef PC_VERSION
            /* left buttons overlay */
            if(bInfo && bLHorse && bHC) {
                gGfxMgr.Blit(PDGG_IPAD(OVLND_BUTTONS_LEFT3), gApp.Surface(), m_pNextHeroCastleBtn->GetScrRect() - offset);
            } else if(!bLHorse) {
                if(bInfo) {
                    gGfxMgr.Blit(PDGG_IPAD(OVLND_BUTTONS_LEFT1), gApp.Surface(), m_pInfoBtn->GetScrRect() - iPoint(0, offset.y));
                }
                if(bHC) {
                    gGfxMgr.Blit(PDGG_IPAD(OVLND_BUTTONS_LEFT1), gApp.Surface(), m_pNextHeroCastleBtn->GetScrRect() - offset);
                }
            } else {
                if(bInfo) {
                    gGfxMgr.Blit(PDGG_IPAD(OVLND_BUTTONS_LEFT2), gApp.Surface(), m_pHorseBtn->GetScrRect() - offset);
                } else if(bHC) {
                    gGfxMgr.Blit(PDGG_IPAD(OVLND_BUTTONS_LEFT2), gApp.Surface(), m_pNextHeroCastleBtn->GetScrRect() - offset);
                } else {
                    gGfxMgr.Blit(PDGG_IPAD(OVLND_BUTTONS_LEFT1), gApp.Surface(), m_pHorseBtn->GetScrRect() - offset);
                }
                
            }
#endif
            /* right buttons overlay */
            offset.x = -3;
            if(bRHorse && bEndTurn
#if defined( PC_VERSION )
               && bRHorse
#endif
               ){
                gGfxMgr.Blit(
#ifndef PC_VERSION
                             PDGG_IPAD(OVLND_BUTTONS_RIGHT2),
#else
                             PDGG_IPAD(OVLND_BUTTONS_RIGHT3),
#endif
                             gApp.Surface(),
#ifndef PC_VERSION
                             m_pHorseBtnR->GetScrRect()
#else
                             m_pNextHeroCastleBtn->GetScrRect()
#endif
                             - offset);
            }
            else {
                if(bRHorse)
                    gGfxMgr.Blit(PDGG_IPAD(OVLND_BUTTONS_RIGHT1), gApp.Surface(), m_pHorseBtnR->GetScrRect() - offset);
                if(bEndTurn)
                    gGfxMgr.Blit(PDGG_IPAD(OVLND_BUTTONS_RIGHT1), gApp.Surface(), m_pEndTurnBtn->GetScrRect() - offset);
#if defined( PC_VERSION )
                if(bHC)
                    gGfxMgr.Blit(PDGG_IPAD(OVLND_BUTTONS_RIGHT1), gApp.Surface(), m_pNextHeroCastleBtn->GetScrRect() - offset);
#endif
            }
        }
    }
    
#ifdef ROYAL_BOUNTY
    // gold amount near hero port
    iMineralSet ms;
    ms.Reset();
    ms.quant[MINERAL_GOLD] = gGame.Map().ActPlayer()->Mineral(MINERAL_GOLD);
    iRect rc = GetMineralSetComposeSize(ms, false);
    rc.x += iIphoneBtn::sm_defSize.w - 25;
    ComposeMineralSet(ms, ms, rc, false, false);
    
#endif
    
    /*if(IsMouseDown(0)) {
     iSize sz = gGfxMgr.Dimension(PDGG(OVLND_PRESS));
     iPoint t = m_TrackAnchor[0];
     sz.w /= 2; sz.h /= 2;
     gGfxMgr.Blit(PDGG(OVLND_PRESS), gApp.Surface(), t - sz);
     }*/
}

void iMainView_touch::ComposeEventList()
{
    iPoint pos(38,18+80);
    if (gGame.Map().ActPlayer()){
        for (iEvtCIt it=m_EventList.First(); it != m_EventList.End(); ++it) {
            if ( (*it)->dstate ){
                pos.y -= (*it)->dstate;
            }
            
            uint8 a = (*it)->dstate?255-(25*(*it)->dstate):255;
            
            gTextComposer.TextOut(mapeventfc, gApp.Surface(), pos, iFormat(_T("#A%02X"),a)+(*it)->text);
            pos.y+=15;
        }
    }
}


bool iMainView_touch::Process(fix32 t)
{
    iMainView::Process(t);
    m_pHCSelect->Process(t);
    m_pInfoBar->Process(t);
    if(1 || m_bPathTapSelected) {
        if(!IsMouseDown(0) && !m_bPathConfirmed) {
            m_pathDisapTimer += t;
            if(m_pathDisapTimer >= fix32(5.0f)) {
                iHero *pHero = gGame.Map().CurHero();
                /*if(pHero) {
                 pHero->ResetPath();
                 UpdateButtons();
                 }
                 */
                m_pathDisapTimer = fix32::zero;
            }
        }
    }
    
    // overland animation
    m_globalAniTimer += t;
    if(m_globalAniTimer >= fix32(2.0f))
        m_globalAniTimer = fix32::zero;
    Invalidate();
    
    //buttons blinking
    m_turnBlinkTimer += t;
    if(m_turnBlinkTimer > fix32(1.0f) && m_turnBlinkTimer < fix32(2.0f) ) {
        bool active = false;
        iPlayer* pPlayer = gGame.Map().CurPlayer();
        
        for (iPlayer::iHLIt hit = pPlayer->HeroFirst(); hit != pPlayer->HeroEnd(); ++hit) {
            if ( !(*hit)->Exhausted() ) {
                active = true;
                break;
            }
        }
        if(!active && !m_bEndTurnBtnBlinking) {
            m_bEndTurnBtnBlinking = true;
            if(!gApp.ViewMgr().HasModalDlg())
                gTutorial.Trigger(TUT_NEXT_TURN);
        }
        if(active)
            m_bEndTurnBtnBlinking = false;
        
        if(m_bEndTurnBtnBlinking)
            m_pEndTurnBtn->SetHighlighted(true);
        /*else if(m_pHeroBtn->IsVisible() && gGame.Map().CurHero() && gGame.Map().CurHero()->Exhausted())
         m_pNextHeroCastleBtn->SetHighlighted(true);
         */
#ifndef ROYAL_BOUNTY
        if(m_bHorseBtnBlinking) {
#ifndef PC_VERSION
            m_pHorseBtn->SetHighlighted(true);
#endif
            m_pHorseBtnR->SetHighlighted(true);
        }
#endif
    } else if(m_turnBlinkTimer > fix32(2.0f)) {
        m_turnBlinkTimer = fix32::zero;
        m_pEndTurnBtn->SetHighlighted(false);
#ifndef PC_VERSION
        m_pHorseBtn->SetHighlighted(false);
#endif
        m_pHorseBtnR->SetHighlighted(false);
        //m_pNextHeroCastleBtn->SetHighlighted(false);
    }
    /*
     #ifdef OS_IPHONE
     // check if no fingers, disappear infobar
     if(!IsMouseDown(0) && !m_pInfoBtn->IsMouseDown(0)) {
     m_pInfoBar->SetVisible(false);
     m_pInfoBar->SetCell(cInvalidPoint);
     }
     #endif
     */
#if defined( PC_VERSION ) //For HotKey E (End turn)
    if( m_bEndTurn ){
        
        m_bEndTurn = false;
        
        NextTurn();
    }
#endif
    
    return true;
}

// Game notifications
void iMainView_touch::OnPlayerChanged(iPlayer* pPlayer, bool bAct)
{
    //  if(bAct)
    //      gGame.Map().NextPlayer();
    
    iMainView::OnPlayerChanged(pPlayer, bAct);
    
    
    if(gGame.Map().CurHero() && !gGame.Map().CurHero()->Path().IsEmpty())
    {
        m_bHorseBtnBlinking = true;
    }
    
    m_pMenuBtn->SetVisible(bAct);
    m_pEndTurnBtn->SetVisible(bAct);
    m_pNextHeroCastleBtn->SetVisible(bAct);
#ifndef PC_VERSION
    m_pInfoBtn->SetVisible(bAct);
#endif
    m_pInfoBar->SetCell(cInvalidPoint);
    m_pOverlandToolbar->SetMapPos(cInvalidPoint);
    if(bAct) {
        m_pHeroBtn->SetHero(gGame.Map().CurHero());
        m_pCastleBtn->SetCastle(gGame.Map().CurCastle());
        m_pHeroBtn->SetVisible(
#ifndef PC_VERSION
                               !m_pInfoBtn->IsMouseDown(0) &&
#endif
                               gGame.Map().CurHero());
        m_pCastleBtn->SetVisible(
#ifndef PC_VERSION
                                 !m_pInfoBtn->IsMouseDown(0) &&
#endif
                                 gGame.Map().CurCastle());
        m_pResourceBar->SetMinerals(&pPlayer->Minerals());
    } else {
        m_pHeroBtn->SetVisible(false);
        m_pCastleBtn->SetVisible(false);
    }
    UpdateButtons();
}

void iMainView_touch::OnHeroListChanged()
{
    UpdateButtons();
}

bool iMainView_touch::ObjIsOnInfoBar(iBaseMapObject* pObj)
{
#ifndef PC_VERSION
    if(!(m_pInfoBtn->GetButtonState() & iButton::Pressed)) return NULL;
#endif
    if(pObj == m_pInfoBar->CurObj()) return pObj;
    
    return NULL;
}

void iMainView_touch::OnHeroReceivedExperience(iHero* pHero, sint32 oldperc, sint32 newperc)
{
    if(pHero == gGame.Map().CurHero() && gGame.Map().CurPlayer() == gGame.Map().ActPlayer())
        m_pOverlandToolbar->AnimateExp(oldperc, newperc);
}

void iMainView_touch::OnHeroFinishedReceivingExperience(iHero* pHero)
{
    if(pHero == gGame.Map().CurHero() && gGame.Map().CurPlayer() == gGame.Map().ActPlayer())
        m_pOverlandToolbar->FinishAnimatingExp();
}

void iMainView_touch::OnPlayerReceivedMinerals(const iPlayer *pPlayer, const iMineralSet& was, const iMineralSet& now)
{
    if(pPlayer == gGame.Map().ActPlayer())
        m_pResourceBar->AnimateResources(was, now);
}

void iMainView_touch::NextTurn()
{
    m_bEndTurnBtnBlinking = false;
    iPlayer* pPlayer = gGame.Map().CurPlayer();
    // Are you sure you want to end your turn?
    if (gSettings.GetEntryValue(CET_ENDTURNCONF)) {
        for (iPlayer::iHLIt hit = pPlayer->HeroFirst(); hit != pPlayer->HeroEnd(); ++hit) {
            if ( !(*hit)->Exhausted() ) {
                iQuestDlg qdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_END_TURN], pPlayer->PlayerId());
                if (qdlg.DoModal() != DRC_YES) {
                    return;
                }
                break;
            }
        }
    }
    // Next turn
    gGame.Map().NextPlayer();
#ifdef MULTIPLAYER
    gMPMgr.OnEndTurn();
#endif
}

void iMainView_touch::SetHighlighted( bool high ){
#ifdef PC_VERSION
    m_pMenuBtn->SetHighlightedBtn(high);
    m_pNextHeroCastleBtn->SetHighlightedBtn(high);
    m_pEndTurnBtn->SetHighlightedBtn(high);
    m_pHorseBtnR->SetHighlightedBtn(high);

    if(gUseIpadUI)
    {
        m_pQuickSaveBtn->SetHighlightedBtn(high);
        m_pQuickLoadBtn->SetHighlightedBtn(high);
    } else {
        m_pCastBtn->SetHighlightedBtn(high);
        m_pOverlandToolbar->SetHighlighted(high);
    }
#endif
}

void iMainView_touch::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
    if (uid == VPUID_BTN_NTURN) {
        
        NextTurn();
        
    } else if (uid == VPUID_BTN_STATE) {
#ifdef MULTIPLAYER
        if(!gMPMgr.IsReplaying())
#endif
            m_pMgr->TrackPopup(new iGamePopupMenu(m_pMgr), iPoint(gApp.Surface().GetWidth(),15), iRect(iPoint(0, 0), gApp.Surface().GetSize()),AlignTopRight);
    }
    else if (uid == VPUID_BTN_INFO) {
#ifndef PC_VERSION
        if(cmd == CCI_HOLD) {
            gTutorial.Trigger(TUT_INFO_MODE);
            ShowInfobar(true);
        }
        else if(cmd == CCI_RELEASE) {
            m_pInfoBar->SetCell(cInvalidPoint);
            m_pOverlandToolbar->SetMapPos(cInvalidPoint);
        }
        m_pOverlandToolbar->SetInfoMode(m_pInfoBtn->GetButtonState() & iButton::Pressed);
        UpdateButtons();
#endif
    }
    else if(uid == VPUID_BTN_MENU)
    {
        //gGame.ShowMainMenu(true);
#ifdef PC_VERSION
        m_pMenuBtn->SetHighlightedBtn(false);
#endif
        gGame.ShowView(CV_GAMEMENU);
    }
    else if (uid == VPUID_BTN_CAST) {
        // Cast spell
        iDlg_Spellbook csDlg(m_pMgr, gGame.Map().ActPlayer()->CurHero(), SPT_MASK_OVERLAND, !!gGame.Map().ActPlayer()->CurHero()->InShip());
        if (csDlg.DoModal() == DRC_YES) {
            gGame.Map().ActPlayer()->CurHero()->CastSpell((MAGIC_SPELL)csDlg.SelSpell());
        }
    } else if (uid == 115) {
        //gGame.Map().ActPlayer()->CurHero()->Army().AddGroup(CREAT_BLACK_DRAGON, 32);
        //return;
        iHero* pHero = gGame.Map().ActPlayer()->CurHero();
        check(pHero);
        pHero->Dig();
        //pHero->ReceiveExperience(500000);
        
        //gGame.Map().ActPlayer()->CurHero()->Army().AddGroup(CREAT_BLACK_DRAGON, 32);
        // Dig
        // iTextDlg tdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_NOTIMPL], gGame.Map().ActPlayer()->PlayerId());
        // tdlg.DoModal();
        // mem_report();
    } else if(uid == VPUID_BTN_NHERO) {
        /*gGame.Map().CurPlayer()->NextHeroCastle();
         m_pInfoBar->SetMode(gGame.Map().CurHero() ? iInfoBar_touch::Default : iInfoBar_touch::Castle);
         */
        if(cmd == CCI_HOLD) {
            gTutorial.Trigger(TUT_HERO_CASTLE_SELECT);
            m_pHCSelect->SetVisible(true);
            
            if(gGame.Map().CurPlayer()) {
                if(gGame.Map().CurPlayer()->CurHero())
                    gGame.Map().CurPlayer()->NextHero();
                else if(gGame.Map().CurPlayer()->HeroCount() > 0)
                    gGame.Map().CurPlayer()->SetCurHero(*gGame.Map().CurPlayer()->HeroFirst());
                else if(gGame.Map().CurPlayer()->CastleCount() > 0)
                    gGame.Map().CurPlayer()->SetCurCastle(*gGame.Map().CurPlayer()->CastleFirst());
                else
                    check(0);
                m_pHCSelect->SetRect(AlignChild(m_pHCSelect->GetSize(gGame.Map().CurPlayer()->HeroCount(), gGame.Map().CurPlayer()->CastleCount()), AlignCenter));
            }
            m_pHCSelect->Update();
        }
        else if(cmd == CCI_RELEASE) {
            HideHCSelect();
        }
        
        UpdateButtons();
    } else if(uid == VPUID_BTN_HERO) {
        gGame.HeroView()->SetHero(gGame.Map().CurHero());
        gGame.ShowView(iChildGameView::CV_HERO);
    } else if(uid == VPUID_BTN_CASTLE) {
        gGame.CastleView()->SetCastle(gGame.Map().CurCastle());
        gGame.ShowView(iChildGameView::CV_CASTLE);
    } else if(uid == VPUID_BTN_HORSE || uid == VPUID_BTN_HORSE2) {
        if(gGame.Map().CurHero()) {
            gTutorial.Trigger(TUT_HORSE_BUTTON);
            //			gGame.Map().CurHero()->ReceiveExperience(10000);
            m_OLComposer.CenterView(gGame.Map().CurHero()->Pos());
            gGame.Map().CurHero()->SetPath();
            
            
            m_bHorseBtnBlinking = false;
        }
    } else if(uid == VPUID_BTN_MINIMAP) {
#if defined(PC_VERSION)
        if( !m_bMinimap ){
#endif
            if(cmd == CCI_HOLD) {
                if(!gUseIpadUI)
                    gTutorial.Trigger(TUT_MINIMAP);
                
                m_pMinimapView->PrepareSurface();
                m_pMinimapView->SetCenterCell(Composer().GetCenterCell());
                m_pMinimapView->SetVisible(true);
                UpdateButtons();
            }
            else if(cmd == CCI_RELEASE) {
                m_pMinimapView->SetVisible(false);
                UpdateButtons();
            }
#if defined(PC_VERSION)
        }
        else{
            m_bMinimap = false;
            m_pMinimapView->SetVisible(false);
            UpdateButtons();
        }
#endif
    } else if(uid == VPUID_MINIMAPVIEW) {
        Composer().CenterView(m_pMinimapView->GetCenterCell());
    } else if(uid == VPUID_BTN_QLOAD)
        gGame.Quickload();
    else if(uid == VPUID_BTN_QSAVE)
        gGame.Quicksave();
    
}

/* Iphone Info Bar */
iInfoBar_touch::iInfoBar_touch(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect)
: iView(pViewMgr, rect, GENERIC_VIEWPORT, 0, Visible|Enabled), m_mode(Default), m_pObj(NULL), m_vl(VL_BASIC),
m_scrollTimer(fix32::zero),
m_scrollPos(0), m_cell(cInvalidPoint)
{
}

void iInfoBar_touch::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
}

void iInfoBar_touch::OnMouseEntered(const iPoint& pos, MouseId mID)
{
}

void iInfoBar_touch::Setup(const iPoint& pos)
{
    Mode oldmode = m_mode;
    if (gGame.Map().IsValidPos(pos.x,pos.y)){
        uint8 dcfg, fcfg;
        //check(!m_pPopupView);
        if (!gGame.Map().ActPlayer()->FogMap().GetFogCfg(pos,dcfg,fcfg) && dcfg==15) {
            SetMode(iInfoBar_touch::UnchartedTerritory);
        } else {
            iStringT name;
            iBaseMapObject* pObj;
            if (/*(m_pParent->IsMouseDown(1) || gGame.MainView()->InfoButton()->IsMouseDown(1)) && */ (pObj = gGame.Map().GetCellObj(pos, name))) {
                SetObj(pObj);
                VISION_LEVEL vl = VL_BASIC;
                iHero* pHero = gGame.Map().CurHero();
                sint32 radius=0;
                if (!gGame.Map().ActPlayer()->FogMap().IsHidden(pObj->Pos()) && pHero) vl = pHero->VisionLevel(radius);
                if (vl == VL_EXPERT && pHero->Pos().GetDelta(pObj->Pos()) > radius) vl = VL_BASIC;
                SetVisionLevel(vl);
                if (DynamicCast<iVisCnst*>(pObj)) {
                    SetMode(iInfoBar_touch::VisCnst);
                } else if (iOwnCnst* pOwnCnst = DynamicCast<iOwnCnst*>(pObj)) {
                    if (pOwnCnst->Owner() == gGame.Map().ActPlayer()->PlayerId()) {
                        vl = VL_EXPERT;
                        SetVisionLevel(vl);
                    }
                    SetMode(iInfoBar_touch::OwnCnst);
                } else if (iCastle *pCastle = DynamicCast<iCastle*>(pObj)) {
                    if (pCastle->Owner() == gGame.Map().ActPlayer()->PlayerId()) {
                        vl = VL_EXPERT;
                        SetVisionLevel(vl);
                    }
                    SetMode(iInfoBar_touch::Castle);
                } else if (DynamicCast<iMapItem_Mineral*>(pObj) || DynamicCast<iMapItem_ManaCrystal*>(pObj) || DynamicCast<iMapItem_CampFire*>(pObj) || DynamicCast<iMapItem_Artifact*>(pObj) || DynamicCast<iMapItem_Chest*>(pObj) || DynamicCast<iMapItem_SpellScroll*>(pObj) || DynamicCast<iMapItem_KeyGuard*>(pObj) || DynamicCast<iMapItem_Lamp*>(pObj)) {
                    SetMode(iInfoBar_touch::Mineral);
                } else if (DynamicCast<iMapGuard*>(pObj)) {
                    SetMode(iInfoBar_touch::Guard);
                } else if (iHero* pHero = DynamicCast<iHero*>(pObj)) {
                    if (pHero->Owner()->PlayerId() == gGame.Map().ActPlayer()->PlayerId()) {
                        vl = VL_EXPERT;
                        SetVisionLevel(vl);
                    }
                    SetMode(iInfoBar_touch::Hero);
                } else if(iShip *pShip = DynamicCast<iShip*>(pObj)) {
                    SetMode(iInfoBar_touch::Ship);
                } else {
                    check(0);
                }
            } else {
                SetMode(iInfoBar_touch::Default);
                SetObj(NULL);
            }
        }
        //check( m_pPopupView != NULL );
    }
    if(m_mode != oldmode)
        m_scrollPos = 0;
}

void iInfoBar_touch::Process(fix32 t)
{
    m_scrollTimer += t;
    if((m_mode == OwnCnst || m_mode == VisCnst) && m_scrollTimer >= fix32(0.03f)) {
        m_scrollPos += 3;
        if(m_scrollPos > 1000)
            m_scrollPos = 0;
        Invalidate();
        m_scrollTimer = fix32::zero;
    }
}

void iInfoBar_touch::OnCompose()
{
    //*if(!gGame.MainView()->InfoButton()->IsMouseDown(1) && !m_pParent->IsMouseDown(1)/*|| (pHero && pHero->Path().IsEmpty())*/) {
    //  SetMode(Default);
    //  m_pObj = NULL;
    //}
    //else {
    
    iPoint ncell = m_cell;//pHero->Path().DestPoint();
    Setup(ncell);
    //}
    
    iPlayer* pPlayer = gGame.Map().ActPlayer();
    iRect rect = GetScrRect();
    
    gGfxMgr.Blit(PDGG(INFOBAR), gApp.Surface(), rect);
    iPoint tpos = iPoint(rect.x + 10, rect.y + 10);
    
    //  m_mode = HeroArmy;
    
    bool drawnHero = false;
    
    if (m_mode == Default) {
        tpos.x = rect.x + rect.w / 2 - 10;
        // our minerals
        for (uint32 xx=0; xx<MINERAL_COUNT; ++xx) {
            gGfxMgr.Blit(PDGG(RES_ICONS)+xx,gApp.Surface(),tpos-gGfxMgr.Anchor(PDGG(RES_ICONS)+xx));
            if (pPlayer) gTextComposer.TextOut(infobarfc, gApp.Surface(),tpos + iPoint(0, 25),FormatShortNumber(pPlayer->Mineral((MINERAL_TYPE)xx)).CStr(), iRect(tpos + iPoint(0, 25), iSize(gGfxMgr.Dimension(PDGG(RES_ICONS)+xx).w, 5)), AlignTop);
            tpos.x+=34;
            //  if (xx==MINERAL_GOLD) tpos.x += 20;
        }
#if 0 // this is not supported until OS 3.0
        // Battery state
        uint32 bval = 0;
        gGfxMgr.Blit(PDGG(ICN_BATTERY), gApp.Surface(), tpos);
        tpos.y+=20;
        if (bval) gTextComposer.TextOut(fc, gApp.Surface(), tpos, iFormat(_T("%d%%"),bval));
        else gTextComposer.TextOut(fc, gApp.Surface(), tpos, _T("???%"));
#endif
        // Game date
        iRect daterc = iRect(rect.x + rect.w / 2, rect.y, rect.w / 2, rect.h - 10);
        uint32 days = gGame.Map().m_CurDay-1;
        gTextComposer.TextOut(infobarfc, gApp.Surface(), cInvalidPoint, iFormat(_T("#FDB9%s: #FFDB%d  #FDB9%s: #FFDB%d  #FDB9%s: #FFDB%d"),gTextMgr[TRID_MONTH], days/28+1,gTextMgr[TRID_WEEK], (days%28)/7+1,gTextMgr[TRID_DAY],days%7+1), daterc, AlignBottom);
    }
    
    if(m_mode == Default || m_mode == Hero) {
        tpos = iPoint(rect.x + (m_mode == Hero ? m_Rect.w / 4 + 20 : 10), rect.y + 10);
        iHero* pHero = NULL;
        
        pHero = DynamicCast<iHero*>(m_pObj);
        if(!pHero)
            pHero = gGame.Map().CurHero();
        
        if(pHero == gGame.Map().CurHero()) m_vl = VL_EXPERT;
        if(pHero) {
            drawnHero = true;
            
            
            // Icon
            SpriteId sid = pHero->Proto()->m_icn46;
            //ButtonFrame(gApp.Surface(),iRect(orc.x+(orc.w/2-17),orc.y,34,22),iButton::Pressed);
            gGfxMgr.Blit(sid,gApp.Surface(), tpos);
            
            //			iRect prc(tpos, iSize(50, 50));
            //			ComposeHeroAvatar(gApp.Surface(), prc, pHero, false, false, true);
            // Name
            iRect hname(tpos, iSize((GetSize().w - 10) / 2, GetSize().h - 50));
            //          tpos.x += 44;
            gTextComposer.TextOut(infobarfc, gApp.Surface(), tpos, pHero->Name(), hname, AlignTop);
            
            // Skills
            hname.DeflateRect(0, 16, 0, 0);
            //tpos.y += 16;
            iStringT psText;
            for (sint32 psid=0; psid<PRSKILL_COUNT; ++psid) {
                if (m_vl==VL_EXPERT) psText.Addf(_T("#I%04X #FCCC%d "), PDGG(ICN_PSKILLS)+psid, pHero->FurtSkill((FURTHER_SKILLS)(FSK_ATTACK+psid)));
                else psText.Addf(_T("#I%04X #FCCC? "), PDGG(ICN_PSKILLS)+psid);
            }
            gTextComposer.TextOut(gApp.Surface(), tpos, psText, hname, AlignBottom);
            
            tpos.x = rect.x + (m_mode == Hero ? m_Rect.w / 4 : 0);
            tpos.y = 43;
            ComposeArmyCellsRow(gApp.Surface(), tpos, iSize(30,32), pHero->Army(), m_vl, false);
        }
    }
    
    if(m_mode == Guard) {
        tpos.y += 4;
        tpos.x += 50;
        iMapGuard *pGuard = DynamicCast<iMapGuard*>(m_pObj);
        if(pGuard) {
            iHero *pHero = pPlayer->CurHero();
            iStringT text1, text2;
            text1 = _T("");//CreatGroup2Text(pGuard->Creatures().Type(),pGuard->Creatures().Count(), m_vl);
            if (m_vl == VL_EXPERT) {
                iMapGuard::MEET_RESULT mr = pGuard->CheckMeetResult(pHero);
                if (mr == iMapGuard::MR_ATTACK) text2 = gTextMgr[TRID_MSG_GUARDS_POPUP_ATTACK];
                else if (mr == iMapGuard::MR_FLEE) text2 = gTextMgr[TRID_MSG_GUARDS_POPUP_FLEE];
                else text2 = gTextMgr[TRID_MSG_GUARDS_POPUP_JOIN];
            }
            iSize sz = gTextComposer.GetTextSize(text1, infobarfc),
            sz2 = gTextComposer.GetTextSize(text2, infobarfc),
            sz3 = gGfxMgr.ObjRect(CREAT_DESC[pGuard->Creatures().Type()].sprite).size();
            iSize sz1 = sz;
            sz.w += sz2.w + sz3.w + 10;
            tpos = AlignRect(sz, m_Rect, AlignCenter);
            
            /*
             gGfxMgr.Blit(CREAT_DESC[pGuard->Creatures().Type()].sprite, gApp.Surface(), tpos);
             */
            ComposeCreatureCell(gApp.Surface(), iRect(tpos - iPoint(0, 20), iSize(50, 50)), pGuard->Creatures().Type(), pGuard->Creatures().Count(), m_vl);
            tpos.x += 50;
            gTextComposer.TextOut(infobarfc, gApp.Surface(), tpos, text1);
            tpos.x += sz1.w;
            gTextComposer.TextOut(infobarfc, gApp.Surface(), tpos, text2);
        }
    } else if(m_mode == UnchartedTerritory) {
        gTextComposer.TextOut(infobarfc, gApp.Surface(), tpos, gTextMgr[TRID_UNCHARTED_TERRITORY], m_Rect, AlignCenter);
    } else if(m_mode == VisCnst) {
        iHero *pHero = pPlayer->CurHero();
        iStringT text;
        iVisCnst* pVisCnst = DynamicCast<iVisCnst*>(m_pObj);
        if(pVisCnst) {
            // Construction title
            text += gTextMgr[pVisCnst->Proto()->NameKey()];
            
            // Custom popup message
            iStringT cmsg;
            if (pVisCnst->CustomPopupText(pHero, cmsg)){
                text += iStringT(_T(" (")) + cmsg + iStringT(_T(") "));
            }
            
            // guard?
            if (!pVisCnst->Guard().Empty()){
                text += iStringT(_T(". ")) + gTextMgr[TRID_GUARD] + iStringT(_T(" "));
                for (uint32 xx=0; xx<7; ++xx) {
                    if (pVisCnst->Guard()[xx].IsValid()){
                        text += CreatGroup2Text( pVisCnst->Guard()[xx].Type(), pVisCnst->Guard()[xx].Count(), m_vl) + _T("   ");
                    }
                }
            }
            
            // Visited ?
            if (pVisCnst->ShowVisitedLabel()) {
                TextResId trid = (pHero && pVisCnst->Visited(pHero))?TRID_VISITED:TRID_NOT_VISITED;
                text.Addf((_T("  (%s)  ")),gTextMgr[trid]);
            }
            iSize sz = gTextComposer.GetTextSize(text, infobarfc);
            if(sz.w <= GetScrRect().w - 20) {
                tpos = AlignRect(sz, m_Rect, AlignCenter);
                gTextComposer.TextOut(infobarfc,gApp.Surface(),tpos, text);
            }
            else {
                iRect rc = GetScrRect();
                rc.w -= 40;
                /*tpos.y = (rc.h - sz.h) / 2;
                 tpos.x += 20;
                 */
                rc.x += 20;
                gTextComposer.TextOutWithScroll(infobarfc,gApp.Surface(), tpos, text, rc, AlignCenter, iPoint(), m_scrollPos);
            }
        }
    } else if(m_mode == OwnCnst) {
        iOwnCnst *pOwnCnst = DynamicCast<iOwnCnst*>(m_pObj);
        if(pOwnCnst) {
            iStringT text;
            
            // Construction title
            text += gTextMgr[pOwnCnst->Proto()->NameKey()];
            
            // guard?
            if (!pOwnCnst->Guard().Empty()){
                text += iStringT(_T(". ")) + gTextMgr[TRID_GUARD] + iStringT(_T(" "));
                for (uint32 xx=0; xx<7; ++xx) {
                    if (pOwnCnst->Guard()[xx].IsValid()){
                        text += CreatGroup2Text( pOwnCnst->Guard()[xx].Type(), pOwnCnst->Guard()[xx].Count(), m_vl) + _T("   ");
                    }
                }
            }
            
            iSize sz = gTextComposer.GetTextSize(text, infobarfc);
            if(sz.w <= GetScrRect().w - 20) {
                tpos = AlignRect(sz, m_Rect, AlignCenter);
                gTextComposer.TextOut(infobarfc,gApp.Surface(),tpos, text);
            }
            else {
                iRect rc = GetScrRect();
                rc.w -= 40;
                //tpos.y = (rc.h - sz.h) / 2;
                rc.x += 20;
                gTextComposer.TextOutWithScroll(infobarfc,gApp.Surface(), tpos, text, rc, AlignCenter, iPoint(), m_scrollPos);
            }
        }
    } else if(m_mode == Castle || m_mode == Default) {
        
        iCastle* pCastle = NULL;
        pCastle = DynamicCast<iCastle*>(m_pObj);
        if(!pCastle && !gGame.Map().CurHero())
            pCastle = gGame.Map().CurCastle();
        
        if(pCastle == gGame.Map().CurCastle()) m_vl = VL_EXPERT;
        if(pCastle && !drawnHero) {
            tpos.x += m_mode == Castle ? m_Rect.w / 4 + 15 : 0;
            
            // Icon
            SpriteId sid = PDGG(BTN_CASTLES) + pCastle->Proto()->Type();
            gGfxMgr.Blit(sid,gApp.Surface(), tpos);
            
            // Name
            iRect hname(tpos, iSize((GetSize().w - 10) / 2, GetSize().h - 50));
            gTextComposer.TextOut(infobarfc, gApp.Surface(), tpos,pCastle->Name(), hname, AlignTop);
            tpos.x = rect.x + (m_mode == Castle ? m_Rect.w / 4 : 0);
            tpos.y = 43;
            // Garrison
            ComposeArmyCellsRow(gApp.Surface(), tpos, iSize(30,32), pCastle->Garrison(), m_vl, false);
        }
    } else if(m_mode == Mineral) {
        
        iMapItem* mapItem = (iMapItem*)m_pObj;
        if(mapItem) {
            gTextComposer.TextOut(infobarfc, gApp.Surface(), tpos, gTextMgr[mapItem->NameKey()], m_Rect, AlignCenter);
            //AddChild(m_pPopupView = new iTextPopupDlg(m_pMgr,gTextMgr[pMapItem->NameKey()],PID_NEUTRAL));
        }
        
    } else if(m_mode == Ship) {
        
        iShip* pShip = (iShip*)m_pObj;
        if(pShip) {
            gTextComposer.TextOut(infobarfc, gApp.Surface(), tpos, gTextMgr[TRID_OBJ_SHIP], m_Rect, AlignCenter);
            //AddChild(m_pPopupView = new iTextPopupDlg(m_pMgr,gTextMgr[pMapItem->NameKey()],PID_NEUTRAL));
        }
        
    }
}

void iMainView_touch::OnActivate(bool bActivate)
{
    m_KeyTrackMask = 0;
    SetSurveyMode(false);
    SetForceHelp(false);
    UpdateButtons();
    if(bActivate) {
#ifndef PC_VERSION
        m_pInfoBtn->Unhighlight();
#endif
        m_pNextHeroCastleBtn->Unhighlight();
    }
}

bool iMainView_touch::OnKeyDown(sint32 key)
{
    /*if(key == KEY_F8) {
     openAllMap = !openAllMap;
     if(openAllMap)
     gGame.Map().ActPlayer()->FogMap().DiscoverMap(0, 0, 16386);
     } else if(key == KEY_ENTER) {
     gGame.ShowView(CV_MINIMAP);
     }
     */
    return true;
}

void iMainView_touch::SetLHandMode(bool lhandmode)
{
    /*    m_pInfoBtn->SetPos(AlignRect(m_pInfoBtn->GetSize(), m_Rect, lhandmode ? AlignRight : AlignLeft));
     m_pInfoBtn->SetSprite(lhandmode ? PDGG(BTN_INFOBAR_L): PDGG(BTN_INFOBAR),
     lhandmode ? PDGG(BTN_INFOBAR_L_PRESSED) : PDGG(BTN_INFOBAR_PRESSED));
     m_pInfoBtn->SetAlign(lhandmode ? AlignRight : AlignLeft);
     */
}

void iMainView_touch::OnCastleListChanged()
{
}

void iMainView_touch::HideHCSelect()
{
    m_pHCSelect->Hide();
    m_pNextHeroCastleBtn->Release();
    UpdateButtons();
}

inline sint32 MAXCTLS()
{
    if(gUseIpadUI) return 18;
    else return 4;
}

inline sint32 MAXCTLS_WIDTH()
{
    if(gUseIpadUI) return 6;
    else return 4;
}

inline sint32 MAXCTLS_HEIGHT()
{
    if(gUseIpadUI) return 3;
    else return 1;
}

iHeroCastleSelectView::iHeroCastleSelectView( iViewMgr *pViewMgr, const iRect& rc, uint32 uid ):
iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled), m_curPage(0),
m_bIsHiding(false), m_hideTimer(fix32::zero)
{
    AddChild(m_pPrevBtn = new iImgBtn(pViewMgr, this, iRect(), 100, PDGG(BTN_SWITCH_CASTLES), PDGG(BTN_SWITCH_CASTLES) + 2));
    AddChild(m_pNextBtn = new iImgBtn(pViewMgr, this, iRect(), 101, PDGG(BTN_SWITCH_CASTLES) + 1, PDGG(BTN_SWITCH_CASTLES) + 3));
}

void iHeroCastleSelectView::Process(fix32 t)
{
    if(m_bIsHiding) {
        m_hideTimer += t;
        if(IsOneHanded() || m_hideTimer > fix32(0.5f)) {
            m_bIsHiding = false;
            SetVisible(false);
            gGame.MainView()->UpdateButtons();
        }
    }
}

void iHeroCastleSelectView::Hide()
{
    m_bIsHiding = true;
    m_hideTimer = fix32::zero;
    m_curPage = 0;
}

void iHeroCastleSelectView::UpdateSize()
{
    m_pPrevBtn->SetRect(AlignChild(iSize(50, 50), AlignBottom) - iPoint(50, 0));
    m_pNextBtn->SetRect(AlignChild(iSize(50, 50), AlignBottom) + iPoint(50, 0));
    
    /* setup the buttons */
    iRect herorc = AlignRect(GetHeroRectSize(m_heroBtns.GetSize()), m_Rect, AlignTop) + iPoint(0, 30);
    iRect castlerc = AlignRect(GetCastleRectSize(m_castleBtns.GetSize()), m_Rect, AlignBottom) - iPoint(0, 15 + ((m_castleBtns.GetSize() > MAXCTLS()) ? 55 : 0));
    
    iRect crc = iRect(herorc.point(), GetDefBtnSize()) - m_Rect;
    for(uint32 xx=0; xx<m_heroBtns.GetSize(); xx++)
    {
        if(xx > 0 && xx % 4 == 0) {
            crc.y += crc.h;
            crc.x = herorc.x - m_Rect.x;
        }
        
        m_heroBtns[xx]->SetRect(crc);
        
        
        crc.x += crc.w;
    }
    
    crc = iRect(castlerc.point(), GetDefBtnSize()) - m_Rect;
    
    for(uint32 xx=0; xx<m_castleBtns.GetSize(); xx++)
        if(xx < m_curPage * MAXCTLS() || xx >= (m_curPage+1) * MAXCTLS())
            m_castleBtns[xx]->SetVisible(false);
        else
        {
            m_castleBtns[xx]->SetVisible(true);
            
            if(xx > m_curPage * MAXCTLS() && xx % MAXCTLS_WIDTH() == 0)
            {
                crc.y += crc.h;
                crc.x = castlerc.x - m_Rect.x;
            }
            m_castleBtns[xx]->SetRect(crc);
            
            crc.x += crc.w;
            
        }
    m_pPrevBtn->SetVisible(m_castleBtns.GetSize() > MAXCTLS());
    m_pNextBtn->SetVisible(m_castleBtns.GetSize() > MAXCTLS());
    UpdateHighlights();
}


void iHeroCastleSelectView::Update()
{
    // remove old buttons first
    while(m_heroBtns.GetSize() > 0) {
        RemoveChild(m_heroBtns.At(0));
        m_heroBtns.RemoveAt(0);
    }
    while(m_castleBtns.GetSize() > 0) {
        RemoveChild(m_castleBtns.At(0));
        m_castleBtns.RemoveAt(0);
    }
    
    // now add a button for each hero and each castle of the current player
    iPlayer *pPlayer = gGame.Map().CurPlayer();
    uint32 cnt = 0;
    for (iPlayer::iHLIt hit = pPlayer->HeroFirst(); hit != pPlayer->HeroEnd(); ++hit) {
        iBigHeroPortBtn* pBtn = new iBigHeroPortBtn(&gApp.ViewMgr(), this, iRect(), 200 + cnt);
        pBtn->SetHero(*hit);
        AddChild(pBtn);
        m_heroBtns.Add(pBtn);       
        cnt++;
    }
    
    cnt = 0;
    for (iPlayer::iCtLIt cit = pPlayer->CastleFirst(); cit != pPlayer->CastleEnd(); ++cit) {
        iBigCastleGlyphBtn* pBtn = new iBigCastleGlyphBtn(&gApp.ViewMgr(), this, iRect(), 300 + cnt);
        pBtn->SetCastle(*cit);
        AddChild(pBtn);
        m_castleBtns.Add(pBtn);
        cnt++;
    }
    UpdateSize();
}


void iHeroCastleSelectView::OnCompose()
{
    iRect rc = GetScrRect();
    
    if(m_castleBtns.GetSize() > MAXCTLS())
        rc.h -= 55;
    
    ComposeGlowBkgnd(gApp.Surface(), rc);
    
}

void iHeroCastleSelectView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
{
    uint32 uid = pView->GetUID();
    if(uid >= 300) {
        gGame.Map().CurPlayer()->SetCurCastle(((iBigCastleGlyphBtn*)pView)->Castle());      
        gGame.MainView()->InfoBar()->SetCell(gGame.Map().CurCastle()->Pos());
        
        UpdateHighlights();
        if(cmd == CCI_BTNDBLCLICK) {
            gGame.MainView()->HideHCSelect();
            gGame.CastleView()->SetCastle(((iBigCastleGlyphBtn*)pView)->Castle());
            gGame.ShowView(iChildGameView::CV_CASTLE);			
        }
        if(gSettings.GetEntryValue(CET_ONE_HANDED_MODE) > 0)
            gGame.MainView()->HideHCSelect();
        
    } else if(uid >= 200) {
        gGame.Map().CurPlayer()->SetCurHero(((iBigHeroPortBtn*)pView)->Hero());     
        gGame.MainView()->InfoBar()->SetCell(gGame.Map().CurHero()->Pos());
        UpdateHighlights();
        if(cmd == CCI_BTNDBLCLICK) {
            gGame.MainView()->HideHCSelect();
            gGame.HeroView()->SetHero(((iBigHeroPortBtn*)pView)->Hero());
            gGame.ShowView(iChildGameView::CV_HERO);
        }
        if(gSettings.GetEntryValue(CET_ONE_HANDED_MODE) > 0)
            gGame.MainView()->HideHCSelect();
    } else if(uid == 100) {
        if(m_curPage == 0) 
            m_curPage = MaxPage();
        else
            m_curPage--;
        Update();
    } else if(uid == 101) {
        if(m_curPage == MaxPage())
            m_curPage = 0;
        else
            m_curPage++;
        Update();
    }
}

iSize iHeroCastleSelectView::GetHeroRectSize( uint32 heroCount )
{
    return iSize(GetDefBtnSize().w * iMIN<uint32>(4, heroCount ), GetDefBtnSize().h * (uint32)ceil((double)heroCount / 4)); 
}

iSize iHeroCastleSelectView::GetCastleRectSize( uint32 castleCount )
{
    return iSize(GetDefBtnSize().w * iMIN<uint32>(MAXCTLS_WIDTH(), castleCount), GetDefBtnSize().h * iMIN<uint32>(MAXCTLS_HEIGHT(), (uint32)ceil((double)castleCount / MAXCTLS_WIDTH())));
}

iSize iHeroCastleSelectView::GetSize( uint32 heroCount, uint32 castleCount )
{
    iSize herosz = GetHeroRectSize(heroCount), castlesz = GetCastleRectSize(castleCount);
    
    return iSize(iMAX(herosz.w, castlesz.w), herosz.h + castlesz.h + ((castleCount > MAXCTLS()) ? 55 : 0) + ((heroCount > 0) ? 15 : 0) + ((castleCount > 0) ? 15 : 0) + 15);
}

uint32 iHeroCastleSelectView::MaxPage()
{
    return ceil((double)m_castleBtns.GetSize() / MAXCTLS()) - 1;
}

void iHeroCastleSelectView::UpdateHighlights()
{
    for(uint32 xx=0; xx<m_castleBtns.GetSize(); xx++)
        m_castleBtns[xx]->SetHighlighted(m_castleBtns[xx]->Castle() == gGame.Map().CurCastle());
    for(uint32 xx=0; xx<m_heroBtns.GetSize(); xx++)
        m_heroBtns[xx]->SetHighlighted(m_heroBtns[xx]->Hero() == gGame.Map().CurHero());
    if(gGame.Map().CurCastle())
        gGame.MainView()->InfoBar()->SetCell(gGame.Map().CurCastle()->Pos());	
    else if(gGame.Map().CurHero())
        gGame.MainView()->InfoBar()->SetCell(gGame.Map().CurHero()->Pos());		
}



iSize iHeroCastleSelectView::GetDefBtnSize()
{
    if(!gUseIpadUI)
        return iSize(100, 75);
    else
        return iSize(130, 110);
}
