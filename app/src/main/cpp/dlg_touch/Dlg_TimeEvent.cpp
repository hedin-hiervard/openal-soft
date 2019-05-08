#include "stdafx.h"
#include "Dlg_TimeEvent.h"

iDlg_TimeEvent::iDlg_TimeEvent(iViewMgr* pViewMgr, PLAYER_ID pid, const iTimeEvent& timeEvent)
: iTextDlg(pViewMgr, _T(""), gGame.Map().GetText(TET_TEVENT_TEXT, (void*)&timeEvent) + _T("\n\n#FFF0") + MineralSet2Text(timeEvent.m_minerals, true), pid), m_timeEvent(timeEvent)
{}

void iDlg_TimeEvent::OnCreateDlg()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(DRC_OK);
		return;
	}		
#endif
}

void iDlg_TimeEvent::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	iTextDlg::DoCompose(clRect);		
}

iSize iDlg_TimeEvent::ClientSize() const
{
	iSize res = iTextDlg::ClientSize();
    res.w = iMIN<sint32>(gApp.Surface().GetWidth(), res.w);
    res.h = iMIN<sint32>(gApp.Surface().GetHeight(), res.h);
    
	return res;
}

