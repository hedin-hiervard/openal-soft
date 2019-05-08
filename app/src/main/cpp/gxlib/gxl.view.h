#ifndef GXLIB_GXL_VIEW_H_
#define GXLIB_GXL_VIEW_H_

enum VIEWCLSID {
	GENERIC_VIEWPORT = 0,
	MODAL_DIALOG,
	PUSH_BUTTON,
	TABBED_SWITCH,
	SCROLL_BAR,
	LIST_BOX
};

//////////////////////////////////////////////////////////////////////////
enum CTRL_EVT_ID {
	CEI_BUTTONDOWN = 0,
	CEI_BUTTONUP,
	CEI_TRACKING,
	CEI_CLICK
};

//////////////////////////////////////////////////////////////////////////
class iViewMgr;
class iView
{
public:
	enum ViewState {
		Visible	= 0x1,
		Enabled = 0x2
	};

public:
	iView(iViewMgr* pViewMgr, const iRect& rect, uint32 clsId, uint32 uid, uint32 state);
	virtual ~iView();

	bool AddChild(iView* pChild);
	bool RemoveChild(iView* pChild);
	iView* GetChildByPos(const iPoint& pos) const;
	iView* GetChildById(uint32 uid) const;
    iView* GetChildByIndex(uint32 index) const;
    void SetVisibleById(uint32 uid, bool state) const;
	iViewMgr* ViewMgr() { return( m_pMgr ); }
	
	// Message process
	void Compose(iRect& rect);
	bool MouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	bool MouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	bool MouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	bool MouseEntered(const iPoint& pos, MouseId mID);
	bool MouseLeave(const iPoint& pos, MouseId mID);
	bool MouseWheelDown(sint16 line, MouseId mID);
	bool MouseWheelUp(sint16 line, MouseId mID);
	bool MouseRightDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	bool MouseRightUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);    
    
	inline bool IsMouseDown(MouseId mID) { return m_bTracking[mID]; }
	
	// Overrides
	virtual void OnTimer(uint32 tid) { };
	virtual void OnCompose() { };
	virtual void AfterCompose() { };
	virtual void OnRectChanged(const iRect& rc) {}
	virtual void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) {}	
	virtual void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID) {}
	virtual void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID) {}
	virtual void OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID) {}
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID) {}
	virtual void OnMouseEntered(const iPoint& pos, MouseId mID) {}
	virtual void OnMouseLeave(const iPoint& pos, MouseId mID) {}
	virtual void OnMouseWheelDown(sint16 line, MouseId mID) {}
	virtual void OnMouseWheelUp(sint16 line, MouseId mID) {}
	virtual void OnMouseRightDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) {}	
	virtual void OnMouseRightUp(const iPoint& pos, MouseId mID, MouseButtonId mbID) {}
    
	virtual void OnPinchOut(sint32 len) {}
	virtual void OnPinchIn(sint32 len) {}
	virtual void OnDualMouseTrack(const iPoint& pos) {}
	virtual void OnSandGlass() {}
	virtual void OnSwipeRight(sint32 len) {}
	virtual void OnSwipeLeft(sint32 len) {}
	virtual void OnSwipeUp(sint32 len) {}
	virtual void OnSwipeDown(sint32 len) {}
	virtual void OnDualSwipeRight(sint32 len) {}
	virtual void OnDualSwipeLeft(sint32 len) {}
	virtual void OnDualSwipeDown(sint32 len) {}
	virtual void OnDualSwipeUp(sint32 len) {}
	virtual void OnTripleSwipeRight(sint32 len) {}
	virtual void OnTripleSwipeLeft(sint32 len) {}
	void TransferMouseState(iView *fromView);
	
	virtual inline bool AlwaysReceiveMouse() { return false; }
	
	// View metrics
	void SetSize(const iSize& nsiz);
	void SetPos(const iPoint& npos);
	void SetRect(const iRect& rc);

	// inlines
	inline iSize GetSize() const { return m_Rect.size(); }
	inline iPoint GetPos() const { return m_Rect.point(); }
	inline iRect GetRect() const { return m_Rect; }
	inline iRect AlignChild(iSize sz, Alignment al, sint32 topOffset = 0) { iRect rc = m_Rect; rc.DeflateRect(0, topOffset, 0, 0); return AlignRect(sz, rc, al) - m_Rect; }
	inline iRect& MovableRect() { return m_Rect; }
	inline iPoint GetScrPos() const { iPoint res(m_Rect.point()); if (m_pParent) res+=m_pParent->GetScrPos();  return res; }
	inline iRect GetScrRect() const { iRect res(m_Rect); if(m_pParent) res += m_pParent->GetScrPos(); return res; }
	inline iView* GetParent() const { return m_pParent; }
	inline bool IsEnabled() const { return m_bEnabled && (!m_pParent || (m_pParent && m_pParent->IsEnabled())); }
	inline void SetEnabled(bool bEnabled=true) { m_bEnabled = bEnabled; Invalidate(); }
	inline bool IsVisible() const { return m_bVisible; }
	inline void SetVisible(bool bVisible = true) { m_bVisible = bVisible; Invalidate(); }
	inline uint32 GetClassId() const { return m_clsId; }
	inline uint32 GetUID() const { return m_UID; }
	inline bool NeedRedraw() const { return m_bNeedRedraw; }
	inline void SetData(sint32 d) {m_AuxData = d;}
	inline sint32 GetData() {return m_AuxData;}
	virtual void Invalidate();
    virtual void UpdateSize() {OnRectChanged(m_Rect); m_bNeedRedraw = true; Invalidate();};
    //
    virtual bool IsScrolling() { return false; }
private:
	bool CreateView(iViewMgr* pViewMgr, const iRect& rect, uint32 clsId, uint32 uid, uint32 state);
	void CheckUpGestures();
	void CheckTrackGestures();

protected:
	void Cleanup();
	uint8 PerformedFingers();
	uint8 ActiveFingers();
	bool  m_bIsDoubleClicking;
	sint32	m_lastClick;
	typedef iSimpleArray<iView*> iViewList;

	uint32				m_UID;
	uint32				m_clsId;
	iRect				m_Rect;
	sint32				m_AuxData;

	bool				m_bEnabled;
	bool				m_bVisible;
	bool				m_bTracking[MAX_MICE_COUNT];
	iPoint				m_BeginningTrackAnchor[MAX_MICE_COUNT];
	iPoint				m_TrackAnchor[MAX_MICE_COUNT];
	iPoint				m_EndTrackAnchor[MAX_MICE_COUNT];
	bool				m_bNeedRedraw;
	
	iViewMgr*			m_pMgr;
	iView*				m_pParent;
	iViewList			m_Childs;
};


#endif //GXLIB_GXL_VIEW_H_
