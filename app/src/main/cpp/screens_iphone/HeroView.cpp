#include "stdafx.h"
#include "ArtifactList.h"
#include "ArtifactList.touch.h"

#include "HeroView.h"
#include "transitionview.h"
#include "dlg_spellbook.h"
#include "CastleView.h"
#include "Dlg_MLInfo.h"
#include "Dlg_SkillInfo.h"
#include "Dlg_ArtInfo.h"
#include "Dlg_PuzzleMap.touch.h"
#include "Dlg_SpellInfo.h"

#include "CastleViewCommons.h"
#include "ScreenSlider.h"

void ComposeArtifactCell(const iRect& rect, uint16 artId, uint32 flags);

namespace UI_IPHONE
{
    
    iExpLuckMoraleView::iExpLuckMoraleView(iViewMgr *pViewMgr, IViewCmdHandler* pCmdHandler, iRect rc, uint32 uid):
    iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pHero(NULL), m_pCmdHandler(pCmdHandler)
    {
        //AddChild(m_pSkillTabBtns[0] = new iExpBtn(&gApp.ViewMgr(), this, iRect(3,3,53,36), 120));
        AddChild(m_pSkillTabBtns[1] = new iManaBtn(&gApp.ViewMgr(), this, iRect(), 121));
        AddChild(m_pSkillTabBtns[2] = new iActPtsBtn(&gApp.ViewMgr(), this, iRect(), 122));
        AddChild(m_pSkillTabBtns[3] = new iMoraleBtn(&gApp.ViewMgr(), this, iRect(), 123));
        AddChild(m_pSkillTabBtns[4] = new iLuckBtn(&gApp.ViewMgr(), this, iRect(), 124));
    }
    
    void iExpLuckMoraleView::UpdateSize()
    {
        iRect crc(iPoint(0, (m_Rect.h - 36) / 2), iSize(53, 36));
        if (m_pHero) {
            for (uint32 xx=1; xx<5; ++xx)
            {
                m_pSkillTabBtns[xx]->SetRect(crc);
                crc.x += m_Rect.w / 4;
            }
        }
    }
    
    void iExpLuckMoraleView::SetHero(iHero *pHero)
    {
        m_pHero = pHero;
        if (m_pHero) {
            uint32 xx;
            for (xx=1; xx<5; ++xx) m_pSkillTabBtns[xx]->SetHero(m_pHero);
        }
    }
    
