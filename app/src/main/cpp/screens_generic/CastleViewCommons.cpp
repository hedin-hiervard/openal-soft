/*
 *  ArmyExchange.cpp
 *  pheroes
 *
 *  Created by Andrew Shilov on 6/8/11.
 *  Copyright 2011 Palm Heroes Team. All rights reserved.
 *
 */

#include "stdafx.h"
#include "ArmyList.touch.h"
#include "CastleView.h"

const sint32 iArmyExchange::sm_defSingleHeight = 45 + 36;
const sint32 iArmyExchange::sm_defDoubleHeight = 100 + 36;

void iArmyExchange::OnCompose()
{
	iRect rc = GetScrRect();
	//gApp.Surface().FrameRect(rc, 0);
#ifndef PC_VERSION
    if(!m_bGUI) {		
		uint32 count = 0;
		if(m_pTopArmy->IsVisible()) count++;
		if(m_pBottomArmy->IsVisible()) count++;
		
		sint16 sid = PDGG(GARRISON_DELUXE);
		iRect trc = AlignRect(gGfxMgr.Dimension(sid), rc, AlignBottom);
		if(count == 2)
			trc += iPoint(0, -(sint32)rc.h / 4 + 5);
		else if(count == 1)
			trc += iPoint(0, -(sint32)rc.h / 4 + 26);
		gGfxMgr.Blit(sid, gApp.Surface(), trc);
     
	}
#endif
}

iArmyExchange::iArmyExchange( iViewMgr* pViewMgr, IViewCmdHandler* pHandler, const iRect& rc, uint32 uid, bool bButtons, bool bSplitButton, bool bSwapProhibited
#ifndef PC_VERSION
, bool bGUI, iSize cellSize
#endif
                             ) : iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled),
m_pCmdHandler(pHandler)
#ifndef PC_VERSION
, m_bGUI(bGUI)
#endif
, m_pSplitBtn(nullptr)
{
	if(bButtons) {
		AddChild(m_pHeroBtn = new iHeroPortBtn(&gApp.ViewMgr(), this, iRect(), 100, true, false, AlignCenter
#ifndef PC_VERSION
                                               , bGUI
#endif
                                            ));
#ifndef PC_VERSION
        AddChild(m_pHeroBtn2 = new iHeroPortBtn(&gApp.ViewMgr(), this, iRect(), 110, true, false, AlignCenter, bGUI));
        m_pHeroBtn2->SetVisible(false);
#endif
        AddChild(m_pCastleBtn = new iCastleGlyphBtn(&gApp.ViewMgr(), this, iRect(), 101, true
#ifndef PC_VERSION
                                                    , bGUI
#endif
                                                    ));
		m_pHeroBtn->SetVisible(false);
		m_pCastleBtn->SetVisible(false);
	} else {
		m_pHeroBtn = NULL;
#ifndef PC_VERSION
        m_pHeroBtn2 = NULL;
#endif
		m_pCastleBtn = NULL;
	}
	if(bSplitButton){
        if(m_bGUI)
        {
			AddChild(m_pSplitBtn = new iCheckButton(&gApp.ViewMgr(), this, iRect(), PDGG(BTN_SPLIT), PDGG(BTN_SPLIT_PRESSED), 102));
        }
		else {
            if(gUseIpadUI)
            {
                AddChild(m_pSplitBtn = new iCheckButton(&gApp.ViewMgr(), this, iRect(), PDGG_IPAD(OVLND_BTN_SPLIT), PDGG_IPAD(OVLND_BTN_SPLIT_PRESSED), 102));
            }
        }
    }
	else
		m_pSplitBtn = NULL;
	AddChild(m_pBottomArmy = new iArmyListEx_touch(&gApp.ViewMgr(), this, iPoint(), m_pSplitBtn, iArmyListEx_touch::sm_defCellSize, 108, iArmyListEx_touch::GT_BOTTOM, bSwapProhibited
#ifndef PC_VERSION
                                                    , bGUI
#endif
                                                   ));
	AddChild(m_pTopArmy = new iArmyListEx_touch(&gApp.ViewMgr(), this, iPoint(), m_pSplitBtn, iArmyListEx_touch::sm_defCellSize, 109, iArmyListEx_touch::GT_TOP, bSwapProhibited
#ifndef PC_VERSION
                                                    , bGUI
#endif
                                                ));
	
	m_pTopArmy->AddCompetitor(m_pBottomArmy);
	m_pBottomArmy->AddCompetitor(m_pTopArmy);
	
	m_pTopArmy->SetVisible(false);
	m_pBottomArmy->SetVisible(false);	
}
#ifdef PC_VERSION
void iArmyExchange::SetSplitBtn( bool flag ){

    m_pSplitBtn->SetChecked(flag);
}
#endif
void iArmyExchange::UpdateSplitBtn()
{
	if(!this || !m_pSplitBtn) return;
	
	uint32 count = 0;
	if(m_pTopArmy->IsVisible()) count++;
	if(m_pBottomArmy->IsVisible()) count++;

#ifndef PC_VERSION
    if(!m_bGUI) {
		if(count == 2)
			m_pSplitBtn->SetRect(iRect(iPoint(m_Rect.x2() - iIphoneBtn::sm_defSize.w + 9, 29), iIphoneBtn::sm_defSize));			
		else if(count == 1)
			m_pSplitBtn->SetRect(iRect(iPoint(m_Rect.x2() - iIphoneBtn::sm_defSize.w + 9, 50), iIphoneBtn::sm_defSize));						
		return;
	}
#endif
    
	m_pSplitBtn->SetSprite(count == 2 ? PDGG(BTN_SPLIT) : PDGG(BTN_SPLIT_SMALL), count == 2 ? PDGG(BTN_SPLIT_PRESSED) : (-1));
	m_pSplitBtn->SetSize(count == 2 ? iSize(30, 77) : iIphoneBtn::sm_defSize);
	
	if(m_pTopArmy->IsVisible())
		m_pSplitBtn->SetPos(iPoint(m_pTopArmy->GetVisualRect().point() + iPoint(m_pTopArmy->GetSize().w + 3, -4)));
	else
		m_pSplitBtn->SetPos(iPoint(m_pBottomArmy->GetVisualRect().point() + iPoint(m_pBottomArmy->GetSize().w + 3, -4)));
	
	if(m_pSplitBtn->GetSize().h == 77) {
		iPoint p = m_pSplitBtn->GetPos();
		p.y += 4;
		m_pSplitBtn->SetPos(p);
	}
	
	/*iPoint p = m_pSplitBtn->GetPos();
	 p.y += m_pBottomArmy->GetSize().h / 4;
	 m_pSplitBtn->SetPos(p);
	 */
	
	uint32 ttl = 0;
	if(m_pBottomArmy->ArmyOwner()) ttl += m_pBottomArmy->ArmyOwner()->TotalCreatures();
	if(m_pTopArmy->ArmyOwner()) ttl += m_pTopArmy->ArmyOwner()->TotalCreatures();
	m_pSplitBtn->SetVisible(ttl > 1);
}

