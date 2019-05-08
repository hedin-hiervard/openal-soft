#ifndef PHEROES_GAME_DLG_TIMEEVENT_H_
#define PHEROES_GAME_DLG_TIMEEVENT_H_

class iDlg_TimeEvent : public iTextDlg
{
public:
	iDlg_TimeEvent(iViewMgr* pViewMgr, PLAYER_ID pid, const iTimeEvent& timeEvent);

protected:
	virtual void DoCompose(const iRect& clRect);
	virtual iSize ClientSize() const;

private:
	void OnCreateDlg();

	const iTimeEvent&	m_timeEvent;
	PLAYER_ID			m_pid;
};


#endif //PHEROES_GAME_DLG_TIMEEVENT_H_