    void iExpLuckMoraleView::OnCompose() {
        iRect rc = GetScrRect();
        ComposeDlgBkgndPanel(gApp.Surface(), rc);
    }
    void iExpLuckMoraleView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        uint32 uid = pView->GetUID();
        if (uid == 120) {
            // Experience button
            iIconDlg dlg(m_pMgr, iFormat(_T("%s %d"),gTextMgr[TRID_LEVEL], m_pHero->Level()), iStringT(gTextMgr[TRID_INFO_EXPERIENCE]) + _T("\n\n#FEE8") + iFormat(gTextMgr[TRID_MSG_EXP_TO_NEXT_LEVEL], m_pHero->LevelBound(m_pHero->Level()+1) - m_pHero->Experience()), PDGG_IPHONE(DGLYPH_EXP), m_pHero->Owner()->PlayerId());
            dlg.DoModal();
        } else if (uid == 121) {
            // Mana points button
            iIconDlg dlg(m_pMgr, gTextMgr[TRID_MANA], gTextMgr[TRID_INFO_MANA], PDGG_IPHONE(DGLYPH_MANA), m_pHero->Owner()->PlayerId());
            dlg.DoModal();
        } else if (uid == 122) {
            // Travel points button
            iIconDlg dlg(m_pMgr, gTextMgr[TRID_TRAVELPTS], gTextMgr[TRID_INFO_TRAVELPTS], PDGG_IPHONE(DGLYPH_MOVES), m_pHero->Owner()->PlayerId());
            dlg.DoModal();
        } else if (uid == 123) {
            // Morale button
            iMLInfoDlg dlg(m_pMgr, false, m_pHero->FurtSkill(FSK_MORALE), m_pHero->Owner()->PlayerId());
            dlg.DoModal();
        } else if (uid == 124) {
            // Luck button
            iMLInfoDlg dlg(m_pMgr, true, m_pHero->FurtSkill(FSK_LUCK), m_pHero->Owner()->PlayerId());
            dlg.DoModal();
        }
    }
    
    const iSize iExpLuckMoraleView::sm_defSize = iSize(265, 55);
    
    
    //////////////////////////////////////////////////////////////////////////
    iHeroTopView::iHeroTopView(iViewMgr* pViewMgr, const iRect& rect, uint32 uid)
    : iView(pViewMgr, iRect(), GENERIC_VIEWPORT, uid, Visible|Enabled)
    , m_pHero(NULL)
    {
        
    }
    
    void iHeroTopView::OnCompose()
    {
        if(m_pHero) {
            iRect rc = GetScrRect();
            //	gApp.Surface().FrameRect(rc, cColor_White);
            ComposeHeroAvatarTitleName(gApp.Surface(), rc, m_pHero, false, false, false);
        }
    }
    
    void iHeroTopView::SetHero(iHero* pHero)
    {
        m_pHero = pHero;
    }
    
    const iSize iHeroTopView::sm_defSize = iSize(480, 54);
    
    
    iSkillsTabBtn::iSkillsTabBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
    : iButton(pViewMgr, pCmdHandler, rect, uid, Visible|Enabled), m_pHero(NULL)
    {}
    
    inline void	iSkillsTabBtn::SetHero(const iHero* pHero)
    {
        m_pHero = pHero;
        Invalidate();
    }
    
    inline const iHero* iSkillsTabBtn::Hero() const { return m_pHero; }
    
    inline void iSecSkillBtn::SetSkill(const iSecSkillEntry* pSkill) { m_pSkill = pSkill; Invalidate(); }
    
    iSecSkillBtn::iSecSkillBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
    : iButton(pViewMgr, pCmdHandler, rect, uid, Visible|Enabled), m_pSkill(NULL) {}
    
    inline const iSecSkillEntry* iSecSkillBtn::Skill() const { return m_pSkill; }
    
    void iSecSkillBtn::OnCompose()
    {
        iRect rc = GetScrRect();
        //gApp.Surface().FillRect(rc, gGame.Map().Rand(255));
        if (m_pSkill){
            /*if(m_state & Pressed)
             gApp.Surface().FillRect(rc, cColor_White, 128);
             */
            iSize siz = gGfxMgr.Dimension(PDGG_IPHONE(SEC_SKILLS)+m_pSkill->m_skill);
            iRect trc = AlignRect(siz, rc, AlignLeft);
            BlitIcon(gApp.Surface(), PDGG_IPHONE(SEC_SKILLS)+m_pSkill->m_skill, trc, 255, iPoint(), m_state & Pressed);
            trc.w = rc.w - trc.w;
            trc.x += siz.w;
            
            iStringT txt = gTextMgr[TRID_HSKILL_ESTATES+m_pSkill->m_skill];
            
            gTextComposer.TextCut(txt, trc.w, secskillnamefc);
            
            trc.y += gTextComposer.TextBoxOut(secskillnamefc, gApp.Surface(), txt, trc);
            trc.y += gTextComposer.TextBoxOut(secskilllevelfc, gApp.Surface(),gTextMgr[TRID_HSKILL_LEVEL_BASIC+m_pSkill->m_level], trc);
        }
    }
    
    
    iExpBtn::iExpBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid):
    iSkillsTabBtn(pViewMgr, pCmdHandler, rect, uid)
    {}
    
    void iExpBtn::OnCompose()
    {
        iRect rc = GetScrRect();
        if(m_pHero)
        {
            rc.y += ComposeHeroLevel(gApp.Surface(), rc, m_pHero, (m_state & Pressed));
        }
    }
    
    iManaBtn::iManaBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) :
    iSkillsTabBtn(pViewMgr, pCmdHandler, rect, uid)
    {}
    
    void iManaBtn::OnCompose()
    {
        iRect rc = GetScrRect();
        ButtonFrame(gApp.Surface(),rc,GetButtonState());
        if (m_pHero) {
            sint16 sid = PDGG_IPHONE(ICN_MANA);
            iSize siz = gGfxMgr.Dimension(sid);
            sint32 opx = rc.x + (sint32)(rc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(sid).x;
            sint32 opy = rc.y + (sint32)(rc.h/2) - (sint32)(siz.h/2) - gGfxMgr.Anchor(sid).x - 9;
            gGfxMgr.BlitEffect(sid, gApp.Surface(), iPoint(opx,opy),  (m_state & Pressed) ? iGfxManager::EfxGlow : iGfxManager::EfxNone);
            iRect trc(rc.x+(sint32)(rc.w/2)-23,rc.y+rc.h-13, 46, 13);
            DrawRoundRect(gApp.Surface(), trc, RGB16(192,128,32), cColor_Black);
            gTextComposer.TextOut(herolevelfc, gApp.Surface(), iPoint(), iFormat(_T("%d/%d"),m_pHero->ManaPts(),m_pHero->MaxManaPts()), trc, AlignCenter);
        }
    }
    
    iActPtsBtn::iActPtsBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) :
    iSkillsTabBtn(pViewMgr, pCmdHandler, rect, uid)
    {}
    
    void iActPtsBtn::OnCompose()
    {
        iRect rc = GetScrRect();
        ButtonFrame(gApp.Surface(),rc,GetButtonState());
        
        if (m_pHero) {
            iSize siz = gGfxMgr.Dimension(PDGG_IPHONE(DGLYPH_MOVES));
            sint32 opx = rc.x + (sint32)(rc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(PDGG_IPHONE(DGLYPH_MOVES)).x;
            sint32 opy = rc.y + (sint32)(rc.h/2) - (sint32)(siz.h/2) - gGfxMgr.Anchor(PDGG_IPHONE(DGLYPH_MOVES)).y - 1;
            gGfxMgr.BlitEffect(PDGG_IPHONE(DGLYPH_MOVES), gApp.Surface(), iPoint(opx,opy),  (m_state & Pressed) ? iGfxManager::EfxGlow : iGfxManager::EfxNone);
            iRect trc(rc.x+(sint32)(rc.w/2)-20,rc.y+rc.h-13, 40,13);
            DrawRoundRect(gApp.Surface(), trc, RGB16(192,128,32), cColor_Black);
            gTextComposer.TextOut(herolevelfc, gApp.Surface(), iPoint(), iFormat(_T("%d/%d"),m_pHero->Moves(),m_pHero->TotalMoves()), trc, AlignCenter);
        }
    }
    
    iMoraleBtn::iMoraleBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) :
    iSkillsTabBtn(pViewMgr, pCmdHandler, rect, uid)
    {}
    
    void iMoraleBtn::OnCompose()
    {
        iRect rc = GetScrRect();
        
        ButtonFrame(gApp.Surface(),rc,GetButtonState());
        
        if (m_pHero) {
            iRect irc = rc;
            irc.y -= 8;
            ComposeMoraleGlyph(gApp.Surface(), irc, (sint8)m_pHero->FurtSkill(FSK_MORALE), m_state & Pressed, false);
            iRect trc(rc.x+(sint32)(rc.w/2)-15,rc.y+rc.h-13, 30, 13);
            DrawRoundRect(gApp.Surface(), trc, RGB16(192,128,32), cColor_Black);
            gTextComposer.TextOut(herolevelfc, gApp.Surface(), iPoint(), iFormat(_T("%d"), (sint8)m_pHero->FurtSkill(FSK_MORALE)), trc, AlignCenter);
        }
    }
    
    
    iLuckBtn::iLuckBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) : iSkillsTabBtn(pViewMgr, pCmdHandler, rect, uid){}
    
    void iLuckBtn::OnCompose()
    {
        iRect rc = GetScrRect();
        ButtonFrame(gApp.Surface(),rc,GetButtonState());
        
        if (m_pHero) {
            iRect irc = rc;
            irc.y -= 8;
            ComposeLuckGlyph(gApp.Surface(), irc, (sint8)m_pHero->FurtSkill(FSK_LUCK), m_state & Pressed, false);
            
            iRect trc(rc.x+(sint32)(rc.w/2)-15,rc.y+rc.h-13, 30, 13);
            DrawRoundRect(gApp.Surface(), trc, RGB16(192,128,32), cColor_Black);
            gTextComposer.TextOut(herolevelfc, gApp.Surface(), iPoint(), iFormat(_T("%d"), (sint8)m_pHero->FurtSkill(FSK_LUCK)), trc, AlignCenter);
        }
        //gApp.Surface().FrameRect(rc, cColor_Red);
    }
    
    
    iSecSkillsView::iSecSkillsView(iViewMgr *pViewMgr, IViewCmdHandler* pCmdHandler, iPoint pnt, uint32 uid):
    iView(pViewMgr, iRect(pnt, iSize(200, 29*8 + 3)), GENERIC_VIEWPORT, uid, Visible|Enabled), m_pHero(NULL), m_pCmdHandler(pCmdHandler)
    {
        for (uint32 xx=0; xx<8; ++xx) {
            AddChild(m_pBtnSecSkill[xx] = new iSecSkillBtn(&gApp.ViewMgr(),this, iRect(),110 + xx));
        }
    }
    
    void iSecSkillsView::UpdateSize()
    {
        iPoint off(10, 10);
        iRect crc(off.x, off.y, (m_Rect.w - off.x * 2) / 2, (m_Rect.h - off.y * 2) / 4);
        for (uint32 xx=0; xx<8; ++xx) {
            m_pBtnSecSkill[xx]->SetRect(crc);
            if(xx % 2 == 0)
                crc.x = off.x + crc.w;
            else {
                crc.x = off.x;
                crc.y += crc.h;
            }
        }
    }
    
    void iSecSkillsView::SetHero(iHero *pHero)
    {
        m_pHero = pHero;
        if (m_pHero) {
            uint32 xx;
            for (xx=0; xx<m_pHero->SecSkills().Count(); ++xx) m_pBtnSecSkill[xx]->SetSkill(&m_pHero->SecSkills().At(xx));
            for (uint32 zz=xx; zz<8; ++zz) m_pBtnSecSkill[zz]->SetSkill(NULL);
        }
    }
    
    void iSecSkillsView::OnCompose() {
        iRect rc = GetScrRect();
        //ComposeGlowBkgnd(gApp.Surface(), rc);
    }
    void iSecSkillsView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        uint32 uid = pView->GetUID();
        
        if (uid >= 110 && uid < 118) {
            if (const iSecSkillEntry* pSkill = m_pBtnSecSkill[uid - 110]->Skill()) {
                iSecSkillInfoDlg dlg(m_pMgr, (SECONDARY_SKILLS)pSkill->m_skill, (SECSKILL_LEVEL)pSkill->m_level, m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            }
        }
    }
    
    iSize iSecSkillsView::sm_defSize = iSize(310, 155);
    
    
    iPrSkillBtn::iPrSkillBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, FURTHER_SKILLS skill)
    : iButton(pViewMgr, pCmdHandler, rect, uid, Visible|Enabled), m_skill(skill), m_pHero(NULL) {}
    
    void iPrSkillBtn::OnCompose()
    {
        iRect rc = GetScrRect();
        //gApp.Surface().FrameRect(rc, cColor_Black);
        if (!m_pHero) return;
        SpriteId sid; //= PDGG_IPHONE(DGLYPH_PRSKILLS) + (m_skill-FSK_ATTACK);
        switch(m_skill) {
            case FSK_ATTACK: sid = PDGG_IPHONE(BTN_ATTACK); break;
            case FSK_DEFENCE: sid = PDGG_IPHONE(BTN_HERO); break;
            case FSK_POWER: sid = PDGG_IPHONE(TAB_MGUILD); break;
            case FSK_KNOWLEDGE: sid = PDGG_IPHONE(TAB_SPELLBOOK); break;
        }
        iSize siz = gGfxMgr.Dimension(sid);
        sint32 opx = rc.x + (sint32)(rc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(sid).x;
        sint32 opy = rc.y - gGfxMgr.Anchor(sid).y + 1;
        gGfxMgr.BlitEffect(sid, gApp.Surface(), iPoint(opx,opy), (m_state & Pressed) ? iGfxManager::EfxGlow : iGfxManager::EfxNone);
        
        iRect trc(rc.x+(sint32)(rc.w/2)-14, rc.y+rc.h-13, 28, 13);
        DrawRoundRect(gApp.Surface(), trc, RGB16(192,128,32), cColor_Black);
        
        gTextComposer.TextOut(herolevelfc, gApp.Surface(), iPoint(), FormatNumber(m_pHero->FurtSkill(m_skill)), trc, AlignCenter);
    }
    
    void iPrSkillBtn::SetHero(iHero* pHero)
    {
        m_pHero = pHero;
        Invalidate();
    }
    
    
    iPrSkillsView::iPrSkillsView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
    : iView(pViewMgr,rect,GENERIC_VIEWPORT, uid,Visible|Enabled), m_pHero(NULL), m_pHandler(pCmdHandler)
    {
        uint32 xx = 0;
        AddChild(m_pBtns[xx++] = new iExpBtn(pViewMgr, this, iRect(), 113));
        for (; xx<5; ++xx) {
            AddChild(m_pBtns[xx] = new iPrSkillBtn(pViewMgr, this, iRect(), 150 + xx-1, (FURTHER_SKILLS)(FSK_ATTACK+xx-1)));
        }
        AddChild(m_pBtns[xx++] = new iManaBtn(pViewMgr, this, iRect(), 103));
        AddChild(m_pBtns[xx++] = new iMoraleBtn(pViewMgr, this, iRect(), 101));
        AddChild(m_pBtns[xx++] = new iLuckBtn(pViewMgr, this, iRect(), 102));
    }
    
    void iPrSkillsView::UpdateSize()
    {
        iRect crc(iPoint(m_Rect.w / 16, m_Rect.h / 4), iSize(m_Rect.w / 9, m_Rect.h * 2 / 3));
        for (uint32 xx =0; xx<8; ++xx) {
            m_pBtns[xx]->SetRect(crc);
            crc.x += crc.w;
        }
    }
    
    void iPrSkillsView::OnCompose()
    {
        iRect rc = GetScrRect();
        //gApp.Surface().FrameRect(rc, cColor_Red);
        //ComposeGlowBkgnd(gApp.Surface(), rc);
    }
    
    
    
    void iPrSkillsView::SetHero(iHero* pHero)
    {
        m_pHero = pHero;
        ((iExpBtn*)m_pBtns[0])->SetHero(pHero);
        for (uint32 xx=1; xx<=4; ++xx) {
            ((iPrSkillBtn*)m_pBtns[xx])->SetHero(pHero);
        }
        ((iManaBtn*)m_pBtns[5])->SetHero(pHero);
        ((iMoraleBtn*)m_pBtns[6])->SetHero(pHero);
        ((iLuckBtn*)m_pBtns[7])->SetHero(pHero);
        Invalidate();
    }
    
    void iPrSkillsView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        m_pHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, pView->GetUID());
    }
    
    const iSize iPrSkillsView::sm_defSize = iSize(305, 59);
    
    
    
    iKeysView::iKeysView( iViewMgr *pViewMgr, const iRect& rc, uint32 uid ): iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pHero(NULL) {}
    
    void iKeysView::SetHero( iHero *pHero )
    {
        m_pHero = pHero;
        Invalidate();
    }
    
    void iKeysView::OnCompose()
    {
        iRect rc = GetScrRect();
        ComposeGlowBkgnd(gApp.Surface(), rc);
        if(!m_pHero) return;
        iRect crc = AlignRect(gGfxMgr.Dimension(PDGG_IPHONE(NEWHERO_KEYGUARD_STONES)), rc, AlignLeft);
        iPoint off(10, 10);
        crc.w = (m_Rect.w - off.x * 2) / 6;
        crc.x = rc.x + off.x;
        for(uint32 xx=0; xx<6; xx++) {
            //gApp.Surface().FillRect(crc, gGame.Map().Rand(255));
            if(m_pHero->Owner()->HasKey(xx))
                gGfxMgr.Blit(PDGG_IPHONE(NEWHERO_KEYGUARD_STONES) + xx, gApp.Surface(), AlignRect(gGfxMgr.Dimension(PDGG_IPHONE(NEWHERO_KEYGUARD_STONES) + xx), crc, AlignCenter));
            else
                gGfxMgr.Blit(PDGG_IPHONE(NEWHERO_KEYGUARD_STONES) + 6, gApp.Surface(), AlignRect(gGfxMgr.Dimension(PDGG_IPHONE(NEWHERO_KEYGUARD_STONES) + 6), crc, AlignCenter));
            crc.x += crc.w;
        }
    }
    
    const iSize iKeysView::sm_defSize = iSize(255, 46);
    
    
    iHeroView::iHeroView(): IHeroView(true, CV_OVERLAND),
    m_bGestureComplete(false), m_pHero(NULL)
    {
        
        
        AddChild(m_pScreenSlider = new iScreenSlider(&gApp.ViewMgr(), this, iRect(), 99));
        
        STATS_SLIDE = m_pScreenSlider->AddSlide();
        ARTIFACTS_SLIDE = m_pScreenSlider->AddSlide();
        SPELLBOOK_SLIDE = m_pScreenSlider->AddSlide();
        
        iSlide* pSlide = m_pScreenSlider->GetSlide(STATS_SLIDE);
        pSlide->AddChild(m_pHeroTopView = new iHeroTopView(&gApp.ViewMgr(), iRect(), 100));
        pSlide->AddChild(m_pArmyExchange = new iArmyExchange(&gApp.ViewMgr(), this, iRect(), 102, true, false
#ifndef PC_VERSION
                                                             , false, false, iSize(50, 35)
#endif
                                                             ));
        pSlide->AddChild(m_pDismissBtn =  new iImgBtn(&gApp.ViewMgr(), this, iRect(), 103, PDGG_IPHONE(BTN_DISMISS_DELUXE), -1));
        pSlide->AddChild(m_pPuzzleBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 101, PDGG_IPHONE(BTN_PUZZLE_MAP_DELUXE), -1));
        pSlide->AddChild(m_pDigBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 100, PDGG_IPHONE(BTN_DIG_DELUXE), -1));
        pSlide->AddChild(m_pCastBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 115, PDGG_IPHONE(BTN_SPELLBOOK_DELUXE), -1));
        
        pSlide->AddChild(m_pExitBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 111, PDGG_IPHONE(BTN_CANCEL_DELUXE), -1));
        pSlide->AddChild(m_pSplitBtn = new iCheckButton(&gApp.ViewMgr(), this, iRect(), PDGG_IPHONE(BTN_OVERLAND_SPLIT), -1, 130));
        m_pArmyExchange->SetSplitBtn(m_pSplitBtn);
        
        
