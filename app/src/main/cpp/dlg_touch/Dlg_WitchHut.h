#ifndef PHEROES_GAME_DLG_WITCHHUT_H_
#define PHEROES_GAME_DLG_WITCHHUT_H_

class iWitchHutDlg : public iBaseGameDlg
{
public:
	iWitchHutDlg(iViewMgr* pViewMgr, SECONDARY_SKILLS skill, PLAYER_ID pid);
	void OnCreateDlg();
	void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID);
#if defined( PC_VERSION )
	void OnClickNo() {EndDialog(0);}
	void OnClickYes() { EndDialog(0); }
	void OnDown() { EndDialog(0); }
#endif
protected:
	void DoCompose(const iRect& clRect);
	iSize ClientSize() const;

private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
#ifdef PC_VERSION
    void UpdateSize();
#endif
	iImgBtn* m_pTweetBtn;
protected:
	SECONDARY_SKILLS	m_skill;
};


#endif //PHEROES_GAME_DLG_WITCHHUT_H_
