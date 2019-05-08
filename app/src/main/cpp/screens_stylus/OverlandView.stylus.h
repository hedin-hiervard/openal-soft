#ifndef PHEROES_GAME_OVERLANDVIEW_STYLUS_H_
#define PHEROES_GAME_OVERLANDVIEW_STYLUS_H_
#pragma once

class iInfoBar;
class iMainToolBar;

class iMainView_stylus: public iMainView
{
public:
	iMainView_stylus();

	inline void SetForceTrack(bool bForceTrack)	{ m_bForceTrack = bForceTrack; if (bForceTrack) m_OldAnchor = m_OLComposer.GetAnchor();} 
	virtual void UpdateMetrics();
	void EnableToolBar(bool bEnable);
	virtual void OnMouseDown(const iPoint& pos, MouseId mID);
	virtual void OnMouseDoubleClick(const iPoint& pos, MouseId mID);
	virtual void OnMouseClick(const iPoint& pos, MouseId mID);
	virtual void OnMouseUp(const iPoint& pos, MouseId mID);
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID);
	virtual void OnTimer(uint32 tid);
	virtual bool OnKeyDown(sint32 key);
	virtual bool OnKeyUp(sint32 key);	
	virtual void SetSurveyMode(bool bSurveyMode);
	virtual void OnCompose();
	virtual void ComposeEventList();			
	virtual void OnPlayerChanged(iPlayer* pPlayer, bool bAct);
	virtual void OnCastleListChanged();
	virtual void OnHeroListChanged();
	virtual void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	virtual void OnActivate(bool bActivate);
	virtual void UpdateSize();
	virtual void SetLHandMode(bool lhandmode) {};
	virtual void ResetTapMode() {};

private:
	iPoint		m_TrackAnchor;
	iPoint		m_OldAnchor;
	iInfoBar*		m_pInfoBar;
	iMainToolBar*	m_pToolBar;

};

/*
 *	Info bar
 */
class iInfoBar : public iView
{
public:
	enum Mode {
		Minerals,
		Army,
		System,
	};

public:
	iInfoBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect);
	void OnCompose();
	void OnMouseDown(const iPoint& pos, MouseId mID);
    void UpdateSize();

private:
	Mode	m_mode;
};


/*
 *	Overland view Toolbars
 */
class iHeroTB : public iView, public IViewCmdHandler
{
private:
	class iPortBtn : public iButton {
	public:
		iPortBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
		void OnCompose();
	};
	class iMoveGaugeBtn : public iButton {
	public:
		iMoveGaugeBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
		void OnCompose();
	};
	class iManaGaugeBtn : public iButton {
	public:
		iManaGaugeBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
		void OnCompose();
	};
	
public:
	iHeroTB(iViewMgr* pViewMgr, const iRect& rect);
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	void OnCompose();
};
//////////////////////////////////////////////////////////////////////////
class iCnstTB : public iView, public IViewCmdHandler
{
private:
	//
	class iPortBtn : public iButton {
	public:
		iPortBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
		void OnCompose();
	};

public:
	iCnstTB(iViewMgr* pViewMgr, const iRect& rect);
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	void OnCompose();
};
//////////////////////////////////////////////////////////////////////////
class iMainToolBar : public iView, public IViewCmdHandler
{
public:
	iMainToolBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect);
	void OnCompose();
	inline bool InfoMode() const { return false; }

	void EnableHeroBar(bool bEnable);
	void EnableCnstBar(bool bEnable);
    void UpdateSize();

private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

private:
	
	iHeroTB*	m_pHeroTB;
	iCnstTB*	m_pCnstTB;
};
#endif //PHEROES_GAME_OVERLANDVIEW_STYLUS_H_
