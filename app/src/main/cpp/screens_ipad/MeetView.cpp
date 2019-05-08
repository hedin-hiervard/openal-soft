#include "stdafx.h"
#include "ArmyList.h"
#include "ArmyList.touch.h"
#include "ArtifactList.h"
#include "ArtifactList.touch.h"
#include "MeetView.h"
#include "TransitionView.h"
#include "HeroView.h"
#include "CastleView.h"

namespace UI_IPAD
{
    
    iHeroChildView::iHeroChildView( iViewMgr* pViewMgr,  IViewCmdHandler* pHandler,  const iRect& rc, uint32 uid, iSimpleArray<iArtClickItem*>& m_artCompetitors ):
    iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pHandler(pHandler)
    {
        AddChild(m_pArtBag = new iArtifactBag(m_artCompetitors, pViewMgr, 100));
        AddChild(m_pArmyExchange = new iArmyExchange(pViewMgr, this, iRect(), 101, false, false));
        AddChild(m_pPrSkillsView = new iPrSkillsView(pViewMgr, this, iRect(), 102));
    }
    
    void iHeroChildView::UpdateSize()
    {
        m_pArmyExchange->SetRect(AlignChild(m_pArmyExchange->GetDefSize(1), AlignBottom) + iPoint(0, 8));
        m_pArmyExchange->UpdateSize();
        m_pArtBag->SetRect(AlignChild(m_pArtBag->sm_defSize, AlignBottom) - iPoint(0, m_Rect.h - m_pArmyExchange->GetPos().y));
        m_pArtBag->UpdateSize();
        m_pPrSkillsView->SetRect(AlignChild(m_pPrSkillsView->sm_defSize, AlignBottom) - iPoint(0, m_Rect.h - m_pArtBag->GetPos().y + 5));
        m_pPrSkillsView->UpdateSize();
    }
    
    
    void iHeroChildView::OnCompose()
    {
        iRect rc = GetScrRect();
        rc.y += 15;
        rc.y += ComposeHeroTitleName(gApp.Surface(), rc, m_pHero);
        rc.y += ComposeHeroAvatarLevel(gApp.Surface(), rc, m_pHero);
    }
    
    void iHeroChildView::SetHero( iHero *pHero )
    {
        m_pHero = pHero;
        m_pArmyExchange->SetBottomOwner(pHero, true);
        m_pArtBag->SetHero(pHero);
        m_pPrSkillsView->SetHero(pHero);
        Invalidate();
    }
    
    void iHeroChildView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        uint32 uid = pView->GetUID();
        if(uid == 101 && cmd == CCI_CHECKED) {
            m_pHandler->iCMDH_ControlCommand(this, CCI_CHECKED, param);
        }
    }
    
    void iHeroChildView::AddArmyCompetitor( iArmyListEx* comp )
    {
        m_pArmyExchange->AddCompetitor(comp);
    }
    
    void iHeroChildView::SetSplitChecked( sint32 checked )
    {
        m_pArmyExchange->SetSplitChecked(checked);
    }
    
    void iHeroChildView::SetSplitButton( iCheckButton * pSplitBtn )
    {
        m_pArmyExchange->SetSplitBtn(pSplitBtn);
    }
    
    iMeetView::iMeetView() : IMeetView(false, CV_OVERLAND), m_pHero1(NULL), m_pHero2(NULL)
    {
        AddChild(m_pChildView1 = new iHeroChildView(&gApp.ViewMgr(), this, iRect(), 100, m_artCompetitors));
        AddChild(m_pChildView2 = new iHeroChildView(&gApp.ViewMgr(), this, iRect(), 101, m_artCompetitors));
        m_pChildView1->AddArmyCompetitor(m_pChildView2->ArmyExchange().BottomArmyList());
        m_pChildView2->AddArmyCompetitor(m_pChildView1->ArmyExchange().BottomArmyList());
        AddChild(m_pExitBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 111, PDGG_IPAD(BTN_CANCEL), -1, gTextMgr[TRID_HOVER_END_DLG], Visible|Enabled, AlignBottomRight));
        AddChild(m_pSplitBtn = new iCheckButton(&gApp.ViewMgr(), this, iRect(), PDGG_IPAD(BTN_SPLIT_SMALL), -1, 112));
        m_pChildView1->SetSplitButton(m_pSplitBtn);
        m_pChildView2->SetSplitButton(m_pSplitBtn);
    }
    
    void iMeetView::OnActivate(bool bActivate)
    {
        if(bActivate){
            
            bFirstDarken = true;
            FallDown();
        }
    }
    
    void iMeetView::UpdateSize()
    {
        iSize sz = iSize(m_Rect.w / 2, m_Rect.h);
        m_pChildView1->SetRect(AlignChild(sz, AlignLeft));
        m_pChildView1->UpdateSize();
        m_pChildView2->SetRect(AlignChild(sz, AlignRight));
        m_pChildView2->UpdateSize();
        m_pExitBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottomRight) - iPoint(0, 4));
        
        m_pSplitBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottom));
    }
    
    void iMeetView::SetHeroes( iHero* pHero1, iHero* pHero2 )
    {
        m_pChildView1->SetHero(pHero1);
        m_pChildView2->SetHero(pHero2);
    }
    
    void iMeetView::DoCompose()
    {
        iRect rc = GetScrRect();
        gGfxMgr.Blit(PDGG_IPAD(HERO_DIALOG_BACK), gApp.Surface(), rc);
    }
    
    void iMeetView::OnSwipeUp( sint32 length )
    {
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
                gGame.HideViewDelayed(CV_MEET);
            }		
        }
        
    }
    
    void iMeetView::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) {
        if(mID == 0) m_bGestureComplete = false;
    }
    
    void iMeetView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        uint32 uid = pView->GetUID();
        if(uid == 111) {
            gGame.HideView(CV_MEET);
        } else if(cmd == CCI_CHECKED) {
            m_pChildView1->SetSplitChecked(param);
            m_pChildView2->SetSplitChecked(param);
        }
    }
    
}
