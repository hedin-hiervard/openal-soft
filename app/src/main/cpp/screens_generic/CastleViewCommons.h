#ifndef PHEROES_GAME_CASTLEVIEWCOMMONS_H_
#define PHEROES_GAME_CASTLEVIEWCOMMONS_H_

/*
 *  ArmyExchange.h
 *  pheroes
 *
 *  Created by Andrew Shilov on 6/8/11.
 *  Copyright 2011 Palm Heroes Team. All rights reserved.
 *
 */

class iArmyListEx_touch;
class iArmyListEx;
class iCheckButton;
class iHeroPortBtn;
class iCastleGlyphBtn;
class iCheckButton;	

///-------------------------------------------------------------------------------------------------
/// Army exchange. 
///-------------------------------------------------------------------------------------------------

class iArmyExchange: public iView, public IViewCmdHandler {
public:
	
	///-------------------------------------------------------------------------------------------------
	/// Values that represent ButtonType. 
	///-------------------------------------------------------------------------------------------------
	
	enum ButtonType {
		BT_HERO,
		BT_CASTLE
	};
public:
	
	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, manager for pointer to view. 
	/// \param [in,out]	pHandler	If non-null, the pointer to handler. 
	/// \param [in,out]	rc			The rectangle. 
	/// \param	uid					The uid. 
	/// \param	bButtons			true to buttons. 
	/// \param	bSwapProhibited		true if different type groups cannot be swaped.
	///-------------------------------------------------------------------------------------------------
	
	iArmyExchange(iViewMgr* pViewMgr, IViewCmdHandler* pHandler, const iRect& rc, uint32 uid, bool bButtons, bool bSplitButton, bool bSwapProhibited = false
#ifndef PC_VERSION
                  , bool bGUI = true, iSize cellSize = iSize(50, 36)
#endif
                  );
	
	///-------------------------------------------------------------------------------------------------
	/// Updates the size. 
	///-------------------------------------------------------------------------------------------------
	
	void UpdateSize();
	
	///-------------------------------------------------------------------------------------------------
	/// Sets a hero. 
	///
	/// \param [in,out]	pHero	If non-null, the pointer to hero. 
	/// \param	bCanDismiss		true if can dismiss army groups. 
	///-------------------------------------------------------------------------------------------------
	
	void SetTopOwner(iArmyOwner* pOwner, bool bCanDismiss);
	
	///-------------------------------------------------------------------------------------------------
	/// Sets a castle. 
	///
	/// \param [in,out]	pCastle	If non-null, the pointer to castle. 
	/// \param	bCanDismiss		true if can dismiss. 
	///-------------------------------------------------------------------------------------------------
	
	void SetBottomOwner(iArmyOwner *pOwner, bool bCanDismiss);
	
	///-------------------------------------------------------------------------------------------------
	/// Command handler. 
	///
	/// \param [in,out]	pView	If non-null, the pointer to view. 
	/// \param	cmd				The command. 
	/// \param	param			The parameter. 
	///-------------------------------------------------------------------------------------------------
	
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);	
	
	///-------------------------------------------------------------------------------------------------
	/// Gets the hero army list. 
	///
	/// \return	null if it fails, else a list of. 
	///-------------------------------------------------------------------------------------------------
	
	inline iArmyListEx_touch* BottomArmyList() { return m_pBottomArmy; }
	
	///-------------------------------------------------------------------------------------------------
	/// Gets the castle army list. 
	///
	/// \return	null if it fails, else a list of. 
	///-------------------------------------------------------------------------------------------------
	
	inline iArmyListEx_touch* TopArmyList()  { return m_pTopArmy; }
	
	///-------------------------------------------------------------------------------------------------
	/// Adds a competitor. 
	///
	/// \param [in,out]	comp	If non-null, the competitor. 
	///-------------------------------------------------------------------------------------------------
	
	void AddCompetitor(iArmyListEx* comp);	
	
	void SetSplitBtn(iCheckButton* pSplitBtn);
#ifdef PC_VERSION    
    void SetSplitBtn( bool flag );