void iArmyExchange::UpdateSize()
{
    if(!this) return;
	iPoint offset = iPoint(0,0); 
    iPoint boffset = iPoint(0,0);

#ifndef PC_VERSION
    if(!m_bGUI) {
		offset = iPoint(-2, -2);
		boffset = iPoint(0, 4);
	}
#endif
    
	if(m_pTopArmy->IsVisible()) {
		m_pBottomArmy->SetRect(AlignChild(m_pBottomArmy->GetSize(), AlignBottom) + offset);
		m_pBottomArmy->SetGT(iArmyListEx_touch::GT_BOTTOM);
	} else {
		m_pBottomArmy->SetRect(AlignChild(m_pBottomArmy->GetSize(), AlignCenter) + offset);
		m_pBottomArmy->SetGT(iArmyListEx_touch::GT_MIDDLE);
	}
	
	if(m_pBottomArmy->IsVisible()) {
		m_pTopArmy->SetRect(AlignChild(m_pTopArmy->GetSize(), AlignTop) + offset
#if defined(OS_IPHONE) || defined(OS_ANDROID)
                            + iPoint(0, -2) 
#endif
                            );	
		m_pTopArmy->SetGT(iArmyListEx_touch::GT_TOP);
	} else {
		m_pTopArmy->SetRect(AlignChild(m_pTopArmy->GetSize(), AlignCenter) + offset
#if defined(OS_IPHONE) || defined(OS_ANDROID)
                            + iPoint(0, -1) 
#endif
                            );	
		m_pTopArmy->SetGT(iArmyListEx_touch::GT_MIDDLE);
	}
	
    if(m_pHeroBtn)
		m_pHeroBtn->SetRect(AlignRect(iIphoneBtn::sm_defSize, m_pBottomArmy->GetVisualRect(), AlignTopLeft) 
#ifdef PC_VERSION
                            - iPoint(iIphoneBtn::sm_defSize.w, 6)
#else
                            + iPoint(-iIphoneBtn::sm_defSize.w + 12-5, -6) + boffset
#endif
                            );	
	if(m_pCastleBtn)
		m_pCastleBtn->SetRect(AlignRect(iIphoneBtn::sm_defSize, m_pTopArmy->GetVisualRect(), AlignTopLeft) 
#ifdef PC_VERSION
                              - iPoint(iIphoneBtn::sm_defSize.w, 6)
#else
                              + iPoint(-iIphoneBtn::sm_defSize.w + 12-5, -6) + boffset
#endif
                              );
#ifndef PC_VERSION
    if(m_pHeroBtn2)
		m_pHeroBtn2->SetRect(AlignRect(iIphoneBtn::sm_defSize, m_pTopArmy->GetVisualRect(), AlignTopLeft) + iPoint(-iIphoneBtn::sm_defSize.w + 12-5, -10) + boffset);	
#endif

	UpdateSplitBtn();
}

void iArmyExchange::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
{
	uint32 uid = pView->GetUID();
	if(uid == 100 || uid == 101)
		m_pCmdHandler->iCMDH_ControlCommand(this, cmd, uid == 100 ? BT_HERO : BT_CASTLE);
	else if(uid == 102 && cmd == CCI_CHECKED)
		m_pCmdHandler->iCMDH_ControlCommand(this, CCI_CHECKED, param);
	else if(uid == 108)
		m_pCmdHandler->iCMDH_ControlCommand(this, CCI_LBSELCHANGED, m_pBottomArmy->SelectedItem());
	else if(uid == 109)
		m_pCmdHandler->iCMDH_ControlCommand(this, CCI_LBSELCHANGED, m_pTopArmy->SelectedItem());
	
	UpdateSplitBtn();
}

void iArmyExchange::SetBottomOwner( iArmyOwner *pOwner, bool bCanDismiss )
{
	m_pBottomArmy->SetArmy(pOwner, bCanDismiss);
	if(m_pHeroBtn && pOwner && pOwner->GetArmyOwnerHero()) {
		m_pHeroBtn->SetHero(pOwner->GetArmyOwnerHero());
		m_pHeroBtn->SetVisible(true);
	} else if(m_pHeroBtn)
		m_pHeroBtn->SetVisible(false);
	
	m_pBottomArmy->SetVisible(pOwner != NULL);	
	
	Deselect();
	UpdateSize();
	UpdateSplitBtn();
}

void iArmyExchange::SetTopOwner(iArmyOwner *pOwner, bool bCanDismiss )
{
	m_pTopArmy->SetArmy(pOwner, bCanDismiss);
	m_pTopArmy->SetVisible(pOwner != NULL);
	if(m_pCastleBtn && pOwner && pOwner->GetArmyOwnerCastle()) {
		m_pCastleBtn->SetVisible(true);
		m_pCastleBtn->SetCastle(pOwner->GetArmyOwnerCastle());
	} else if(m_pCastleBtn) {
		m_pCastleBtn->SetVisible(false);
	}
	
#ifndef PC_VERSION
    if(m_pHeroBtn2 && pOwner && pOwner->GetArmyOwnerHero()) {
		m_pHeroBtn2->SetVisible(true);
		m_pHeroBtn2->SetHero(pOwner->GetArmyOwnerHero());
	} else {
		m_pHeroBtn2->SetVisible(false);
	}
#endif
    
	Deselect();
	UpdateSize();
	UpdateSplitBtn();
}

iSize iArmyExchange::GetDefSize(uint32 count)
{
	sint32 w = iIphoneBtn::sm_defSize.w + 3 + iArmyListEx_touch::sm_defCellSize.w * 7 + iIphoneBtn::sm_defSize.w;
	sint32 h = count == 2 ? iArmyListEx_touch::sm_defCellSize.h * 4 + 4 : iArmyListEx_touch::sm_defCellSize.h * 2;
	
	return iSize(w, h);
}

void iArmyExchange::AddCompetitor( iArmyListEx* comp )
{
	m_pTopArmy->AddCompetitor(comp); m_pBottomArmy->AddCompetitor(comp);
}

void iArmyExchange::SetSplitChecked( sint32 checked )
{
	if(!m_pSplitBtn) return;
	m_pSplitBtn->SetChecked(checked, false);
}

void iArmyExchange::Deselect()
{
	m_pTopArmy->SetSelectedItem(-1);
	m_pBottomArmy->SetSelectedItem(-1);
}