#ifdef ROYAL_BOUNTY
        m_pDismissBtn->SetVisible(false);
#endif
        pSlide->AddChild(m_pPrSkillsView = new iPrSkillsView(&gApp.ViewMgr(), this, iRect(), 105));
        pSlide->AddChild(m_pSecSkillsView = new iSecSkillsView(&gApp.ViewMgr(), this, iRect(), 106));
        
        pSlide = m_pScreenSlider->GetSlide(ARTIFACTS_SLIDE);
        pSlide->AddChild(m_pPrSkillsView2 = new iPrSkillsView(&gApp.ViewMgr(), this, iRect(), 114));
        pSlide->AddChild(m_pHeroDoll = new iHeroDollView(m_artCompetitors, &gApp.ViewMgr(), iRect(), 111));
        pSlide->AddChild(m_pArtBag = new iArtifactBag(m_artCompetitors, &gApp.ViewMgr(), 112));
        
        pSlide = m_pScreenSlider->GetSlide(SPELLBOOK_SLIDE);
        pSlide->AddChild(m_pSpellbookView = new iSpellBookView(&gApp.ViewMgr(), this, iRect(), 104, NULL, SPT_MASK_ALL, SSM_ALL, 0, false));
        
    }
    
    void iHeroView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        uint32 uid = pView->GetUID();
        if(uid == 99) {
            if(param == STATS_SLIDE)
                gTutorial.Trigger(TUT_HERO_STATS);
            else if(param == ARTIFACTS_SLIDE)
                gTutorial.Trigger(TUT_HERO_ARTIFACTS);
            else if(param == SPELLBOOK_SLIDE)
                gTutorial.Trigger(TUT_HERO_MAGIC);
            
        }
        else if(uid == 100) {
            gGame.HideViewDelayed(CV_HERO);
            m_pHero->Dig();
        } else if(uid == 101) {
            iPuzzleMapDlg tdlg(&gApp.ViewMgr(), *m_pHero->Owner(), 0);
            tdlg.DoModal();
        } else if(uid == 111) {
            gGame.HideView(CV_HERO);
        } else if(uid == 104) {
            iBaseSpell* pSpell = gGame.ItemMgr().m_spellMgr.Spell((MAGIC_SPELL)param);
            iSpellInfoDlg dlg(&gApp.ViewMgr(), pSpell->Id(), pSpell->GetSchoolLevel(m_pHero), PID_NEUTRAL);
            dlg.DoModal();
        } else if(uid == 107) {
            gGame.Map().ActPlayer()->PrevHero();
            SetHero(gGame.Map().CurHero());
        } else if(uid == 108) {
            gGame.Map().ActPlayer()->NextHero();
            SetHero(gGame.Map().CurHero());
        } else if(uid == 103) {
            iQuestDlg qdlg(m_pMgr, m_pHero->Name(), gTextMgr[TRID_MSG_HDISMISS], m_pHero->Owner()->PlayerId());
            if (qdlg.DoModal() == DRC_YES) {
                // Remove hero from the map
                gGame.Map().RemoveHero(m_pHero, true);
                UpdateSize();
            }
        } else if(uid == 105 || uid == 114) { // prskill views
            if(param == 113) {
                // Experience button
                iIconDlg dlg(m_pMgr, iFormat(_T("%s %d"),gTextMgr[TRID_LEVEL], m_pHero->Level()), iStringT(gTextMgr[TRID_INFO_EXPERIENCE]) + _T("\n\n#FEE8") + iFormat(gTextMgr[TRID_MSG_EXP_TO_NEXT_LEVEL], m_pHero->LevelBound(m_pHero->Level()+1) - m_pHero->Experience()), PDGG_IPHONE(DGLYPH_EXP), m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            } else if(param >= 150 ) { // primary skill btn
                iIconDlg dlg(m_pMgr, gTextMgr[TRID_SKILL_ATTACK + (param - 150)], gTextMgr[TRID_INFO_ATTACK + (param - 150)], PDGG_IPHONE(DGLYPH_PRSKILLS) + ((param - 150)), m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            } else if(param == 101) {	// Morale button
                iMLInfoDlg dlg(m_pMgr, false, m_pHero->FurtSkill(FSK_MORALE), m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            } else if (param == 102) {   // Luck button
                iMLInfoDlg dlg(m_pMgr, true, m_pHero->FurtSkill(FSK_LUCK), m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            } else if (param == 103) {
                // Mana points button
                iIconDlg dlg(m_pMgr, gTextMgr[TRID_MANA], gTextMgr[TRID_INFO_MANA], PDGG_IPHONE(DGLYPH_MANA), m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            } else if (param == 104) {
                // Travel points button
                iIconDlg dlg(m_pMgr, gTextMgr[TRID_TRAVELPTS], gTextMgr[TRID_INFO_TRAVELPTS], PDGG_IPHONE(DGLYPH_MOVES), m_pHero->Owner()->PlayerId());
                dlg.DoModal();
            }
        }
        else if(uid == 115) {
            iDlg_Spellbook csDlg(m_pMgr, m_pHero, SPT_MASK_OVERLAND, !!m_pHero->InShip());
            if (csDlg.DoModal() == DRC_YES) {
                m_pHero->CastSpell((MAGIC_SPELL)csDlg.SelSpell());
            }
        }
    }
    
    void iHeroView::OnDualSwipeLeft(sint32 length)
    {
        if(length > 20 && !m_bGestureComplete) {
            gGame.Map().CurPlayer()->NextHero();
            SetHero(gGame.Map().CurHero());
            m_bGestureComplete = true;
        }
    }
    
    void iHeroView::OnDualSwipeRight(sint32 length)
    {
        if(length > 20 && !m_bGestureComplete) {
            gGame.Map().CurPlayer()->PrevHero();
            SetHero(gGame.Map().CurHero());
            m_bGestureComplete = true;
        }
    }
    
    
    void iHeroView::SetHero( iHero* pHero )
    {
        m_pHero = pHero;
        m_pHeroTopView->SetHero(pHero);
        m_pArmyExchange->SetBottomOwner(pHero, true);
        m_pSpellbookView->SetOwner(pHero);
        m_pPrSkillsView->SetHero(pHero);
        m_pPrSkillsView2->SetHero(pHero);
        m_pSecSkillsView->SetHero(pHero);
        //m_pEMLView->SetHero(pHero);
        m_pHeroDoll->SetHero(pHero);
        m_pArtBag->SetHero(pHero);
        //m_pExpBtn->SetHero(pHero);
#ifndef ROYAL_BOUNTY
        m_pDismissBtn->SetVisible(!pHero->IsCrucial());
#endif
        m_pCastBtn->SetVisible(pHero->SpellBook().HasOverlandSpells());
    }
    
    
    void iHeroView::UpdateSize()
    {
        iRect rc = GetScrRect();
        m_pScreenSlider->SetRect(rc);
        m_pScreenSlider->UpdateSize();
        
        //1: army & stats
        m_pHeroTopView->SetRect(AlignRect(iHeroTopView::sm_defSize, rc, AlignTop));
#ifdef PC_VERSION
        m_pArmyExchange->SetRect(AlignRect(iArmyExchange::GetDefSize(1), rc, AlignBottom) + iPoint(-3, 8));
        m_pArmyExchange->UpdateSize();
#else
        iSize isz = m_pArmyExchange->GetDefSize(2);
        m_pArmyExchange->SetRect(AlignChild(isz, AlignBottom) + iPoint(0, isz.h / 4 - 5));
        
        m_pArmyExchange->SetPos(m_pArmyExchange->GetPos() + iPoint(0, 22));
#endif
        
        iRect arc = m_pArmyExchange->GetRect();
        m_pSplitBtn->SetRect(iRect(iPoint(arc.x2() - iIphoneBtn::sm_defSize.w + 8, arc.y + 29), iIphoneBtn::sm_defSize));
        m_pSplitBtn->SetPos(m_pSplitBtn->GetPos() + iPoint(0, 21));
        
        iSize sz = gGfxMgr.Dimension(PDGG_IPHONE(BTN_DISMISS));
        
        m_pPrSkillsView->SetRect(AlignRect(m_pPrSkillsView->sm_defSize, rc, AlignTop) + iPoint(0, iHeroTopView::sm_defSize.h));
        m_pPrSkillsView->UpdateSize();
        
        m_pSecSkillsView->SetRect(AlignRect(iSecSkillsView::sm_defSize, rc, AlignTop) + iPoint(0, m_pPrSkillsView->GetRect().y2()));
        m_pSecSkillsView->UpdateSize();
        
        m_pPuzzleBtn->SetRect(AlignRect(iIphoneBtn::sm_defSizeDeluxe, rc, AlignTopLeft));
        m_pDigBtn->SetRect(AlignRect(iIphoneBtn::sm_defSizeDeluxe, rc, AlignTopLeft) + iPoint(0, iIphoneBtn::sm_defSize.h));
        m_pCastBtn->SetRect(AlignRect(iIphoneBtn::sm_defSizeDeluxe, rc, AlignTopLeft) + iPoint(0, iIphoneBtn::sm_defSize.h * 2));
        m_pDismissBtn->SetRect(AlignRect(iIphoneBtn::sm_defSizeDeluxe, rc, AlignTopRight) + iPoint(0, iIphoneBtn::sm_defSize.h));
        
        
        
        m_pExitBtn->SetRect(AlignRect(iIphoneBtn::sm_defSizeDeluxe, rc, AlignTopRight));
        
        // 2: artifacts
        m_pArtBag->SetRect(AlignRect(iArtifactBag::sm_defSize, rc, AlignBottomLeft) + iPoint(7, 0));
        m_pArtBag->UpdateSize();
        m_pHeroDoll->SetRect(AlignRect(iHeroDollView::sm_defSize, rc, AlignBottomRight));
        m_pHeroDoll->UpdateSize();
        
        m_pPrSkillsView2->SetRect(AlignRect(m_pPrSkillsView->sm_defSize, rc, AlignTop));
        m_pPrSkillsView2->UpdateSize();
        
        // 3: spellbook
        m_pSpellbookView->SetRect(AlignRect(iSpellBookView::sm_defSize, rc, AlignBottom) - iPoint(0, 20));
        m_pSpellbookView->UpdateSize();
    }
    
    void iHeroView::OnCompose()
    {
        gApp.Surface().FillRect(GetScrRect(), cColor_Black);
        gGfxMgr.Blit(PDGG_IPHONE(HERO_DIALOG_BACK), gApp.Surface(), AlignRect(gGfxMgr.Dimension(PDGG_IPHONE(HERO_DIALOG_BACK)), GetScrRect(), AlignCenter));
    }
    
    
    bool iHeroView::Process( fix32 t )
    {
        return true;
    }
    
    void iHeroView::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) {
        if(mID == 0) m_bGestureComplete = false;
    }
    
    void iHeroView::OnSwipeUp( sint32 length )
    {
#if defined(OS_IPHONE) || defined(OS_ANDROID)
        if(length < 40 || gApp.ViewMgr().DragGlyph() ||
           dynamic_cast<iListBox*>(gApp.ViewMgr().CapturedView()) ||
           dynamic_cast<iAmountChooser*>(gApp.ViewMgr().CapturedView()))
            return;
        
        if(!m_bGestureComplete) {
            m_bGestureComplete = true;
            iDib hero(gApp.Surface().GetSize(), iDib::RGB);
            gApp.Surface().CopyToDibXY(hero, iPoint());
            iTransitionView* trView = new iTransitionView(&gApp.ViewMgr(),
                                                          iTransitionView::TVM_VRT_OVERLAY,
                                                          iTransitionView::MM_MANUAL,
                                                          hero, gGame.PeekScreen());
            trView->TransferMouseState(this);
            //		gApp.ViewMgr().ReleaseViewCapture();
            gApp.ViewMgr().SetViewCapture(trView);
            iTransitionView::TV_RESULT res = (iTransitionView::TV_RESULT)trView->DoModal();
            delete trView;
            if(res == iTransitionView::TVR_UP) {
                gGame.HideViewDelayed(CV_HERO);
            }
            //m_bGestureComplete = true;
            //Invalidate();
        }
#endif
    }
    
    void iHeroView::OnMouseTrack( const iPoint& pos, MouseId mID, MouseButtonId mbID )
    {
#ifdef PC_VERSION
        m_pSpellbookView->MouseTrack(pos, mID);
#endif
    }
    
    void iHeroView::OnActivate( bool bActivate )
    {
        if(bActivate) {
            //gTutorial.Trigger(TUT_HERO);
            
            m_pArmyExchange->SetSplitChecked(false);
            m_pScreenSlider->SetCurrentSlide(STATS_SLIDE);
            SetHero(gGame.Map().CurHero());
            m_Rect.x = 0;
            m_pScreenSlider->SetVisible(false);
            FallDown();
            
            gTutorial.ShowFingerHelp();
            m_pScreenSlider->SetVisible(true);
        }
    }
    
    const sint32 HERO_DOLL_CELLS[AC_COUNT][2] = {
        {121, 5}, // head
        {121, 65}, // neck
        {121, 120}, // torso
        {200, 135}, // left hand
        {60, 135},  // right hand
        {200, 90},  // left fingers
        {60, 90},  // right fingers
        {30, 210}, // shoulders
        {90, 180},  // legs
        {145, 230}, // feet
        {0, 20},  // misc1
        {0, 60},  // misc2
        {0, 100},  // misc3
        {0, 140} // misc4
    };
    
    iHeroDollView::iHeroDollView(iSimpleArray<iArtClickItem*> &competitors, iViewMgr *pViewMgr, iRect rc, uint32 uid ): iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled)
    {
        for (uint32 xx=0; xx<AC_COUNT; ++xx) {
            m_pDollItems[xx] = new iArtItemCtrl_Doll(competitors, pViewMgr, this, iPoint(), iIphoneBtn::sm_defSize, 310+xx);
            AddChild(m_pDollItems[xx]);
        }
    }
    
    void iHeroDollView::Deselect() {
        for (uint32 xx=0; xx<AC_COUNT; ++xx) {
            m_pDollItems[xx]->Unhighlight();
        }
    }
    
    void iHeroDollView::UpdateSize()
    {
        for (uint32 xx=0; xx<AC_COUNT; ++xx) {
            m_pDollItems[xx]->SetRect(iRect(iPoint(HERO_DOLL_CELLS[xx][0], HERO_DOLL_CELLS[xx][1]), iIphoneBtn::sm_defSize));
        }
    }
    
    void iHeroDollView::OnCompose()
    {
        iRect rc = GetScrRect();
        //	gApp.Surface().FrameRect(rc, cColor_White);
        
        gGfxMgr.Blit(PDGG_IPHONE(NEWHERO_ARTIFACTS_DOLL), gApp.Surface(), rc + iPoint(10, 0));
    }
    
    void iHeroDollView::SetHero( iHero *pHero )
    {
        m_pHero = pHero;
        for (uint32 xx=0; xx<AC_COUNT; ++xx) {
            m_pDollItems[xx]->SetOwner(m_pHero);
            m_pDollItems[xx]->SetArtCell(&m_pHero->Artifacts().DressedItem((HERO_ART_CELL)xx));
            m_pDollItems[xx]->Deselect();
        }
        Invalidate();
    }
    
    void iHeroDollView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        
    }
    
    const iSize iHeroDollView::sm_defSize = iSize(270, 280);
    const iSize iArtifactBag::sm_defSize = iSize(210, 280);
    const iSize iArtifactBag::sm_contentSize = iSize(2, 2);
    
    iArtifactBag::iArtifactBag(iSimpleArray<iArtClickItem*>& competitors, iViewMgr* pViewMgr, uint32 uid)
    : iArtClickItem(competitors, iIphoneBtn::sm_defSize, pViewMgr, NULL, iRect(), uid),
    iArtBackPackCtrl(iIphoneBtn::sm_defSize, sm_contentSize.w * sm_contentSize.h, 0)
    , m_lastClickedItem(-1)
    , m_Competitors(competitors)
    , m_curPage(0)
    {
        AddChild(m_pPrevPageBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 100, PDGG_IPHONE(BTN_SWITCH_CASTLES), PDGG_IPHONE(BTN_SWITCH_CASTLES) + 2));
        AddChild(m_pNextPageBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 101, PDGG_IPHONE(BTN_SWITCH_CASTLES) + 1, PDGG_IPHONE(BTN_SWITCH_CASTLES) + 3));
    }
    
    void iArtifactBag::UpdateSize()
    {
        m_pPrevPageBtn->SetRect(AlignChild(iSize(50, 50), AlignBottom) - iPoint(50, 0));
        m_pNextPageBtn->SetRect(AlignChild(iSize(50, 50), AlignBottom) + iPoint(50, 0));
    }
    
    void iArtifactBag::SetHero(iHero *pHero){
        m_pHero = pHero;
        SetBackPack(&m_pHero->Artifacts().BackPack(), pHero);
        m_contents.RemoveAll();
        
        // add actual artifacts to contents array
        for(uint32 xx=0; xx < pHero->Artifacts().BackPackCount(); xx++)
            m_contents.Add(pHero->Artifacts().BackPack().At(xx));
        
        Deselect();
        Invalidate();
    }
    
    uint32 iArtifactBag::MaxPage()
    {
        return floor(((double)(m_contents.GetSize()) - 0.01f) / (sm_contentSize.w * sm_contentSize.h));
    }
    
    void iArtifactBag::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        uint32 uid = pView->GetUID();
        if(uid == 100) {
            if(m_curPage > 0)
                m_curPage--;
            else
                m_curPage = MaxPage();
            m_selectedItem = -1;
        }
        else if(uid == 101) {
            m_curPage++;
            if(m_curPage > MaxPage())
                m_curPage = 0;
            m_selectedItem = -1;
        }
    }
    
    sint32 iArtifactBag::CellByPos(const iPoint& pos) const
    {
        sint32 val;
        iPoint p(pos - contentOffset - GetScrPos());
        
        val = (p.x / m_cellSize.w) + (p.y / m_cellSize.h) * sm_contentSize.w;
        
        if (val<0 || val >= (sint32)m_cellsCount) return -1;
        return val;
    }
    
    const iPoint iArtifactBag::contentOffset(27, 105);
    
    void iArtifactBag::OnCompose()
    {
        iRect rc = GetScrRect();
        
        
        gGfxMgr.Blit(PDGG_IPHONE(NEWHERO_ARTIFACTS_BAG), gApp.Surface(), rc);
        iRect crc(rc.x + contentOffset.x, rc.y + contentOffset.y, m_cellSize.w, m_cellSize.h);
        
        // Compose cells
        for (uint32 xx=0; xx<m_cellsCount; ++xx) {
            if(xx%(sm_contentSize.w)==0 && xx>0) {
                crc.x = rc.x + contentOffset.x;
                crc.y += crc.h;
            }
            sint32 bpidx = BackPackIdxByCell(xx);
            ComposeArtifactCell(crc, (bpidx==-1)?INVART:m_contents.At(bpidx), ((sint32)xx==m_selectedItem)?ABF_PRESSED:0);		
            crc.x += crc.w;
        }
    }
    
    void iArtifactBag::Deselect()
    {
        m_selectedItem = -1;
    }
    
    void iArtifactBag::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
    {
        if(!m_bIsDoubleClicking)
            m_lastClickedItem = CellByPos(pos);
        
        // some of the competitors has an item highlighted
        for (uint32 xx=0; xx<m_Competitors.GetSize(); ++xx) {
            if(m_Competitors[xx]->m_selectedItem != -1) {
                
                sint32 frombpidx = m_Competitors[xx]->m_selectedItem;
                sint32 tobpidx = CellByPos(pos);
                if (m_Competitors[xx] == this && tobpidx != -1) {
                    UpdateContent(tobpidx);
                    if (tobpidx != frombpidx && BackPackIdxByCell(tobpidx) != -1) {
                        iSwap(m_contents.At(BackPackIdxByCell(frombpidx)), m_contents.At(BackPackIdxByCell(tobpidx)));
                    }
                    RemoveTrailingSlots();
                } else {
                    if (tobpidx == -1) 					
                        m_contents.Add(m_Competitors[xx]->Remove(frombpidx).artId);
                    else {
                        UpdateContent(tobpidx);
                        sint32 idx = BackPackIdxByCell(tobpidx);
                        if(idx == -1)
                            m_contents.Add(m_Competitors[xx]->Remove(frombpidx).artId);
                        else {
                            if(m_contents.At(idx) == INVART)
                                m_contents.At(idx) = m_Competitors[xx]->Remove(frombpidx).artId;
                            else
                                m_contents.InsertBefore(idx, m_Competitors[xx]->Remove(frombpidx).artId);
                        }					
                    }
                    RemoveTrailingSlots();
                }		
                SyncArts();
                m_selectedItem = -1;
                m_Competitors[xx]->m_selectedItem = -1;
                for (uint32 yy=0; yy<m_Competitors.GetSize(); ++yy)
                    m_Competitors[yy]->Unhighlight();
                Invalidate();
                return;	
            }
        }
        
        if(m_selectedItem != -1) {
            m_selectedItem = -1;
            for (uint32 yy=0; yy<m_Competitors.GetSize(); ++yy)
                m_Competitors[yy]->Unhighlight();
        }
        else if(BackPackIdxByCell(CellByPos(pos)) != -1 && m_contents.At(BackPackIdxByCell(CellByPos(pos))) != INVART) {
            m_selectedItem = CellByPos(pos);
            for (uint32 xx=0; xx<m_Competitors.GetSize(); ++xx)
                m_Competitors[xx]->Highlight(this, ArtId());
        }
        Invalidate();	
    }
    
    void iArtifactBag::OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
    {
        
        if(CellByPos(pos) != m_lastClickedItem) return;
        
        sint32 bpIdx = BackPackIdxByCell(CellByPos(pos));
        if (bpIdx != -1 && m_contents.At(bpIdx) != INVART) {
            iDlg_ArtInfo dlg(m_pMgr, m_contents.At(bpIdx), m_pOwner);
            dlg.DoModal();
            m_selectedItem = -1;
            for (uint32 yy=0; yy<m_Competitors.GetSize(); ++yy)
                m_Competitors[yy]->Unhighlight();
            Invalidate();
        }
    }
    
    uint16 iArtifactBag::ArtId() const
    {
        if(m_selectedItem == -1)
            return INVART;
        sint32 bpIdx = BackPackIdxByCell(m_selectedItem);
        if (bpIdx != -1) {
            return m_contents.At(bpIdx);
        } else
            return INVART;
    }
    
    void iArtifactBag::UpdateContent(sint32 cellIdx) 
    {
        if(cellIdx < 0 || cellIdx >= (sint32)m_cellsCount) return;
        
        /* insert dummy slots to contents if required item idx exceeds its length */
        for(sint32 xx = m_curPage * m_cellsCount + cellIdx - m_contents.GetSize() + 1; xx>0; xx--)
            m_contents.Add(INVART);
        
    }
    
    sint32 iArtifactBag::BackPackIdxByCell( sint32 cellIdx ) const 
    {
        if(cellIdx < 0 || cellIdx >= (sint32)m_cellsCount) return -1;
        
        return ((m_curPage * m_cellsCount + cellIdx) < m_contents.GetSize()) ? (m_curPage * m_cellsCount + cellIdx) : (-1);
    }
    
    void iArtifactBag::SyncArts()
    {
        m_pHero->Artifacts().BackPack().RemoveAll();
        for(uint32 xx=0; xx<m_contents.GetSize(); xx++)
            if(m_contents[xx] != INVART)
                m_pHero->Artifacts().BackPack().Add(m_contents[xx]);
    }
    
    iArtCell iArtifactBag::Replace( sint32 cell, const iArtCell& newArt )
    {
        UpdateContent(cell);
        sint32 bpidx = BackPackIdxByCell(cell);
        check(bpidx != -1);
        iArtCell result(AC_UNDEFINED, m_contents.At(bpidx));
        m_contents.At(bpidx) = newArt.artId;
        SyncArts();
        return result;
    }
    
    iArtCell iArtifactBag::Remove( sint32 cell )
    {	
        UpdateContent(cell);
        sint32 bpidx = BackPackIdxByCell(cell);
        check(bpidx != -1);
        iArtCell result(AC_UNDEFINED, m_contents.At(bpidx));
        m_contents.At(bpidx) = INVART;
        SyncArts();
        return result;
    }
    
    void iArtifactBag::RemoveTrailingSlots()
    {
        /* remove trailing dummy slots */
        if(m_contents.GetSize() > 0) {
            for(sint32 xx=m_contents.GetSize()-1; xx > 0; xx--)
                if(m_contents.At(xx) == INVART)
                    m_contents.RemoveAt(xx);
                else
                    break;
        }
    }
    
    iArtItemCtrl_Doll::iArtItemCtrl_Doll(iSimpleArray<iArtClickItem*>& competitors, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, const iSize& cellSiz, uint32 uid)
    : iArtItemCtrl_touch(competitors, pViewMgr, pCmdHandler, pos, cellSiz, uid) {
        
    }
    
    void iArtItemCtrl_Doll::Highlight(iArtClickItem* pSource,  uint16 artId)
    {
        m_bHighlighted = dynamic_cast<iArtifactBag*>(pSource) && ( gGame.ItemMgr().m_artMgr[artId].Assign() == m_pArtCell->AssignType() && 
                                                                  m_pOwner->Artifacts().CanAttach(artId));
        Invalidate();
    }
    
    void iArtItemCtrl_Doll::Deselect()
    {
        m_bHighlighted = false;
        m_selectedItem = -1;
    }
    
}