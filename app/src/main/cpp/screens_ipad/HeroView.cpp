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

void ComposeArtifactCell(const iRect& rect, uint16 artId, uint32 flags);

namespace UI_IPAD
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
        //	if (m_pHero) {
        for (uint32 xx=1; xx<5; ++xx)
        {
            m_pSkillTabBtns[xx]->SetRect(crc);
            crc.x += m_Rect.w / 4;
        }
        //	}
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
            iIconDlg dlg(m_pMgr, iFormat(_T("%s %d"),gTextMgr[TRID_LEVEL], m_pHero->Level()), iStringT(gTextMgr[TRID_INFO_EXPERIENCE]) + _T("\n\n#FEE8") + iFormat(gTextMgr[TRID_MSG_EXP_TO_NEXT_LEVEL], m_pHero->LevelBound(m_pHero->Level()+1) - m_pHero->Experience()), PDGG(DGLYPH_EXP), m_pHero->Owner()->PlayerId());
            dlg.DoModal();
        } else if (uid == 121) {
            // Mana points button
            iIconDlg dlg(m_pMgr, gTextMgr[TRID_MANA], gTextMgr[TRID_INFO_MANA], PDGG(DGLYPH_MANA), m_pHero->Owner()->PlayerId());
            dlg.DoModal();
        } else if (uid == 122) {
            // Travel points button
            iIconDlg dlg(m_pMgr, gTextMgr[TRID_TRAVELPTS], gTextMgr[TRID_INFO_TRAVELPTS], PDGG(DGLYPH_MOVES), m_pHero->Owner()->PlayerId());
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
            iSize siz = gGfxMgr.Dimension(PDGG(SEC_SKILLS)+m_pSkill->m_skill);
            iRect trc = AlignRect(siz, rc, AlignLeft);
            BlitIcon(gApp.Surface(), PDGG(SEC_SKILLS)+m_pSkill->m_skill, trc, 255, iPoint(), m_state & Pressed);
            trc.w = rc.w - trc.w;
            trc.x += siz.w;
            trc.y += gTextComposer.TextBoxOut(secskillnamefc, gApp.Surface(), gTextMgr[TRID_HSKILL_ESTATES+m_pSkill->m_skill], trc);
            trc.y += gTextComposer.TextBoxOut(secskilllevelfc, gApp.Surface(),gTextMgr[TRID_HSKILL_LEVEL_BASIC+m_pSkill->m_level], trc);
        }
    }
    
    
    iExpBtn::iExpBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) :
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
            iSize siz = gGfxMgr.Dimension(PDGG(DGLYPH_MANA));
            sint32 opx = rc.x + (sint32)(rc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(PDGG(DGLYPH_MANA)).x;
            sint32 opy = rc.y + (sint32)(rc.h/2) - (sint32)(siz.h/2) - gGfxMgr.Anchor(PDGG(DGLYPH_MANA)).x - 5;
            gGfxMgr.BlitEffect(PDGG(DGLYPH_MANA), gApp.Surface(), iPoint(opx,opy),  (m_state & Pressed) ? iGfxManager::EfxGlow : iGfxManager::EfxNone);
            iRect trc(rc.x+(sint32)(rc.w/2)-23,rc.y+rc.h-12, 46,11);
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
            iSize siz = gGfxMgr.Dimension(PDGG(DGLYPH_MOVES));
            sint32 opx = rc.x + (sint32)(rc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(PDGG(DGLYPH_MOVES)).x;
            sint32 opy = rc.y + (sint32)(rc.h/2) - (sint32)(siz.h/2) - gGfxMgr.Anchor(PDGG(DGLYPH_MOVES)).y - 1;
            gGfxMgr.BlitEffect(PDGG(DGLYPH_MOVES), gApp.Surface(), iPoint(opx,opy),  (m_state & Pressed) ? iGfxManager::EfxGlow : iGfxManager::EfxNone);
            iRect trc(rc.x+(sint32)(rc.w/2)-20,rc.y+rc.h-12, 40,11);
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
            ComposeMoraleGlyph(gApp.Surface(), rc, (sint8)m_pHero->FurtSkill(FSK_MORALE), m_state & Pressed);
        }
    }
    
    iLuckBtn::iLuckBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) : iSkillsTabBtn(pViewMgr, pCmdHandler, rect, uid){}
    
    void iLuckBtn::OnCompose()
    {
        iRect rc = GetScrRect();
        ButtonFrame(gApp.Surface(),rc,GetButtonState());
        
        if (m_pHero) {
            ComposeLuckGlyph(gApp.Surface(), rc, (sint8)m_pHero->FurtSkill(FSK_LUCK), m_state & Pressed);
        }
    }
    
    iSecSkillsView::iSecSkillsView(iViewMgr *pViewMgr, IViewCmdHandler* pCmdHandler, iPoint pnt, uint32 uid):
    iView(pViewMgr, iRect(pnt, iSize(110, 29*8 + 3)), GENERIC_VIEWPORT, uid, Visible|Enabled), m_pHero(NULL), m_pCmdHandler(pCmdHandler)
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
        //	ComposeGlowBkgnd(gApp.Surface(), rc);
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
    
    iSize iSecSkillsView::sm_defSize = iSize(260, 160);
    
    
    iPrSkillBtn::iPrSkillBtn(iViewMgr* pViewMgr, const iRect& rect, FURTHER_SKILLS skill)
    : iButton(pViewMgr, this, rect, 500 + skill, Visible|Enabled), m_skill(skill), m_pHero(NULL) {}
    
    void iPrSkillBtn::OnCompose()
    {
        iRect rc = GetScrRect();
        //gApp.Surface().FrameRect(rc, cColor_Black);
        if (!m_pHero) return;
        SpriteId sid  = PDGG(DGLYPH_PRSKILLS) + (m_skill-FSK_ATTACK);
        iSize siz = gGfxMgr.Dimension(sid);
        sint32 opx = rc.x + (sint32)(rc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(sid).x;
        sint32 opy = rc.y - gGfxMgr.Anchor(sid).y + 1;
        gGfxMgr.BlitEffect(sid, gApp.Surface(), iPoint(opx,opy), (m_state & Pressed) ? iGfxManager::EfxGlow : iGfxManager::EfxNone);
        
        iRect trc(rc.x+(sint32)(rc.w/2)-14,rc.y+rc.h-13, 28,13);
        DrawRoundRect(gApp.Surface(), trc, RGB16(192,128,32), cColor_Black);
        
        gTextComposer.TextOut(herolevelfc, gApp.Surface(), iPoint(), FormatNumber(m_pHero->FurtSkill(m_skill)), trc, AlignCenter);
    }
    
    void iPrSkillBtn::SetHero(iHero* pHero)
    {
        m_pHero = pHero;
        Invalidate();
    }
    
    void iPrSkillBtn::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        //uint32 uid = pView->GetUID();
        iIconDlg dlg(m_pMgr, gTextMgr[TRID_SKILL_ATTACK + (m_skill - FSK_ATTACK)], gTextMgr[TRID_INFO_ATTACK + (m_skill - FSK_ATTACK)], PDGG(DGLYPH_PRSKILLS) + (m_skill - FSK_ATTACK), m_pHero->Owner()->PlayerId());
        dlg.DoModal();
    }
    
    
    iPrSkillsView::iPrSkillsView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
    : iView(pViewMgr,rect,GENERIC_VIEWPORT, uid,Visible|Enabled), m_pHero(NULL)
    {
        uint32 ypos = 0;
        for (uint32 xx=0; xx<4; ++xx) {
            AddChild(m_pBtns[xx] = new iPrSkillBtn(pViewMgr, iRect(), (FURTHER_SKILLS)(FSK_ATTACK+xx)));
            ypos += 35;
        }
    }
    
    void iPrSkillsView::UpdateSize()
    {
        iRect crc(iPoint(0, (m_Rect.h - 36) / 2), iSize(53, 36));
        for (uint32 xx=0; xx<4; ++xx) {
            m_pBtns[xx]->SetRect(crc);
            crc.x += m_Rect.w / 4;
        }
    }
    
    void iPrSkillsView::OnCompose()
    {
        iRect rc = GetScrRect();
        ComposeDlgBkgndPanel(gApp.Surface(), rc);
    }
    
    
    
    void iPrSkillsView::SetHero(iHero* pHero)
    {
        m_pHero = pHero;
        for (uint32 xx=0; xx<4; ++xx) {
            m_pBtns[xx]->SetHero(pHero);
        }
        Invalidate();
    }
    
    void iPrSkillsView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
    }
    
    const iSize iPrSkillsView::sm_defSize = iSize(265, 55);
    
    
    
    iKeysView::iKeysView( iViewMgr *pViewMgr, const iRect& rc, uint32 uid ): iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pHero(NULL) {}
    
    void iKeysView::SetHero( iHero *pHero )
    {
        m_pHero = pHero;
        Invalidate();
    }
    
    void iKeysView::OnCompose()
    {
        iRect rc = GetScrRect();
        //	ComposeGlowBkgnd(gApp.Surface(), rc);
        if(!m_pHero) return;
        iRect crc = AlignRect(gGfxMgr.Dimension(PDGG(NEWHERO_KEYGUARD_STONES)), rc, AlignLeft);
        iPoint off(10, 10);
        crc.w = (m_Rect.w - off.x * 2) / 6;
        crc.x = rc.x + off.x;
        for(uint32 xx=0; xx<6; xx++) {
            //gApp.Surface().FillRect(crc, gGame.Map().Rand(255));
            if(m_pHero->Owner()->HasKey(xx))
                gGfxMgr.Blit(PDGG(NEWHERO_KEYGUARD_STONES) + xx, gApp.Surface(), AlignRect(gGfxMgr.Dimension(PDGG(NEWHERO_KEYGUARD_STONES) + xx), crc, AlignCenter));
            else
                gGfxMgr.Blit(PDGG(NEWHERO_KEYGUARD_STONES) + 6, gApp.Surface(), AlignRect(gGfxMgr.Dimension(PDGG(NEWHERO_KEYGUARD_STONES) + 6), crc, AlignCenter));
            crc.x += crc.w;
        }
    }
    
    const iSize iKeysView::sm_defSize = iSize(255, 46);
    
    
    iHeroView::iHeroView(): IHeroView(true, CV_OVERLAND),
    m_bGestureComplete(false), m_pHero(NULL)