iRect iArmyExchange::GetDismissRect()
{
    if(m_pHeroBtn)
		return m_pHeroBtn->GetRect() - iPoint(iIphoneBtn::sm_defSizeDeluxe.w, 0);
    else
        return AlignRect(iIphoneBtn::sm_defSizeDeluxe, m_pBottomArmy->GetVisualRect(), AlignBottomLeft)  - iPoint(iIphoneBtn::sm_defSizeDeluxe.w, 0);
    
//    if(!this) return cInvalidRect;
//	
//	else if(m_pBottomArmy &&  m_pBottomArmy->ArmyOwner() && m_pBottomArmy->ArmyOwner()->GetArmyOwnerHero())
//		return AlignRect(iIphoneBtn::sm_defSizeDeluxe, m_pBottomArmy->GetVisualRect(), AlignBottomLeft)
//#ifdef PC_VERSION
//        - iPoint(iIphoneBtn::sm_defSizeDeluxe.w + 5, 0);
//#else
//        + iPoint(-iIphoneBtn::sm_defSizeDeluxe.w + 1, -13);
//#endif
//	else if(m_pTopArmy && m_pTopArmy->ArmyOwner() && m_pTopArmy->ArmyOwner()->GetArmyOwnerHero())
//		return AlignRect(iIphoneBtn::sm_defSize, m_pTopArmy->GetVisualRect(), AlignBottomLeft) 
//#ifdef PC_VERSION
//        - iPoint(iIphoneBtn::sm_defSizeDeluxe.w + 5, 6);
//#else
//        + iPoint(-iIphoneBtn::sm_defSizeDeluxe.w + 1, -8);
//#endif
//	else
//		return cInvalidRect;
}

void iArmyExchange::SetSplitBtn( iCheckButton* pSplitBtn )
{
	m_pTopArmy->SetSplitBtn(pSplitBtn); 
	m_pBottomArmy->SetSplitBtn(pSplitBtn);
}




iMGuildView::iMGuildView(iViewMgr* pViewMgr, iRect rc, uint32 uid): iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled)
{
#ifdef PC_VERSION
	sint32 cellW, cellH;
	cellW = 75;
	cellH = 45;
#endif
	uint32 xx=0;
	for (uint32 lvl = 0; lvl<5; ++lvl){
		uint32 scnt = 5-lvl;
#ifdef PC_VERSION
		iPoint pos(m_Rect.w / 2 - (cellW*scnt+3*(scnt-1))/2, 1 + (5-lvl-1)*cellH);	
		pos.y += 20;
#endif
		for (uint32 spid=0; spid<scnt; ++spid) {
			AddChild(m_pSpellBtn[xx] = new iSpellBtn(pViewMgr,this
#ifdef PC_VERSION
                                                     ,iRect(pos.x,pos.y,cellW,cellH)
#else
                                                     ,iRect()
#endif
                                                     ,110 + xx));
			++xx;
#ifdef PC_VERSION
			pos.x += cellW+3;
#endif
		}
	}
}

void iMGuildView::OnCompose()
{	
	iRect rc = GetScrRect();
#ifdef PC_VERSION
//	ComposeGlowBkgnd(gApp.Surface(), rc);	
#endif
//	rc.y += 25;
    if(!gUseIpadUI)
        gTextComposer.TextBoxOut(castletopicfc, gApp.Surface(), gTextMgr[TRID_CTLCNST_COMMON_MGUILD], rc);
}

void iMGuildView::UpdateSize()
{
	iRect rc = GetScrRect();
    sint32 offs = 0;
#ifdef PC_VERSION
    offs += 60;
#endif
                    
	sint32 cellW = (m_Rect.w - offs) / 5;
	sint32 cellH = (m_Rect.h - offs) / 5;
	uint32 xx=0;
	sint32 offset = 0;
#ifdef PC_VERSION
	offset += gTextComposer.GetTextBoxSize(gTextMgr[TRID_CTLCNST_COMMON_MGUILD], rc.w, castletopicfc).h;
#endif
	for (uint32 lvl = 0; lvl<5; ++lvl){
		uint32 scnt = 5-lvl;
		iPoint pos(m_Rect.w / 2 - (cellW*scnt+3*(scnt-1))/2, offset + 1 + (5-lvl-1)*cellH);
		
		for (uint32 spid=0; spid<scnt; ++spid) {
			iView *p = 
#ifdef PC_VERSION
            GetChildById(110 + xx);
#else
            m_pSpellBtn[xx];
#endif
			if (p) 
			{
				p->SetRect(iRect(pos.x,pos.y,cellW,cellH));
				p->UpdateSize();
				p->Invalidate();
			}
			++xx;
			pos.x += cellW+3;
					
		}
	}
}

void iMGuildView::SetCastle(iCastle *pCastle)
{
	m_pCastle = pCastle;
	Update();
}

void iMGuildView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid >= 110 && uid < 125) {
		iIconDlg dlg(&gApp.ViewMgr(), gTextMgr[m_pSpellBtn[uid-110]->GetSpell()->NameKey()], gTextMgr[m_pSpellBtn[uid-110]->GetSpell()->DescKey(MSL_NONE)], m_pSpellBtn[uid-110]->GetSpell()->Icon(),m_pCastle->Owner());
		dlg.DoModal();
	}
}

