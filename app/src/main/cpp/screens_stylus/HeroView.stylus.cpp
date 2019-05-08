#include "stdafx.h"
#include "HeroView.h"
#include "GfxHlp.h"

#include "Dlg_Spellbook.h"
#include "Hero_Skills.h"
#include "Hero_Spells.h"
#include "OverlandView.h"

#include "Dlg_SkillInfo.h"
#include "Dlg_MLInfo.h"


/*
*	Common defines
*/
const SpriteId HVT_ICONS[HVT_COUNT] = {
	PDGG_TAB_SKILLS,
	PDGG_TAB_ARTIFACTS,
	PDGG_TAB_SPELLBOOK
};

iHeroViewChild::iHeroViewChild(iViewMgr* pViewMgr)
: iView(pViewMgr,iRect(57,22,gApp.Surface().GetWidth()-57,gApp.Surface().GetHeight() - 43),GENERIC_VIEWPORT,0,Enabled), m_pHero(NULL) 
{}

void iHeroViewChild::SetHero(iHero* pHero)
{
	m_pHero = pHero;
	OnHeroChanged();
}

/*
*	
*/
class iQuestLogTab : public iHeroViewChild
{
public:
	iHero*	m_pHero;

	iQuestLogTab(iViewMgr* pViewMgr) : iHeroViewChild(pViewMgr){}
};

/*
*	Hero view
*/
iExpLuckMoraleView::iExpLuckMoraleView(iViewMgr *pViewMgr, IViewCmdHandler* pCmdHandler, iRect rc, uint32 uid):
iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pHero(NULL), m_pCmdHandler(pCmdHandler)
{
	AddChild(m_pSkillTabBtns[0] = new iExpBtn(&gApp.ViewMgr(), this, iRect(3,3,53,36), 120));
	AddChild(m_pSkillTabBtns[1] = new iManaBtn(&gApp.ViewMgr(), this, iRect(3,43,53,36), 121));
	AddChild(m_pSkillTabBtns[2] = new iActPtsBtn(&gApp.ViewMgr(), this, iRect(3,83,53,36), 122));
	AddChild(m_pSkillTabBtns[3] = new iMoraleBtn(&gApp.ViewMgr(), this, iRect(3,123,52,36), 123));
	AddChild(m_pSkillTabBtns[4] = new iLuckBtn(&gApp.ViewMgr(), this, iRect(3,163,52,36), 124));			
	if (m_pHero) {
		uint32 xx;
		for (xx=0; xx<5; ++xx) m_pSkillTabBtns[xx]->SetHero(m_pHero);
	} 

}

void iExpLuckMoraleView::SetHero(iHero *pHero) 
{
	m_pHero = pHero;
	if (m_pHero) {
		uint32 xx;
		for (xx=0; xx<5; ++xx) m_pSkillTabBtns[xx]->SetHero(m_pHero);
	} 		
}

