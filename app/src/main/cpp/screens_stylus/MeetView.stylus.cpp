#include "stdafx.h"
#include "ArtifactList.h"
#include "MeetView.h"
#include "ArtifactList.h"
#include "ArtifactList.stylus.h"
#include "MeetView.stylus.h"
#include "OverlandView.h"
#include "OverlandView.stylus.h"
#include "ArmyList.stylus.h"

iHeroChild_stylus::iHeroChild_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, iSimpleArray<iArtDragDropItem*>& competitors, const iPoint& pos)
: iHeroChild(pViewMgr, iRect(), 200) 
{
	iPoint offset = iPoint(gApp.Surface().GetWidth() / 2 - 158, ((gApp.Surface().GetHeight() - 20) / 2 - 107) / 2);
	AddChild(m_pPortrait = new iHeroBigPortBtn(pViewMgr, this, iRect(offset + iPoint(3, 20),iSize(48,48)), 201));
	m_pArtBackPackCtrl = new iArtBackPackCtrl_stylus(competitors, pViewMgr, this, offset + iPoint(55,20+18), iSize(32,30), 7, iArtBackPackCtrl::Horizontal, 120);
	AddChild(((iArtBackPackCtrl_stylus*)m_pArtBackPackCtrl));
	AddChild(m_pSplitBtn = new iCheckButton(pViewMgr, this, iRect(offset + iPoint(3 + 42*7, 53+18),iSize(20,36)), PDGG_BTN_SPLIT, 140));
	AddChild(m_pArmyList = new iArmyListEx_stylus(pViewMgr,this,offset + iPoint(3,53+18), m_pSplitBtn, iSize(41,36),110));
	//AddChild(m_pToolBar = new iMeetToolBar(&gApp.ViewMgr(), this, iRect(0,m_Rect.h-21,m_Rect.w,21)));
}

void iHeroChild_stylus::SetHero(iHero* pHero)
{
	iHeroChild::SetHero(pHero);
	m_pPortrait->SetHero(m_pHero);
}


void iHeroChild_stylus::OnCompose()
{
	iRect rc = GetScrRect();
	gGfxMgr.BlitTile(PDGG_BKTILE, gApp.Surface(),rc);
	gApp.Surface().Darken25Rect(rc);
	check(m_pHero);

	// Name
	iStringT txt;
	iRect trc(rc.x,rc.y,rc.w,17);
	ButtonFrame(gApp.Surface(),trc,0);
	txt.Addf(_T("#S0#FEEE%s#S2#FAFF %s #S0#FEEE%s %d"),gTextMgr[TRID_HERO_TYPE_KNIGHT+m_pHero->Type()],m_pHero->Name().CStr(),gTextMgr[TRID_LEVEL],m_pHero->Level());
	gTextComposer.TextOut(gApp.Surface(),trc,txt,trc,AlignCenter);

	gApp.Surface().HLine(iPoint(rc.x,rc.y+17),rc.x2(),cColor_Black);
}

iMeetView_stylus::iMeetView_stylus()
: iMeetView()
{
	AddChild(m_pChilds[0] = new iHeroChild_stylus(&gApp.ViewMgr(), this, m_Competitors, iPoint(0,0)));
	AddChild(m_pChilds[1] = new iHeroChild_stylus(&gApp.ViewMgr(), this, m_Competitors, iPoint(0, (gApp.Surface().GetHeight() - 20) / 2)));
	m_pChilds[0]->ArmyList()->AddCompetitor(m_pChilds[1]->ArmyList());
	m_pChilds[1]->ArmyList()->AddCompetitor(m_pChilds[0]->ArmyList());
	m_pChilds[0]->SetFriend(m_pChilds[1]);
	m_pChilds[1]->SetFriend(m_pChilds[0]);
	AddChild(m_pToolBar = new iMeetToolBar(&gApp.ViewMgr(), this, iRect(0,m_Rect.h-21,m_Rect.w,21)));	
}
