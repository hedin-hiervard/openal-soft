#ifndef PHEROES_GAME_DLG_ARMYROOM_H_
#define PHEROES_GAME_DLG_ARMYROOM_H_

class iDlg_ArmyRoom : public iBaseGameDlg
{
public:
	iDlg_ArmyRoom(iViewMgr* pViewMgr, iHero& hero, const iCreatGroup& cgroup);

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
	iHero&		m_hero;
	iArmyOwner	m_fOwner;
	iStringT	m_Title;
    iArmyExchange* pArmyExchange;
    iButton* m_pBtnOk;
};




#endif //PHEROES_GAME_DLG_ARMYROOM_H_