void iExpLuckMoraleView::OnCompose() {
	iRect rc = GetScrRect();
	ComposeDlgBkgndPanel(gApp.Surface(), iRect(rc.point(), iSize(rc.w, 40)), gGame.Map().ActPlayer()->PlayerId(), false);
	ComposeDlgBkgndPanel(gApp.Surface(), iRect(rc.point() + iPoint(0, 41), iSize(rc.w, rc.h - 41)), gGame.Map().ActPlayer()->PlayerId(), false);

	//		ComposeDlgBkgndPanel(gApp.Surface(), rc, gGame.Map().ActPlayer()->PlayerId(), false);
}
void iExpLuckMoraleView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == 120) {
		// Experience button
		iIconDlg dlg(m_pMgr, iFormat(_T("%s %d"),gTextMgr[TRID_LEVEL], m_pHero->Level()), iStringT(gTextMgr[TRID_INFO_EXPERIENCE]) + _T("\n\n#FEE8") + iFormat(gTextMgr[TRID_MSG_EXP_TO_NEXT_LEVEL], m_pHero->LevelBound(m_pHero->Level()+1) - m_pHero->Experience()), PDGG_DGLYPH_EXP, m_pHero->Owner()->PlayerId());
		dlg.DoModal();
	} else if (uid == 121) {
		// Mana points button
		iIconDlg dlg(m_pMgr, gTextMgr[TRID_MANA], gTextMgr[TRID_INFO_MANA], PDGG_DGLYPH_MANA, m_pHero->Owner()->PlayerId());
		dlg.DoModal();
	} else if (uid == 122) {
		// Travel points button
		iIconDlg dlg(m_pMgr, gTextMgr[TRID_TRAVELPTS], gTextMgr[TRID_INFO_TRAVELPTS], PDGG_DGLYPH_MOVES, m_pHero->Owner()->PlayerId());
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


iSkillsTabBtn::iSkillsTabBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
: iButton(pViewMgr, pCmdHandler, rect, uid, Visible|Enabled), m_pHero(NULL) 
{}

inline void	iSkillsTabBtn::SetHero(const iHero* pHero)	
{ 
	m_pHero = pHero; 
	Invalidate(); 
}

inline const iHero* iSkillsTabBtn::Hero() const { return m_pHero; }

iSecSkillBtn::iSecSkillBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
: iButton(pViewMgr, pCmdHandler, rect, uid, Visible|Enabled), m_pSkill(NULL) {}

inline void iSecSkillBtn::SetSkill(const iSecSkillEntry* pSkill) { m_pSkill = pSkill; Invalidate(); }
inline const iSecSkillEntry* iSecSkillBtn::Skill() const { return m_pSkill; }

void iSecSkillBtn::OnCompose()
{
	iRect rc = GetScrRect();
#ifndef OS_IPHONE
	ButtonFrame(gApp.Surface(),rc,GetButtonState());
#endif
	if (m_pSkill){
		BlitIcon(gApp.Surface(), PDGG_SEC_SKILLS+m_pSkill->m_skill, iRect(rc.x+2,rc.y+3,32,24));
		rc.DeflateRect(35,0,0,0);
		gTextComposer.TextOut(iTextComposer::FontConfig(FS_MEDIUM,RGB16(255,220,64)), gApp.Surface(),iPoint(rc.x,rc.y+3),gTextMgr[TRID_HSKILL_ESTATES+m_pSkill->m_skill]);
		gTextComposer.TextOut(iTextComposer::FontConfig(FS_SMALL,RGB16(192,192,192)), gApp.Surface(),iPoint(rc.x,rc.y+13),gTextMgr[TRID_HSKILL_LEVEL_BASIC+m_pSkill->m_level]);
	} else {
		//gApp.Surface().FillRect(rc,0,32);
	}
}


iExpBtn::iExpBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) : 
iSkillsTabBtn(pViewMgr, pCmdHandler, rect, uid) 
{}

void iExpBtn::OnCompose()
{
	iRect rc = GetScrRect();
#ifndef OS_IPHONE
	ButtonFrame(gApp.Surface(),rc,GetButtonState());
#endif
	if (m_pHero) {
		iSize siz = gGfxMgr.Dimension(PDGG_DGLYPH_EXP);
		sint32 opx = rc.x + (sint32)(rc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(PDGG_DGLYPH_EXP).x;
		sint32 opy = rc.y + (sint32)(rc.h/2) - (sint32)(siz.h/2) - gGfxMgr.Anchor(PDGG_DGLYPH_EXP).x - 2;
		gGfxMgr.Blit(PDGG_DGLYPH_EXP, gApp.Surface(), iPoint(opx,opy));
		iTextComposer::FontConfig fc(FS_MEDIUM, RGB16(255,255,0));
		iRect trc(rc.x+(sint32)(rc.w/2)-20,rc.y+rc.h-12, 40,11);
		DrawRoundRect(gApp.Surface(), trc, RGB16(192,128,32), cColor_Black);
		gTextComposer.TextOut(fc, gApp.Surface(), iPoint(), iFormat(_T("%s %d"), gTextMgr[TRID_LVL], m_pHero->Level()), trc, AlignCenter);
	}
}	

iManaBtn::iManaBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) : 
iSkillsTabBtn(pViewMgr, pCmdHandler, rect, uid) 
{}

