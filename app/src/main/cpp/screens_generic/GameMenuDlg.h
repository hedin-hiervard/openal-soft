#ifndef PHEROES_GAME_GAMEMENUDLG_H_
#define PHEROES_GAME_GAMEMENUDLG_H_

enum {
	GMRC_NEWGAME,
	GMRC_SAVE,
	GMRC_LOAD,
	GMRC_RESTART,
	GMRC_MAINMENU,
	GMRC_QUIT,
	GMRC_RETURN,
	GMRC_COUNT
};

class iGameMenuDlg : public iBaseGameDlg
{
public:
	iGameMenuDlg(iViewMgr* pViewMgr, bool battle);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect) {}
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	bool m_battle;

protected:
	iTextButton*	m_pbtnOk;
};



#endif //PHEROES_GAME_GAMEMENUDLG_H_
