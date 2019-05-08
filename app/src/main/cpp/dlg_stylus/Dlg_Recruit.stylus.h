#ifndef PHEROES_GAME_DLG_RECRUIT_STYLUS_H_
#define PHEROES_GAME_DLG_RECRUIT_STYLUS_H_

class iCreatDescView;

class iDlg_Recruit : public iBaseGameDlg
{
public:
	iDlg_Recruit(iViewMgr* pViewMgr, iCreatGroup& creats, const iArmy& army, PLAYER_ID owner, iHero *m_pHero);
private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	inline void OnMouseUp(const iPoint &pos, MouseId mID) {}

private:
	uint32			m_avail;
	uint32			m_recr;
	uint32			m_max;

	iMineralSet		m_crCost;

	iIconButton*	m_pIcnButton;
	iPHScrollBar*	m_pSlider;
	iCreatGroup&	m_creats;
	const iArmy&			m_army;
	iHero*          m_pHero;
};


#endif //PHEROES_GAME_DLG_RECRUIT_STYLUS_H_