void iMGuildView::Update()
{
	if (m_pCastle) {
		uint32 xx=0;
		for (uint32 lvl=0; lvl<5; ++lvl) {
			iCtlCnst* pCnst = m_pCastle->FindCnst((CTL_CONSTR)(CTLCNST_MAGEGUILD_L1+lvl));
			uint32 tcnt = (5-lvl);
			if (pCnst && pCnst->IsBuilt()) {
				iMGuildCtlCnst* pMGuild = DynamicCast<iMGuildCtlCnst*>(pCnst);
				check(pMGuild);
				uint32 spid;
				for (spid=0; spid < pMGuild->SpellsCount(); ++spid, ++xx) m_pSpellBtn[xx]->SetSpell(gGame.ItemMgr().m_spellMgr.Spell(pMGuild->Spell(spid)));
				for (uint32 fsp=spid; fsp<tcnt; ++fsp, ++xx) m_pSpellBtn[xx]->SetSpell(NULL);
			} else {
				for (uint32 fsp=0; fsp<tcnt; ++fsp, ++xx) m_pSpellBtn[xx]->SetSpell(NULL);
			}
		}
	} else {
		for (uint32 xx=0; xx<15; ++xx) m_pSpellBtn[xx]->SetSpell(NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
iRecruitHeroView::iRecruitHeroView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
: iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pCastle(NULL), m_pTavern(NULL), m_pCmdHandler(pCmdHandler)
{           
	AddChild(m_pRecrBtn = new iImgBtn(m_pMgr, this, iRect(), 110, PDGG(BTN_BUY_DELUXE), -1));        
	m_pRecrBtn->SetVisible(false);
}

void iRecruitHeroView::UpdateSize()
{
	m_pRecrBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottom) + iPoint(-10, -70));	
}

void iRecruitHeroView::SetCastle(iCastle* pCastle)
{
	m_pCastle = pCastle;	
	Update();
};

void iRecruitHeroView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{ 
	uint32 uid = pView->GetUID();
	if (uid == 110) 
	{
		if(m_pTavern->CanRecruit())
			m_pTavern->RecruitVisitor();		
		// SHL TODO!!! gGame.CastleView()->SetViewMode(iCastleView::CVM_MAIN);
	}
	if(m_pCmdHandler)
		m_pCmdHandler->iCMDH_ControlCommand(this, cmd, param);
	
};
#ifdef PC_VERSION
void iRecruitHeroView::SetHighlighted(bool flag){

    m_pRecrBtn->SetHighlighted(flag);
}
#endif
void iRecruitHeroView::OnCompose()
{
	iRect rc = GetScrRect();
    
    if(rc.w < 400) {
        iRect bkrc = rc;
        bkrc.DeflateRect(0, 0, 5, 0);
        gApp.Surface().Darken25Rect(bkrc);
        bkrc.InflateRect(1);
        gApp.Surface().FrameRect(bkrc, cColor_Black);
    }

    
	//rc.y += 25;
	if (!m_pCastle) return;
	check(m_pTavern && m_pTavern->IsBuilt());	
	
	rc.DeflateRect(10, 0);
	
	if (const iHero* pVisitor = m_pTavern->Visitor()) 
	{
		iPoint offset(10, 0);
		// draw avatar and exp
		iRect tmp = AlignRect(iSize(rc.w/2, 48), rc, AlignTopLeft);
		iRect avrc = AlignRect(iSize(48, 48), tmp, AlignRight) - offset + iPoint(0, 40);		
		
		ComposeHeroAvatar(gApp.Surface(), avrc, pVisitor);
		
		tmp = AlignRect(iSize(rc.w/2, 48), rc, AlignTopRight);
		
		iRect exprc = tmp + offset  + iPoint(0, 40);
		exprc.w = 50;
		iSize siz = gGfxMgr.Dimension(PDGG(DGLYPH_EXP));
		
		sint32 opx = exprc.x + (sint32)(exprc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(PDGG(DGLYPH_EXP)).x;
		sint32 opy = exprc.y + (sint32)(exprc.h/2) - (sint32)(siz.h/2) - gGfxMgr.Anchor(PDGG(DGLYPH_EXP)).x - 2;
		
		ComposeHeroLevel(gApp.Surface(), iRect(opx, opy, exprc.w, exprc.h), pVisitor);		
		
		rc.y += 48;
		// print name		
		avrc.y += 55;
		avrc.InflateRect(50, 0);		
		avrc.h = gTextComposer.GetFontHeight(castleheronamefc.fontSize);
		gTextComposer.TextBoxOut(castleheronamefc, gApp.Surface(), pVisitor->Name(), avrc);
		
		// cost		
		rc.y += avrc.h;
		iRect tmprc = iRect(rc.x, m_pRecrBtn->GetScrPos().y + DEF_BTN_HEIGHT, rc.w, 50);
		/*ComposeMineralSet(pVisitor->Proto()->m_Cost, gGame.Map().FindPlayer(m_pCastle->Owner())->Minerals(), AlignRect(GetMineralSetComposeSize(pVisitor->Proto()->m_Cost, false), tmprc, AlignBottom), false
#ifndef PC_VERSION
                          , false, true
#endif
                          );
         */
//        void iTextComposer::TextOut(const FontConfig &fcc, iDib& dib, const iPoint& pos, const iStringT& text, const iRect& rect, Alignment al, const iPoint& offset)

        iPlayer *pPlayer = gGame.Map().FindPlayer(m_pTavern->Owner()->Owner());
        if(m_pRecrBtn->IsVisible())
            gTextComposer.TextOut(minsetfc, gApp.Surface(), iPoint(), iFormat(_T("1500")), tmprc, AlignBottom);
        else {
            if(m_pCastle->Visitor()) {
                iSize sz = gTextComposer.GetTextBoxSize(gTextMgr[TRID_MSG_TOWN_OCCUPIED], rc.w, minsetfc);
                gTextComposer.TextBoxOut(minsetfc, gApp.Surface(), gTextMgr[TRID_MSG_TOWN_OCCUPIED], AlignRect(sz, tmprc, AlignBottom));            
            }
            else if(!pPlayer->Minerals().Has(m_pTavern->Visitor()->Proto()->m_Cost))
            {  
                iMineralSet cost = m_pTavern->Visitor()->Proto()->m_Cost;
                ComposeMineralSet(cost, pPlayer->Minerals(), AlignRect(GetMineralSetComposeSize(cost, true), tmprc, AlignBottom), true);		
            } else {
                iSize sz = gTextComposer.GetTextBoxSize(gTextMgr[TRID_MSG_NO_MORE_HEROES_ALLOWED], rc.w, minsetfc);
                gTextComposer.TextBoxOut(minsetfc, gApp.Surface(), gTextMgr[TRID_MSG_NO_MORE_HEROES_ALLOWED], AlignRect(sz, tmprc, AlignBottom));
            }
        }
	} 
	else 
	{		
		iSize sz = gTextComposer.GetTextBoxSize(gTextMgr[TRID_MSG_NO_HERO_TO_RECRUIT], rc.w, dlgfc_plain);
		rc.y += (rc.h - sz.h) / 2;
		gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), gTextMgr[TRID_MSG_NO_HERO_TO_RECRUIT], rc);
	}
	
};

void iRecruitHeroView::Update()
{
	if (m_pCastle)         
		m_pTavern = m_pCastle->GetTavern(true);
	else 
		m_pTavern = NULL;            
	
	m_pRecrBtn->SetVisible(m_pTavern && m_pTavern->CanRecruit());	
}



iRecrSet::iRecrSet(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, uint32 uid, const iRect& rc)
: iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled)
, m_pCmdHandler(pCmdHandler), m_pCastle(NULL), m_selectedCT(CREAT_UNKNOWN)
{	
	for (uint32 xx=0; xx<6; ++xx) {
		AddChild(m_pBtns[xx] = new iRecrButton(pViewMgr, this, iRect(), 100 + xx, xx));	
	}
}

