#ifndef GXLIB_GXL_STDCTRL_H_
#define GXLIB_GXL_STDCTRL_H_

//////////////////////////////////////////////////////////////////////////
enum CTRL_CMD_ID {
	CCI_BTNCLICK,
	CCI_BTNDBLCLICK,
	CCI_TABCHANGED,
	CCI_CHECKED,
	CCI_SBLINEUP,
	CCI_SBLINEDOWN,
	CCI_SBPAGEUP,
	CCI_SBPAGEDOWN,
	CCI_SBTRACKING,
	CCI_LBSELCHANGED,
	CCI_LBSELDBLCLICK,
	CCI_LBSELECTED,
	CCI_EDITCHANGED,
	CCI_HOLD,
	CCI_RELEASE
};

//////////////////////////////////////////////////////////////////////////
struct IViewCmdHandler
{
	virtual void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param) =0;
};

/*
 *	Base view-port based control
 */
class iBaseCtrl : public iView
{
public:
	iBaseCtrl(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 clsId, uint32 uid, uint32 state);
	void SetCommandHandler(IViewCmdHandler* pCmdHandler);
	virtual ~iBaseCtrl() {}

protected:
	IViewCmdHandler*	m_pCmdHandler;
};

/*
 *	Generic push putton
 */
class iButton : public iBaseCtrl
{
public:
	enum State {
		Pressed		= 0x1,
		Selected	= 0x2,
		Disabled	= 0x4
	};
	
	iButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 state);
	virtual ~iButton() {}	
	inline uint32 GetButtonState() const { return m_state | (IsEnabled()?0:Disabled); }
	inline void SetHoldable(bool h) { m_bHoldable = h; }
	inline void SetSwitch(bool h) { m_bSwitch = h; }
	void Release();
	void Unhighlight();
private:
	void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	
protected:
	virtual void OnBtnDown() const {}
	virtual void OnBtnUp() const {}

protected:
	uint32				m_state;
	uint32				m_btnLastClick;
	bool				m_bHoldable;
	bool				m_bSwitch;
#if defined( PC_VERSION )
	bool m_bEntered;
#endif
};

/*
 * Tabbed switch control	
 */
class iTabbedSwitch : public iBaseCtrl
{
public:
	iTabbedSwitch(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 tabcnt, uint32 uid, uint32 state = Visible|Enabled);
	virtual ~iTabbedSwitch();

	// Pure methods
	virtual void ComposeTabItem(uint32 idx, uint32 itemState, const iRect& rect) =0;

	// inlines
	void EnableTab(sint32 idx, bool bEnable = true) { check(idx<m_TabsCount); m_tabStates[idx] = bEnable; }
	inline bool IsTabEnabled(sint32 idx) const { check(idx<m_TabsCount); return m_tabStates[idx] != 0; }
	inline sint32 GetTabsCount() const { return m_TabsCount; }
	inline void SetCurrentTab(sint32 ntab) { check(ntab<m_TabsCount); m_CurTab = ntab; }
	inline sint32 GetCurrentTab() const	{  return m_CurTab; }
	inline sint32 GetFocusedTab() const	{  return m_CurFocTab; }
	inline sint32 GetEnabledTabsCount() const { sint32 res = 0; for(sint32 x=0; x<m_TabsCount; x++) res += IsTabEnabled(x); return res; }
	inline sint32 GetEnabledCurrentTab() const { sint32 tmp = m_CurTab; for(sint32 x=0; x<=m_CurTab; x++) tmp -= !IsTabEnabled(x); return tmp; }
	
private:
	void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnCompose();

protected:

	inline sint32 GetTabByPos(const iPoint& pos) const { if (!iRect(m_Rect.size()).PtInRect(pos)) return -1; return (pos.x) / m_ItemWidth; }

	uint32*				m_tabStates;
	uint32				m_ItemWidth;
	sint32				m_TabsCount;
	sint32				m_CurTab;
	sint32				m_CurFocTab;
	sint32				m_FocTab;
};

/*
 *	Scroll Bar
 */
class iScrollBar : public iBaseCtrl, public IViewCmdHandler
{
public:
	enum Element {
		EL_Bkgnd = 0,
		EL_Thumb,
		EL_BtnUp,
		EL_BtnDown,
		EL_BtnLeft,
		EL_BtnRight
	};

	enum HitTestRes {
		HTR_PgUp = 0,
		HTR_PgDown,
		HTR_Thumb
	};

	enum Flags {
		Horizontal = 0x1,	// default is vertical
	};

public:
	class iTBButton : public iButton
	{
	public:
		iTBButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 state, iScrollBar* pScrollBar, Element el);
		void OnCompose();

	private:
		Element		m_el;
		iScrollBar* m_pScrollBar;
	};
	
