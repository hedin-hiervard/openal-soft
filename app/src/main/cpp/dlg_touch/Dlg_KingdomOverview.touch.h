#ifndef PHEROES_GAME_DLG_KINGDOMOVERVIEW_TOUCH_H_
#define PHEROES_GAME_DLG_KINGDOMOVERVIEW_TOUCH_H_

class iKigdomOverviewDlg : public iDialog	
{
public:
	iKigdomOverviewDlg(iViewMgr* pViewMgr, const iPlayer& player);
	void OnCreateDlg();
	void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	iSize GetDlgMetrics() const;
	
protected:
	void DoCompose(const iRect& clRect);
	iSize ClientSize() const;

private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	static void ComposeCell(iDib& surf, const iRect& rect, MINERAL_TYPE mt, sint32 quant);

protected:
	const iPlayer&	m_player;
};



#endif //PHEROES_GAME_DLG_KINGDOMOVERVIEW_TOUCH_H_
