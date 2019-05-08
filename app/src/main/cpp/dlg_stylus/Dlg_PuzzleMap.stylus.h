#ifndef PHEROES_GAME_DLG_PUZZLEMAP_STYLUS_H_
#define PHEROES_GAME_DLG_PUZZLEMAP_STYLUS_H_

class iPuzzleMapDlg : public iBaseGameDlg
{
public:
	iPuzzleMapDlg(iViewMgr* pViewMgr, const iPlayer& player, uint16 newOpened);
	void OnCreateDlg();
	void OnTimer(uint32 tid);
	void OnMouseClick(const iPoint &pos, MouseId mID);
	
protected:
	void PrepareMap();
	void DoCompose(const iRect& clRect);
	iSize ClientSize() const;

private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

protected:
	uint8			m_aniTimer;
	uint16			m_newOpened;
	const iPlayer&	m_player;
	iDib			m_dibMap;
	iDib			m_puzzledCell;
};



#endif //PHEROES_GAME_DLG_PUZZLEMAP_STYLUS_H_
