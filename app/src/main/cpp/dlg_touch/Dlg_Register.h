#ifndef PHEROES_GAME_DLG_REGISTER_H_
#define PHEROES_GAME_DLG_REGISTER_H_

//////////////////////////////////////////////////////////////////////////
class iEditCtrl;
class iDlg_Register : public iBaseGameDlg
{
public:
	iDlg_Register(iViewMgr* pViewMgr);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	inline void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {}

private:
	uint32			m_devId;
	iTextButton*	m_pRegBtn;
	iEditCtrl*		m_pEditCtrl;
};



#endif //PHEROES_GAME_DLG_REGISTER_H_
