#ifndef PHEROES_GAME_DLG_ARTINFO_H_
#define PHEROES_GAME_DLG_ARTINFO_H_

class iDlg_ArtInfo : public iBaseGameDlg
{
public:
	iDlg_ArtInfo(iViewMgr* pViewMgr, uint16 artId, const iHero* pOwner);
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
	uint16			m_artId;
	const iHero*	m_pOwner;
	iStringT			m_reqMsg;
	iTextComposer::FontConfig m_fcReqMsg;
	iImgBtn*		m_pTweetBtn;
};


#endif //PHEROES_GAME_DLG_ARTINFO_H_