#if defined( PC_VERSION )
    , m_bSpell(false)
#endif
    {
        AddChild(m_pDigBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 100, PDGG(BTN_DIG_DELUXE), -1, gTextMgr[TRID_HOVER_DIG], Visible|Enabled, AlignBottomLeft));
        AddChild(m_pPuzzleBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 101, PDGG(BTN_PUZZLE_MAP_DELUXE), -1, gTextMgr[TRID_HOVER_PUZZLE], Visible|Enabled, AlignBottom));
        AddChild(m_pExitBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 111, PDGG(BTN_CANCEL_DELUXE), -1, gTextMgr[TRID_HOVER_END_DLG], Visible|Enabled, AlignBottomRight));
        
        AddChild(m_pArmyExchange = new iArmyExchange(&gApp.ViewMgr(), this, iRect(), 102, false, true));
        AddChild(m_pDismissBtn =  new iImgBtn(&gApp.ViewMgr(), this, iRect(), 103, PDGG(BTN_DISMISS_DELUXE), -1, gTextMgr[TRID_HOVER_DISMISS], Visible|Enabled, AlignBottom));
        AddChild(m_pPrSkillsView = new iPrSkillsView(&gApp.ViewMgr(), this, iRect(), 105));
        AddChild(m_pSecSkillsView = new iSecSkillsView(&gApp.ViewMgr(), this, iRect(), 106));
        
        AddChild(m_pPrevHeroBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 107, PDGG(BTN_SWITCH_CASTLES_DELUXE), PDGG(BTN_SWITCH_CASTLES_DELUXE) + 2, iStringT(_T("")), Visible|Enabled));
        AddChild(m_pNextHeroBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 108, PDGG(BTN_SWITCH_CASTLES_DELUXE) + 1, PDGG(BTN_SWITCH_CASTLES_DELUXE) + 3, iStringT(_T("")), Visible|Enabled));
        
        AddChild(m_pKeysView = new iKeysView(&gApp.ViewMgr(), iRect(), 109));
        AddChild(m_pEMLView = new iExpLuckMoraleView(&gApp.ViewMgr(), this, iRect(), 110));
        
        //    m_pEMLView->SetRect(AlignRect(m_pEMLView->sm_defSize, r, AlignLeft) + iPoint(40, 0));
        //    m_pEMLView->UpdateSize();
        
        AddChild(m_pCastBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 115, PDGG(BTN_SPELLBOOK_DELUXE), -1, gTextMgr[TRID_HOVER_CAST], Visible|Enabled, AlignBottomLeft));
        
        AddChild(m_pHeroDoll = new iHeroDollView(m_artCompetitors, &gApp.ViewMgr(), iRect(), 111));
        AddChild(m_pArtBag = new iArtifactBag(m_artCompetitors, &gApp.ViewMgr(), 112));
        
        AddChild(m_pSpellbookView = new iSpellBookView(&gApp.ViewMgr(), this, iRect(), 104, NULL, SPT_MASK_ALL, SSM_ALL, 0, false));
        
        AddChild(m_pExpBtn = new iExpBtn(&gApp.ViewMgr(), this, iRect(), 113));
        
        //    UpdateSize();
    }
    
    void iHeroView::SetHighlighted(bool flag){
#ifdef PC_VERSION
        m_pDigBtn->SetHighlightedBtn( flag );
        m_pPuzzleBtn->SetHighlightedBtn( flag );
        m_pExitBtn->SetHighlightedBtn( flag );
        m_pDismissBtn->SetHighlightedBtn( flag );
        m_pCastBtn->SetHighlightedBtn( flag );
        m_pArmyExchange->SetSplitBtn( flag );
#endif
    }
    
    void iHeroView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        uint32 uid = pView->GetUID();
        if(uid == 100) {
            gGame.HideViewDelayed(CV_HERO);
            m_pHero->Dig();
        } else if(uid == 101) {
            iPuzzleMapDlg tdlg(&gApp.ViewMgr(), *m_pHero->Owner(), 0);
            tdlg.DoModal();
            SetHighlighted(false);
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
            SetHighlighted(false);
        } else if(uid == 113) {
            // Experience button
            iIconDlg dlg(m_pMgr, iFormat(_T("%s %d"),gTextMgr[TRID_LEVEL], m_pHero->Level()), iStringT(gTextMgr[TRID_INFO_EXPERIENCE]) + _T("\n\n#FEE8") + iFormat(gTextMgr[TRID_MSG_EXP_TO_NEXT_LEVEL], m_pHero->LevelBound(m_pHero->Level()+1) - m_pHero->Experience()), PDGG(DGLYPH_EXP), m_pHero->Owner()->PlayerId());
            dlg.DoModal();
        } else if(uid == 115) {
            gGame.HideView(iChildGameView::CV_HERO);
            SpellDlg();
            SetHighlighted(false);
        }
    }
    
    void iHeroView::SpellDlg(){
        
        iDlg_Spellbook csDlg(m_pMgr, m_pHero, SPT_MASK_OVERLAND, !!m_pHero->InShip());
#if defined( PC_VERSION )
        if( m_bSpell )
            gGame.HideView(iChildGameView::CV_HERO);
#endif
        if (csDlg.DoModal() == DRC_YES) {
            m_pHero->CastSpell((MAGIC_SPELL)csDlg.SelSpell());
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
        m_pArmyExchange->SetBottomOwner(pHero, true);
        m_pSpellbookView->SetOwner(pHero);
        m_pPrSkillsView->SetHero(pHero);
        m_pSecSkillsView->SetHero(pHero);
        m_pKeysView->SetHero(pHero);
        m_pEMLView->SetHero(pHero);
        m_pHeroDoll->SetHero(pHero);
        m_pArtBag->SetHero(pHero);
        m_pExpBtn->SetHero(pHero);
        if(!pHero->IsCrucial())
        {
            m_pDismissBtn->SetVisible(!pHero->IsCrucial());
            
        }
        //m_pDigBtn->SetVisible(gGame.Map().CanDig(pHero->Pos()));
    }
    
    
    void iHeroView::UpdateSize()
    {
        m_pArmyExchange->SetRect(AlignChild(iArmyExchange::GetDefSize(1), AlignBottom));
        m_pArmyExchange->UpdateSize();
        
        if(m_pDismissBtn)
            m_pDismissBtn->SetRect(m_pArmyExchange->GetDismissRect() + m_pArmyExchange->GetPos());
        m_pCastBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottomLeft) + iPoint(iIphoneBtn::sm_defSizeDeluxe.w, 0));
        
        m_pDigBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottomLeft));
        m_pExitBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottomRight));
        
        m_pPuzzleBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottomRight) - iPoint(iIphoneBtn::sm_defSizeDeluxe.w, 0));
        
        m_pKeysView->SetRect(AlignChild(iKeysView::sm_defSize, AlignBottom) - iPoint(0, m_Rect.h - m_pArmyExchange->GetPos().y) - iPoint(0, 40));
        m_pSecSkillsView->SetRect(AlignChild(iSecSkillsView::sm_defSize, AlignBottom) - iPoint(0, m_Rect.h - m_pKeysView->GetPos().y));
        m_pSecSkillsView->UpdateSize();
        m_pSpellbookView->SetRect(AlignChild(iSpellBookView::sm_defSize, AlignBottom) - iPoint(0, m_Rect.h - m_pSecSkillsView->GetPos().y));
        m_pSpellbookView->UpdateSize();
        
        iRect tmp = AlignChild(iSize(m_Rect.w, 55), AlignBottomLeft);
        tmp.y = 70;
        
        iRect l = AlignRect(iSize(m_Rect.w / 2, tmp.h), tmp, AlignLeft);
        iRect r = AlignRect(iSize(m_Rect.w / 2, tmp.h), tmp, AlignRight);
        
        m_pPrSkillsView->SetRect(AlignRect(m_pPrSkillsView->sm_defSize, l, AlignRight) - iPoint(40, 0));
        m_pPrSkillsView->UpdateSize();
        m_pEMLView->SetRect(AlignRect(m_pEMLView->sm_defSize, r, AlignLeft) + iPoint(40, 0));
        m_pEMLView->UpdateSize();
        
        m_pPrevHeroBtn->SetRect(AlignChild(iSize(50, 50), AlignTop) - iPoint(300, 0));
        m_pNextHeroBtn->SetRect(AlignChild(iSize(50, 50), AlignTop) + iPoint(300, 0));
        /*
         if( m_pHero->Owner() ){
         
         m_pPrevHeroBtn->SetVisible(m_pHero->Owner()->HeroCount() > 1);
         m_pNextHeroBtn->SetVisible(m_pHero->Owner()->HeroCount() > 1);
         }
         else {
         m_pPrevHeroBtn->SetVisible(false);
         m_pNextHeroBtn->SetVisible(false);
         }
         */
        iRect tmprc = iRect(iPoint(0, 0), iPoint(m_pSpellbookView->GetPos().x, m_Rect.h));
        
        m_pArtBag->SetRect(AlignRect(iArtifactBag::sm_defSize, tmprc, AlignCenter) + iPoint(0, 40));
        m_pArtBag->UpdateSize();
        m_pHeroDoll->SetRect(AlignChild(iHeroDollView::sm_defSize, AlignRight) + iPoint(0, 40));
        m_pHeroDoll->UpdateSize();
        
        
        iPoint pt = iPoint(m_Rect.w / 2 - 25, 55);
        pt.y += gTextComposer.GetFontHeight(castletitlefc.fontSize);
        m_pExpBtn->SetRect(iRect(pt, iSize(50, 50)));
    }
    
    void iHeroView::DoCompose()
    {
        gGfxMgr.Blit(PDGG(HERO_DIALOG_BACK), gApp.Surface(), GetScrPos());
        
        //gApp.Surface().FillRect(m_pArmyExchange->GetScrRect(), 255);
        //gApp.Surface().FillRect(m_pDismissBtn->GetScrRect(), 0);
        
        if(m_pHero) {
            // hero name
            iRect rc = GetScrRect();
            //rc.y += 15;
            rc.y += ComposeHeroTitleName(gApp.Surface(), rc, m_pHero);
            rc.y += ComposeHeroAvatar(gApp.Surface(), rc, m_pHero);
        }
    }
    
    
    bool iHeroView::Process( fix32 t )
    {
#if defined( PC_VERSION )
        if( m_bSpell ){
            
            m_bSpell = false;
            
            SpellDlg();
        }
#endif
        return true;
    }
    
    void iHeroView::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) {
        if(mID == 0) m_bGestureComplete = false;
    }
    
    void iHeroView::OnSwipeUp( sint32 length )
    {
#ifdef OS_IPHONE
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
        m_pSpellbookView->MouseTrack(pos, mID, mbID);
    }
    
    void iHeroView::OnActivate( bool bActivate )
    {
        if(bActivate) {
            
            bFirstDarken = true;
            SetHero(gGame.Map().CurHero());
            FallDown();
        }
    }
    
    const sint32 HERO_DOLL_CELLS[AC_COUNT][2] = {
        {161, 0}, // head
        {161, 65}, // neck
        {161, 130}, // torso
        {260, 135}, // left hand
        {80, 135},  // right hand
        {260, 90},  // left fingers
        {80, 90},  // right fingers
        {60, 210}, // shoulders
        {130, 200},  // legs
        {205, 305}, // feet
        {45, 350},  // misc1
        {120, 350},  // misc2
        {195, 350},  // misc3
        {270, 350} // misc4
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
        gGfxMgr.Blit(PDGG(NEWHERO_ARTIFACTS_DOLL), gApp.Surface(), rc);
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
    
    const iSize iHeroDollView::sm_defSize = iSize(360, 397);
    const iSize iArtifactBag::sm_defSize = iSize(270, 470);
    
    iArtifactBag::iArtifactBag(iSimpleArray<iArtClickItem*>& competitors, iViewMgr* pViewMgr, uint32 uid)
    : iArtClickItem(competitors, iIphoneBtn::sm_defSize, pViewMgr, NULL, iRect(), uid),
    iArtBackPackCtrl(iIphoneBtn::sm_defSize, 9, 0)
    , m_lastClickedItem(-1)
    , m_Competitors(competitors)
    , m_curPage(0)
    {
        AddChild(m_pPrevPageBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 100, PDGG(BTN_SWITCH_CASTLES_DELUXE), PDGG(BTN_SWITCH_CASTLES_DELUXE) + 2));
        AddChild(m_pNextPageBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 101, PDGG(BTN_SWITCH_CASTLES_DELUXE) + 1, PDGG(BTN_SWITCH_CASTLES_DELUXE) + 3));
    }
    
    void iArtifactBag::UpdateSize()
    {
        m_pPrevPageBtn->SetRect(AlignChild(iSize(50, 50), AlignBottom) - iPoint(70, 0));
        m_pNextPageBtn->SetRect(AlignChild(iSize(50, 50), AlignBottom) + iPoint(30, 0));
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
        return floor(((double)(m_contents.GetSize()) - 0.01f) / 9);
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
        
        val = (p.x / m_cellSize.w) + (p.y / m_cellSize.h) * 3;
        
        if (val<0 || val >= (sint32)m_cellsCount) return -1;
        return val;
    }
    
    const iPoint iArtifactBag::contentOffset(20, 195);
    
    void iArtifactBag::OnCompose()
    {
        iRect rc = GetScrRect();
        
        gGfxMgr.Blit(PDGG(NEWHERO_ARTIFACTS_BAG), gApp.Surface(), rc);
        iRect crc(rc.x + contentOffset.x, rc.y + contentOffset.y, m_cellSize.w, m_cellSize.h);
        
        // Compose cells
        for (uint32 xx=0; xx<m_cellsCount; ++xx) {
            if(xx%3==0 && xx>0) {
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