void iRecrSet::SetCastle(iCastle* pCastle)
{
	m_pCastle = pCastle;
	m_selectedCT = CREAT_UNKNOWN;
}

void iRecrSet::UpdateSize()
{        
	iRect rc = GetScrRect();
	iRect crc = iRect(0, 0, rc.w / 6, rc.h);
	for (uint32 xx=0; xx<6; ++xx) {
		m_pBtns[xx]->SetRect(crc);
		crc.x += rc.w / 6;
	}
	
}


void iRecrSet::SetSelectedCT(CREATURE_TYPE ct) {
	m_selectedCT = ct;
	for (uint32 xx=0; xx<6; ++xx) {
		m_pBtns[xx]->SetSelected(m_pBtns[xx]->Dwelling() && m_pBtns[xx]->Dwelling()->CrType() == ct);
	}		
    Update();
}

/* Recruit button */
iRecrButton::iRecrButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 idx)
: iButton(pViewMgr, pCmdHandler, rect, uid, Enabled|Visible), m_pDwel(NULL), m_bSelected(false), m_idx(idx)
{
}


void iRecrButton::SetDwelling(iDwellCtlCnst* pDwel)
{
	m_pDwel = pDwel;
	Invalidate();
}


void iRecrButton::OnCompose()
{
	iRect rc = GetScrRect();
	
	uint16 clr = RGB16(144, 160, 152);
    iRect rctmp = rc;
    rctmp.w += 5;
	iRect bot = AlignRect(iSize(rc.w - (m_idx == 5 ? 14 : 0), 10), rctmp, AlignBottom) + (m_idx < 5 ? iPoint(7, 1) : iPoint(0, 1));
	gGfxMgr.BlitTile(PDGG(DLG_HTILES), gApp.Surface(), bot);
	
	if(m_idx < 5) {
		iRect right = AlignRect(iSize(10, rc.h - 10 - 3), rc, AlignRight) + iPoint(0, 1);
		gGfxMgr.BlitTile(PDGG(DLG_VTILES), gApp.Surface(), right);
	}
	
	if(m_bSelected) {
		iRect bkrect(rc);
		if(m_idx > 0)
			bkrect.DeflateRect(0, 5, 5, 4);//bkrect.x -= 3;
		else
			bkrect.DeflateRect(5, 0, 0, 0);
			bkrect.DeflateRect(0, 7, 5, 5);				
		
		if(m_idx == 0) {
			uint16 sid = PDGG(DLG_CORNERS);//PDGG(DLG_CORNERS_PRESSED);
			iSize sz = gGfxMgr.Dimension(sid);
			gGfxMgr.Blit(sid, gApp.Surface(), AlignRect(sz, rc, AlignTopLeft));	
#ifndef PC_VERSION
            if(bkrect.w > 10)
                bkrect.DeflateRect(5, 0, 0, 0);
#else
            bkrect.DeflateRect(7, 5, 5, 4);
#endif
			gApp.Surface().FillRect(bkrect, clr);	
            if(!gUseIpadUI)
            {
                iRect left = iRect(bkrect.x - 8, bkrect.y + sz.h - 7, sz.w - 5, bkrect.h - sz.h + 7);
                gApp.Surface().FillRect(left, clr);
            }
		}
		else if(m_idx == 5) {			
			uint16 sid = PDGG(DLG_CORNERS)+1;//PDGG(DLG_CORNERS_PRESSED) + 1;
			iSize sz = gGfxMgr.Dimension(sid);
            if(!gUseIpadUI)
                bkrect.DeflateRect(0, 0, 10, 0);
            else
                bkrect.DeflateRect(0, 0, -2, 0);

			gGfxMgr.Blit(sid, gApp.Surface(), AlignRect(sz, rc, AlignTopRight) + iPoint(4, 0));
			gApp.Surface().FillRect(bkrect, clr);	
            if(!gUseIpadUI)
            {
                iRect right = iRect(bkrect.x2(), bkrect.y + sz.h - 7, sz.w - 5, bkrect.h - sz.h + 7);
                gApp.Surface().FillRect(right, clr);
            }
		} else 
			gApp.Surface().FillRect(bkrect, clr);	
	} else {
		if(m_idx == 0) {
			uint16 sid = PDGG(DLG_CORNERS);
			iSize sz = gGfxMgr.Dimension(sid);
			gGfxMgr.Blit(sid, gApp.Surface(), AlignRect(sz, rc, AlignTopLeft));						
		}
		else if(m_idx == 5) {			
			uint16 sid = PDGG(DLG_CORNERS) + 1;
			iSize sz = gGfxMgr.Dimension(sid);			
			gGfxMgr.Blit(sid, gApp.Surface(), AlignRect(sz, rc, AlignTopRight) + iPoint(4, 0));			
		}
	}
	
	rc.y += 10;
	
	if (m_pDwel) {				
		// Name
		rc.y += gTextComposer.TextBoxOut(creatinfofc_qty, gApp.Surface(), iStringT(gTextMgr[m_pDwel->CrType()*3+TRID_CREATURE_PEASANT_F2]), rc);
		
		// Creature icon
		SpriteId sid = PDGG(MINIMON) + m_pDwel->CrType();
		iRect icRect = AlignRect(iSize(55, 55), rc, AlignTopLeft);
		iPoint sprpos = GetRealSpritePos(sid, icRect, false, AlignBottom) - iPoint(0, 10);
		gGfxMgr.Blit(sid,gApp.Surface(), sprpos);
		
		sint32 icx2 = sprpos.x + gGfxMgr.ObjRect(sid).w;
		
		iRect trc = iRect(iPoint(icx2 + 5, icRect.y), iPoint(rc.x2(), icRect.y2()));
		
		// Available
		trc.y += gTextComposer.TextBoxOut(creatinfofc_small, gApp.Surface(), iFormat(_T("%s: %d"), gTextMgr[TRID_AVAILABLE], m_pDwel->CrCount()), trc);		
		
		// Growth
		trc.y += gTextComposer.TextBoxOut(creatinfofc_small, gApp.Surface(), iFormat(_T("%s: %d"), gTextMgr[TRID_GROWTH], m_pDwel->Growth()), trc);		
		
		iSize sz = gTextComposer.GetTextSize( iFormat(_T("%s: %d"), gTextMgr[TRID_GROWTH], m_pDwel->Growth()), creatinfofc_small);
		trc.x += (trc.w - sz.w) / 2;
		trc.y += 5;
		
		// Cost
		//iSize sz = gTextComposer.GetTextSize(iStringT(MineralSet2Text(CREAT_DESC[m_pDwel->CrType()].cost)), creatinfofc_small);
		gTextComposer.TextOut(creatinfofc_small, gApp.Surface(), trc.point(), iStringT(MineralSet2Text(CREAT_DESC[m_pDwel->CrType()].cost)));		
	}
}