void iManaBtn::OnCompose()
{
	iRect rc = GetScrRect();
#ifndef OS_IPHONE
	ButtonFrame(gApp.Surface(),rc,GetButtonState());
#endif
	if (m_pHero) {
		iSize siz = gGfxMgr.Dimension(PDGG_DGLYPH_MANA);
		sint32 opx = rc.x + (sint32)(rc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(PDGG_DGLYPH_MANA).x;
		sint32 opy = rc.y + (sint32)(rc.h/2) - (sint32)(siz.h/2) - gGfxMgr.Anchor(PDGG_DGLYPH_MANA).x - 5;
		gGfxMgr.Blit(PDGG_DGLYPH_MANA, gApp.Surface(), iPoint(opx,opy));
		iTextComposer::FontConfig fc(FS_MEDIUM, RGB16(255,255,0));
		iRect trc(rc.x+(sint32)(rc.w/2)-20,rc.y+rc.h-12, 40,11);
		DrawRoundRect(gApp.Surface(), trc, RGB16(192,128,32), cColor_Black);
		gTextComposer.TextOut(fc, gApp.Surface(), iPoint(), iFormat(_T("%d/%d"),m_pHero->ManaPts(),m_pHero->MaxManaPts()), trc, AlignCenter);
	}
}	

iActPtsBtn::iActPtsBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) : 
iSkillsTabBtn(pViewMgr, pCmdHandler, rect, uid) 
{}

void iActPtsBtn::OnCompose()
{
	iRect rc = GetScrRect();
#ifndef OS_IPHONE
	ButtonFrame(gApp.Surface(),rc,GetButtonState());
#endif
	if (m_pHero) {
		iSize siz = gGfxMgr.Dimension(PDGG_DGLYPH_MOVES);
		sint32 opx = rc.x + (sint32)(rc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(PDGG_DGLYPH_MOVES).x;
		sint32 opy = rc.y + (sint32)(rc.h/2) - (sint32)(siz.h/2) - gGfxMgr.Anchor(PDGG_DGLYPH_MOVES).y - 1;
		gGfxMgr.Blit(PDGG_DGLYPH_MOVES, gApp.Surface(), iPoint(opx,opy));
		iTextComposer::FontConfig fc(FS_MEDIUM, RGB16(255,255,0));
		iRect trc(rc.x+(sint32)(rc.w/2)-20,rc.y+rc.h-12, 40,11);
		DrawRoundRect(gApp.Surface(), trc, RGB16(192,128,32), cColor_Black);
		gTextComposer.TextOut(fc, gApp.Surface(), iPoint(), iFormat(_T("%d/%d"),m_pHero->Moves(),m_pHero->TotalMoves()), trc, AlignCenter);
	}
}

iMoraleBtn::iMoraleBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) : 
iSkillsTabBtn(pViewMgr, pCmdHandler, rect, uid)
{}

void iMoraleBtn::OnCompose()
{
	iRect rc = GetScrRect();
#ifndef OS_IPHONE
	ButtonFrame(gApp.Surface(),rc,GetButtonState());
#endif
	if (m_pHero) {
		ComposeMoraleGlyph(gApp.Surface(), rc, (sint8)m_pHero->FurtSkill(FSK_MORALE));
	}
}

iLuckBtn::iLuckBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) : iSkillsTabBtn(pViewMgr, pCmdHandler, rect, uid){}

void iLuckBtn::OnCompose()
{
	iRect rc = GetScrRect();
#ifndef OS_IPHONE
	ButtonFrame(gApp.Surface(),rc,GetButtonState());
#endif
	if (m_pHero) {
		ComposeLuckGlyph(gApp.Surface(), rc, (sint8)m_pHero->FurtSkill(FSK_LUCK));
	}
}

