#ifndef PHEROES_GAME_DLG_BATTLECASTLE_H_
#define PHEROES_GAME_DLG_BATTLECASTLE_H_

class iDlg_BattleCastle : public iBaseGameDlg
{
public:
	enum Result {
		Close = DRC_CANCEL,
		AutoBattle,
		MainMenu
	};

public:
	iDlg_BattleCastle(iViewMgr* pViewMgr, iCastle& castle, bool bActiveMember);
	void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
#ifdef PC_VERSION
    void UpdateSize();
#endif
private:
	iCastle&	m_castle;
	bool		m_bActiveMember;
    iTextButton*    m_pBtnAutobattle;
    iTextButton*    m_pBtnMainMenu;
};



#endif //PHEROES_GAME_DLG_BATTLECASTLE_H_
