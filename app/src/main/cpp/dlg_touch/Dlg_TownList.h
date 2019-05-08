#ifndef PHEROES_GAME_DLG_TOWNLIST_H_
#define PHEROES_GAME_DLG_TOWNLIST_H_

class iTownListBox;
class iDlg_TownList : public iBaseGameDlg
{
public:
	iDlg_TownList(iViewMgr* pViewMgr, iPlayer* pOwner, const iStringT& title, TextResId okBtnText, bool bCanCancel);
	inline iCastle* Destination() { return m_pDestination; }
	virtual bool Process(fix32 t);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
#ifdef PC_VERSION
    void UpdateSize();
#endif
protected:
	iCastle*		m_pDestination;
	iPlayer*		m_pOwner;
	iStringT		m_title;
	TextResId		m_okBtnText;
	iTownListBox*	m_pTownList;
	iPHScrollBar*	m_pScrollBar;
	iButton*	m_pbtnOk;
	iButton*	m_pBtnCancel;
	bool m_bCanCancel;
};



#endif //PHEROES_GAME_DLG_TOWNLIST_H_
