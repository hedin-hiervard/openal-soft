#include "stdafx.h"
#include "GameMenuDlg.h"

//#include "xxc/xxc.security.h"

//Settings, Save game, Main menu, Quit, Return to game
iGameMenuDlg::iGameMenuDlg(iViewMgr* pViewMgr, bool battle)
: iBaseGameDlg(pViewMgr), m_battle(battle)
{ 
}

void iGameMenuDlg::OnCreateDlg()
{
	iRect rc = ClientRect();
	rc.h = DEF_BTN_HEIGHT+2;
	uint32 saveload = 1;
#ifdef MULTIPLAYER
	if(gMPMgr.IsMultiplayer()) 
		saveload = 0;
#endif

	AddChild(new iTextButton(m_pMgr, this, rc, TRID_MENU_SAVEGAME, GMRC_SAVE, ((!m_battle) && saveload)?(Visible|Enabled):Visible));
	rc.y+=DEF_BTN_HEIGHT+5;
	AddChild(new iTextButton(m_pMgr, this, rc, TRID_MENU_LOADGAME, GMRC_LOAD, saveload ? (Visible|Enabled):Visible));
	rc.y+=DEF_BTN_HEIGHT+5;
	AddChild(new iTextButton(m_pMgr, this, rc, TRID_MENU_EXITTOMAINMENU, GMRC_MAINMENU, Visible|Enabled));
	rc.y+=DEF_BTN_HEIGHT+5;
	AddChild(new iTextButton(m_pMgr, this, rc, TRID_MENU_EXITGAME, GMRC_QUIT, Visible|Enabled));
	rc.y+=DEF_BTN_HEIGHT+15;
	AddChild(new iTextButton(m_pMgr, this, rc, TRID_MENU_RETURNTOGAME, GMRC_RETURN, Visible|Enabled));
}

iSize iGameMenuDlg::ClientSize() const
{
	return iSize(120,5*DEF_BTN_HEIGHT+3*5+15);
}

void iGameMenuDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	EndDialog(pView->GetUID());
}

