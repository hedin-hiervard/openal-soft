#ifndef PHEROES_GAME_DLG_BATTLELOG_H_
#define PHEROES_GAME_DLG_BATTLELOG_H_

class iDlg_BattleLog : public iBaseGameDlg
{
public:
	iDlg_BattleLog(iViewMgr* pViewMgr, PLAYER_ID pid, const iSimpleArray<iStringT>& log);
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
	const iSimpleArray<iStringT>&	m_log;
	iListBox*						m_pLogList;
	iPHScrollBar*					m_pScrollBar;
};


#endif //PHEROES_GAME_DLG_BATTLELOG_H_
