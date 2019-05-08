#ifndef PHEROES_GAME_DLG_RISESKELETONS_H_
#define PHEROES_GAME_DLG_RISESKELETONS_H_

class iDlg_RiseSkeletons : public iBaseGameDlg
{
public:
	iDlg_RiseSkeletons(iViewMgr* pViewMgr, PLAYER_ID pid, CREATURE_TYPE ct, sint32 quant );

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID);

private:
	CREATURE_TYPE	m_ct;
	sint32			m_quant;
};



#endif //PHEROES_GAME_DLG_RISESKELETONS_H_
