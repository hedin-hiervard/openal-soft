#ifndef PHEROES_GAME_DLG_SKILLINFO_H_
#define PHEROES_GAME_DLG_SKILLINFO_H_

class iSecSkillInfoDlg : public iBaseGameDlg
{
public:
	iSecSkillInfoDlg(iViewMgr* pViewMgr, SECONDARY_SKILLS skill, SECSKILL_LEVEL level, PLAYER_ID pid);
	virtual void OnCreateDlg();
	void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID);
#if defined( PC_VERSION )
	void OnClickNo() {EndDialog(0);}
	void OnClickYes() { EndDialog(0); }
	void OnDown() { EndDialog(0); }
#endif	
protected:
	virtual void DoCompose(const iRect& clRect);
	virtual iSize ClientSize() const;

private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

protected:
	SECONDARY_SKILLS	m_skill;
	SECSKILL_LEVEL		m_level;
};


#endif //PHEROES_GAME_DLG_SKILLINFO_H_
