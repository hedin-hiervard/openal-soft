#ifndef PHEROES_GAME_COMMONDIALOGS_H_
#define PHEROES_GAME_COMMONDIALOGS_H_

/*
 *	Base game dialog
 */
/*class iBaseGameDlg : public iDialog, public  IViewCmdHandler
{
public:
	iBaseGameDlg(iViewMgr* pViewMgr);
	virtual ~iBaseGameDlg() {}

	iRect ClientRect() const;
	iSize GetMaxClientSize() const;

	iRect AlignChild(iSize sz, Alignment al, sint32 offsetFromTop = 0);

	// pure virtuals
	virtual void DoCompose(const iRect& clRect)  =0;
	virtual iSize ClientSize() const  =0;
protected:
	void OnCompose();
private:	
	iSize GetDlgMetrics() const;
};
*/
class iGlowGameDlg : public iDialog, public  IViewCmdHandler
{
public:
	iGlowGameDlg(iViewMgr* pViewMgr);
	virtual ~iGlowGameDlg() {}

	iRect ClientRect() const;
	iSize GetMaxClientSize() const;

	iRect AlignChild(iSize sz, Alignment al, sint32 offsetFromTop = 0);

	// pure virtuals
	virtual void DoCompose(const iRect& clRect)  =0;
	virtual iSize ClientSize() const  =0;
protected:
	void OnCompose();
public:
	static sint32 GetTopOffset();
	static sint32 GetDefaultWidth();
protected:
private:	
	iSize GetDlgMetrics() const;
private:	

};

typedef iGlowGameDlg iBaseGameDlg;

/*
 *	Base text dialog MB_OK
 */
class iTextDlg : public iBaseGameDlg
{
public:
	iTextDlg(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, PLAYER_ID pid, iTextComposer::FontConfig fc_title = dlgfc_hdr, iTextComposer::FontConfig fc_text = dlgfc_plain);
	virtual ~iTextDlg() {}
	virtual void OnCreateDlg();
	void SetText(const iStringT& text);
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
	iTextComposer::FontConfig m_fcTitle; 
	iTextComposer::FontConfig m_fcText;
	iStringT	m_title;
	iStringT	m_text;
};

/*
 *	Extended text dialog MB_OK
 */
class iExtTextDlg : public iBaseGameDlg
{
public:
	iExtTextDlg(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, const iStringT& sign, PLAYER_ID pid);
	virtual ~iExtTextDlg() {}
	virtual void OnCreateDlg();
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
#ifdef PC_VERSION
    void UpdateSize();    
#endif
    iTextButton* m_pBtnOk;
protected:
	iStringT	m_title;
	iStringT	m_text;
	iStringT	m_sign;
};

/*
 *	Base text with icon dialog MB_OK
 */
class iIconDlg : public iTextDlg
{
public:
	iIconDlg(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, SpriteId sid, PLAYER_ID pid);
	virtual ~iIconDlg() {}

protected:
	virtual void DoCompose(const iRect& clRect);
	virtual iSize ClientSize() const;

protected:
	SpriteId	m_sid;
};

/*
 *	Question dialog MB_YESNO
 */
class iQuestDlg : public iTextDlg
{
public:
	iQuestDlg(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, PLAYER_ID pid);
	virtual ~iQuestDlg() {}
	inline void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {}
#if defined( PC_VERSION )
	void OnClickNo() { EndDialog(3); }
	void OnClickYes() { EndDialog(2); }
	void OnDown() {}
#endif	
private:
	iSize ClientSize() const;
	void OnCreateDlg();
#ifdef PC_VERSION    
    void UpdateSize();
#endif
    iButton* m_pBtnYes;
    iButton* m_pBtnNo;
};


/*
 *	Pursue dialog MB_YESNO, MB_QUICK
 */
class iPursueDlg : public iTextDlg
{
public:
	iPursueDlg(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, PLAYER_ID pid);
	virtual ~iPursueDlg() {}
	inline void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {}
		
private:
	iSize ClientSize() const;
	void OnCreateDlg();
#ifdef PC_VERSION
    void UpdateSize();
#endif
    iTextButton* m_pBtnYes;
    iTextButton* m_pBtnNo;
    iTextButton* m_pBtnQuick;
};


#endif //PHEROES_GAME_COMMONDIALOGS_H_
