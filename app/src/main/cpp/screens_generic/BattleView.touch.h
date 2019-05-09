#ifndef PHEROES_GAME_BATTLEVIEW_TOUCH_H_
#define PHEROES_GAME_BATTLEVIEW_TOUCH_H_
#pragma once

class iCreatDescView;
#if defined(OS_IPHONE) || defined(OS_ANDROID)
class iZoomer;
class iActionWheel;
#endif

class iBattleView_touch: public iBattleView {
public:
	iBattleView_touch();
	void OnDualSwipeLeft(sint32 length);
	void OnDualSwipeDown(sint32 length);
	void OnDualSwipeUp(sint32 length);
	virtual bool OnGroupChanged();

	virtual void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
#ifdef PC_VERSION
    virtual void OnMouseEntered(const iPoint& pos, MouseId mID);
    virtual void OnMouseRightDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual void OnMouseRightUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
#endif
	void RemoveCreaturePopup();

	virtual void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);

	virtual bool Process(fix32 t);
	virtual void ComposeSpellCursor();
	virtual void ComposeBattleCursors() {}
	virtual bool IsAcceptingCmds();
	virtual void UpdateSize();
#if defined(OS_IPHONE) || defined(OS_ANDROID)
    virtual inline bool AlwaysReceiveMouse() { return m_bAlwaysReceiveMouse; }
    void AfterCompose();
#endif
	virtual iSize GetDefaultSize() const { return iSize(iMIN<sint32>(1024, gApp.ViewMgr().Metrics().w), iMIN<sint32>(768, gApp.ViewMgr().Metrics().h)); }

private:
#if defined(OS_IPHONE) || defined(OS_ANDROID)
    bool IsInterestingCell(iBattleGroup* pGroup, const iPoint& cell, sint32 radius) const;
#endif
	void BeginBattle(iBattleWrapper* pBattle, SURF_TYPE st, BATTLE_BACK_TYPE background, BATTLE_SKY_TYPE sky);
	virtual iPoint& SpellTrackCell() { return m_SpellTrack; }
	virtual void UpdateToolbarMode() {}
public:
	virtual void EnableControls(uint32 flags);
private:
	iSimpleArray<iBattleGroup*> FindBattleGroupsWithAttraction(const iPoint &cell, sint32 attr);
	iBattleGroup* FindNearestGroup(iSimpleArray<iBattleGroup*> &list, const iPoint &pos, sint32 &dist);
	iPoint FindNearestCell(iSimpleArray<iPoint> &list, const iPoint &pos, sint32 &best);

	virtual bool BeginSpellTrack(iCombatSpell* pSpell);
	virtual void SetInfoMode(bool bInfoMode);
	virtual void HighlightMemberButton( uint32 type, bool bHighlight );

private:
	iHeroPortBtn*	m_pBtnAssaulter;
	iHeroPortBtn*	m_pBtnDefenderHero;
	iCastleGlyphBtn* m_pBtnDefenderCastle;
	iImgBtn*		m_pBtnCastSpell;
	iImgBtn*		m_pBtnWait;
	iImgBtn*		m_pBtnDefend;
	iImgBtn*		m_pBtnInfo;
	bool			m_bGesturesEnabled;
	fix32			m_moveDisapTimer;
	bool			m_bCurSelFlag;
	iPoint			m_initialTrackCell;
	bool			m_bModeChangeFlag;
	bool			m_bGestureComplete;
	iPoint			m_SpellTrack;
	bool			m_bInfoMode;
private:
	static const iPoint fingerOffset;
	iCreatDescView *m_pCreatInfoView;
#if defined(OS_IPHONE) || defined(OS_ANDROID)
    iZoomer			*m_pZoomer;
	iActionWheel	*m_pActionWheel;
	bool			m_bAlwaysReceiveMouse;
#endif
};

class iActionWheel: public iDialog, public IViewCmdHandler
{
public:
#if defined(OS_IPHONE) || defined(OS_ANDROID)
    iActionWheel(iViewMgr* pViewMgr, iBattleGroup *pGroup, iPoint cell, iDib& srf, iView* pUnderView, iRect src_rect);
#else
	iActionWheel(iViewMgr* pViewMgr, iBattleGroup *pGroup, iPoint cell);
#endif
    void OnMouseUp(const iPoint &rect, MouseId mID, MouseButtonId mbID);
	sint32 DoModal();
	void OnCompose();
#if defined(OS_IPHONE) || defined(OS_ANDROID)
    void UpdateSurface();
#endif
private:
	void OnCreateDlg();
	virtual iSize GetDlgMetrics() const;

	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
private:
	iBattleGroup*	m_pGroup;
	iPoint			m_cell;
#if defined(OS_IPHONE) || defined(OS_ANDROID)
    iDib& m_srf;
	iDib	m_buf;
	iView*	m_pUnderView;
	iRect m_src_rect;
#endif
};

#if defined(OS_IPHONE) || defined(OS_ANDROID)
class iZoomer: public iDialog, public IViewCmdHandler
{
public:
	iZoomer(iViewMgr* pViewMgr, iView *underView, iDib& srf, iRect src_rect);
	void OnMouseDown(const iPoint &rect, MouseId mID, MouseButtonId mbID);
	void OnMouseTrack(const iPoint &rect, MouseId mID, MouseButtonId mbID);
	void OnMouseUp(const iPoint &rect, MouseId mID, MouseButtonId mbID);
	sint32 DoModal();
	void OnCompose();
	inline iPoint GetRes() const { return m_res; }
	void UpdateSurface();
	iPoint ZoomedToReal(const iPoint& pos) const;
private:
	void OnCreateDlg();
	virtual iSize GetDlgMetrics() const;

	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
private:
	iDib& m_srf;
	iRect m_src_rect;
	iPoint	m_res;
	iDib	m_buf;
	iView*	m_pUnderView;
};
#endif

#endif //PHEROES_GAME_BATTLEVIEW_TOUCH_H_