iSecSkillsView::iSecSkillsView(iViewMgr *pViewMgr, IViewCmdHandler* pCmdHandler, iPoint pnt, uint32 uid):
iView(pViewMgr, iRect(pnt, iSize(110, 29*8 + 3)), GENERIC_VIEWPORT, uid, Visible|Enabled), m_pHero(NULL), m_pCmdHandler(pCmdHandler)
{
	iPoint pos(3, 3);		
	sint32 cellW = 90;
	sint32 cellH = 29;
	for (uint32 xx=0; xx<8; ++xx) {
		AddChild(m_pBtnSecSkill[xx] = new iSecSkillBtn(&gApp.ViewMgr(),this,iRect(pos.x, pos.y,cellW,cellH),110 + xx));			
		pos.y += cellH;	
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
	ComposeDlgBkgndPanel(gApp.Surface(), rc, gGame.Map().ActPlayer()->PlayerId(), false);
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


iHeroView::iHeroView(CHILD_VIEW parentView)
: iChildGameView(false, parentView)
, m_curTab(-1)
{
	for (uint32 xx=0; xx<HVT_COUNT; ++xx) 
		m_pChilds[xx] = NULL;
	
}

iHeroView::~iHeroView()
{}



void iHeroView::ShowPage(uint32 nIdx)
{
	if (m_curTab != -1) m_pChilds[m_curTab]->SetVisible(false);
	m_pChilds[nIdx]->SetVisible(true);
	m_pChilds[nIdx]->OnActivate();
	m_curTab = nIdx;
}

void iHeroView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	sint32 clsId = pView->GetClassId();
	uint32 uid = pView->GetUID();

	if (uid == 101) {
	} else if (uid == 103) {
		check(clsId == TABBED_SWITCH);
		if (cmd == CCI_TABCHANGED) ShowPage(param);
	} else if (uid == 104) {
		iQuestDlg qdlg(m_pMgr, m_pHero->Name(), gTextMgr[TRID_MSG_HDISMISS], m_pHero->Owner()->PlayerId());
		if (qdlg.DoModal() == DRC_YES) {
			// Remove hero from the map
			gGame.Map().RemoveHero(m_pHero, true);
		}
	} else if (uid == VPUID_BTN_CLOSEHERO) {
		gGame.HideView(CV_HERO);
	} else if (uid == 150) {
		iPoint pp = pView->GetScrPos();
		m_pMgr->TrackPopup(new iHeroListPopup(m_pMgr), pp, iRect(0,0,gApp.Surface().GetWidth(), gApp.Surface().GetHeight()),AlignBottomLeft);
	} else if (uid == 201) {
		gGame.Map().CurPlayer()->PrevHero();
	} else if (uid == 202) {
		gGame.Map().CurPlayer()->NextHero();
	}	
}

void iHeroView::OnCompose()
{
	gApp.Surface().FillRect(GetScrRect(),0);
}

void iHeroView::SetHero(iHero* pHero)
{
	m_pHero = pHero;
	for (uint32 xx=0; xx<HVT_COUNT; ++xx) 
		if(m_pChilds[xx])
			m_pChilds[xx]->SetHero(m_pHero);
	m_pPrSkillsView->SetHero(m_pHero);
	m_pTitleBar->SetHero(m_pHero);
	m_pToolBar->SetHero(m_pHero, ParentView() != CV_OVERLAND);
}


/*
*	Primary Skills view
*/
iPrSkillBtn::iPrSkillBtn(iViewMgr* pViewMgr, const iRect& rect, FURTHER_SKILLS skill) 
: iButton(pViewMgr, this, rect, 500 + skill, Visible|Enabled), m_skill(skill), m_pHero(NULL) {}

void iPrSkillBtn::OnCompose()
{
	iRect rc = GetScrRect();
	//gApp.Surface().FrameRect(rc, cColor_Black);
	if (!m_pHero) return;
	SpriteId sid  = PDGG_DGLYPH_PRSKILLS + (m_skill-FSK_ATTACK);
	iSize siz = gGfxMgr.Dimension(sid);
	sint32 opx = rc.x + (sint32)(rc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(sid).x;
	sint32 opy = rc.y - gGfxMgr.Anchor(sid).y + 1;
	gGfxMgr.Blit(sid, gApp.Surface(), iPoint(opx,opy));

	iRect trc(rc.x+(sint32)(rc.w/2)-14,rc.y+rc.h-13, 28,13);
	DrawRoundRect(gApp.Surface(), trc, RGB16(192,128,32), cColor_Black);

	iTextComposer::FontConfig fc(FS_LARGE, RGB16(255,255,0));
	gTextComposer.TextOut(fc, gApp.Surface(), iPoint(), FormatNumber(m_pHero->FurtSkill(m_skill)), trc, AlignCenter);
}

void iPrSkillBtn::SetHero(iHero* pHero)
{
	m_pHero = pHero;
	Invalidate();
}

void iPrSkillBtn::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	//uint32 uid = pView->GetUID();
	iIconDlg dlg(m_pMgr, gTextMgr[TRID_SKILL_ATTACK + (m_skill - FSK_ATTACK)], gTextMgr[TRID_INFO_ATTACK + (m_skill - FSK_ATTACK)], PDGG_DGLYPH_PRSKILLS + (m_skill - FSK_ATTACK), m_pHero->Owner()->PlayerId());
	dlg.DoModal();
}

//////////////////////////////////////////////////////////////////////////
iHeroPrSkillView::iHeroPrSkillView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect)
: iView(pViewMgr,rect,GENERIC_VIEWPORT,VPUID_HEROVIEWTITLELBAR,Visible|Enabled), m_pHero(NULL)
{
	uint32 ypos = 0;
	for (uint32 xx=0; xx<4; ++xx) {
		AddChild(m_pBtns[xx] = new iPrSkillBtn(pViewMgr, iRect(1,ypos,rect.w-2,35), (FURTHER_SKILLS)(FSK_ATTACK+xx)));
		ypos += 35;
	}
}


