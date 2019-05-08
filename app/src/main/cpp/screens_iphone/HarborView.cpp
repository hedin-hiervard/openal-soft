#include "stdafx.h"
#include "HarborView.h"
#include "cm_ships.h"
#include "TransitionView.h"


namespace UI_IPHONE
{
    
    iHarborControlView::iHarborControlView( iViewMgr *pViewMgr, const iRect& rc, uint32 uid, iHarborDockView *pParentDock):
    iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pParentDock(pParentDock)
    {
        AddChild(m_pBtn = new iImgBtn(pViewMgr, this, iRect(), 100, -1, -1));
    }
    
    void iHarborControlView::UpdateSize()
    {
        m_pBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom));
    }
    
    void iHarborControlView::OnCompose()
    {
        iRect rc = GetScrRect();
        
        iRect bkrc = rc;
        gApp.Surface().Darken25Rect(bkrc);
        bkrc.InflateRect(1);
        gApp.Surface().FrameRect(bkrc, cColor_Black);
        
        
        rc.y += 5;
        
        HarborState hs = m_pParentDock->GetHarborState();
        switch(hs) {
            case HS_BUILD:
            case HS_NA:
            {
                rc.y += ComposeChildScreenTitle(gApp.Surface(), rc, gTextMgr[TRID_MSG_HARBOR_BUILD]);
                uint16 sid = SHIP_TYPE_DESC[m_pParentDock->m_dockId].sprite + 13;
                iSize sz = gGfxMgr.Dimension(sid);
                rc.y += 50;
                gGfxMgr.Blit(sid, gApp.Surface(), AlignRect(sz, rc, AlignTop));
                rc.y += gGfxMgr.ObjRect(sid).h;
                sz = GetMineralSetComposeSize(SHIP_TYPE_DESC[m_pParentDock->m_dockId].cost, true);
                ComposeMineralSet(SHIP_TYPE_DESC[m_pParentDock->m_dockId].cost, m_pParentDock->m_pHero->Owner()->Minerals(), AlignRect(sz, rc, AlignTop), false);
                break;
            }
            case HS_OCCUPY:
            case HS_OCCUPIED:
            {
                uint16 sid = SHIP_TYPE_DESC[m_pParentDock->m_dockId].sprite + ((m_pParentDock->m_dockId == 0) ? 30 : 43);
                rc.y += ComposeChildScreenTitle(gApp.Surface(), rc,
                                                gTextMgr[hs == HS_OCCUPY ? TRID_MSG_HARBOR_BOARD : TRID_MSG_HARBOR_OCCUPIED]);
                rc.y += 50;
                iIsoMetric im(im_coef);
                iPoint pts = iPoint(rc.x + rc.w / 2, rc.y + rc.h / 2);
                pts -= iPoint(40, 50);
                iVisCnst_Harbor *vsh = (iVisCnst_Harbor*)(m_pParentDock->m_pHero->GetLocation());
                const iVisCnstT* pCnstT = vsh->Proto();
                iPoint spos = pts + im.Map2Screen(m_pParentDock->m_pHarbor->GetShipPos(m_pParentDock->m_dockId) - vsh->Pos());
                if(m_pParentDock->m_dockId == 1)
                    spos += iPoint(5, 25);
                gGfxMgr.Blit(sid, gApp.Surface(), spos);
                for (uint32 cvr = 0; cvr < pCnstT->AvatarsCount(); ++cvr)
                {
                    const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[pCnstT->AvatarEntry(cvr).aid];
                    iPoint cpos(pCnstT->AvatarEntry(cvr).ox, pCnstT->AvatarEntry(cvr).oy);
                    for (uint32 sid = 0; sid < avatar.SpritesCount(); ++sid)
                    {
                        uint32 id = avatar.GetSpriteId(sid);
                        iPoint opos = im.Map2Screen(cpos) + pts + avatar.GetSpriteOffset(sid);
                        gGfxMgr.Blit(id, gApp.Surface(), opos);
                    }
                }
                break;
            }
        }
    }
    void iHarborControlView::Update()
    {
        HarborState hs = m_pParentDock->GetHarborState();
        switch(hs) {
            case HS_BUILD:
                m_pBtn->SetSprite(PDGG(BTN_BUILD_DELUXE), -1);
                m_pBtn->SetVisible(m_pParentDock->m_pHero->Owner()->Minerals().Has(SHIP_TYPE_DESC[m_pParentDock->m_dockId].cost));
                break;
            case HS_OCCUPY:
                m_pBtn->SetSprite(PDGG(BTN_BOARD_DELUXE), -1);
                m_pBtn->SetVisible(true);
                break;
            default:
                m_pBtn->SetVisible(false);
                break;
        }
    }
    
    void iHarborControlView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        uint32 uid = pView->GetUID();
        if(uid == 100) { // the only button :)
            HarborState hs = m_pParentDock->GetHarborState();
            switch(hs) {
                case HS_BUILD:
                    gGame.Map().AddShip(m_pParentDock->m_pHarbor->GetShipPos(m_pParentDock->m_dockId), m_pParentDock->m_dockId);
                    check(m_pParentDock->m_pHarbor->GetShip(m_pParentDock->m_dockId));
                    m_pParentDock->m_pHero->Owner()->AddMineralSet(-SHIP_TYPE_DESC[m_pParentDock->m_dockId].cost);
                    m_pParentDock->m_pHarbor->ShipBought(m_pParentDock->m_dockId);
                    break;
                    
                case HS_OCCUPY:
                    check(m_pParentDock->m_pHarbor->GetShip(m_pParentDock->m_dockId));
                    m_pParentDock->m_pHarbor->GetShip(m_pParentDock->m_dockId)->Occupy(m_pParentDock->m_pHero);
                    gGame.HideView(iChildGameView::CV_HARBOR);
                    break;
            }
            Update();
        }
    }
    
    const iSize iHarborControlView::sm_defSize = iSize(240, 320);
    
    
    iHarborDockView::iHarborDockView( iViewMgr *pViewMgr, const iRect& rc, uint32 uid, uint32 dockId ):
    iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled), m_dockId(dockId), m_pHarbor(NULL)
    {
        AddChild(m_pHCView = new iHarborControlView(pViewMgr, iRect(), 100, this));
    }
    
    void iHarborDockView::UpdateSize()
    {
        m_pHCView->SetRect(AlignChild(m_pHCView->sm_defSize, AlignBottom));
        m_pHCView->UpdateSize();
    }
    
    void iHarborDockView::SetHarbor( IHarbor* pHarbor )
    {
        m_pHarbor = pHarbor;
        m_pHCView->Update();
        check(m_dockId < pHarbor->NumOfDocks());
        Invalidate();
    }
    
    HarborState iHarborDockView::GetHarborState()
    {
        check(m_pHarbor->IsBoardable());
        
        if(m_pHarbor->GetShip(m_dockId) && !m_pHarbor->GetShip(m_dockId)->IsLoaded())
            return HS_OCCUPY;
        else if(m_pHarbor->GetShip(m_dockId) && m_pHarbor->GetShip(m_dockId)->IsLoaded())
            return HS_OCCUPIED;
        else if(!m_pHarbor->GetShip(m_dockId) && m_pHero->Owner()->Minerals().Has(SHIP_TYPE_DESC[m_dockId].cost))
            return HS_BUILD;
        else
            return HS_NA;
    }
    
    void iHarborDockView::SetHero( iHero* pHero )
    {
        m_pHero = pHero;
        Invalidate();
    }
    
    void iHarborDockView::OnCompose()
    {
        iRect rc = GetScrRect();
        
        //	rc.y += ComposeChildScreenTitle(gApp.Surface(), rc, iFormat(_T("%s %d"), gTextMgr[TRID_OBJ_HARBOR], m_dockId + 1));
    }
    
    
    iHarborView::iHarborView(): IHarborView(false, CV_OVERLAND), m_bGestureComplete(false)
    {
        AddChild(m_pDockView1 = new iHarborDockView(&gApp.ViewMgr(), iRect(), 100, 0));
        AddChild(m_pDockView2 = new iHarborDockView(&gApp.ViewMgr(), iRect(), 101, 1));
    }
    
    void iHarborView::UpdateSize()
    {
        iSize sz(m_Rect.w / 2, m_Rect.h);
        m_pDockView1->SetRect(AlignChild(sz, AlignLeft));
        m_pDockView1->UpdateSize();
        m_pDockView2->SetRect(AlignChild(sz, AlignRight));
        m_pDockView2->UpdateSize();
    }
    
    
    void iHarborView::OnActivate(bool bActivate)
    {
        if(bActivate) {
            FallDown();
            gTutorial.ShowFingerHelp();
        }
    }
    
    void iHarborView::SetHero( iHero* pHero )
    {
        m_pDockView1->SetHero(pHero);
        m_pDockView2->SetHero(pHero);
    }
    
    void iHarborView::SetHarbor( IHarbor *pHarbor )
    {
        m_pDockView1->SetHarbor(pHarbor);
        m_pDockView2->SetHarbor(pHarbor);
    }
    
    void iHarborView::OnCompose()
    {
        gApp.Surface().Fill(cColor_Black);
        iSize sz = gGfxMgr.Dimension(PDGG(HERO_DIALOG_BACK));
        gGfxMgr.Blit(PDGG(HERO_DIALOG_BACK), gApp.Surface(), AlignRect(sz, iRect(iPoint(), gApp.Surface().GetSize()), AlignCenter));
        
    }
    
    void iHarborView::OnSwipeUp( sint32 length )
    {
#ifdef OS_IPHONE
        if(length < 40 || gApp.ViewMgr().DragGlyph() || 
           dynamic_cast<iListBox*>(gApp.ViewMgr().CapturedView()) ||
           dynamic_cast<iAmountChooser*>(gApp.ViewMgr().CapturedView()))
            return;
        
        if(!m_bGestureComplete) {
            m_bGestureComplete = true;
            iDib castle(gApp.Surface().GetSize(), iDib::RGB);
            gApp.Surface().CopyToDibXY(castle, iPoint());
            iTransitionView* trView = new iTransitionView(&gApp.ViewMgr(), 
                                                          iTransitionView::TVM_VRT_OVERLAY,
                                                          iTransitionView::MM_MANUAL, 						
                                                          castle, gGame.PeekScreen());
            trView->TransferMouseState(this);
            //		gApp.ViewMgr().ReleaseViewCapture();
            gApp.ViewMgr().SetViewCapture(trView);
            iTransitionView::TV_RESULT res = (iTransitionView::TV_RESULT)trView->DoModal();
            delete trView;
            if(res == iTransitionView::TVR_UP) {
                gGame.HideViewDelayed(CV_HARBOR);			
            }
            //m_bGestureComplete = true;
            //Invalidate();
        }
#endif
    }
    
    void iHarborView::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) {
        if(mID == 0) m_bGestureComplete = false;
    }
    
    void iHarborView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        uint32 uid = pView->GetUID();
        if(uid == 111)
            gGame.HideView(CV_HARBOR);
    }
}
