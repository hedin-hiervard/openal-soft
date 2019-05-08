#ifndef PHEROES_GAME_DLG_REWARDS_H_
#define PHEROES_GAME_DLG_REWARDS_H_

class iRewardBtn;
///
class iDlg_Rewards : public iBaseGameDlg
{

friend class iRewardBtn;

public:
	iDlg_Rewards(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, iHero* pHero, const iRewardsCtr& rewards);	
	void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
#if defined( PC_VERSION )
	void OnClickNo() {EndDialog(0);}
	void OnClickYes() { EndDialog(0); }
	void OnDown() { EndDialog(0); }
#endif
private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	
    void UpdateSize();

private:
	iStringT			m_title;
	iStringT			m_text;
	iHero*				m_pHero;
	const iRewardsCtr&	m_rewards;
    iSimpleArray<iRewardBtn*> m_pRewards;
};


#endif //PHEROES_GAME_DLG_REWARDS_H_
