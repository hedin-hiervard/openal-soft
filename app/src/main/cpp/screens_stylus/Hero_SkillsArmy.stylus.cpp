#include "stdafx.h"
#include "HeroView.h"
#include "HeroView.stylus.h"
#include "Hero_Skills.h"
#include "Hero_SkillsArmy.stylus.h"
#include "Dlg_MLInfo.h"
#include "Dlg_SkillInfo.h"
#include "ArmyList.stylus.h"

iSkillsArmyTab_stylus::iSkillsArmyTab_stylus(iViewMgr* pViewMgr) : iSkillsTab(pViewMgr)
{
	uint32 wid = (gApp.Surface().GetWidth() - 53) / 5 - 1;
	AddChild(m_pSkillTabBtns[0] = new iExpBtn(pViewMgr, this, iRect(0,0, wid,36), 120));
	AddChild(m_pSkillTabBtns[1] = new iManaBtn(pViewMgr, this, iRect(wid,0, wid,36), 121));
	AddChild(m_pSkillTabBtns[2] = new iActPtsBtn(pViewMgr, this, iRect(wid*2,0,wid,36), 122));
	AddChild(m_pSkillTabBtns[3] = new iMoraleBtn(pViewMgr, this, iRect(wid*3,0,wid,36), 123));
	AddChild(m_pSkillTabBtns[4] = new iLuckBtn(pViewMgr, this, iRect(wid*4,0,wid,36), 124));
	iPoint pos(0,37);
	sint32 cellW = (gApp.Surface().GetWidth() - 58) / 2;
	sint32 cellH = 30;
	for (uint32 xx=0; xx<8; ++xx) {
		AddChild(m_pBtnSecSkill[xx] = new iSecSkillBtn(pViewMgr,this,iRect(pos.x+((xx&1)?cellW+1:0),pos.y+((xx>>1)*cellH),cellW,cellH),110 + xx));
	}

	AddChild(m_pSplitBtn = new iCheckButton(pViewMgr, this, iRect(m_Rect.w-18,m_Rect.h-39,18,39), PDGG_BTN_SPLIT, 102));
	AddChild(m_pArmyList = new iArmyListEx_stylus(pViewMgr, this, iPoint(0,m_Rect.h-39), m_pSplitBtn, iSize((gApp.Surface().GetWidth() - 82) / 7,39), 101));
}

void iSkillsArmyTab_stylus::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
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
	else if (uid >= 110 && uid < 118) {
		if (const iSecSkillEntry* pSkill = m_pBtnSecSkill[uid - 110]->Skill()) {
			iSecSkillInfoDlg dlg(m_pMgr, (SECONDARY_SKILLS)pSkill->m_skill, (SECSKILL_LEVEL)pSkill->m_level, m_pHero->Owner()->PlayerId());
			dlg.DoModal();
		}
	}
}

void iSkillsArmyTab_stylus::OnHeroChanged() 
{
	if (m_pHero) {
		uint32 xx;
		for (xx=0; xx<m_pHero->SecSkills().Count(); ++xx) m_pBtnSecSkill[xx]->SetSkill(&m_pHero->SecSkills().At(xx));
		for (uint32 zz=xx; zz<8; ++zz) m_pBtnSecSkill[zz]->SetSkill(NULL);
		m_pArmyList->SetArmy(m_pHero, false);
		for (xx=0; xx<5; ++xx) m_pSkillTabBtns[xx]->SetHero(m_pHero);
	}  
	else
		for (uint32 zz=0; zz<8; ++zz) m_pBtnSecSkill[zz]->SetSkill(NULL);

	Invalidate();
}


