#include "stdafx.h"
#include "ArmyList.stylus.h"
#include "Dlg_ArmyRoom.h"

iDlg_ArmyRoom::iDlg_ArmyRoom(iViewMgr* pViewMgr, iHero& hero, const iCreatGroup& cgroup)
: iBaseGameDlg(pViewMgr, hero.Owner()->PlayerId()), m_hero(hero)
{
	m_fOwner.AddGroupToArmy(cgroup.Type(), cgroup.Count());
	m_Title.Setf(gTextMgr[TRID_MAKE_ROOM_MESSAGE], gTextMgr[TRID_CREATURE_PEASANT_F3+cgroup.Type()*3]);
}

void iDlg_ArmyRoom::OnCreateDlg()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(DRC_OK);
		return;
	}		
#endif

	iRect clRect = ClientRect();

	iCheckButton* pSplitBtn = new iCheckButton(m_pMgr, this, iRect(clRect.x+5+35*7+7,clRect.y+20,20,35+35+1), PDGG_BTN_SPLIT, 100);
	AddChild(pSplitBtn);

	iArmyListEx* pFakeArmyCtrl = new iArmyListEx_stylus(m_pMgr, this, iPoint(clRect.x+5,clRect.y+20), pSplitBtn, iSize(35,35), 101);
	pFakeArmyCtrl->SetArmy(&m_fOwner, true);
	AddChild(pFakeArmyCtrl);
	iArmyListEx* pHeroArmyCtrl = new iArmyListEx_stylus(m_pMgr, this, iPoint(clRect.x+5, clRect.y+20+36), pSplitBtn, iSize(35,35), 102);
	pHeroArmyCtrl->SetArmy(&m_hero, false);
	AddChild(pHeroArmyCtrl);


	pFakeArmyCtrl->AddCompetitor(pHeroArmyCtrl);
	pHeroArmyCtrl->AddCompetitor(pFakeArmyCtrl);

	AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
}

void iDlg_ArmyRoom::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	// title
	iTextComposer::FontConfig fc = dlgfc_stopic; 
	fc.cmpProps = iDibFont::ComposeProps(RGB16(255,255,0));
	gTextComposer.TextOut(fc, gApp.Surface(),rc.point(),m_Title, iRect(rc.x,rc.y,rc.w,15),AlignTop);	
}

iSize iDlg_ArmyRoom::ClientSize() const
{
	sint32 w = 282;
	sint32 h = 0;

	// title
	h += 20;

	// fake army control
	h += 36;

	// hero's army control
	h += 36;

	// ok button
	h += DEF_BTN_HEIGHT + 10;
	return iSize(w,h);
}

void iDlg_ArmyRoom::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == DRC_OK && cmd == CCI_BTNCLICK) {
		EndDialog(pView->GetUID());
	}
}