void iRecrSet::OnCompose()
{
	iRect rc = GetScrRect();	
}

void iRecrSet::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	check(m_pCastle);
	check(m_pBtns[pView->GetUID()-100]->Dwelling());
	iDwellCtlCnst* pDwel = m_pBtns[pView->GetUID()-100]->Dwelling();
	iCreatGroup cg(pDwel->CrType(), pDwel->CrCount());
	m_pCmdHandler->iCMDH_ControlCommand(this, cmd, pDwel->CrType());
#ifdef	MULTIPLAYER
	gMPMgr.OnCastleUpdate(m_pCastle);	
#endif
	
}

void iRecrSet::Update()
{
	sint32 c=0;
	for (uint32 xx=0; xx<m_pCastle->Cnsts().Count(); ++xx) {
		if (iDwellCtlCnst* pDwel = DynamicCast<iDwellCtlCnst*>(m_pCastle->Cnst(xx))) {
			if (pDwel->IsBuilt()) {
				check(c<6);
				m_pBtns[c]->SetEnabled(true);
				m_pBtns[c]->SetDwelling(pDwel);			
			} else {
				m_pBtns[xx]->SetEnabled(false);
				m_pBtns[xx]->SetDwelling(NULL);
			}
			c++;
		}
	}	
	for(uint32 xx=c; xx<6; xx++)
	{
		m_pBtns[xx]->SetEnabled(false);
		m_pBtns[xx]->SetDwelling(NULL);
	}
}

iDockView::	iDockView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 dt)
: iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pHero(NULL), m_CurState(DS_BUILD), m_dock_type(dt),m_pCmdHandler(pCmdHandler),
m_pHarbor(NULL)
{			
	AddChild(m_pBtn = new iImgBtn(pViewMgr, this, iRect(), 102, PDGG(BTN_SPLIT_SMALL), -1, iStringT(), Visible | Enabled));
	SwitchState(DS_CANT);
}

/*void iDockView::UpdateSize()
 {
 m_pBtn->SetRect(iRect((m_Rect.w - 80) / 2, m_Rect.h - 2 * DEF_BTN_HEIGHT, 80, DEF_BTN_HEIGHT));
 m_pBtn->Invalidate();
 }
 */

iPlayer* iDockView::GetOwner()
{
	return m_pHarbor->GetOwner(); //m_pHero ? m_pHero->Owner() : NULL;
}

void iDockView::OnCompose()
{
	SpriteId sid, sidbase;
	iRect rc = GetScrRect();
	
    if(rc.w < 400) {
        iRect bkrc = rc;        
        bkrc.DeflateRect(5, 0, 0, 0);
        gApp.Surface().Darken25Rect(bkrc);
        bkrc.InflateRect(1);
        gApp.Surface().FrameRect(bkrc, cColor_Black);
    }

    
	rc.y += iBaseGameDlg::GetTopOffset();
	
	iPlayer *pPlayer = GetOwner() ? GetOwner() :  (m_pHero ? m_pHero->Owner() : NULL);
	check(pPlayer);
	
	
	sidbase = SHIP_TYPE_DESC[m_dock_type].sprite;
	
	if (m_CurState == DS_BUILD)
	{
		sid = sidbase + 37;
		iRect spr = AlignRect(gGfxMgr.Dimension(sid), rc, AlignTop) - gGfxMgr.Anchor(sid);		
		gGfxMgr.Blit(sid, gApp.Surface(), spr);
		rc.y += spr.h;
		iMineralSet cost = SHIP_TYPE_DESC[m_dock_type].cost;
		iRect tmprc = iRect(rc.x, m_pBtn->GetScrRect().y2() + 10, rc.w, 100);
		ComposeMineralSet(cost, pPlayer->Minerals(), AlignRect(GetMineralSetComposeSize(cost, false), tmprc, AlignTop), false);		
		
	}
	else if (m_CurState == DS_OCCUPY)
	{
		sid = sidbase + 43;
		gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), gTextMgr[TRID_MSG_HARBOR_BOARD], iRect(rc.x + 5, rc.y + 10, m_Rect.w - 10, 20));            
		iPoint pts = iPoint(rc.x + rc.w / 2, rc.y + rc.h / 2);                     
		
		
		/* TODO: return the image of the harbor to this screen
		 if (m_is_harbor)
		 {
		 iIsoMetric im(im_coef);
		 iVisCnst_Harbor *vsh = (iVisCnst_Harbor*)(m_pHero->GetLocation());                
		 const iVisCnstT* pCnstT = vsh->Proto();
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
		 
		 } */
		gGfxMgr.Blit(sid, gApp.Surface(), pts + iPoint(-60, -80));
	}
	else if (m_CurState == DS_BUSY)
	{
		sid = sidbase + 2;
		gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), gTextMgr[TRID_MSG_HARBOR_OCCUPIED], iRect(rc.x + 5, rc.y + 120, m_Rect.w - 10, 50));
		iSize sz = gGfxMgr.Dimension(sid);
		gGfxMgr.BlitTile(sid, gApp.Surface(), iRect(35 + rc.x + (rc.w - sz.w) / 2, 69 + rc.y + 20, sz.w, sz.h));
	}
	else if (m_CurState == DS_CANT)
	{
		iRect rc = GetScrRect();
		iSize sz = gTextComposer.GetTextBoxSize(gTextMgr[TRID_MSG_HARBOR_NOSHIP], rc.w, dlgfc_plain);
		rc.y += (rc.h - sz.h) / 2;
		gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), gTextMgr[TRID_MSG_HARBOR_NOSHIP], rc);            
	}
}

void iDockView::SetHarbor(IHarbor* pHarbor)
{
	m_pHarbor = pHarbor;
	UpdateControls();
}

void iDockView::SetHero(iHero *pHero)
{
	m_pHero = pHero;
	UpdateControls();
}

void iDockView::UpdateControls()
{
	if(!m_pHarbor) { SwitchState(DS_CANT); return; }
	
	if (m_pHarbor->IsBoardable() && m_pHarbor->GetShip(m_dock_type) && m_pHarbor->GetShip(m_dock_type)->IsLoaded())
		SwitchState(DS_BUSY);
	else if(m_pHarbor->IsBoardable() && m_pHarbor->GetShip(m_dock_type))
		SwitchState(DS_OCCUPY);
	else if(!m_pHarbor->GetShip(m_dock_type) && m_pHarbor->ShipsAvailable(m_dock_type) > 0 && m_pHarbor->GetShipPos(m_dock_type) != cInvalidPoint)
		SwitchState(DS_BUILD);
	else
		SwitchState(DS_CANT);	
}


