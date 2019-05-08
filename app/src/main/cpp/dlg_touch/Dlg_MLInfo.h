#ifndef PHEROES_GAME_DLG_MLINFO_H_
#define PHEROES_GAME_DLG_MLINFO_H_

class iMLInfoDlg : public iBaseGameDlg
{
public:
	iMLInfoDlg(iViewMgr* pViewMgr, bool bLuck, sint8 val, PLAYER_ID pid);
	void OnCreateDlg();
	void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID);
#if defined( PC_VERSION )
	void OnClickNo() {EndDialog(0);}
	void OnClickYes() { EndDialog(0); }
	void OnDown() { EndDialog(0); }
#endif
private:
	void DoCompose(const iRect& clRect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

private:
	iStringT	m_title;
	iStringT	m_text;
	bool		m_bLuck;
	sint8		m_val;
};


#endif //PHEROES_GAME_DLG_MLINFO_H_