#endif
public:
	
	///-------------------------------------------------------------------------------------------------
	/// Gets a default size. 
	///
	/// \param	count	Number of lines. 
	///
	/// \return	The default size. 
	///-------------------------------------------------------------------------------------------------
	
	static iSize GetDefSize(uint32 count);
	
	///-------------------------------------------------------------------------------------------------
	/// Updates the split button. 
	///-------------------------------------------------------------------------------------------------
	
	void UpdateSplitBtn();
	
	///-------------------------------------------------------------------------------------------------
	/// Sets a split checked. 
	///
	/// \param	checked	The checked. 
	///-------------------------------------------------------------------------------------------------
	
	void SetSplitChecked( sint32 checked );
	void Deselect();
	iRect GetDismissRect();
	void OnCompose();
public: 
	static const sint32 sm_defSingleHeight;
	static const sint32 sm_defDoubleHeight;
private:
	iHeroPortBtn*		m_pHeroBtn;		///< The pointer to hero button
#ifndef PC_VERSION
    iHeroPortBtn*		m_pHeroBtn2;		///< The pointer to hero button
#endif
	iCastleGlyphBtn*	m_pCastleBtn;   ///< The pointer to castle button
	iCheckButton*		m_pSplitBtn;	///< The pointer to split button	
	
	iArmyListEx_touch*  m_pBottomArmy;	
	iArmyListEx_touch*  m_pTopArmy;  
	IViewCmdHandler*	m_pCmdHandler;  ///< The pointer to command handler
#ifndef PC_VERSION
    bool				m_bGUI;
#endif
};

///-------------------------------------------------------------------------------------------------
/// Magic guild view. 
///-------------------------------------------------------------------------------------------------
class iSpellBtn;
class iMGuildView: public iView, public  IViewCmdHandler
{
public:
	
	///-------------------------------------------------------------------------------------------------
	/// Sets a castle. 
	///
	/// \param [in,out]	pCastle	If non-null, the pointer to castle. 
	///-------------------------------------------------------------------------------------------------
	
	void SetCastle(iCastle* pCastle);	
	
	///-------------------------------------------------------------------------------------------------
	/// Updates this object. 
	///-------------------------------------------------------------------------------------------------
	
	void Update();
	
	///-------------------------------------------------------------------------------------------------
	/// Updates the size. 
	///-------------------------------------------------------------------------------------------------
	
	void UpdateSize();
	
	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, pointer to view manager. 
	/// \param	rc					The rectangle. 
	/// \param	uid					The uid. 
	///-------------------------------------------------------------------------------------------------
	
	iMGuildView(iViewMgr* pViewMgr, iRect rc, uint32 uid);
	
	///-------------------------------------------------------------------------------------------------
	/// Executes the compose action. 
	///-------------------------------------------------------------------------------------------------
	
	void OnCompose();
private:
	
	///-------------------------------------------------------------------------------------------------
	/// Command handler. 
	///
	/// \param [in,out]	pView	If non-null, the pointer to view. 
	/// \param	cmd				The command. 
	/// \param	param			The parameter. 
	///-------------------------------------------------------------------------------------------------
	
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);	
private:
	iCastle* m_pCastle; ///< The pointer to castle
	iSpellBtn*	m_pSpellBtn[15];	///< The pointer to spell buttons array
};


///-------------------------------------------------------------------------------------------------
/// Recruit hero view. 
///-------------------------------------------------------------------------------------------------

class iTavernCtlCnst;
class iImgBtn;
class iRecruitHeroView : public iView, public  IViewCmdHandler
{
public:
	
	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, pointer to view manager. 
	/// \param [in,out]	pCmdHandler	If non-null, the pointer to command handler. 
	/// \param	rect				The rectangle. 
	/// \param	uid					The uid. 
	///-------------------------------------------------------------------------------------------------
	
