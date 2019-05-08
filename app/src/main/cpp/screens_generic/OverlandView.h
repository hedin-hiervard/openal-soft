#ifndef PHEROES_GAME_OVERLANDVIEW_H_
#define PHEROES_GAME_OVERLANDVIEW_H_

/*
 *	Overland (Main) View
 */
class iOpTurnBar;

class iMainView : public iChildGameView
{
public:
	enum HighlightType
	{
		HLT_NONE = -1,
		HLT_AVAILABLE = 0,
		HLT_NOT_AVAILABLE
	};
public:
	iMainView();
	~iMainView();

	virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); } 
	virtual void OnTimer(uint32 tid) =0;
	void OnSizeChanged(const iSize& nsiz);
	virtual void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) =0;
	virtual void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID) =0;
	virtual void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID) =0;
	virtual void OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID) =0;
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID) =0;
	virtual void OnMouseEntered(const iPoint& pos, MouseId mID) =0;
	virtual bool OnKeyDown(sint32 key) =0;
	virtual bool OnKeyUp(sint32 key) =0;
	virtual void OnCompose() =0;			

	virtual void EnableToolBar(bool bEnable) =0;
	// Process
	bool Process(fix32 t);
	virtual void OnActivate(bool bActivate) =0;
	
	// Game notifications
	virtual void OnPlayerChanged(iPlayer* pPlayer, bool bAct);

	virtual CHILD_VIEW Type() const { return CV_OVERLAND; }

	// Events
	void AddMsg(const iStringT& text);
	void AddCellMsg(const iStringT& msg, const iPoint& pos);

	iBaseMapObject*	ObjectHitTest(const iPoint& scr_pos);
	inline bool IsForceTrack() const { return m_bForceTrack; }
	
	virtual void SetSurveyMode(bool bSurveyMode) =0;
	inline void SetForceHelp(bool bForceHelp) { m_bForceHelp = bForceHelp;} 
	inline bool IsForceHelp() const { return m_bForceHelp; }
	inline uint8 GetMouseTrackMask() const { return m_MouseTrackMask; }
	inline uint8 GetKeyTrackMask() const { return m_KeyTrackMask; }
	inline iOLComposer& Composer() { return m_OLComposer; }

	void ShowHelpPopup(const iPoint& pos);

	virtual void SetLHandMode(bool lhandmode) =0;
	virtual void ResetTapMode() =0;
	inline bool IsHighlightedObject(iBaseMapObject* pObj) const { return pObj == m_pLastHighlightedObject || pObj == m_pHighlightedObject; }
	inline bool IsHighlightedCell(const iPoint& pt) const { return pt == m_highlightedCell; }
	inline HighlightType ObjHighlightType() const { return m_objHighlightType; }

	inline void HighlightObject(iBaseMapObject* pObject, HighlightType type) { m_pHighlightedObject = pObject; m_pLastHighlightedObject = NULL; m_objHighlightTimer = fix32::zero; m_objHighlightType = type; }

	iRect GetVisibleMapRect() const;
#ifdef MULTIPLAYER
	inline bool		ShowMPActivity() const { return m_bShowMpActIcon; }
#endif	
	virtual void UpdateMetrics() =0;
    virtual void UpdateSize();

	
	// 
	virtual void OnCastleListChanged() =0;
	virtual void OnHeroListChanged() =0;
	
	inline void CleanupAni() {
		m_OLComposer.Cleanup();
	}
	
protected:
	virtual void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param) =0;
	virtual void ComposeEventList() =0;
	
protected:
	iOLComposer		m_OLComposer;
	iEventList		m_EventList;
	iPopupView*		m_pPopupView;
	
	bool		m_bForceTrack;
#ifdef MULTIPLAYER
	bool m_bInactive;
#endif
	iOpTurnBar*	m_pOpTurnBar;	
	bool		m_bForceHelp;

	uint8		m_MouseTrackMask;	// 4 bits Up,Down,Left,Right
	uint8		m_KeyTrackMask;

	bool		m_bSurveyMode;
	iDib		m_dibSurveyBuff;
	iDib		m_tmpDib;
	sint32		m_holdTimer;
	iPoint		m_holdPos;
	
	iRect		rcScrLeft;
	iRect		rcScrRight;
	iRect		rcScrTop;
	iRect		rcScrBottom;
#ifdef MULTIPLAYER	
	bool		m_bShowMpActIcon;	

#endif
	fix32		m_objHighlightTimer;
	iBaseMapObject* m_pHighlightedObject;
	iBaseMapObject* m_pLastHighlightedObject;
	iPoint		m_highlightedCell;
	iPoint		m_lastHighlightedCell;
	HighlightType m_objHighlightType;
};

/*
 *	Opponent turn information bar
 */
class iOpTurnBar : public iView
#ifdef MULTIPLAYER
, public IViewCmdHandler
#endif
{
public:
	iOpTurnBar(iViewMgr* pViewMgr, const iRect& rect);
	void OnCompose();
private:
#ifdef MULTIPLAYER
	iButton*	m_pKickBtn;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
#endif
};




#endif //PHEROES_GAME_OVERLANDVIEW_H_