void iDockView::BuildShip()
{
	iPoint pt = m_pHarbor->GetShipPos(m_dock_type);
	iPlayer *pPlayer = GetOwner() ? GetOwner() :  (m_pHero ? m_pHero->Owner() : NULL);
	check(pPlayer)
	
	// calculate ship coordinates	
	if (pt == cInvalidPoint)
	{
		return;		
	}
	
	// decrease player resources
	pPlayer->AddMineralSet(-SHIP_TYPE_DESC[m_dock_type].cost);
	// decrease available ship count
	m_pHarbor->ShipBought(m_dock_type);        
	
	gGame.Map().AddShip(pt, m_dock_type);
	// update UI state
	UpdateControls();
}    


void iDockView::OccupyShip()
{
	check(m_pHero && m_pHarbor->IsBoardable());
	
	iShip* pShip = m_pHarbor->GetShip(m_dock_type);
	
	if(pShip)               
		pShip->Occupy(m_pHero);        			
}


void iDockView::SwitchState(DOCK_STATE NewState)
{
	// switch button label and visibility
	switch (NewState)
	{
		case DS_NONE:
			check(0);
			break;
		case DS_BUILD:
		{	
			iPlayer* pPlayer = GetOwner();
			iMineralSet cost = SHIP_TYPE_DESC[m_dock_type].cost;
			sint32 has = pPlayer->Minerals().Has(cost);
			m_pBtn->SetVisible(has > 0);
			((iImgBtn*)m_pBtn)->SetSprite(PDGG(BTN_BUY_DELUXE), -1);			
		}
			break;
		case DS_OCCUPY:		
			m_pBtn->SetVisible(true);
			break;
		case DS_BUSY:
			m_pBtn->SetVisible(false);
			break;
		case DS_CANT:
			m_pBtn->SetVisible(false);
			break;
	}
	m_CurState = NewState;
	Invalidate();
}

void iDockView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if ( uid == 102 ) 
	{
		if (m_CurState == DS_BUILD)
		{
			BuildShip();			
		}
		else if (m_CurState == DS_OCCUPY)
		{
			OccupyShip();
			gGame.HideView(iChildGameView::CV_HARBOR);
		}
	} 
}

void iDockView::UpdateSize()
{
	m_pBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottom)
#ifdef PC_VERSION
                        - iPoint(0, 15));	
#else
    + iPoint(-10, -70));	
#endif
}


/* Market view */
iMarketPanel::iMarketPanel( iViewMgr* pViewMgr, IViewCmdHandler* pHandler, const iRect& rc, uint32 uid, PanelType type) : iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled), 
m_pCmdHandler(pHandler), m_selResource(-1), m_type(type)
{
	for(uint32 xx=0; xx<MINERAL_COUNT; xx++)
		AddChild(m_pBtns[xx] = new iMineralBtn(pViewMgr, this, iRect(), 100 + xx, (MINERAL_TYPE)(MINERAL_GOLD + xx)));
}

void iMarketPanel::UpdateSize()
{
	iRect rc = GetRect();
	
	iRect crc = iRect(iPoint(), iSize(rc.w / 3, rc.h / 3));
	for(uint32 xx=0; xx<MINERAL_COUNT; xx++) {
		m_pBtns[xx]->SetRect(crc);
		crc.x += crc.w;
		if(crc.x2() >= (sint32)rc.w) {
			if(m_type == MP_LEFT || xx < MINERAL_COUNT - 2) // the alignment of the last (incomplete) row differs
				crc.x = 0;
			else {
				crc.x = rc.w * 2 / 3;
			}
			crc.y += crc.h;
		}
	}
}

void iMarketPanel::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	Deselect();
	((iMineralBtn*)pView)->SetState(true);
	m_selResource = pView->GetUID() - 100;
	m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, m_selResource);
}

MINERAL_TYPE iMarketPanel::SelectedMineral()
{
	return (MINERAL_TYPE)m_selResource;
}

void iMarketPanel::Deselect()
{
	if(m_selResource != -1)
		m_pBtns[m_selResource]->SetState(false);
	m_selResource = -1;
}



void iMarketPanel::UpdateAsLeftSide(const iMineralSet &ms)
{		
	for (uint32 xx=0; xx<MINERAL_COUNT; ++xx) {
		m_pBtns[xx]->SetQuant( FractionCoeff(ms.quant[xx], 1));
#ifndef PC_VERSION
        m_pBtns[xx]->SetEnabled(ms.quant[xx] > 0);
#endif 
	}
}

void iMarketPanel::UpdateAsRightSide(MINERAL_TYPE from, sint32 marketIdx)
{
	if (from == MINERAL_UNKNOWN) {
		for (uint32 xx=0; xx<MINERAL_COUNT; ++xx) m_pBtns[xx]->SetQuant(FractionCoeff());
	} else {		
		for (uint32 xx=0; xx<MINERAL_COUNT; ++xx) {
			MINERAL_TYPE to = (MINERAL_TYPE)(xx);
			if (from != to)	{
				FractionCoeff nval = MineralExchRate(from, to, marketIdx).GetNormalized();
				m_pBtns[xx]->SetQuant(nval);
			} else {
				m_pBtns[xx]->SetQuant(FractionCoeff());
			}
		}
	}
}

iMarketView::iMarketView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, bool is_harbor)
: iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pCmdHandler(pCmdHandler),
m_marketsCount(1), m_is_harbor(is_harbor)
{
	AddChild(m_pLeftPanel = new iMarketPanel(pViewMgr, this, iRect(), 100, iMarketPanel::MP_LEFT));
	AddChild(m_pRightPanel = new iMarketPanel(pViewMgr, this, iRect(), 101, iMarketPanel::MP_RIGHT));
	
	AddChild(m_pAmountChooser = new iAmountChooserGlow(pViewMgr, this, iRect(), 102, true));
	AddChild(m_pTradeBtn = new iImgBtn(pViewMgr, this, iRect(), 103, PDGG(BTN_MARKET_EXCHANGE_DELUXE), -1, iStringT(), Visible | Enabled));	
}


void iMarketView::SetOwner(iPlayer* pOwner)
{
	m_pOwner = pOwner;
	
	m_pLeftPanel->Deselect();
	m_pRightPanel->Deselect();
	Update();
	
}

