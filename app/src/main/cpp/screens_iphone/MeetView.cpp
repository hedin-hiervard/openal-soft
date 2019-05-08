#include "stdafx.h"
#include "ArmyList.h"
#include "ArmyList.touch.h"
#include "ArtifactList.h"
#include "ArtifactList.touch.h"
#include "MeetView.h"
#include "TransitionView.h"
#include "HeroView.h"
#include "CastleView.h"
#include "ScreenSlider.h"
#include "CastleViewCommons.h"
#include "Dlg_MLInfo.h"

namespace UI_IPHONE
{
    
    iHeroInfoView::iHeroInfoView(iViewMgr* pViewMgr, iRect rc, uint32 uid)
    : iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled)
    {
        AddChild(m_pPrSkillsView = new iPrSkillsView(pViewMgr, this, iRect(), 100));
    }
    
    void iHeroInfoView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        uint32 uid = pView->GetUID();
        if(uid == 100) {
            if(param == 113) {
                // Experience button
                iIconDlg dlg(m_pMgr, iFormat(_T("%s %d"),gTextMgr[TRID_LEVEL], m_pHero->Level()), iStringT(gTextMgr[TRID_INFO_EXPERIENCE]) + _T("\n\n#FEE8") + iFormat(gTextMgr[TRID_MSG_EXP_TO_NEXT_LEVEL], m_pHero->LevelBound(m_pHero->Level()+1) - m_pHero->Experience()), PDGG(DGLYPH_EXP), m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            } else if(param >= 150 ) { // primary skill btn
                iIconDlg dlg(m_pMgr, gTextMgr[TRID_SKILL_ATTACK + (param - 150)], gTextMgr[TRID_INFO_ATTACK + (param - 150)], PDGG(DGLYPH_PRSKILLS) + ((param - 150)), m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            } else if(param == 101) {	// Morale button
                iMLInfoDlg dlg(m_pMgr, false, m_pHero->FurtSkill(FSK_MORALE), m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            } else if (param == 102) {   // Luck button
                iMLInfoDlg dlg(m_pMgr, true, m_pHero->FurtSkill(FSK_LUCK), m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            } else if (param == 103) {
                // Mana points button
                iIconDlg dlg(m_pMgr, gTextMgr[TRID_MANA], gTextMgr[TRID_INFO_MANA], PDGG(DGLYPH_MANA), m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            } else if (param == 104) {
                // Travel points button
                iIconDlg dlg(m_pMgr, gTextMgr[TRID_TRAVELPTS], gTextMgr[TRID_INFO_TRAVELPTS], PDGG(DGLYPH_MOVES), m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            }
        }
    }
    
    void iHeroInfoView::OnCompose()
    {
        iRect rc = GetScrRect();
        iRect avrc = AlignRect(iSize(48, 48), m_pPrSkillsView->GetScrRect(), AlignLeft);
        avrc -= iPoint(48, 0);
        ComposeHeroAvatar(gApp.Surface(), avrc, m_pHero);
        rc.y += 5;
        ComposeHeroTitleName(gApp.Surface(), rc, m_pHero);
    }
    
    void iHeroInfoView::UpdateSize()
    {
        m_pPrSkillsView->SetRect(AlignChild(iPrSkillsView::sm_defSize, AlignBottom) + iPoint(50, 0));
        m_pPrSkillsView->UpdateSize();
    }
    
    void iHeroInfoView::SetHero(iHero* pHero)
    {
        m_pHero = pHero;
        m_pPrSkillsView->SetHero(pHero);
    }
    
    iHeroArtView::iHeroArtView(iSimpleArray<iArtClickItem*>& competitors, iViewMgr* pViewMgr, iRect rc, uint32 uid)
    : iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled)
    {
        AddChild(m_pArtBag = new iArtifactBag(competitors, pViewMgr, 100));
    }
    
    void iHeroArtView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        
    }
    
    void iHeroArtView::OnCompose()
    {
        iRect rc = GetScrRect();
        iRect trc = iRect(rc.x + rc.w / 8, rc.y + 7, rc.w * 3 / 4, m_pArtBag->GetScrRect().y - rc.y);
        ComposeHeroAvatarName(gApp.Surface(), trc, m_pHero, false, false, false);
    }
    
    void iHeroArtView::SetHero(iHero* pHero)
    {
        m_pHero = pHero;
        m_pArtBag->SetHero(pHero);
    }
    
    void iHeroArtView::UpdateSize()
    {
        m_pArtBag->SetRect(AlignChild(iArtifactBag::sm_defSize, AlignBottom));
        m_pArtBag->UpdateSize();
    }
    
    
    iMeetView::iMeetView() : IMeetView(false, CV_OVERLAND),
    m_pHero1(NULL), m_pHero2(NULL)
    {
        AddChild(m_pScreenSlider = new iScreenSlider(&gApp.ViewMgr(), this, iRect(), 99));
        
        ARMY_SLIDE = m_pScreenSlider->AddSlide();
        ARTIFACTS_SLIDE = m_pScreenSlider->AddSlide();
        
        iSlide* pSlide = m_pScreenSlider->GetSlide(ARMY_SLIDE);
        pSlide->AddChild(m_pHeroInfoView1 = new iHeroInfoView(&gApp.ViewMgr(), iRect(), 100));
        pSlide->AddChild(m_pHeroInfoView2 = new iHeroInfoView(&gApp.ViewMgr(), iRect(), 101));
        pSlide->AddChild(m_pArmyExchange = new iArmyExchange(&gApp.ViewMgr(), this, iRect(), 102, true, true
#ifndef PC_VERSION
                                                             , false, false, iSize(50, 35)
#endif
                                                             ));
        pSlide->AddChild(m_pExitBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 111, PDGG(BTN_CANCEL_DELUXE), PDGG(BTN_DELUXE_PRESSED)));
        pSlide->AddChild(m_pSplitBtn = new iCheckButton(&gApp.ViewMgr(), this, iRect(), PDGG(BTN_SPLIT_SMALL), -1, 200));
        m_pArmyExchange->SetSplitBtn(m_pSplitBtn);
        
        
        pSlide = m_pScreenSlider->GetSlide(ARTIFACTS_SLIDE);
        pSlide->AddChild(m_pHeroArtView1 = new iHeroArtView(m_artCompetitors, &gApp.ViewMgr(), iRect(), 103));
        pSlide->AddChild(m_pHeroArtView2 = new iHeroArtView(m_artCompetitors, &gApp.ViewMgr(), iRect(), 104));
    }
    
    void iMeetView::OnActivate(bool bActivate)
    {
        if(bActivate) {
            //gTutorial.Trigger(TUT_HERO_MEETING);
            
            m_pScreenSlider->SetVisible(false);
            m_pScreenSlider->SetCurrentSlide(ARMY_SLIDE);
            FallDown();
            gTutorial.ShowFingerHelp();
            m_pScreenSlider->SetVisible(true);
        }
    }
    
    void iMeetView::UpdateSize()
    {
        iRect rc = GetScrRect();
        m_pScreenSlider->SetRect(rc);
        m_pScreenSlider->UpdateSize();
        
        iSlide* pSlide = m_pScreenSlider->GetSlide(ARMY_SLIDE);
        m_pExitBtn->SetRect(pSlide->AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignTopRight));
        
        iSize sz = m_pArmyExchange->GetDefSize(2);
        m_pArmyExchange->SetRect(AlignChild(sz, AlignBottom) + iPoint(0, sz.h / 4 - 5));
        m_pArmyExchange->UpdateSize();
        
        iRect arc = m_pArmyExchange->GetRect();
        m_pSplitBtn->SetRect(iRect(iPoint(arc.x2() - iIphoneBtn::sm_defSize.w + 8, arc.y + 29), iIphoneBtn::sm_defSize));
        
        iRect trc = iRect(0, 0, m_Rect.w, m_pArmyExchange->GetPos().y / 2);
        m_pHeroInfoView1->SetRect(trc);
        m_pHeroInfoView1->UpdateSize();
        trc.y += trc.h;
        m_pHeroInfoView2->SetRect(trc);
        m_pHeroInfoView2->UpdateSize();
        
        
        trc = iRect(0, 0, rc.w / 2, rc.h);
        m_pHeroArtView1->SetRect(trc);
        m_pHeroArtView1->UpdateSize();
        trc.x += trc.w;
        m_pHeroArtView2->SetRect(trc);
        m_pHeroArtView2->UpdateSize();
    }
    
    void iMeetView::SetHeroes( iHero* pHero1, iHero* pHero2 )
    {
        m_pHero1 = pHero1;
        m_pHero2 = pHero2;
        
        m_pHeroInfoView1->SetHero(pHero1);
        m_pHeroInfoView2->SetHero(pHero2);
        
        m_pHeroArtView1->SetHero(pHero1);
        m_pHeroArtView2->SetHero(pHero2);
        
        m_pArmyExchange->SetTopOwner(pHero1, true);
        m_pArmyExchange->SetBottomOwner(pHero2, true);
    }
    
    void iMeetView::OnCompose()
    {
        gApp.Surface().Fill(cColor_Black);
        iSize sz = gGfxMgr.Dimension(PDGG(HERO_DIALOG_BACK));
        gGfxMgr.Blit(PDGG(HERO_DIALOG_BACK), gApp.Surface(), AlignRect(sz, GetScrRect(), AlignCenter));
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
        } else if(uid == 99) {
            if(param == ARMY_SLIDE)
                gTutorial.Trigger(TUT_HERO_MEETING_ARMY);
            else if(param == ARTIFACTS_SLIDE)
                gTutorial.Trigger(TUT_HERO_MEETING_ARTIFACTS);
        }
    }
    
}
