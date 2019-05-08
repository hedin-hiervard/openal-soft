#ifndef PHEROES_GAME_DLG_LEAVEGUARDS_H_
#define PHEROES_GAME_DLG_LEAVEGUARDS_H_

class iArmyExchange;

class iDlg_LeaveGuards : public iBaseGameDlg
{
public:
	iDlg_LeaveGuards(iViewMgr* pViewMgr, iArmyOwner* pMainOwner, iArmyOwner* pLeaveBehind, TextResId title);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	inline void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {}
#if defined( PC_VERSION )
	void OnClickNo() {EndDialog(0);}
	void OnClickYes() { EndDialog(0); }
    void UpdateSize();
#endif    
private:
	iArmyOwner*		m_pMainOwner;
	iArmyOwner*		m_pLeaveBehind;
	iArmyExchange*	m_pArmyExchange;
	TextResId		m_title;
    iImgBtn*        m_pBtnOk;
};


#endif //PHEROES_GAME_DLG_LEAVEGUARDS_H_