void iMarketView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if ( uid == 102 ) {
		// Slider
		OnSliderPosChanged();
	} else if ( uid == 103 ) {
		// Trade button		
		check(m_pOwner);
		MINERAL_TYPE mfrom = m_pLeftPanel->SelectedMineral();
		MINERAL_TYPE mto = m_pRightPanel->SelectedMineral();
		FractionCoeff nval = MineralExchRate(mfrom, mto, m_marketsCount).GetNormalized();
		sint32 amount = m_pAmountChooser->CurPos();
		m_pOwner->AddMineral(mfrom, -nval.denum * amount);
		m_pOwner->AddMineral(mto, nval.num * amount);				
		m_pLeftPanel->Deselect();
		m_pRightPanel->Deselect();
        gGame.CastleView()->Update();	
        m_pTradeBtn->SetHighlighted(false);
	} else { 
		// click on some mineral in the panels: update information
		Update();
	}
}

void iMarketView::OnMouseWheelDown(sint16 line, MouseId mID){

    sint32 pos = m_pAmountChooser->CurPos();
    pos += iSIGN(line);
    
    m_pAmountChooser->SetCurPos(pos);
    OnSliderPosChanged();
    Invalidate();    
}

void iMarketView::UpdateSize()
{	
	iRect rc = GetRect();
	m_pTradeBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom));
	m_pAmountChooser->SetRect(AlignChild(iAmountChooserGlow::sm_defSize, AlignBottom) - iPoint(0, m_Rect.h - m_pTradeBtn->GetPos().y) + iPoint(0, 10));
	m_pAmountChooser->UpdateSize();
	iRect free = iRect(0, 15, rc.w, m_pAmountChooser->GetRect().y - 15);
	iSize ps = iSize((free.w - gGfxMgr.Dimension(PDGG(MARKET_BALANCE)).w) / 2, free.h);
	m_pLeftPanel->SetRect(AlignRect(ps, free, AlignLeft));
	m_pRightPanel->SetRect(AlignRect(ps, free, AlignRight));
	m_pLeftPanel->UpdateSize();
	m_pRightPanel->UpdateSize();
}

void iMarketView::OnCompose()
{
	iRect rc = GetScrRect();
#ifdef PC_VERSION
//	ComposeGlowBkgnd(gApp.Surface(), rc);
#endif
	gGfxMgr.Blit(PDGG(MARKET_BALANCE), gApp.Surface(), AlignRect(gGfxMgr.Dimension(PDGG(MARKET_BALANCE)), rc/*m_Rect*/, AlignCenter) - iPoint(0, 15));
	//gApp.Surface().FillRect(m_pRightPanel->GetScrRect(), 0);
	//gTextComposer.TextOut(dlgfc_plain, gApp.Surface(), rc.point(), gTextMgr[TRID_KINGDOM_RESOURCES], iRect(rc.x+rc.w/4-m_cellSiz.w*3/2,rc.y,m_cellSiz.w*3,16), AlignTop);
	//gTextComposer.TextOut(dlgfc_plain, gApp.Surface(), rc.point(), gTextMgr[TRID_AVAILABLE_FOR_TRADE], iRect(rc.x+rc.w/2 + rc.w/4-m_cellSiz.w*3/2,rc.y,m_cellSiz.w*3,16), AlignTop);
	
	if(m_pTradeBtn->IsVisible()) {
		MINERAL_TYPE mfrom = m_pLeftPanel->SelectedMineral();
		MINERAL_TYPE mto = m_pRightPanel->SelectedMineral();
		FractionCoeff nval = MineralExchRate(mfrom, mto, m_marketsCount).GetNormalized();
		sint32 fh = gTextComposer.GetFontHeight(castlemarketfc.fontSize);
		iRect trc(rc.x, m_pAmountChooser->GetScrRect().y - fh, rc.w, fh);
		sint32 amount = m_pAmountChooser->CurPos();
		gTextComposer.TextBoxOut(castlemarketfc, gApp.Surface(), iFormat(gTextMgr[TRID_MSG_MARKET_SFORS], Mineral2Text(mfrom, nval.denum * amount, false).CStr(), Mineral2Text(mto, nval.num * amount).CStr()), trc);
	} 
	
}

void iMarketView::Update()
{
	if (m_is_harbor)
		m_marketsCount = 5; // harbor-dock have fixed exchange rate
	else
		m_marketsCount = iMIN<sint32>(9, m_pOwner->GetMarketIdx());
	
	m_pLeftPanel->UpdateAsLeftSide(m_pOwner->Minerals());
	m_pRightPanel->UpdateAsRightSide(m_pLeftPanel->SelectedMineral(), m_marketsCount);
	
	bool dall = m_pLeftPanel->SelectedMineral() == MINERAL_UNKNOWN || m_pRightPanel->SelectedMineral() == MINERAL_UNKNOWN ||
	m_pLeftPanel->SelectedMineral() == m_pRightPanel->SelectedMineral();
	
	if (!dall) {
		sint32 lnum = m_pOwner->Mineral(m_pLeftPanel->SelectedMineral());
		sint32 rnum = MineralExchRate(m_pLeftPanel->SelectedMineral(), m_pRightPanel->SelectedMineral(), m_marketsCount).GetNormalized().denum;
		m_pAmountChooser->SetVisible((sint32)(lnum / rnum) > 0);
		m_pAmountChooser->SetMax(lnum/rnum);					
	} else {
		m_pAmountChooser->SetVisible(false);
	}
	
	m_pTradeBtn->SetVisible(false);	
	m_pAmountChooser->SetCurPos(0);
	
}

void iMarketView::OnSliderPosChanged()
{
	m_pTradeBtn->SetVisible(m_pAmountChooser->CurPos() > 0);		
}
#ifdef PC_VERSION
void iMarketView::SetHighlighted(bool flag){

    m_pTradeBtn->SetHighlighted(flag);
}
#endif
const iSize iMarketView::sm_defSize = iSize(510, 360);

/* Mineral button */
iMineralBtn::iMineralBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, MINERAL_TYPE mt)
: iButton(pViewMgr, pCmdHandler, rect, uid, Visible|Enabled), m_mt(mt), m_bSelected(false) {}

void iMineralBtn::SetState(bool bSeleted)
{
	m_bSelected = bSeleted;
	Invalidate();
}

void iMineralBtn::SetQuant(FractionCoeff nval)
{
	m_quant = nval;
	Invalidate();
}

void iMineralBtn::OnCompose()
{
	iRect rc = GetScrRect();
	
	iStringT txt;	
	if (!m_quant.IsValid()) 
		txt = gTextMgr[TRID_NA];
	else if (m_quant.num == 0 || m_quant.num >= m_quant.denum) 
		txt = FormatNumber(m_quant.num);
	else 
		txt = iFormat(_T("%d : %d"), m_quant.num, m_quant.denum);
	
	ComposeResourceIcon(m_mt, rc, m_bSelected, true, txt, true
                            , true
                        );
}