void iHeroPrSkillView::SetHero(iHero* pHero)
{
	m_pHero = pHero;
	for (uint32 xx=0; xx<4; ++xx) {
		m_pBtns[xx]->SetHero(pHero);
	}
	Invalidate();
}

void iHeroPrSkillView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{	
}

/*
*	Hero Titlebar
*/

void iHeroTitleBar::SetHero(iHero* pHero)
{
	m_pHero = pHero;
	Invalidate();
}

/*
*	Hero Toolbar
*/
iHeroToolBar::iHeroToolBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect)
: iView(pViewMgr,rect,GENERIC_VIEWPORT,VPUID_HEROVIEWTOOLBAR,Enabled|Visible), m_pHero(NULL)
{
	// Button with Popup
	AddChild(m_pHeroPortBtn = new iHeroPortBtn(pViewMgr, pCmdHandler, iRect(0,1,32,20), 150));

	// Tab switch
	m_pTabSwitch = new iBarTabSwitch(pViewMgr, pCmdHandler, iRect(33,1,HVT_COUNT*32,20), HVT_COUNT, 103);
	for (uint32 xx=0; xx<HVT_COUNT; ++xx) m_pTabSwitch->SetTabIcon(HVT_ICONS[xx]);
	m_pTabSwitch->SetCurrentTab(HVT_SKILLS);
	//m_pTabSwitch->EnableTab(HVT_QUESTLOG, false);
	AddChild(m_pTabSwitch);

	// Dismiss button
	AddChild(m_pDismissBtn = new iIconButton(pViewMgr, pCmdHandler, iRect(rect.w-61,1,30,20),PDGG_BTN_DISMISS,104));

	// Close button
	AddChild(new iIconButton(pViewMgr, pCmdHandler, iRect(rect.w-30,1,30,20),PDGG_BTN_CLOSE,VPUID_BTN_CLOSEHERO));
}

void iHeroToolBar::OnCompose()
{
	iRect rect = GetScrRect();
	sint32 rval = 33 + m_pTabSwitch->GetSize().w+1;
	iRect rc(rect.x+rval,rect.y+1,rect.w-(60+rval)-2,20);
	gGfxMgr.BlitTile(PDGG_BKTILE, gApp.Surface(), rc);
	ButtonFrame(gApp.Surface(), rc, 0);
}

void iHeroToolBar::SetHero(iHero* pHero, bool bReadOnly)
{
	m_pDismissBtn->SetEnabled(!bReadOnly && pHero && (pHero->Owner()->CastleCount() || pHero->Owner()->HeroCount() > 1));
	m_pHeroPortBtn->SetHero(pHero);
	m_pHero = pHero;
	Invalidate();
}

