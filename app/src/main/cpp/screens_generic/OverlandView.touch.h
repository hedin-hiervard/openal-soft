#ifndef PHEROES_GAME_OVERLANDVIEW_TOUCH_H_
#define PHEROES_GAME_OVERLANDVIEW_TOUCH_H_
#pragma once

class iMinimapRawView;

///-------------------------------------------------------------------------------------------------
/// Hero or castle alt-tab like select view for overland screen. 
///-------------------------------------------------------------------------------------------------

class iHeroCastleSelectView: public iView, public IViewCmdHandler {
public:

	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, pointer to view manager. 
	/// \param	rc					The rectangle. 
	/// \param	uid					The uid. 
	///-------------------------------------------------------------------------------------------------

	iHeroCastleSelectView(iViewMgr *pViewMgr, const iRect& rc, uint32 uid);

	///-------------------------------------------------------------------------------------------------
	/// Executes the compose action. 
	///-------------------------------------------------------------------------------------------------

	void OnCompose();

	///-------------------------------------------------------------------------------------------------
	/// Updates the size. 
	///-------------------------------------------------------------------------------------------------

	void UpdateSize();

	///-------------------------------------------------------------------------------------------------
	/// Updates this object. 
	///-------------------------------------------------------------------------------------------------

	void Update();

	///-------------------------------------------------------------------------------------------------
	/// Command handler. 
	///
	/// \param [in,out]	pView	If non-null, the pointer to view. 
	/// \param	cmd				The command. 
	/// \param	param			The parameter. 
	///-------------------------------------------------------------------------------------------------
	
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

	///-------------------------------------------------------------------------------------------------
	/// Gets a size. 
	///
	/// \param	heroCount	Number of heroes. 
	/// \param	castleCount	Number of castles. 
	///
	/// \return	The size. 
	///-------------------------------------------------------------------------------------------------
	
	iSize GetSize(uint32 heroCount, uint32 castleCount);
	
	void Hide();
	void Process(fix32 t);
private:
	

	///-------------------------------------------------------------------------------------------------
	/// Gets a castle rectangle size. 
	///
	/// \param	castleCount	Number of castles. 
	///
	/// \return	The castle rectangle size. 
	///-------------------------------------------------------------------------------------------------

	iSize GetCastleRectSize( uint32 castleCount );

	///-------------------------------------------------------------------------------------------------
	/// Gets a hero rectangle size. 
	///
	/// \param	heroCount	Number of heroes. 
	///
	/// \return	The hero rectangle size. 
	///-------------------------------------------------------------------------------------------------

	iSize GetHeroRectSize( uint32 heroCount );

	///-------------------------------------------------------------------------------------------------
	/// Gets the maximum page for castle area. 
	///
	/// \return	zero-based maximum page. 
	///-------------------------------------------------------------------------------------------------

	uint32 MaxPage();    
	void UpdateHighlights();
private:
	static iSize GetDefBtnSize();
private:
	iSimpleArray<iBigHeroPortBtn*> m_heroBtns; ///< The hero buttons
	iSimpleArray<iBigCastleGlyphBtn*> m_castleBtns;	///< The castle buttons
	iButton* m_pPrevBtn;	///< The pointer to previous screen button
	iButton* m_pNextBtn;	///< The pointer to next screen button
	uint32	m_curPage;  ///< The current page
	bool	m_bIsHiding;
	fix32	m_hideTimer;
};


class iInfoBar_touch;
class iOverlandToolbar;
class iResourceBar;

class iMainView_touch: public iMainView
{
public:
	enum InterfaceType {
		IT_BARLESS,
		IT_TOOLBAR
	};
public:
	iMainView_touch();

	void OnDualMouseTrack(const iPoint& pos);
	void OnDualSwipeLeft(sint32 length);
	void OnDualSwipeRight(sint32 length);
	void OnTripleSwipeLeft(sint32 length);
	void OnTripleSwipeRight(sint32 length);
	void OnPinchIn(sint32 length);
	void OnPinchOut(sint32 length);
	void OnSandGlass();

	virtual void OnTimer(uint32 tid);

	virtual void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual void OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual void OnMouseEntered(const iPoint& pos, MouseId mID);
	virtual void OnMouseLeave(const iPoint& pos, MouseId mID);
    virtual bool IsScrolling() { return true; }
    virtual void OnMouseWheelDown(sint16 line, MouseId mID) {
    
#ifdef PC_VERSION
        if( gGame.bRightDown ){
        
            iPoint cell = gGame.MainView()->Composer().GetAnchor();
            
            if( line > 0 )
                cell.y += 40;
            else
                cell.y -= 40;
            
            gGame.MainView()->Composer().SetAnchor(cell);
        }        
#endif
    }
	virtual void OnMouseWheelUp(sint16 line, MouseId mID) {
/*    
        if( gGame.bRightDown ){
            
            iPoint cell = gGame.MainView()->Composer().GetAnchor();
            cell.y -= 40;
            gGame.MainView()->Composer().SetAnchor(cell);
        } 
 */
    }
	
	virtual bool OnKeyDown(sint32 key);
	virtual bool OnKeyUp(sint32 key) { return true; }

	virtual void EnableToolBar(bool bEnable) {}

	virtual void SetSurveyMode(bool bSurveyMode);
	virtual void OnCompose();				

	virtual void ComposeEventList();

