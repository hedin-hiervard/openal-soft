#ifndef PHEROES_GAME_DLG_SURRENDERCOND_H_
#define PHEROES_GAME_DLG_SURRENDERCOND_H_

class iDlg_SurrenderCond : public iBaseGameDlg
{
public:
	iDlg_SurrenderCond(iViewMgr* pViewMgr, const iHero* pHero,  uint32 cost);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
#ifdef PC_VERSION
    void UpdateSize();
#endif
private:
	const iHero*	m_pHero;
	uint32			m_cost;
    iTextButton*    m_pBtnOk;
    iTextButton*    m_pBtnCancel;
};


#endif //PHEROES_GAME_DLG_SURRENDERCOND_H_