	iRecruitHeroView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);        
	
	///-------------------------------------------------------------------------------------------------
	/// Updates the size. 
	///-------------------------------------------------------------------------------------------------
	
	void UpdateSize();    
	
	///-------------------------------------------------------------------------------------------------
	/// Sets a castle. 
	///
	/// \param [in,out]	pCastle	If non-null, the pointer to castle. 
	///-------------------------------------------------------------------------------------------------
	
	void SetCastle(iCastle* pCastle);
	
	///-------------------------------------------------------------------------------------------------
	/// Command handler. 
	///
	/// \param [in,out]	pView	If non-null, the pointer to view. 
	/// \param	cmd				The command. 
	/// \param	param			The parameter. 
	///-------------------------------------------------------------------------------------------------
	
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	
	///-------------------------------------------------------------------------------------------------
	/// Executes the compose action. 
	///-------------------------------------------------------------------------------------------------
	
	void OnCompose();
	
	///-------------------------------------------------------------------------------------------------
	/// Updates this object. 
	///-------------------------------------------------------------------------------------------------
	
	void Update();
#ifdef PC_VERSION
    void SetHighlighted(bool flag);    
#endif
private:
	iCastle*    	    m_pCastle;  ///< The pointer to castle
	iImgBtn*			m_pRecrBtn; ///< The pointer to recr button
	iTavernCtlCnst*		m_pTavern;  ///< The pointer to tavern
	IViewCmdHandler*	m_pCmdHandler;  ///< The pointer to command handler
};

///-------------------------------------------------------------------------------------------------
/// Recruit button. 
///-------------------------------------------------------------------------------------------------
class iDwellCtlCnst;
class iRecrButton : public iButton
{
public:
	
	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, pointer to view manager. 
	/// \param [in,out]	pCmdHandler	If non-null, the pointer to command handler. 
	/// \param	rect				The rectangle. 
	/// \param	uid					The uid. 
	/// \param	idx					Zero-based index of the. 
	///-------------------------------------------------------------------------------------------------
	
	iRecrButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 idx);
	
	///-------------------------------------------------------------------------------------------------
	/// Executes the compose action. 
	///-------------------------------------------------------------------------------------------------
	
	void OnCompose();				inline void SetSelected(bool sel) { m_bSelected = sel; }
	
	///-------------------------------------------------------------------------------------------------
	/// Sets a dwelling. 
	///
	/// \param [in,out]	pDwel	If non-null, the pointer to dwel. 
	///-------------------------------------------------------------------------------------------------
	
	void SetDwelling(iDwellCtlCnst* pDwel);
	
	///-------------------------------------------------------------------------------------------------
	/// Gets the dwelling. 
	///
	/// \return	null if it fails, else. 
	///-------------------------------------------------------------------------------------------------
	
	inline iDwellCtlCnst* Dwelling() { return m_pDwel; }			
protected:
	bool	m_bSelected;	///< is this button selected
	iDwellCtlCnst*	m_pDwel;	///< The pointer to dwelling construction
	uint32		m_idx;  ///< Zero-based index of the button
};


///-------------------------------------------------------------------------------------------------
/// Recruit buttons set. 
///-------------------------------------------------------------------------------------------------

class iRecrSet : public iView, public IViewCmdHandler
{
public:	
	
	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, pointer to view manager. 
	/// \param [in,out]	pCmdHandler	If non-null, the pointer to command handler. 
	/// \param	uid					The uid. 
	/// \param [in,out]	rc			The rectangle. 
	///-------------------------------------------------------------------------------------------------
	
	iRecrSet(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, uint32 uid, const iRect& rc);	
	
	///-------------------------------------------------------------------------------------------------
	/// Sets a castle. 
	///
	/// \param [in,out]	pCastle	If non-null, the pointer to castle. 
	///-------------------------------------------------------------------------------------------------
	
	void SetCastle(iCastle* pCastle);
	
	///-------------------------------------------------------------------------------------------------
	/// Updates this object. 
	///-------------------------------------------------------------------------------------------------
	
	void Update();
	
	///-------------------------------------------------------------------------------------------------
	/// Gets the selected creature type. 
	///
	/// \return	. 
	///-------------------------------------------------------------------------------------------------
	
	inline CREATURE_TYPE SelectedCT() { return m_selectedCT; }
	
