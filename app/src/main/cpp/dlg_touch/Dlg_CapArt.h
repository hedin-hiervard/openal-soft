#ifndef PHEROES_GAME_DLG_CAPART_H_
#define PHEROES_GAME_DLG_CAPART_H_

class iDlg_CapArt : public iBaseGameDlg
{
public:
	iDlg_CapArt(iViewMgr* pViewMgr, const iHero* pHero, const iArtList& artList);
	void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
#ifdef PC_VERSION
    void UpdateSize();
#endif
private:
	const iHero*	m_pHero;
	bool			m_bSingle;
	iStringT		m_Title;
	iStringT		m_TwitterTitle;
	const iArtList&	m_artList;
	iImgBtn*		m_pTweetBtn;
    iArtListCtrl*   m_pArtCtrl;
};



#endif //PHEROES_GAME_DLG_CAPART_H_
