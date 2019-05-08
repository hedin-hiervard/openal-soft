#ifndef PHEROES_GAME_DLG_FIGHTGUARD_H_
#define PHEROES_GAME_DLG_FIGHTGUARD_H_

/*
 *	Fight guard question dialog
 */
class iDlg_FightGuard : public iTextDlg
{
public:
	iDlg_FightGuard(iViewMgr* pViewMgr, const iStringT& itemName, const iArmy& guard, PLAYER_ID pid, VISION_LEVEL vl);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	inline void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {}
#ifdef PC_VERSION
    void UpdateSize();
#endif
private:
	iStringT		m_itemName;
	const iArmy&	m_guard;
	VISION_LEVEL	m_vl;
    iTextButton*    m_pBtnYes;
    iTextButton*    m_pBtnNo;
    iTextButton*    m_pBtnQuick;
};


#endif //PHEROES_GAME_DLG_FIGHTGUARD_H_