	///-------------------------------------------------------------------------------------------------
	/// Sets a selected creature type. 
	///
	/// \param	ct	Type of the creature. 
	///-------------------------------------------------------------------------------------------------
	
	void SetSelectedCT(CREATURE_TYPE ct);
	
	///-------------------------------------------------------------------------------------------------
	/// Updates the size. 
	///-------------------------------------------------------------------------------------------------
	
	void UpdateSize();
protected:	
	
	///-------------------------------------------------------------------------------------------------
	/// Executes the compose action. 
	///-------------------------------------------------------------------------------------------------
	
	void OnCompose();		
	
	///-------------------------------------------------------------------------------------------------
	/// Command handler. 
	///
	/// \param [in,out]	pView	If non-null, the pointer to view. 
	/// \param	cmd				The command. 
	/// \param	param			The parameter. 
	///-------------------------------------------------------------------------------------------------
	
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
protected:
	iCastle*			m_pCastle;  ///< The pointer to castle
	iRecrButton*		m_pBtns[6]; ///< The pointer to recruit buttons array
	IViewCmdHandler*	m_pCmdHandler;  ///< The pointer to command handler
	
	CREATURE_TYPE   m_selectedCT;   ///< Type of the selected creature
};

///-------------------------------------------------------------------------------------------------
/// Dock view. 
///-------------------------------------------------------------------------------------------------

class iHarbor;

class iDockView : public iView, public IViewCmdHandler
{
protected:
	
	///-------------------------------------------------------------------------------------------------
	/// Values that represent DOCK_STATE. 
	///-------------------------------------------------------------------------------------------------
	
	enum DOCK_STATE
	{
		DS_NONE = 0,  // Unknown state
		DS_BUILD,     // Build ship screen
		DS_OCCUPY,    // Occupy ship screen
		DS_BUSY,      // Ship busy screen
		DS_CANT       // Can't buy ship screen
	};
	
public:
	
	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, pointer to view manager. 
	/// \param [in,out]	pCmdHandler	If non-null, the pointer to command handler. 
	/// \param	rect				The rectangle. 
	/// \param	uid					The uid. 
	/// \param	dt					The dt. 
	///-------------------------------------------------------------------------------------------------
	
	iDockView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 dt);
	
	///-------------------------------------------------------------------------------------------------
	/// Updates the size. 
	///-------------------------------------------------------------------------------------------------
	
	void UpdateSize();	
	
	///-------------------------------------------------------------------------------------------------
	/// Executes the compose action. 
	///-------------------------------------------------------------------------------------------------
	
	void OnCompose();	    
	
	///-------------------------------------------------------------------------------------------------
	/// Sets a hero. 
	///
	/// \param [in,out]	pHero	If non-null, the pointer to hero. 
	///-------------------------------------------------------------------------------------------------
	
	void SetHero(iHero *pHero);    
	
	///-------------------------------------------------------------------------------------------------
	/// Updates the controls. 
	///-------------------------------------------------------------------------------------------------
	
	void UpdateControls();	
	
	///-------------------------------------------------------------------------------------------------
	/// Gets the owner of this item. 
	///
	/// \return	null if it fails, else the owner. 
	///-------------------------------------------------------------------------------------------------
	
	iPlayer* GetOwner();
	
	///-------------------------------------------------------------------------------------------------
	/// Builds the ship. 
	///-------------------------------------------------------------------------------------------------
	
	void BuildShip();
	
	///-------------------------------------------------------------------------------------------------
	/// Occupy ship. 
	///-------------------------------------------------------------------------------------------------
	
	void OccupyShip();    
	
	///-------------------------------------------------------------------------------------------------
	/// Switch state. 
	///
	/// \param	NewState	new state. 
	///-------------------------------------------------------------------------------------------------
	
	void SwitchState(DOCK_STATE NewState);
	
	///-------------------------------------------------------------------------------------------------
	/// Sets a harbor. 
	///
	/// \param [in,out]	pHarbor	If non-null, the pointer to harbor. 
	///-------------------------------------------------------------------------------------------------
	
	void SetHarbor(IHarbor* pHarbor);
	
	///-------------------------------------------------------------------------------------------------
	/// Check resources. Sets the buttons enabled if player has enough resources to build the ship
	///-------------------------------------------------------------------------------------------------
	
	void CheckResources();
	
	///-------------------------------------------------------------------------------------------------
	/// Command players. 
	///
	/// \param [in,out]	pView	If non-null, the pointer to view. 
	/// \param	cmd				The command. 
	/// \param	param			The parameter. 
	///-------------------------------------------------------------------------------------------------
	
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);	
protected:
	DOCK_STATE          m_CurState; ///< The current state
	iHero*              m_pHero;	///< The pointer to hero
	IHarbor*			m_pHarbor;  ///< The pointer to harbor
	iButton*			m_pBtn; ///< The pointer to button
	uint32              m_dock_type;	///< Type of the dock
	IViewCmdHandler*    m_pCmdHandler;  ///< The pointer to command handler
};


