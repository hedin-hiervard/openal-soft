#include "stdafx.h"
#include "Dlg_BattleCastle.h"

iDlg_BattleCastle::iDlg_BattleCastle(iViewMgr* pViewMgr, iCastle& castle,  bool bActiveMember)
: iBaseGameDlg(pViewMgr), m_castle(castle), m_bActiveMember(bActiveMember)
{}

void iDlg_BattleCastle::OnCreateDlg()
{
	iRect clRect = ClientRect();

	if(m_bActiveMember) {
		sint32 bw = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_AUTO_COMBAT], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_MENU_EXITGAME], btnfc_normal).w) + 30;
		AddChild(m_pBtnAutobattle = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0),TRID_AUTO_COMBAT, AutoBattle));
		AddChild(m_pBtnMainMenu = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, 0),TRID_MENU_EXITGAME, MainMenu));
	}
}
#ifdef PC_VERSION
void iDlg_BattleCastle::UpdateSize(){

    if(m_bActiveMember) {
		sint32 bw = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_AUTO_COMBAT], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_MENU_EXITGAME], btnfc_normal).w) + 30;
		m_pBtnAutobattle->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0) );
		m_pBtnMainMenu->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, 0) );
	}
}
#endif
void iDlg_BattleCastle::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();

	// Portrait
	iRect prc = AlignRect(iIphoneBtn::sm_defSize, rc, AlignTop);	
	SpriteId sid = sid = PDGG(BTN_CASTLES) + m_castle.Proto()->Type();
	gGfxMgr.Blit(sid,gApp.Surface(), prc);

	rc.y += 55;

	// Name
	iStringT txt;
	txt.Addf(_T("#S2#FAFF %s"),m_castle.Name().CStr());	
	rc.y += gTextComposer.TextBoxOut(gApp.Surface(),txt,rc);
	//rc.DeflateRect(5);
}

iSize iDlg_BattleCastle::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();

	iStringT txt;
	txt.Addf(_T("#S2#FAFF %s"),m_castle.Name().CStr());	


	// image
	h += 55;

	// Name
	h += gTextComposer.GetTextBoxSize(txt,w, dlgfc_topic).h;
	
	// buttons
	if(m_bActiveMember)
		h += DEF_BTN_HEIGHT;
	return iSize(w,h);
}

void iDlg_BattleCastle::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	//uint32 uid = pView->GetUID();
	{
		EndDialog(pView->GetUID());
	}
}

void iDlg_BattleCastle::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	EndDialog(0);
}

