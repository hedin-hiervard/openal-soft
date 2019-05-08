#ifndef PHEROES_GAME_DLG_NEWWEEK_H_
#define PHEROES_GAME_DLG_NEWWEEK_H_

class iDlg_NewWeek : public iBaseGameDlg
{
public:
	iDlg_NewWeek(iViewMgr* pViewMgr, PLAYER_ID pid, const iWeekDesc& weekDesc);
	void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
#ifdef PC_VERSION
    void OnClickNo() {EndDialog(0);}
	void OnClickYes() { EndDialog(0); }
    void OnDown() { EndDialog(0); }
#endif
private:
	iStringT	m_title;
	iStringT	m_subTitle;
	iStringT	m_text;
	iStringT	m_text2;
	iWeekDesc	m_weekDesc;
};


#endif //PHEROES_GAME_DLG_NEWWEEK_H_
