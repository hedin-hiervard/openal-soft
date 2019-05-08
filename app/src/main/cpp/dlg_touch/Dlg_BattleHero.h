#ifndef PHEROES_GAME_DLG_BATTLEHERO_H_
#define PHEROES_GAME_DLG_BATTLEHERO_H_

class iDlg_BattleHero : public iBaseGameDlg
{
public:
	enum Result {
		Close = DRC_CANCEL,
		Surrender,
		Retreat,
		AutoBattle,
		MainMenu
	};

	enum Flags {
		CanSurrender	= 0x1,
		CanRetreat		= 0x2
	};


public:
	iDlg_BattleHero(iViewMgr* pViewMgr, iHero& hero, bool bActiveMember, uint32 flags);
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
	iHero&	m_hero;
	uint32	m_flags;
	bool m_bActiveMember;
    iTextButton* m_pBtnSurrender;
    iTextButton* m_pBtnretreat;
    iTextButton* m_pBtnAutocombat;
    iTextButton* m_pBtnExitgame;
};



#endif //PHEROES_GAME_DLG_BATTLEHERO_H_
