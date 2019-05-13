#include "stdafx.h"
#include "UpgradeView.h"
#include "TransitionView.h"
#include "CastleView.h"
#include "Dlg_CreatInfo.h"
#include "CastleViewCommons.h"

namespace UI_IPHONE
{
    
    iCreatUpgradeView::iCreatUpgradeView( iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rc, uint32 uid, iCreatGroup::UpgradeWay way):
    iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled), m_way(way), m_pHandler(pCmdHandler), m_idx(-1)
    {
        AddChild(m_pBtn = new iImgBtn(pViewMgr, this, iRect(), 100, PDGG(BTN_CASTLE_RECRUIT_MAX_DELUXE), -1, iStringT(), Visible|Enabled, AlignCenter));
        AddChild(m_pDescView = new iCreatDescView(pViewMgr, this, iRect(), 101, CDV_GLOW | CDV_COST | CDV_TITLE, NULL));
    }
    
    void iCreatUpgradeView::SetCreatureGroup(iArmyOwner* pOwner, sint32 idx)
    {
        m_idx = idx;
        
        if(idx == -1)
        {
            m_pDescView->SetType(CREAT_UNKNOWN);
            m_pBtn->SetVisible(false);
            return;
        }
        m_pArmyOwner = pOwner;
        
        const iCreatGroup &cg = pOwner->Army().At(idx);
        bool bUpgrade = cg.GetUpgradedType(m_way) != CREAT_UNKNOWN;
        
        if(!bUpgrade) {
            m_idx = -1;
            m_pDescView->SetType(CREAT_UNKNOWN);
            m_pBtn->SetVisible(false);
        } else {
            iCreatGroup upg(cg.GetUpgradedType(m_way), cg.Count());
            m_pDescView->SetType(upg.Type());
            m_pDescView->SetCost(CREAT_DESC[cg.Type()].upgradecost);
            m_pDescView->SetAmount(upg.Count());
            m_pDescView->SetMinerals(&m_pArmyOwner->GetArmyOwnerPlayer()->Minerals());
            m_pBtn->SetVisible(m_pArmyOwner->GetArmyOwnerPlayer()->Minerals().Has(CREAT_DESC[cg.Type()].upgradecost * cg.Count()));
        }
    }
    
    void iCreatUpgradeView::OnCompose()
    {
        //    if(m_idx != -1) {
        //        iRect rc = GetScrRect();
        //        //	gApp.Surface().FrameRect(rc, cColor_Green);
        //        iRect bkrc = rc;
        //        bkrc.DeflateRect(50, 0);
        //        gApp.Surface().Darken25Rect(bkrc);
        //        bkrc.InflateRect(1);
        //        gApp.Surface().FrameRect(bkrc, cColor_Black);
        //    }
    }
    
    void iCreatUpgradeView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        uint32 uid = pView->GetUID();
        if(uid == 100) { // upgrade button click
            const iCreatGroup &cg = m_pArmyOwner->Army().At(m_idx);
            m_pArmyOwner->GetArmyOwnerPlayer()->AddMineralSet(-CREAT_DESC[cg.Type()].upgradecost * cg.Count());
            m_pArmyOwner->UpgradeArmyGroup(m_idx, m_way);
            m_pHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, m_way);
        }
    }
    
    void iCreatUpgradeView::SetInfoMode(bool bInfoMode)
    {
#if defined(OS_IPHONE) || defined(OS_ANDROID)
        m_pBtn->SetVisible(!bInfoMode);
        uint32 flags = CDV_TITLE | CDV_GLOW;
        if(bInfoMode)
            flags |= CDV_STATS | CDV_NOTITLE_PERKS /*CDV_PERKS*/;
        else
            flags |= CDV_COST;
        
        m_pDescView->SetRect(AlignChild(iSize(m_Rect.w, bInfoMode ? m_Rect.h : m_pBtn->GetRect().y), AlignTop));
        m_pDescView->SetFlags(flags);
        m_pDescView->UpdateButtons();
#endif
    }
    
    void iCreatUpgradeView::UpdateSize()
    {
        m_pBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom));
        m_pDescView->SetRect(AlignChild(iSize(m_Rect.w, m_pBtn->GetPos().y), AlignTop));
    }
    
    iUpgradeView::iUpgradeView(): IUpgradeView(false, CV_OVERLAND),
    m_pHero(NULL), m_bGestureComplete(false), m_idx(-1), m_bInfoMode(false)
    {
        AddChild(m_pUpgView1 = new iCreatUpgradeView(&gApp.ViewMgr(), this, iRect(), 100, iCreatGroup::UW_FIRST));
        AddChild(m_pUpgView2 = new iCreatUpgradeView(&gApp.ViewMgr(), this, iRect(), 101, iCreatGroup::UW_SECOND));
        AddChild(m_pArmyExchange = new iArmyExchange(&gApp.ViewMgr(), this, iRect(), 102, true, false
#ifndef PC_VERSION
                                                     , true, false, iSize(50, 35)
#endif
                                                     ));
        //AddChild(m_pExitBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 111, PDGG(BTN_CANCEL), -1));
        AddChild(m_pSplitBtn = new iCheckButton(&gApp.ViewMgr(), this, iRect(), PDGG(BTN_SPLIT_SMALL), -1, 103));
        m_pArmyExchange->SetSplitBtn(m_pSplitBtn);
        
        AddChild(m_pInfoBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 112, PDGG(BTN_INFO_DELUXE), -1));
        m_pInfoBtn->SetHoldable(true);
        m_pInfoBtn->SetVisible(false);
    }
    
    void iUpgradeView::OnActivate(bool bActivate)
    {
        if(bActivate) {
            FallDown();
            gTutorial.ShowFingerHelp();
        }
    }
    
    void iUpgradeView::UpdateSize()
    {
        iSize isz = m_pArmyExchange->GetDefSize(2);
        m_pArmyExchange->SetRect(AlignChild(isz, AlignBottom) + iPoint(0, isz.h / 4 - 5));
        
        m_pArmyExchange->SetPos(m_pArmyExchange->GetPos() + iPoint(0, 22));
        
        //	m_pArmyExchange->SetRect(AlignChild(m_pArmyExchange->GetDefSize(1), AlignBottom));
        m_pArmyExchange->UpdateSize();
        iRect tmprc = AlignChild(sm_defCentralSize, AlignTop);// + iPoint(0, 45);
        m_pUpgView1->SetRect(AlignRect(iSize(tmprc.w / 2, tmprc.h), tmprc, AlignLeft));
        m_pUpgView1->UpdateSize();
        m_pUpgView2->SetRect(AlignRect(iSize(tmprc.w / 2, tmprc.h), tmprc, AlignRight));
        m_pUpgView2->UpdateSize();
        
        m_pInfoBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignCenter));
        iRect arc = m_pArmyExchange->GetRect();
        m_pSplitBtn->SetRect(iRect(iPoint(arc.x2() - iIphoneBtn::sm_defSize.w + 8, arc.y + 50), iIphoneBtn::sm_defSize));
        
        //m_pExitBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottomRight));
    }
    
    void iUpgradeView::SetHero( iHero* pHero )
    {
        m_pHero = pHero;
        m_idx = -1;
        m_pArmyExchange->SetBottomOwner(pHero, true);
        m_pInfoBtn->SetVisible(false);
        m_pArmyExchange->SetVisible(true);
        m_pSplitBtn->SetVisible(true);
        m_pUpgView1->SetCreatureGroup(pHero, -1);
        m_pUpgView2->SetCreatureGroup(pHero, -1);
    }
    
    
    void iUpgradeView::OnCompose()
    {
        gApp.Surface().Fill(cColor_Black);
        iSize sz = gGfxMgr.Dimension(PDGG(HERO_DIALOG_BACK));
        gGfxMgr.Blit(PDGG(HERO_DIALOG_BACK), gApp.Surface(), AlignRect(sz, iRect(iPoint(), gApp.Surface().GetSize()), AlignCenter));
        
        iRect rc = GetScrRect();
        //rc.y += 30;
        //rc.y += ComposeChildScreenTitle(gApp.Surface(), rc, gTextMgr[TRID_UPGRADE_CREATURES]);
        
        //ComposeResourcesBarGlow(rc, m_pHero->GetArmyOwnerPlayer()->Minerals());
        
        iRect tmprc = AlignChild(sm_defCentralSize, AlignCenter) + m_Rect.point();
        if(m_pUpgView1->GetCT() != CREAT_UNKNOWN || m_pUpgView2->GetCT() != CREAT_UNKNOWN)
        {
            //ComposeGlowBkgndGrid(gApp.Surface(), tmprc, iSize(2, 1));
        } else {
            // ComposeGlowBkgnd(gApp.Surface(), tmprc);
            if(m_idx == -1) {
                iSize sz = gTextComposer.GetTextSize(gTextMgr[TRID_OBJ_WAR_ACADEMY_MSG], dlgfc_hdr);
                gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), gTextMgr[TRID_OBJ_WAR_ACADEMY_MSG], tmprc + iPoint(0, (tmprc.h - sz.h) / 2));
            } else {
                iSize sz = gTextComposer.GetTextSize(gTextMgr[TRID_OBJ_WAR_ACADEMY_NO_UPGRADE], dlgfc_hdr);
                gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), gTextMgr[TRID_OBJ_WAR_ACADEMY_NO_UPGRADE], tmprc + iPoint(0, (tmprc.h - sz.h) / 2));
            }
        }
        
    }
    
    void iUpgradeView::OnSwipeUp( sint32 length )
    {
#if defined(OS_IPHONE) || defined(OS_ANDROID)
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
                gGame.HideViewDelayed(CV_UPGRADE);
            }
            //m_bGestureComplete = true;
            //Invalidate();
        }
