#ifdef MULTIPLAYER

#include "stdafx.h"
#include "MPLobbyView.h"
#include "Dlg_ScenList.h"
#include "shellapi.h"

iMPLobbyView::iMPLobbyView(): iChildGameView(false, iChildGameView::CV_MENU) 
{
	iPoint offset((gApp.Surface().GetWidth() - 320) / 2, (gApp.Surface().GetHeight() - 240) / 2);

	//AddChild(new iUserListBox(&gApp.ViewMgr(), this, iRect(10, 10, 155, 200), 100));
	AddChild(m_gameList = new iGameListBox(&gApp.ViewMgr(), this, iRect(offset + iPoint(60, 80), iSize(210, 100)), 101));
	AddChild(m_joinBtn = new iTextButton(&gApp.ViewMgr(), this, iRect(offset + iPoint(3, 210), iSize(75, DEF_BTN_HEIGHT)), TRID_JOIN, 104, Visible));
	AddChild(m_createBtn = new iTextButton(&gApp.ViewMgr(), this, iRect(offset + iPoint(83, 210), iSize(75, DEF_BTN_HEIGHT)), TRID_CREATE, 103, Visible));
	AddChild(m_profileBtn = new iTextButton(&gApp.ViewMgr(), this, iRect(offset + iPoint(163, 210), iSize(75, DEF_BTN_HEIGHT)), TRID_PROFILE, 105, Visible));	
	AddChild(m_cancelBtn = new iTextButton(&gApp.ViewMgr(), this, iRect(offset + iPoint(243, 210), iSize(75, DEF_BTN_HEIGHT)), TRID_CANCEL, 102, Visible | Enabled));		

#ifndef OS_IPHONE
	iPHScrollBar* pScrollBar = new iPHScrollBar(&gApp.ViewMgr(), this, iRect(offset + iPoint(262, 80), iSize(15, 100)), 300);
	AddChild(pScrollBar);
	m_gameList->SetScrollBar(pScrollBar);
#endif

}

iMPLobbyView::~iMPLobbyView() {}

void iMPLobbyView::OnCompose()
{
	gApp.Surface().FillRect(iRect(iPoint(), gApp.Surface().GetSize()), RGB16(0,0,0));
	iPoint offset((gApp.Surface().GetWidth() - 320) / 2, (gApp.Surface().GetHeight() - 240) / 2);
	gGfxMgr.Blit(PDGG(MENU_BACK), gApp.Surface(), offset);
	iStringT text = iStringT(CvtA2T<>(gMPMgr.GetNickname().CStr()));
	gTextComposer.TextOut(gApp.Surface(), offset + iPoint(169, 195), text);
	
	gTextComposer.TextOut(gApp.Surface(), offset + iPoint(10, 195), iFormat(_T("%s: %d"), gTextMgr[TRID_PLAYERS_IN_LOBBY], gMPMgr.GetUserList().GetSize()));
}

BOOL WINAPI ShellExecuteEx(LPSHELLEXECUTEINFO);

void iMPLobbyView::SetActive(bool bAct)
{
	m_createBtn->SetEnabled(bAct);
	m_profileBtn->SetEnabled(bAct);
	m_cancelBtn->SetEnabled(bAct);
}

void iMPLobbyView::iCMDH_ControlCommand(iView *pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if(uid == 101) {
		if(cmd == CCI_LBSELCHANGED)
			m_joinBtn->SetEnabled(param != -1);
	} else if(uid == 102) { //exit
		gMPMgr.DisconnectFromServer();
		//gGame.MainMenu();
	} else if(uid == 103) { //create game
		iScenListDlg sldlg(&gApp.ViewMgr(), SCL_MULTI);
		sint32 res = sldlg.DoModal();
		if (res == DRC_OK) {
			iMapInfo scenProps = sldlg.SelScen();
			scenProps.ReorderPlayers();
			gMPMgr.CreateGame(scenProps);			
		} 						
	} else if(uid == 104) { //join
		if(m_gameList->SelItem() != -1) {
			gMPMgr.JoinGame(gMPMgr.GetGameList()[m_gameList->SelItem()].m_id);
		}
	} else if(uid == 105) {
//#ifdef OS_WIN32
		iStringT tmp = iStringT(CvtA2T<>(gMPMgr.AuthKey().CStr()));		
		iStringT tmpstr = iFormat(_T("http://173.203.98.85/userpanel.php?key=%s"), tmp.CStr());
		SHELLEXECUTEINFO cShellExecuteInfo;
		memset(&cShellExecuteInfo, 0, sizeof(cShellExecuteInfo)); // Set up memory block
		cShellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		cShellExecuteInfo.fMask = 0;//SEE_MASK_NOCLOSEPROCESS;
		cShellExecuteInfo.hwnd = NULL;
		cShellExecuteInfo.lpVerb = _T("Open");
		cShellExecuteInfo.lpFile = tmpstr.CStr();
		cShellExecuteInfo.nShow = SW_SHOWNORMAL;
		ShellExecuteEx(&cShellExecuteInfo);
		gGame.Quit();
		//HINSTANCE r = ShellExecuteW(NULL, _T("open"), iFormat(_T("http://91.151.255.160/userpanel.php?key=%s"), tmp.CStr()).CStr(), NULL, NULL, SW_SHOWNORMAL);
//#endif
	}
}


#endif //MULTIPLAYER