///-------------------------------------------------------------------------------------------------
/// Market panel
///-------------------------------------------------------------------------------------------------

class iMineralBtn;
class iMarketPanel: public iView, public IViewCmdHandler {
public:
	
	///-------------------------------------------------------------------------------------------------
	/// Values that represent PanelType. 
	///-------------------------------------------------------------------------------------------------
	
	enum PanelType {
		MP_LEFT = 0,
		MP_RIGHT
	};
public:
	
	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, pointer to view manager. 
	/// \param [in,out]	pHandler	If non-null, the pointer to handler. 
	/// \param	rc					The rectangle. 
	/// \param	uid					The uid. 
	/// \param	type				The type. 
	///-------------------------------------------------------------------------------------------------
	
	iMarketPanel(iViewMgr* pViewMgr, IViewCmdHandler* pHandler, const iRect& rc, uint32 uid, PanelType type); 
	
	///-------------------------------------------------------------------------------------------------
	/// Updates the size. 
	///-------------------------------------------------------------------------------------------------
	
	void UpdateSize();
	
	///-------------------------------------------------------------------------------------------------
	/// Command handler. 
	///
	/// \param [in,out]	pView	If non-null, the pointer to view. 
	/// \param	cmd				The command. 
	/// \param	param			The parameter. 
	///-------------------------------------------------------------------------------------------------
	
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	
	///-------------------------------------------------------------------------------------------------
	/// Gets the selected mineral. 
	///
	/// \return	. 
	///-------------------------------------------------------------------------------------------------
	
	MINERAL_TYPE SelectedMineral();
	
	///-------------------------------------------------------------------------------------------------
	/// Deselects this object. 
	///-------------------------------------------------------------------------------------------------
	
	void Deselect();
	
	///-------------------------------------------------------------------------------------------------
	/// Updates as left side described by ms. 
	///
	/// \param	ms	the mineral set the player has
	///-------------------------------------------------------------------------------------------------
	
	void UpdateAsLeftSide(const iMineralSet &ms);
	
	///-------------------------------------------------------------------------------------------------
	/// Updates as right side. 
	///
	/// \param	from		Source for the exchange. 
	/// \param	marketCount	Number of markets. 
	///-------------------------------------------------------------------------------------------------
	
	void UpdateAsRightSide(MINERAL_TYPE from, sint32 marketCount);
private:
	iMineralBtn		*m_pBtns[MINERAL_COUNT];	///< The pointer to btns
	IViewCmdHandler *m_pCmdHandler; ///< The pointer to command handler
	sint32			m_selResource;  ///< The selected resource
	PanelType			m_type; ///< The type of market panel
};


///-------------------------------------------------------------------------------------------------
/// Market view. 
///-------------------------------------------------------------------------------------------------
class iAmountChooserGlow;
class iImgBtn;

class iMarketView : public iView, public IViewCmdHandler
{
public:
	
	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, pointer to view manager. 
	/// \param [in,out]	pCmdHandler	If non-null, the pointer to command handler. 
	/// \param	rect				The rectangle. 
	/// \param	uid					The uid. 
	/// \param	is_harbor			true if is harbor. 
	///-------------------------------------------------------------------------------------------------
	