public:
	iScrollBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 flags=0);
	virtual ~iScrollBar() {}

	void SetMetrics(sint32 totSiz, sint32 pagSiz);
	bool SetCurPos(sint32 nVal);
	bool PageUp();
	bool PageDown();
	inline sint32 CurPos() const { return m_curPos; }
	inline sint32 MaxPos() const { return m_totSiz - m_pagSiz; }
	inline bool IsHorizontal() const { return (m_flags & Horizontal)!=0; }
	uint32 GetDefBtnSiz() const { return 19; } 

	// Pure methods
	virtual void ComposeSBElement(Element el, const iRect& rc, uint32 flags) =0;

private:
	void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnCompose();
	
protected:
	HitTestRes HitTest(const iPoint& pos);
	inline sint32 ItemsHeight(sint32 icnt) { return (m_thumbMetrix.h * icnt)/m_totSiz; }
	inline sint32 NItems(sint32 n) { return (m_totSiz * n)/m_thumbMetrix.h; }
	void CalcThumbMetrix();
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

protected:
	struct ThumbMetrix {
		sint32	es, h, s1, th, s2;
	};

	uint32				m_flags;
	bool				m_bThumbTrack;
	iPoint				m_trackAnchor;
	sint32				m_trackPos;

	ThumbMetrix			m_thumbMetrix;
	sint32				m_totSiz;
	sint32				m_pagSiz;
	sint32				m_curPos;
	iTBButton*			m_pUp;
	iTBButton*			m_pDown;
};

/*
 *	List Box (virtual)
 */
class iListBox : public iBaseCtrl, public IViewCmdHandler
{
public:
	iListBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
	virtual ~iListBox() {}

	void UpdateListBox();

	void SetCurPos(sint32 sval);
	void SetCurSel(sint32 idx, bool bNotify);
	void SelPrev();
	void SelNext();
	inline sint32 SelItem() const { return m_selItem; }
	inline uint32 PageSize() const { return m_Rect.h / LBItemHeight(); }
public:
	inline virtual sint32 LBTopOffset() const { return 0; }
	virtual void ComposeUpArrow() const {}
	virtual void ComposeDownArrow() const {}
	// Pure methods
	virtual void ComposeLBBackground(const iRect& rect) =0;
	virtual void ComposeLBItem(uint32 iIdx, bool bSel, const iRect& irc) =0;
	virtual uint32 LBItemHeight() const =0;
	virtual uint32 LBItemsCount() const =0;
	void Process(fix32 t);
	
private:
	void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);	
		
protected:   

    virtual void OnCompose(); // moved by Cotulla

	inline bool IsValidIdx(sint32 idx) { return idx >= 0 && idx < (sint32)LBItemsCount(); }
	sint32 IdxByPos(const iPoint& pos);
	sint32 TopIdx(sint32 scroll);
	void SetScrollPos(sint32 newpos);
protected:
	sint32		m_selItem;

	sint32		m_initialPos;
	sint32		m_scrollPos;

	sint32		m_velocity;
	fix32		m_timer;	
	fix32		m_holdTimer;
};


// new iListBoxEx

class iListBoxView
{
public:
	
	virtual void OnMouseWheelDown(sint16 line, MouseId mID) = 0;
	virtual void OnMouseWheelUp(sint16 line, MouseId mID) = 0;	
};

class iListBoxEx : public iListBox, public iListBoxView
{
public:
    iListBoxEx(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, iDib &dib, uint32 uid) 
        : iListBox(pViewMgr, pCmdHandler, rect, uid), m_dib(dib)
    { }

    virtual void ExComposeLBItem(iDib& dib, sint32 iIdx, bool bSel, const iRect& irc) = 0;
    virtual void ComposeUpArrow() const {}
    virtual void ComposeDownArrow() const {}
    virtual void ComposeLBBackground(const iRect& rect) =0;
    virtual void ComposeLBItem(uint32 iIdx, bool bSel, const iRect& irc) {};
    virtual uint32 LBItemHeight() const =0;
    virtual uint32 LBItemsCount() const =0;

    bool IsScrolling() { return true; }
    
	void OnMouseWheelDown(sint16 line, MouseId mID){
	
		SetScrollPos(m_scrollPos - line*6);		
		//m_velocity = line;
		
		Invalidate();
		
	//	m_scrollPos += line;
		//m_velocity += line;				
		//SelNext();
	}
	
	void OnMouseWheelUp(sint16 line, MouseId mID){
	
		SetScrollPos(m_scrollPos + line*6);
		//m_velocity += line;
		//SelPrev();
	}

protected:
    virtual void OnCompose();
    iDib &m_dib;
};




#endif //GXLIB_GXL_STDCTRL_H_
