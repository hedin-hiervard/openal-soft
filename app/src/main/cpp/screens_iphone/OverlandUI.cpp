#include "stdafx.h"
#include "OverlandView.h"
#include "OverlandView.touch.h"
#include "MinimapView.h"
#include "OverlandUI.h"
#include "ArmyList.h"
#include "ArmyList.touch.h"
#include "ArtifactList.h"
#include "ArtifactList.touch.h"
#include "HeroView.h"
#include "CastleView.h"

namespace UI_IPHONE
{
    
    
    iResourceBar::iResourceBar(iViewMgr* pViewMgr, uint32 uid):
    IResourceBar(pViewMgr, iRect(), GENERIC_VIEWPORT, uid, Visible|Enabled)
    , m_ms(NULL), m_bAnimating(false)
    {
        
    }
    
    iSize iResourceBar::GetDefSize()
    {
        return iSize();//return gGfxMgr.Dimension(PDGG(OVLND_RESOURCEBAR));
    }
    
    void iResourceBar::OnCompose()
    {
        //return;
        iRect rc = GetScrRect();
        
        const iMineralSet* todraw = m_bAnimating ? &m_currentMinSet : m_ms;
        
        // !!
        //	gGfxMgr.Blit(PDGG(OVLND_RESOURCEBAR), gApp.Surface(), rc);
        if(todraw) {
            iRect crc(rc.x + 25, rc.y, (rc.w - 45) / (MINERAL_COUNT), rc.h - 23);
            for(uint32 xx=0; xx<MINERAL_COUNT; xx++)
            {
                iRect trc =  AlignRect(iSize(crc.w, gTextComposer.GetFontHeight(minsetfc.fontSize)), crc, AlignBottom);
                trc.InflateRect(10, 0);
                gTextComposer.TextBoxOut(minsetfc, gApp.Surface(), iFormat(_T("%d"), (*todraw).quant[xx]), trc);
                crc.x += crc.w;
            }
        }
    }
    
    void iResourceBar::AnimateResources(const iMineralSet& was, const iMineralSet& now)
    {
        m_bAnimating = true;
        m_currentMinSet = was;
        static AniQID mainQ = INVALID_QUEUE;
        static AniQID resourceQ[MINERAL_COUNT] = {
            INVALID_QUEUE,
            INVALID_QUEUE,
            INVALID_QUEUE,
            INVALID_QUEUE,
            INVALID_QUEUE,
            INVALID_QUEUE,
            INVALID_QUEUE
        };
        
        if(mainQ != INVALID_QUEUE)
            gAniHost.StopAnimation(mainQ, true);
        for(uint32 xx=0; xx<MINERAL_COUNT; xx++)
            if(resourceQ[xx] != INVALID_QUEUE)
                gAniHost.StopAnimation(resourceQ[xx], true);
        
        mainQ = gAniHost.AddAniObj(new iGAniObj_ChangeValDisc<bool>(fix32::zero, m_bAnimating, true));
        gAniHost.SetQFixed(mainQ, true);
        for(uint32 xx=0; xx<MINERAL_COUNT; xx++) {
            resourceQ[xx] = gAniHost.AddAniObj(new iGAniObj_ChangeVal<sint32>(fix32(2.0f), m_currentMinSet.quant[xx], now.quant[xx]), true, mainQ);
            gAniHost.SetQFixed(resourceQ[xx], true);
        }
        gAniHost.AddAniObjToQ(new iGAniObj_ChangeValDisc<bool>(fix32(2.0f), m_bAnimating, false), mainQ);
    }
    
