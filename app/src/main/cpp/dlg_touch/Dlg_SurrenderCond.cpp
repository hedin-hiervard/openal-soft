#include "stdafx.h"
#include "Dlg_SurrenderCond.h"

iDlg_SurrenderCond::iDlg_SurrenderCond(iViewMgr* pViewMgr, const iHero* pHero,  uint32 cost)
: iBaseGameDlg(pViewMgr), m_pHero(pHero), m_cost(cost)
{
}

void iDlg_SurrenderCond::OnCreateDlg()
{
	iRect clRect = ClientRect();

	sint32 bw = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_OK], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_CANCEL], btnfc_normal).w) + 30;
	AddChild(m_pBtnOk = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0),TRID_OK, DRC_OK));
	AddChild(m_pBtnCancel = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, 0),TRID_CANCEL, DRC_CANCEL));
}
#ifdef PC_VERSION
void iDlg_SurrenderCond::UpdateSize(){

    Center();
    
    iRect clRect = ClientRect();
    
	sint32 bw = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_OK], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_CANCEL], btnfc_normal).w) + 30;
	AddChild(m_pBtnOk = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0),TRID_OK, DRC_OK));
	AddChild(m_pBtnCancel = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, 0),TRID_CANCEL, DRC_CANCEL));
}
#endif
void iDlg_SurrenderCond::DoCompose(const iRect& rect)
{
	iRect rc(rect);
	rc.DeflateRect(5);

	// Portrait
	ButtonFrame(gApp.Surface(),iRect(rc.x,rc.y,50,50),iButton::Pressed);
	gGfxMgr.Blit(m_pHero->Proto()->m_icn48,gApp.Surface(),iPoint(rc.x+1,rc.y+1));

	rc.y += 50;

	// text
	rc.y += gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(), iFormat(gTextMgr[TRID_SURRENDER_STATES],m_pHero->Name().CStr()), rc);	
	rc.y += gTextComposer.TextBoxOut(dlgfc_splain, gApp.Surface(), iFormat(gTextMgr[TRID_SURRENDER_CONDITION],m_cost), rc);	
}

iSize iDlg_SurrenderCond::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();

	h += 50;
	h += gTextComposer.GetTextBoxSize(iFormat(gTextMgr[TRID_SURRENDER_STATES],m_pHero->Name().CStr()), w, dlgfc_topic).h;	
	h += gTextComposer.GetTextBoxSize(iFormat(gTextMgr[TRID_SURRENDER_CONDITION],m_cost), w, dlgfc_topic).h;	

	h += DEF_BTN_HEIGHT;

	return iSize(w, h); 
}

void iDlg_SurrenderCond::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == DRC_OK || uid == DRC_CANCEL) {
		EndDialog(uid); 
	}
}


