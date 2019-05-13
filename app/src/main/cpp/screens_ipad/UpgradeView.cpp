#include "stdafx.h"
#include "UpgradeView.h"
#include "TransitionView.h"
#include "CastleView.h"
#include "Dlg_CreatInfo.h"

namespace UI_IPAD
{
    
    iCreatUpgradeView::iCreatUpgradeView( iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rc, uint32 uid, iCreatGroup::UpgradeWay way):
    iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled), m_way(way), m_pHandler(pCmdHandler), m_idx(-1)
    {
        AddChild(m_pBtn = new iImgBtn(pViewMgr, this, iRect(), 100, PDGG_IPAD(BTN_CASTLE_RECRUIT_MAX), PDGG_IPAD(BTN_CASTLE_RECRUIT_MAX_PRESSED)));
        AddChild(m_pDescView = new iCreatDescView(pViewMgr, this, iRect(), 101, CDV_GLOW | CDV_STATS | CDV_COST | CDV_TITLE | CDV_NOTITLE_PERKS, NULL));
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
        
        m_pBtn->SetHighlighted(false);
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
    
    void iCreatUpgradeView::UpdateSize()
    {
        m_pBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottom) - iPoint(0, 15));
        m_pDescView->SetRect(AlignChild(iSize(m_Rect.w, m_pBtn->GetPos().y - 15), AlignTop, 15));
    }
    
    iUpgradeView::iUpgradeView(): IUpgradeView(false, CV_OVERLAND),
    m_pHero(NULL), m_bGestureComplete(false), m_idx(-1)
    {
        AddChild(m_pUpgView1 = new iCreatUpgradeView(&gApp.ViewMgr(), this, iRect(), 100, iCreatGroup::UW_FIRST));
        AddChild(m_pUpgView2 = new iCreatUpgradeView(&gApp.ViewMgr(), this, iRect(), 101, iCreatGroup::UW_SECOND));
        AddChild(m_pArmyExchange = new iArmyExchange(&gApp.ViewMgr(), this, iRect(), 102, true, true, true));
        AddChild(m_pExitBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 111, PDGG_IPAD(BTN_CANCEL), -1));
    }
    
    void iUpgradeView::OnActivate(bool bActivate)
    {
        if(bActivate){
            
            bFirstDarken = true;
            FallDown();
        }
    }
    
    void iUpgradeView::UpdateSize()
    {
        m_pArmyExchange->SetRect(AlignChild(m_pArmyExchange->GetDefSize(1), AlignBottom));
        m_pArmyExchange->UpdateSize();
        iRect tmprc = AlignChild(sm_defCentralSize, AlignCenter) + iPoint(0, 45);
        m_pUpgView1->SetRect(AlignRect(iSize(tmprc.w / 2, tmprc.h), tmprc, AlignLeft));
        m_pUpgView1->UpdateSize();
        m_pUpgView2->SetRect(AlignRect(iSize(tmprc.w / 2, tmprc.h), tmprc, AlignRight));
        m_pUpgView2->UpdateSize();
        
        m_pExitBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottomRight));
    }
    
    void iUpgradeView::SetHero( iHero* pHero )
    {
        m_pHero = pHero;
        m_pArmyExchange->SetBottomOwner(pHero, true);
        m_pUpgView1->SetCreatureGroup(pHero, -1);
        m_pUpgView2->SetCreatureGroup(pHero, -1);
    }
    
    
    void iUpgradeView::DoCompose()
    {
        gGfxMgr.Blit(PDGG_IPAD(HERO_DIALOG_BACK), gApp.Surface(), m_Rect.point());
        iRect rc = GetScrRect();
        rc.y += 30;
        rc.y += ComposeChildScreenTitle(gApp.Surface(), rc, gTextMgr[TRID_UPGRADE_CREATURES]);
        
        ComposeResourcesBarGlow(rc, m_pHero->GetArmyOwnerPlayer()->Minerals());
        
        iRect tmprc = AlignChild(sm_defCentralSize, AlignCenter) + iPoint(0, 45) + m_Rect.point();
        if(m_pUpgView1->GetCT() != CREAT_UNKNOWN || m_pUpgView2->GetCT() != CREAT_UNKNOWN)
        {
            ComposeGlowBkgndGrid(gApp.Surface(), tmprc, iSize(2, 1));
        } else {
            ComposeGlowBkgnd(gApp.Surface(), tmprc);
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
            m_idx = -1;
        } else if(uid == 102 && cmd == CCI_LBSELCHANGED) { // a creature group was selected in army list
            m_idx = param;
            m_pUpgView1->SetCreatureGroup(m_pHero, param);
            m_pUpgView2->SetCreatureGroup(m_pHero, param);
        } else if(uid == 102 && cmd == CCI_BTNCLICK && param == iArmyExchange::BT_HERO) { // hero icon was clicked
            //gGame.ShowView(CV_HERO);
        } else if(uid == 111) {
            gGame.HideView(CV_UPGRADE);
        }
    }
    
    const iSize iUpgradeView::sm_defCentralSize = iSize(900, 550);
    
}