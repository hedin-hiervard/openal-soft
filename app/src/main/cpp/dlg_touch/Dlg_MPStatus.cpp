#ifdef MULTIPLAYER

#include "stdafx.h"
#include "Dlg_MPStatus.h"

iMPStatusDlg::iMPStatusDlg(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, PLAYER_ID pid, iTextComposer::FontConfig fc_title, iTextComposer::FontConfig fc_text)
: iTextDlg(pViewMgr, title ,text, pid, fc_title, fc_text), m_percent(-1)
{	
}

void iMPStatusDlg::OnCreateDlg()
{
	iRect r = GetScrRect();
	AddChild(m_pCancelBtn = new iTextButton(&gApp.ViewMgr(), this, iRect(10, r.h - DEF_BTN_HEIGHT - 10, r.w - 20, DEF_BTN_HEIGHT), TRID_CANCEL, 100, Visible|Enabled));
}	
void iMPStatusDlg::OnCompose()
{	
	iTextDlg::OnCompose();
	
	if(m_percent != -1) {
		iRect r = GetScrRect();
		iRect rc = iRect(r.x + 13, r.y + r.h - 18 - DEF_BTN_HEIGHT, r.w - 26, 10);
		ComposeProgressBar(false, gApp.Surface(), rc, 48, m_percent, 100);
	}	
}

void iMPStatusDlg::Show(bool bCancelBtn)
{	
	Center();
	OnCreateDlg();
	m_pCancelBtn->SetVisible(bCancelBtn);
	SetVisible();
	gApp.ViewMgr().PushModalDlg(this);
	iRect scrRect = GetScrRect();
	Compose(scrRect);
}

void iMPStatusDlg::SetPercent(sint32 percent)
{	
	m_percent = percent;
	iRect scrRect = GetScrRect();
	Compose(scrRect);
}

void iMPStatusDlg::Hide()
{
	gApp.ViewMgr().PopModalDlg();
}

iSize iMPStatusDlg::ClientSize() const {
	iSize res = iTextDlg::ClientSize();
	res.h += DEF_BTN_HEIGHT;
	return res;
}

void iMPStatusDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	if(pView->GetUID() == 100) {
		Hide();
		gMPMgr.DisconnectFromServer();		
	}
}

#endif //MULTIPLAYER