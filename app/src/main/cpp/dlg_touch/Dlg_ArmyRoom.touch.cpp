#include "stdafx.h"
#include "ArmyList.touch.h"
#include "Dlg_ArmyRoom.h"

#include "CastleView.h"

iDlg_ArmyRoom::iDlg_ArmyRoom(iViewMgr* pViewMgr, iHero& hero, const iCreatGroup& cgroup)
: iBaseGameDlg(pViewMgr), m_hero(hero)
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

/*	iCheckButton* pSplitBtn = new iCheckButton(m_pMgr, this, iPoint(), PDGG(BTN_SPLIT), PDGG(BTN_SPLIT_PRESSED), 100);
	AddChild(pSplitBtn);

	iArmyListEx* pFakeArmyCtrl = new iArmyListEx_touch(m_pMgr, this, iPoint(), pSplitBtn, iArmyListEx_touch::sm_defCellSize, 101);
	pFakeArmyCtrl->SetArmy(&m_fOwner, true);
	AddChild(pFakeArmyCtrl);
	iArmyListEx* pHeroArmyCtrl = new iArmyListEx_touch(m_pMgr, this, iPoint(), pSplitBtn, iArmyListEx_touch::sm_defCellSize, 102);
	pHeroArmyCtrl->SetArmy(&m_hero, false);
	AddChild(pHeroArmyCtrl);

	sint32 w = gApp.Surface().GetWidth() / 2;	
	sint32 off = 15 + gTextComposer.GetTextBoxSize(m_Title, w, dlgfc_topic).h;
	
	pFakeArmyCtrl->SetRect(AlignChild(pFakeArmyCtrl->GetSize(), AlignTop, off));
	pHeroArmyCtrl->SetRect(AlignChild(pHeroArmyCtrl->GetSize(), AlignTop, off) + iPoint(0, pFakeArmyCtrl->GetSize().h + 3));
	
	iRect rc = pFakeArmyCtrl->GetRect();
	pSplitBtn->SetPos(iPoint(rc.x + rc.w + 5, rc.y));

	pFakeArmyCtrl->AddCompetitor(pHeroArmyCtrl);
	pHeroArmyCtrl->AddCompetitor(pFakeArmyCtrl);
*/
	
	AddChild(pArmyExchange = new iArmyExchange(&gApp.ViewMgr(), this, iRect(), 100, true, false));	
	pArmyExchange->SetTopOwner(&m_fOwner, true);
	pArmyExchange->SetBottomOwner(&m_hero, true);


	sint32 txtoff = gTextComposer.GetTextSize(m_Title, dlgfc_hdr).h;	
	pArmyExchange->SetRect(AlignChild(pArmyExchange->GetDefSize(2), AlignTop) + iPoint(0, txtoff + 15));
	pArmyExchange->UpdateSize();	

    sint32 bw = iIphoneBtn::sm_defSize.w;
    AddChild(m_pBtnOk = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSize, AlignBottom), DRC_OK, PDGG(BTN_OK_DELUXE), -1));
}
#ifdef PC_VERSION
void iDlg_ArmyRoom::UpdateSize(){

    Center();
    
    sint32 txtoff = gTextComposer.GetTextSize(m_Title, dlgfc_hdr).h;	
	pArmyExchange->SetRect(AlignChild(pArmyExchange->GetDefSize(2), AlignTop) + iPoint(0, txtoff + 15));
	pArmyExchange->UpdateSize();
    
    sint32 bw = gTextComposer.GetTextSize(gTextMgr[TRID_OK], btnfc_normal).w + 30;
    
    m_pBtnOk->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) );
}
#endif
void iDlg_ArmyRoom::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);	
	
	// title		
	gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(), m_Title, rc);	
}

iSize iDlg_ArmyRoom::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();

	// title
	h += gTextComposer.GetTextBoxSize(m_Title, w, dlgfc_topic).h;

	h += iArmyExchange::GetDefSize(2).h;

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