    iOvalButton::iOvalButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, iRect rc, uint32 uid)
    : iButton(pViewMgr, pCmdHandler, rc, uid, Visible | Enabled)
    , m_pHero(NULL), m_pCastle(NULL)
    {
        
    }
    
    void iOvalButton::OnCompose()
    {
        //return;
        iRect rc = GetScrRect();
        if(m_pHero) {
            sint16 sid = m_pHero->Proto()->m_icn48;
            iRect hrc = AlignRect(gGfxMgr.Dimension(sid), rc, AlignBottom) - iPoint(0, 7);
            gGfxMgr.Blit(sid, gApp.Surface(), hrc);
        } else if(m_pCastle) {
            sint16 sid = PDGG(CASTLES_BIG) + m_pCastle->Proto()->Type();
            iRect hrc = AlignRect(gGfxMgr.Dimension(sid), rc, AlignBottom) - iPoint(0, 7);
            gGfxMgr.Blit(sid, gApp.Surface(), hrc);
        }
        //gGfxMgr.Blit(PDGG(OVLND_BAR_PORTRAIT), gApp.Surface(), rc);
        //	if(m_state & Pressed)
        //		gGfxMgr.Blit(PDGG(OVLND_BAR_PORTRAIT_PRESSED), gApp.Surface(), rc);
    }
    
    iOverlandToolbar::iOverlandToolbar(iViewMgr* pViewMgr, uint32 uid)
    : IOverlandToolbar(pViewMgr, iRect(), GENERIC_VIEWPORT, uid, Visible | Enabled),
    m_expPerc(-1),
    m_expBarSprite(-1),
    m_pos(cInvalidPoint), m_pHero(NULL), m_pCastle(NULL)
    {
        AddChild(m_pMinimap = new iMinimapRawView(pViewMgr, this, iRect(), 100));
        
        AddChild(m_pHeroBtn = new iOvalButton(pViewMgr, this, iRect(), 101));
        
        //	AddChild(m_pSplitBtn = new iCheckButton(&gApp.ViewMgr(), this, iRect(), PDGG(OVLND_BTN_SPLIT), PDGG(OVLND_BTN_SPLIT_PRESSED), 102));
        AddChild(m_pArmyList = new iArmyListEx_touch(pViewMgr, this, iPoint(), m_pSplitBtn, iSize(52, 35), 103, iArmyListEx_touch::GT_MIDDLE, false, false));
        //	m_pMinimap->SetVisible(false);
        //	m_pArmyList->SetVisible(false);
    }
    
    void iOverlandToolbar::UpdateMinimap()
    {
        m_pMinimap->PrepareSurface();
    }
    
    void iOverlandToolbar::CenterMinimap()
    {
        m_pMinimap->SetCenterCell(gGame.MainView()->Composer().GetCenterCell(), false);
    }
    
    void iOverlandToolbar::SetHero(iHero* pHero)
    {
        m_pHero = pHero;
        m_pHeroBtn->SetHero(pHero);
        if(pHero != NULL)
            m_pArmyList->SetArmy(pHero, true);
    }
    
    void iOverlandToolbar::SetCastle(iCastle* pCastle)
    {
        m_pCastle = pCastle;
        m_pHeroBtn->SetCastle(pCastle);
        if(pCastle != NULL)
            m_pArmyList->SetArmy(pCastle, true);
    }
    
    void iOverlandToolbar::UpdateSize()
    {
#if 0
        m_pMinimap->SetRect(AlignChild(iSize(214, 137), AlignBottomRight));
        m_pMinimap->UpdateSize();
        
        iRect hrc(50, -51, 200, 95);
        //uint16 sid = PDGG(OVLND_BAR_PORTRAIT);
        iSize sz = gGfxMgr.Dimension(sid);
        iRect portrc(AlignRect(sz, hrc, AlignTop));
        
        m_pHeroBtn->SetRect(portrc);
        
        
        /*sid = PDGG(OVLND_BAR);
         sz = gGfxMgr.Dimension(sid);
         iRect brc(m_Rect.x, m_Rect.y2() - 109 - 9, m_Rect.w, sz.h);
         
         iRect armrc = AlignRect(iSize(370, 30), brc, AlignTop) - m_Rect.point() - iPoint(0, 22);
         m_pArmyList->SetPos(armrc);
         m_pSplitBtn->SetRect(iRect(iPoint(armrc.x2() + 3, armrc.y + 14), iIphoneBtn::sm_defSize));
         */
#endif
    }
    
    void iOverlandToolbar::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        uint32 uid = pView->GetUID();
        if(uid == 100) { // minimap
            gGame.MainView()->Composer().CenterView(m_pMinimap->GetCenterCell());
            
        } else if(uid == 101) { // hero/castle oval button
            if(m_pHero) {
                gGame.HeroView()->SetHero(m_pHero);
                gGame.ShowView(iChildGameView::CV_HERO);
            }
            else if(m_pCastle)
            {
                gGame.CastleView()->SetCastle(m_pCastle);
                gGame.ShowView(iChildGameView::CV_CASTLE);
            }
        }
    }
    
    void iOverlandToolbar::ComposeInfoBar()
    {
#if 0
        iRect rc = GetScrRect();
        sint16 sid = PDGG(OVLND_BAR);
        iSize sz = gGfxMgr.Dimension(sid);
        iRect brc(rc.x, rc.y2() - 109 - 9, rc.w, sz.h);
        sz = gGfxMgr.Dimension(PDGG(OVLND_BAR_INFO));
        iRect irc = AlignRect(iSize(sz.w - 165, 70), rc, AlignBottom) - iPoint(0, 10);
        
        //gApp.Surface().FrameRect(irc, cColor_Red);
        
        iBaseMapObject *pMapObj = NULL;
        
        if (gGame.Map().IsValidPos(m_pos.x, m_pos.y))
        {
            uint8 dcfg, fcfg;
            if (gGame.Map().ActPlayer()->FogMap().GetFogCfg(m_pos,dcfg,fcfg) || dcfg!=15) {
                pMapObj = gGame.Map().GetCellObj(m_pos);
            }
        }
        
        if(m_pos == cInvalidPoint || pMapObj == NULL) {
            // Game date
            uint32 days = gGame.Map().m_CurDay-1;
            gTextComposer.TextOut(ovrlndfc_infobar, gApp.Surface(), cInvalidPoint, iFormat(_T("#FDB9%s: #FFDB%d  #FDB9%s: #FFDB%d  #FDB9%s: #FFDB%d"),gTextMgr[TRID_MONTH], days/28+1,gTextMgr[TRID_WEEK], (days%28)/7+1,gTextMgr[TRID_DAY],days%7+1), irc, AlignCenter);
            return;
        }
        
        VISION_LEVEL vl = VL_BASIC;
        iHero* pHero = gGame.Map().CurHero();
        sint32 radius=0;
        if (!gGame.Map().ActPlayer()->FogMap().IsHidden(pMapObj->Pos()) && pHero) vl = pHero->VisionLevel(radius);
        if (vl == VL_EXPERT && pHero->Pos().GetDelta(pMapObj->Pos()) > radius) vl = VL_BASIC;
        
        iPoint tpos;
        
        // MAP GUARD
        if(iMapGuard *pGuard = dynamic_cast<iMapGuard*>(pMapObj)) {
            iStringT text1, text2;
            text1 = gTextMgr[TRID_CREATURE_PEASANT_F3 + pGuard->Creatures().Type() * 3];
            if (vl == VL_EXPERT) {
                iMapGuard::MEET_RESULT mr = pGuard->CheckMeetResult(pHero);
                if (mr == iMapGuard::MR_ATTACK) text2 = gTextMgr[TRID_MSG_GUARDS_POPUP_ATTACK];
                else if (mr == iMapGuard::MR_FLEE) text2 = gTextMgr[TRID_MSG_GUARDS_POPUP_FLEE];
                else text2 = gTextMgr[TRID_MSG_GUARDS_POPUP_JOIN];
            }
            iSize sz = gTextComposer.GetTextSize(text1, ovrlndfc_infobar),
            sz2 = gTextComposer.GetTextSize(text2, ovrlndfc_infobar_small),
            //		sz3 = gGfxMgr.ObjRect(CREAT_DESC[pGuard->Creatures().Type()].sprite).size();
            sz3 = iSize(50, 70);
            iSize sz1 = sz;
            
            sz.w = sz1.w + sz3.w;
            sz.h = iMAX<sint32>(sz1.h, sz3.h);
            tpos = AlignRect(sz, irc, AlignTop);
            /*
             gGfxMgr.Blit(CREAT_DESC[pGuard->Creatures().Type()].sprite, gApp.Surface(), tpos);
             */
            ComposeCreatureCell(gApp.Surface(), iRect(tpos, iSize(50, 50)), pGuard->Creatures().Type(), pGuard->Creatures().Count(), vl);
            tpos.x += sz3.w + 10;
            gTextComposer.TextOut(ovrlndfc_infobar, gApp.Surface(), tpos + iPoint(0, 25), text1);
            tpos = AlignRect(sz2, irc, AlignBottom);
            gTextComposer.TextOut(ovrlndfc_infobar_small, gApp.Surface(), tpos, text2);
            return;
        }
        
        // HERO
        if(iHero *pHero = dynamic_cast<iHero*>(pMapObj)) {
            if(pHero == gGame.Map().CurHero()) vl = VL_EXPERT;
            
            tpos = irc.point();
            
            // Name
            gTextComposer.TextOut(ovrlndfc_infobar, gApp.Surface(), iPoint(), pHero->Name(), irc, AlignTop);
            
            // Icon
            SpriteId sid = pHero->Proto()->m_icn32;
            iSize sz = gTextComposer.GetTextSize(pHero->Name(), ovrlndfc_infobar);
            gGfxMgr.Blit(sid,gApp.Surface(), AlignRect(sz, irc, AlignTop) - iPoint(50, -4));
            
            
            /*// Skills
             iStringT psText;
             for (sint32 psid=0; psid<PRSKILL_COUNT; ++psid) {
             if (vl==VL_EXPERT) psText.Addf(_T("#I%04X #FCCC%d "), PDGG(ICN_PSKILLS)+psid, pHero->FurtSkill((FURTHER_SKILLS)(FSK_ATTACK+psid)));
             else psText.Addf(_T("#I%04X #FCCC? "), PDGG(ICN_PSKILLS)+psid);
             }
             gTextComposer.TextOut(gApp.Surface(), iPoint(), psText, irc, AlignBottom);
             */
            
            sint32 cnt = iMAX(1, pHero->Army().GroupCount());
            sz.h = 50;
            sz.w = iMIN<sint32>(irc.w / cnt, 70);
            iSize tsz;
            tsz.w = sz.w * cnt;
            tsz.h = sz.h;
            tpos = AlignRect(tsz, irc, AlignBottom);
            
            ComposeGuard(gApp.Surface(), tpos, sz, pHero->Army(), vl, false);
            return;
        }
        
        // VISITABLE
        if(iVisCnst* pVisCnst = dynamic_cast<iVisCnst*>(pMapObj)) {
            // Construction title
            iRect trc = irc;
            iStringT text;
            text = gTextMgr[pVisCnst->Proto()->NameKey()];
            
            // Custom popup message
            iStringT cmsg;
            if (pVisCnst->CustomPopupText(pHero, cmsg)){
                text += iStringT(_T(" (")) + cmsg + iStringT(_T(") "));
            }
            
            // Visited ?
            if (pVisCnst->ShowVisitedLabel()) {
                TextResId trid = (pHero && pVisCnst->Visited(pHero))?TRID_VISITED:TRID_NOT_VISITED;
                text += iFormat((_T("  (%s)  ")),gTextMgr[trid]);
            }
            
            if(pVisCnst->Guard().Empty())
            {
                sz = gTextComposer.GetTextSize(text, ovrlndfc_infobar);
                trc.y = AlignRect(sz, irc, AlignCenter).y;
            }
            
            trc.y += gTextComposer.TextBoxOut(ovrlndfc_infobar,gApp.Surface(), text, trc);
            
            // guard?
            if (!pVisCnst->Guard().Empty())
            {
                sint32 cnt = iMAX(1, pVisCnst->Guard().GroupCount());
                sz.h = 50;
                sz.w = iMIN<sint32>(irc.w / cnt, 70);
                iSize tsz;
                tsz.w = sz.w * cnt;
                tsz.h = sz.h;
                tpos = AlignRect(tsz, irc, AlignBottom);
                ComposeGuard(gApp.Surface(), tpos, sz, pVisCnst->Guard(), vl);
            }
            
            return;
        }
        
        // OWNERABLE
        if(	iOwnCnst *pOwnCnst = dynamic_cast<iOwnCnst*>(pMapObj)) {
            iStringT text;
            iRect trc = irc;
            
            // Construction title
            text += gTextMgr[pOwnCnst->Proto()->NameKey()];
            
            // owned by
            if(pOwnCnst->Owner() != PID_NEUTRAL) {
                PLAYER_ID pid = pOwnCnst->Owner();
                text += iStringT(_T(" (")) + gTextMgr[TRID_OWNEDBY] + _T(" ") + gTextMgr[TRID_PLAYER_RED_D + pid] +
                _T(" ") + gTextMgr[TRID_PLAYER_D] + _T(")");
            }
            
            if(pOwnCnst->Guard().Empty())
            {
                sz = gTextComposer.GetTextSize(text, ovrlndfc_infobar);
                trc.y = AlignRect(sz, irc, AlignCenter).y;
            }
            
            trc.y += gTextComposer.TextBoxOut(ovrlndfc_infobar,gApp.Surface(), text, trc);
            
            // guard?
            if (!pOwnCnst->Guard().Empty())
            {
                sint32 cnt = iMAX(1, pOwnCnst->Guard().GroupCount());
                sz.h = 50;
                sz.w = iMIN<sint32>(irc.w / cnt, 70);
                iSize tsz;
                tsz.w = sz.w * cnt;
                tsz.h = sz.h;
                tpos = AlignRect(tsz, irc, AlignBottom);
                ComposeGuard(gApp.Surface(), tpos, sz, pOwnCnst->Guard(), vl);
            }
            
            
            return;
        }
        
        // MAP ITEM
        if(iMapItem* mapItem = dynamic_cast<iMapItem*>(pMapObj)) {
            gTextComposer.TextOut(ovrlndfc_infobar, gApp.Surface(), tpos, gTextMgr[mapItem->NameKey()], irc, AlignCenter);
            return;
        }
        
        // SHIP
        if(dynamic_cast<iShip*>(pMapObj)) {
            gTextComposer.TextOut(ovrlndfc_infobar, gApp.Surface(), tpos, gTextMgr[TRID_OBJ_SHIP], irc, AlignCenter);
            return;
        }
#endif
    }
    
    void iOverlandToolbar::OnCompose()
    {
#if 0
        //return;
        iRect rc = GetScrRect();
        iRect arc = iRect(rc.x, rc.y2() - 109, rc.w, 110);
        
        // !!
        gApp.Surface().FillRect(arc, RGB16(23, 22, 20));
        
        sint16 sid = PDGG(OVLND_BAR);
        iSize sz = gGfxMgr.Dimension(sid);
        iRect brc(rc.x, rc.y2() - 109 - 9, rc.w, sz.h);
        
        // !!
        gGfxMgr.BlitTile(sid, gApp.Surface(), brc);
        sz = gGfxMgr.Dimension(PDGG(OVLND_BAR_INFO));
        iRect irc = AlignRect(sz, rc, AlignBottom);
        // !!
        gGfxMgr.Blit(PDGG(OVLND_BAR_INFO), gApp.Surface(), irc);
#endif
        
    }
    void iOverlandToolbar::AfterCompose()
    {
#if 0
        //return;
        iRect rc = GetScrRect();
        //	gApp.Surface().FrameRect(rc, 0);
        sint16 sid = PDGG(OVLND_BAR_MINIMAP);
        iRect mrc = AlignRect(gGfxMgr.Dimension(sid), rc, AlignTopRight) - iPoint(0, 80);
        gGfxMgr.Blit(sid, gApp.Surface(), mrc.point());
        
        iSize sz = gGfxMgr.Dimension(PDGG(OVLND_BAR_INFO));
        iRect irc = AlignRect(sz, rc, AlignBottom);
        
        // !!
        gGfxMgr.Blit(PDGG(OVLND_BAR_INFO_OVERLAY), gApp.Surface(), irc);
        
        ComposeInfoBar();
        
        /* move points and mana */
        if(m_pHero) {
            sint32 barcnt = iCLAMP<sint32>(0, 39, m_pHero->Moves() * 39 / m_pHero->TotalMoves());
            iRect mvrc = iRect(irc.x + 39, irc.y + 55, 6, 39);
            for(sint32 xx=0; xx<barcnt; xx++) {
                gGfxMgr.Blit(PDGG(OVLND_TRAVELPOINT), gApp.Surface(), mvrc + iPoint(0, -1 - xx));
            }
            
            barcnt = iCLAMP<sint32>(0, 39, m_pHero->ManaPts() * 39 / iMAX<sint32>(1, m_pHero->MaxManaPts()));
            mvrc = iRect(irc.x + 62, irc.y + 55, 6, 39);
            for(sint32 xx=0; xx<barcnt; xx++) {
                gGfxMgr.Blit(PDGG(OVLND_MANAPOINT), gApp.Surface(), mvrc + iPoint(0, -1 - xx));
            }
            
        }
        /* hero information */
        if(m_pHero) {
            iRect hrc(rc.x + 50, rc.y + 20, 200, 95);
            iRect crc(hrc.x, hrc.y + 15, hrc.w, gTextComposer.GetFontHeight(castleheronamefc.fontSize));
            gTextComposer.TextBoxOut(castleheronamefc, gApp.Surface(), m_pHero->Name(), crc);
            crc.y += crc.h;
            gTextComposer.TextOut(ovrlndfc_herolvl, gApp.Surface(), iPoint(), iFormat(_T("%s %d"), gTextMgr[TRID_LEVEL], m_pHero->Level()), crc, AlignCenter);
            
            sint32 needed = m_pHero->LevelBound(m_pHero->Level()+1) - m_pHero->LevelBound(m_pHero->Level());
            sint32 acq = m_pHero->Experience() - m_pHero->LevelBound(m_pHero->Level());
            sint32 barcnt;
            sint32 left;
            
            if(m_expPerc == -1)
            {
                barcnt = iCLAMP<sint32>(0, 86, acq * 86 / needed);
                left = needed - acq;
            }
            else
            {
                barcnt = iCLAMP<sint32>(0, 86, m_expPerc * 86 / 100);
                left = needed - (needed * m_expPerc / 100);
            }
            
            
            crc.y += crc.h;
            iRect barrc = AlignRect(gGfxMgr.Dimension(m_expBarSprite), crc, AlignTop) + iPoint(16, 0);
            gGfxMgr.Blit(m_expBarSprite, gApp.Surface(), barrc - iPoint(16, 19));
            
            //gApp.Surface().FrameRect(barrc, 0);
            
            for(sint32 xx=0; xx<barcnt; xx++)
            {
                gGfxMgr.Blit(PDGG(OVLND_EXPPOINT), gApp.Surface(), barrc.point() + iPoint(xx + 7, 5));
            }
            
            gTextComposer.TextOut(ovrlndfc_heroexp, gApp.Surface(), barrc.point() + iPoint(100, 2), iFormat(_T("%d"), left));
            
            crc.y += 20;
            crc.h = 31;
            
            iRect cell = iRect(crc.x, crc.y, crc.w / 6, crc.h);
            
            //gApp.Surface().FrameRect(cell, 0);
            
            gGfxMgr.Blit(PDGG(BTN_ATTACK), gApp.Surface(), GetRealSpritePos(PDGG(BTN_ATTACK), cell, false, AlignTop));
            gTextComposer.TextOut(ovrlndfc_heroskills, gApp.Surface(), iPoint(), iFormat(_T("%d"), m_pHero->FurtSkill(FSK_ATTACK)), cell, AlignBottom);		
            cell.x += cell.w;
            
            gGfxMgr.Blit(PDGG(BTN_HERO), gApp.Surface(), GetRealSpritePos(PDGG(BTN_HERO), cell, false, AlignTop));
            gTextComposer.TextOut(ovrlndfc_heroskills, gApp.Surface(), iPoint(), iFormat(_T("%d"), m_pHero->FurtSkill(FSK_DEFENCE)), cell, AlignBottom);		
            cell.x += cell.w;
            
            gGfxMgr.Blit(PDGG(TAB_MGUILD), gApp.Surface(), GetRealSpritePos(PDGG(TAB_MGUILD), cell, false, AlignTop));
            gTextComposer.TextOut(ovrlndfc_heroskills, gApp.Surface(), iPoint(), iFormat(_T("%d"), m_pHero->FurtSkill(FSK_POWER)), cell, AlignBottom);		
            cell.x += cell.w;
            
            gGfxMgr.Blit(PDGG(TAB_SPELLBOOK), gApp.Surface(), GetRealSpritePos(PDGG(TAB_SPELLBOOK), cell, false, AlignTop));
            gTextComposer.TextOut(ovrlndfc_heroskills, gApp.Surface(), iPoint(), iFormat(_T("%d"), m_pHero->FurtSkill(FSK_KNOWLEDGE)), cell, AlignBottom);		
            
            cell.x += cell.w;
            
            ComposeLuckGlyph(gApp.Surface(), cell, (sint8)m_pHero->FurtSkill(FSK_LUCK), false, false);
            cell.x += cell.w;
            
            ComposeMoraleGlyph(gApp.Surface(), cell, (sint8)m_pHero->FurtSkill(FSK_MORALE), false, false);
            cell.x += cell.w;
        }
        
        /* castle information */
        if(m_pCastle) {
            iRect hrc(rc.x + 50, rc.y + 20, 200, 95);
            iRect crc(hrc.x, hrc.y + 15, hrc.w, gTextComposer.GetFontHeight(castleheronamefc.fontSize));
            gTextComposer.TextBoxOut(castleheronamefc, gApp.Surface(), m_pCastle->Name(), crc);
        }		
        
        
        /* this is a hack for split button to draw on top of overlay */
        m_pSplitBtn->OnCompose();
#endif
    }
    
    /*void iOverlandToolbar::BlinkArmyCell(sint32 idx)
     {
     m_pArmyList->BlinkCell(idx);
     }
     */
    
    void iOverlandToolbar::AnimateExp(sint32 oldperc, sint32 newperc)
    {
#if 0
        //m_expPerc = oldperc;
        //gAniHost.StopAllAnimation(false);
        AniQID q = gAniHost.AddAniObj(new iGAniObj_ChangeSprite(fix32(0.5f), m_expBarSprite, PDGG(OVLND_EXPBAR_HIGHLIGHTED)));
        gAniHost.AddAniObjToQ(new iGAniObj_ChangeSprite(fix32(0.5f), m_expBarSprite, PDGG(OVLND_EXPBAR)), q);
        gAniHost.AddAniObjToQ(new iGAniObj_ChangeSprite(fix32(0.5f), m_expBarSprite, PDGG(OVLND_EXPBAR_HIGHLIGHTED)), q);
        gAniHost.AddAniObjToQ(new iGAniObj_ChangeSprite(fix32(0.5f), m_expBarSprite, PDGG(OVLND_EXPBAR)), q);
        gAniHost.AddAniObjToQ(new iGAniObj_ChangeSprite(fix32(0.5f), m_expBarSprite, PDGG(OVLND_EXPBAR_HIGHLIGHTED)), q);
        gAniHost.AddAniObjToQ(new iGAniObj_ChangeSprite(fix32(0.5f), m_expBarSprite, PDGG(OVLND_EXPBAR)), q);
#endif
    }
    
    void iOverlandToolbar::FinishAnimatingExp()
    {
        //m_expPerc = -1;
        //gAniHost.StopAllAnimation(false);
    }
    
    void iOverlandToolbar::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
    {
        //gGame.MainView()->OnMouseDown(pos, mID);
    }
    
    void iOverlandToolbar::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
    {
        //gGame.MainView()->OnMouseUp(pos, mID);	
    }
    
    void iOverlandToolbar::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
    {
        //gGame.MainView()->OnMouseClick(pos, mID);
    }
    
    void iOverlandToolbar::OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
    {
        //gGame.MainView()->OnMouseDoubleClick(pos, mID);
    }
    
    void iOverlandToolbar::OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
    {
        //gGame.MainView()->OnMouseDown(pos, mID);	
    }
    
    void iOverlandToolbar::SetInfoMode(bool bInfoMode)
    {
        m_pArmyList->SetInfoMode(bInfoMode);
    }
}