	virtual bool Process(fix32 t);
	virtual void OnPlayerChanged(iPlayer* pPlayer, bool bAct);
	virtual void OnCastleListChanged();
	virtual void OnHeroListChanged();
	virtual void OnHeroReceivedExperience(iHero* pHero, sint32 oldperc, sint32 newperc);
	virtual void OnPlayerReceivedMinerals(const iPlayer *pPlayer, const iMineralSet& was, const iMineralSet& now);
	virtual void OnHeroFinishedReceivingExperience(iHero* pHero);
	virtual void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	virtual void OnActivate(bool bActivate);
	virtual void UpdateSize();

	inline void ShowInfoBtn() { 
#ifndef PC_VERSION
		m_pInfoBtn->SetVisible(true);  
#endif		
		gGame.Map().CurPlayer()->ViewSettings().showInfobar = false; 
		m_pCastleBtn->SetVisible(false);
		m_pHeroBtn->SetVisible(false); 
	}

	void ShowOnInfobar(const iPoint& pos);
	bool ObjIsOnInfoBar(iBaseMapObject* pObj);
	void UpdateButtons();
#ifndef PC_VERSION
	inline iButton* InfoButton() { return m_pInfoBtn; }
#endif	
	virtual void UpdateMetrics();
	
	virtual void SetLHandMode(bool lhandmode);
	virtual void ResetTapMode() { m_bPathTapSelected = true; }
	void HideHCSelect();
	iInfoBar_touch* InfoBar() { return m_pInfoBar; }
    IResourceBar*	ResourceBar() { return m_pResourceBar; }
	void SwitchInterface(InterfaceType it); 
	void ShowInfobar(bool bShow);
	void OnGameStarted();
	void OnComposerAnchorChanged();
	
	void SetHeroPath( iPoint ncell );

	void SetHighlighted( bool high );
#if defined( PC_VERSION )
	iHeroCastleSelectView* HeroCastleSelectView() { return( m_pHCSelect ); }
	iMinimapRawView* MinimapRawView() { return( m_pMinimapView ); }
#endif	
	void NextTurn();
private:
	
	fix32		m_pathDisapTimer;
	fix32		m_turnBlinkTimer;
	bool		m_bPathTapSelected;
	bool		m_bPathConfirmed;
	bool		m_bPreciseMode;
	bool		m_bTracked;
	iPoint		m_UnzoomedAnchor;
	iPoint		m_ScrollAnchor;
	uint32		m_downTime;
	iPoint		m_OldAnchor;
	bool		m_bWillScroll;
	bool		m_bGestureReset;
	iInfoBar_touch*	m_pInfoBar;
	iHeroPortBtn*	m_pHeroBtn;
	iCastleGlyphBtn*    m_pCastleBtn;
	iHeroCastleSelectView *m_pHCSelect; ///< The pointer to hero/castle select
	iMinimapRawView *m_pMinimapView;
	iImgBtn*	m_pMenuBtn;
	iImgBtn*    m_pNextHeroCastleBtn;
	iImgBtn*	m_pEndTurnBtn;
	iImgBtn*	m_pHorseBtnR;
	iImgBtn*	m_pQuickSaveBtn;
	iImgBtn*	m_pQuickLoadBtn;
	iImgBtn*	m_pCastBtn;
#ifndef PC_VERSION
	iImgBtn*	m_pInfoBtn;
	iImgBtn*	m_pHorseBtn;
#endif
	iButton*	m_pMinimapBtn;
	bool		m_bManualZoomed;
	fix32 m_globalAniTimer;
	bool m_bHorseBtnBlinking;
	bool	m_bVoidNextClick;
    bool m_bEndTurnBtnBlinking;
	IOverlandToolbar* m_pOverlandToolbar;
	IResourceBar*	  m_pResourceBar;
	InterfaceType	  m_it;
	iPoint			  m_ncell;
#if defined( PC_VERSION )
public:
	bool m_bEndTurn;	//For HotKey E (End turn)
	bool m_bMinimap;	//For HotKey M (Minimap)
#endif
};

class iInfoBar_touch : public iView
{
public:
	enum Mode {
		Default,
		Hero,
		UnchartedTerritory,
		VisCnst,
		OwnCnst,
		Castle,
		Mineral,
		Guard,
		Ship
	};

	iInfoBar_touch(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect);
	void OnCompose();
	void OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	void OnMouseEntered(const iPoint &pos, MouseId mID);
	void Process(fix32 t);
	inline void  SetMode(Mode newm) { m_mode = newm; Invalidate(); }
	inline void SetCell(iPoint cell) { m_cell = cell; if(cell == cInvalidPoint) SetObj(NULL); Setup(cell); }
	inline const iBaseMapObject* CurObj() const { return m_pObj; }
private:
	void Setup(const iPoint& pos);
	inline void	 SetObj(iBaseMapObject *obj) { m_pObj = obj; if(!obj) SetMode(Default); Invalidate(); }
	inline void  SetVisionLevel(VISION_LEVEL vl) { m_vl = vl; }

private:
	fix32			 m_scrollTimer;
	uint32			 m_scrollPos;
	iPoint			 m_cell;
	Mode m_mode;
	iBaseMapObject*  m_pObj;
	VISION_LEVEL m_vl;
	//void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
};

#endif //PHEROES_GAME_OVERLANDVIEW_TOUCH_H_