	iMarketView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, bool is_harbor);
	
	///-------------------------------------------------------------------------------------------------
	/// Sets an owner. 
	///
	/// \param [in,out]	pOwner	If non-null, the owner of the pointer to. 
	///-------------------------------------------------------------------------------------------------
	
	void SetOwner(iPlayer* pOwner);	
	
	///-------------------------------------------------------------------------------------------------
	/// Command handler. 
	///
	/// \param [in,out]	pView	If non-null, the pointer to view. 
	/// \param	cmd				The command. 
	/// \param	param			The parameter. 
	///-------------------------------------------------------------------------------------------------
	
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	
	///-------------------------------------------------------------------------------------------------
	/// Executes the slider position changed action. 
	///-------------------------------------------------------------------------------------------------
	
	void OnSliderPosChanged();	
	
	///-------------------------------------------------------------------------------------------------
	/// Updates this object. 
	///-------------------------------------------------------------------------------------------------
	
	void Update();	
	
	///-------------------------------------------------------------------------------------------------
	/// Executes the compose action. 
	///-------------------------------------------------------------------------------------------------
	
	void OnCompose();
	
	///-------------------------------------------------------------------------------------------------
	/// Updates the size. 
	///-------------------------------------------------------------------------------------------------
	
	void UpdateSize();	
#ifdef PC_VERSION
    void SetHighlighted(bool flag);
#endif
    
    bool IsScrolling() {return true;}
    
    void OnMouseWheelDown(sint16 line, MouseId mID);
public:
	static const iSize sm_defSize;
protected:		
	iPlayer*            m_pOwner;   ///< The pointer to the owning player
	sint32				m_marketsCount; ///< Number of markets
	bool                m_is_harbor;	///< true if is harbor
	IViewCmdHandler*	m_pCmdHandler;  ///< The pointer to command handler
	iAmountChooserGlow*	m_pAmountChooser;   ///< The pointer to amount chooser
	iImgBtn*			m_pTradeBtn;	///< The pointer to trade button
	iMarketPanel*		m_pLeftPanel;   ///< The pointer to left panel
	iMarketPanel*		m_pRightPanel;  ///< The pointer to right panel
	
};

///-------------------------------------------------------------------------------------------------
/// Mineral button. 
///-------------------------------------------------------------------------------------------------

class iMineralBtn : public iButton
{
public:
	
	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, pointer to view manager. 
	/// \param [in,out]	pCmdHandler	If non-null, the pointer to command handler. 
	/// \param	rect				The rectangle. 
	/// \param	uid					The uid. 
	/// \param	mt					The mt. 
	///-------------------------------------------------------------------------------------------------
	
	iMineralBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, MINERAL_TYPE mt);		
	
	///-------------------------------------------------------------------------------------------------
	/// Gets the mineral type. 
	///
	/// \return	. 
	///-------------------------------------------------------------------------------------------------
	
	inline MINERAL_TYPE MineralType() const { return m_mt; }
	
	///-------------------------------------------------------------------------------------------------
	/// Sets a state. 
	///
	/// \param	bSelected	if this button is selected. 
	///-------------------------------------------------------------------------------------------------
	
	void SetState(bool bSelected);		
	
	///-------------------------------------------------------------------------------------------------
	/// Sets a quantity. 
	///
	/// \param	nval	The fraction coefficient for the exchange. 
	///-------------------------------------------------------------------------------------------------
	
	void SetQuant(FractionCoeff nval);		
	
	///-------------------------------------------------------------------------------------------------
	/// Gets the quantity. 
	///
	/// \return	fraction coefficient of the exchange. 
	///-------------------------------------------------------------------------------------------------
	
	inline FractionCoeff Quantity() const { return m_quant; }
	
	///-------------------------------------------------------------------------------------------------
	/// Executes the compose action. 
	///-------------------------------------------------------------------------------------------------
	
	void OnCompose();		
	
protected:
	bool			m_bSelected;	///< if the button is selected
	MINERAL_TYPE	m_mt;   ///< The mineral type
	FractionCoeff	m_quant;	///< The quantity
};

#endif
