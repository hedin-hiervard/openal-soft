#ifndef PHEROES_GAME_DLG_PUZZLEMAP_TOUCH_H_
#define PHEROES_GAME_DLG_PUZZLEMAP_TOUCH_H_

class iPuzzleMapDlg : public iDialog	
{
public:
	iPuzzleMapDlg(iViewMgr* pViewMgr, const iPlayer& player, uint16 newOpened);
	void OnCreateDlg();
	void OnTimer(uint32 tid);
	void OnMouseClick(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	iSize GetDlgMetrics() const;
	void OnCompose();
#if defined( PC_VERSION )
	void OnClickNo(){EndDialog(0);}
	void OnClickYes() { EndDialog(0); }
	void OnDown() { EndDialog(0); }
#endif	
protected:
	void PrepareMap();
	void DoCompose(const iRect& clRect);	

#if defined( PC_VERSION )
	void UpdateSize();
	iRect GetScrRect();
#endif

private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	iRect GetPuzzleCardRect(iPoint pt);
	iPoint GetPuzzleCardCoord(uint32 idx);
	iRect GetPuzzleCardRectWithPoint(const iPoint& pt);
protected:
	uint8			m_aniTimer;
	uint16			m_newOpened;
	const iPlayer&	m_player;
	iDib			m_dibMap;
	iDib			m_puzzledCell;
	iDib			m_mask;
};



#endif //PHEROES_GAME_DLG_PUZZLEMAP_TOUCH_H_