#endif
    }
    
    void iUpgradeView::OnMouseDown( const iPoint& pos, MouseId mID, MouseButtonId mbID )
    {
        m_bGestureComplete = false;
    }
    
    void iUpgradeView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        uint32 uid = pView->GetUID();
        if(uid == 100 || uid == 101) { // a creature was upgraded
            m_pUpgView1->SetCreatureGroup(m_pHero, -1);
            m_pUpgView2->SetCreatureGroup(m_pHero, -1);		
            m_pArmyExchange->Deselect();
            m_pInfoBtn->SetVisible(m_pUpgView1->GetCT() != CREAT_UNKNOWN || m_pUpgView2->GetCT() != CREAT_UNKNOWN);		
            m_idx = -1;
        } else if(uid == 102 && cmd == CCI_LBSELCHANGED) { // a creature group was selected in army list
            m_idx = param;
            m_pUpgView1->SetCreatureGroup(m_pHero, param);
            m_pUpgView2->SetCreatureGroup(m_pHero, param);
            m_pInfoBtn->SetVisible(m_pUpgView1->GetCT() != CREAT_UNKNOWN || m_pUpgView2->GetCT() != CREAT_UNKNOWN);		
        } else if(uid == 102 && cmd == CCI_BTNCLICK && param == iArmyExchange::BT_HERO) { // hero icon was clicked
            //gGame.ShowView(CV_HERO);
        } else if(uid == 111) {
            gGame.HideView(CV_UPGRADE);
        } else if(uid == 112) {
            if(cmd != CCI_HOLD && cmd != CCI_RELEASE) return;
            m_bInfoMode = (cmd == CCI_HOLD);
            m_pArmyExchange->SetVisible(!m_bInfoMode);
            m_pSplitBtn->SetVisible(m_pArmyExchange->IsVisible());
            iRect rc = m_pUpgView1->GetRect();
            rc.h = m_bInfoMode ? m_Rect.h : sm_defCentralSize.h;
            m_pUpgView1->SetRect(rc);
            
            rc = m_pUpgView2->GetRect();
            rc.h = m_bInfoMode ? m_Rect.h : sm_defCentralSize.h;
            m_pUpgView2->SetRect(rc);
            
            m_pUpgView1->SetInfoMode(m_bInfoMode);
            m_pUpgView2->SetInfoMode(m_bInfoMode);
        }
    }
    
    const iSize iUpgradeView::sm_defCentralSize = iSize(480, 240);
    
}
