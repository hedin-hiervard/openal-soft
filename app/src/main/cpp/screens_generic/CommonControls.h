#ifndef PHEROES_GAME_COMMONCONTROLS_H_
#define PHEROES_GAME_COMMONCONTROLS_H_

class iTooltipableView
{
public:
	virtual void OnMouseEntered(const iPoint& pos, MouseId mID)=0;
	virtual void OnMouseLeave(const iPoint& pos, MouseId mID)=0;
};

class iImgBtn : public iButton, public iTooltipableView
{
public:
	iImgBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, sint16 sid, sint16 pressedSid, iStringT txt = iStringT(), uint32 state = Visible|Enabled, Alignment align = AlignTopLeft, bool bUnclick = false)\
		: iButton(pViewMgr, pCmdHandler, rect, uid, state), m_sid(sid), m_align(align), m_pressedSid(pressedSid), m_high(false), m_bUnclickable(bUnclick), text(txt), m_bHighBtn(false)
	{}

	void OnBtnDown() const
	{
		//gSfxMgr.PlaySound(CSND_BUTTON);
	}

	inline void SetHighlighted(bool high) { m_high = high; Invalidate(); }
	inline void SetAlign(Alignment align) { m_align = align; Invalidate(); }
#if defined( PC_VERSION )
	inline void SetHighlightedBtn(bool high) { m_bHighBtn = m_high = m_bEntered = high; Invalidate(); }
#endif
	void OnCompose();
	
	inline void SetSprite(sint16 sid, sint16 pressedSid) { m_sid = sid; m_pressedSid = pressedSid; Invalidate(); }

	void OnMouseEntered(const iPoint& pos, MouseId mID){
#if defined( PC_VERSION )
		m_bEntered = m_high = true;
#endif
	}
	void OnMouseLeave(const iPoint& pos, MouseId mID){
#if defined( PC_VERSION )	
		m_bEntered = m_high = false;
#endif
	}
/*	
	void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID){
	
	}
*/		
private:
	sint16 m_sid, m_pressedSid;
	Alignment m_align;
	bool m_high;	
	bool m_bUnclickable;
	iStringT text;
	bool m_bHighBtn;
};	



/* iphone button with edge */
class iIphoneBtn : public iButton
	{
	public:
		iIphoneBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 state = Visible|Enabled, sint16 img = -1, bool unclk = false, Alignment align = AlignTopLeft)
		: iButton(pViewMgr, pCmdHandler, rect, uid, state), m_img(img), m_bUnclickable(unclk), m_high(false), m_align(align)
		{}
		
		void OnCompose();
		inline void SetHighlighted(bool high) { m_high = high; Invalidate(); }
		inline void SetUnclickable(bool uncl) { m_bUnclickable = uncl; }
		inline void SetSprite(sint16 sid) { m_img = sid; Invalidate(); }

		static iSize sm_defSize;
        static iSize sm_defSizeDeluxe;
        static iSize sm_defPerkSize;

	protected:		
		bool m_high;
		sint16 m_img;
		bool m_bUnclickable;
		Alignment m_align;
		
	};

/*
 *	Popup view
 */
class iFramePopupView : public iPopupView
{
public:
	iFramePopupView(iViewMgr* pViewMgr, PLAYER_ID pid);

	// Pure virtuals
	virtual void DoCompose(const iRect& clRect)  =0;
	virtual iSize ClientSize()  =0;

private:
	void OnCompose();
	iSize PopupViewSize();
	iSize PopupViewMinSize();

private:
	PLAYER_ID	m_pid;
};

/*
 *	Common controls
 */

// Label
class iPHLabel : public iBaseCtrl
{
public:
	iPHLabel(iViewMgr* pViewMgr, const iRect& rect, const iStringT& text, Alignment align, iTextComposer::FontConfig fc, TextResId rid = (uint16)(-1));
	void SetText(const iStringT& text) { m_text = text; Invalidate(); }
	void OnCompose();
	inline void UpdateText() { m_text = gTextMgr[m_rid]; Invalidate(); }
private:
	iStringT	m_text;
	TextResId   m_rid;
	Alignment	m_align;
	iTextComposer::FontConfig	m_fc;
};

// Text Box
class iPHTextBox : public iBaseCtrl
{
public:
	iPHTextBox(iViewMgr* pViewMgr, const iRect& rect, const iStringT& text, iTextComposer::FontConfig fc);
	void SetText(const iStringT& text) { m_text = text; Invalidate(); }
	void OnCompose();

private:
	iStringT	m_text;
	iTextComposer::FontConfig	m_fc;
};

// ToolbarSplitter
class iTBSplitter : public iBaseCtrl
{
public:
	iTBSplitter(iViewMgr* pViewMgr, const iRect& rect);
	void OnCompose();
};

// Text button
/*
class iTextButton : public iButton
{
public:
	iTextButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, TextResId textKey, uint32 uid, uint32 state = Visible|Enabled);
	void OnBtnDown() const;
	void OnCompose();
	void SetCaption(TextResId textKey) { m_TextKey = textKey; Invalidate(); }

protected:
	TextResId	m_TextKey;
};
*/

class iGlowTextButton: public iButton {
public:
	iGlowTextButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, TextResId textKey, uint32 uid, uint32 state = Visible|Enabled);
	void OnCompose();
	void SetCaption(TextResId textKey) { m_TextKey = textKey; Invalidate(); }
protected:
	TextResId	m_TextKey;
};

typedef iGlowTextButton iTextButton;

// Transparent button
class iTransparentButton : public iButton
	{
	public:
		iTransparentButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, TextResId textKey, uint32 uid, uint32 state = Visible|Enabled);
		void OnBtnDown() const;
		void OnCompose();
		void SetCaption(TextResId textKey) { m_TextKey = textKey; Invalidate(); }
		
	private:
		TextResId	m_TextKey;
	};

// Icon button
class iIconButton : public iButton
{
public:
	iIconButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, SpriteId sid, uint32 uid, uint32 state = Visible|Enabled);
	void OnBtnDown() const;
	void OnCompose();

private:
	SpriteId	m_spriteId;
};

// Dialog Icon button
class iDlgIconButton : public iButton
{
public:
	iDlgIconButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, SpriteId sid, uint32 uid, uint32 state = Visible|Enabled);
	void OnBtnDown() const;
	void OnCompose();

private:
	SpriteId	m_spriteId;
};

// Icon button
class iCheckButton : public iImgBtn, public IViewCmdHandler
{
public:
//SHL TODO	iCheckButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pt, SpriteId sid, SpriteId psid, uint32 uid, uint32 state = Visible|Enabled, bool bChecked = false);
	iCheckButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint pt, SpriteId sid, SpriteId psid, uint32 uid, uint32 state = Visible|Enabled, bool bChecked = false);
	void SetChecked(bool bChecked, bool bNotify = true);
	inline bool IsChecked() const { return m_bChecked; }
	void OnCompose();
	
	void OnMouseEntered(const iPoint& pos, MouseId mID) {}
	void OnMouseLeave(const iPoint& pos, MouseId mID) {}
public:
	static iSize sm_defSize;
private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
private:
	IViewCmdHandler*	m_pOwnCmdHandler;
	bool				m_bChecked;
	SpriteId			m_spriteId;
};

// Iphone Icon button
class iIphoneIconButton : public iButton, public IViewCmdHandler
{
public:
	iIphoneIconButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, SpriteId sid, uint32 uid, uint32 state = Visible|Enabled);
	
private:
	void OnCompose();
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	
private:
	IViewCmdHandler*	m_pOwnCmdHandler;
	SpriteId			m_spriteId;
};

// Check box
class iCheckBox : public iButton, public IViewCmdHandler
{
public:
	iCheckBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, const iStringT& text, uint32 uid, uint32 state = Visible|Enabled, bool bChecked = false);
	void SetChecked(bool bChecked, bool bNotify = true);
	inline bool IsChecked() const { return m_bChecked; }

private:
	void OnCompose();
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

private:
	IViewCmdHandler*	m_pOwnCmdHandler;
	bool				m_bChecked;
	iStringT			m_text;
};

// New Main Menu Check box
class iNMCheckBox : public iButton, public IViewCmdHandler
{
public:
    iNMCheckBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, const iStringT& text, uint32 uid, uint32 state = Visible|Enabled, bool bChecked = false);
    void SetChecked(bool bChecked, bool bNotify = true);
    inline bool IsChecked() const { return m_bChecked; }

private:
    void OnCompose();
    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

private:
    IViewCmdHandler*	m_pOwnCmdHandler;
    bool				m_bChecked;
    iStringT			m_text;
};

// Scroll bar
class iPHScrollBar : public iScrollBar
{
public:
	iPHScrollBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 flags=0);
	void ComposeSBElement(Element el, const iRect& rc, uint32 flags);
};

// Mini slider (used as slider)
class iPHMiniSlider : public iScrollBar
{
public:
	iPHMiniSlider(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
	void ComposeSBElement(Element el, const iRect& rc, uint32 flags);
};

// 5-position clickable bar consisting of cells
class iPHVolumeBar : public iBaseCtrl, public IViewCmdHandler
	{
	public:
		iPHVolumeBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
		void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
		void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
		void OnCompose();
		inline sint32 CurPos() const { return m_pos; }
		inline void SetCurPos(sint32 newval) { m_pos = iCLAMP<sint32>(0, 4, newval); } 
	private:
		void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	private:
		sint32	m_pos;
	};


// new menu 5-position clickable bar consisting of cells
class iNMVolumeBar : public iBaseCtrl, public IViewCmdHandler
{
public:
    iNMVolumeBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
    void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
    void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
    void OnCompose();
    inline sint32 CurPos() const { return m_pos; }
    inline void SetCurPos(sint32 newval) { m_pos = iCLAMP<sint32>(0, /*4*/5, newval); } 
private:
    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
private:
    sint32	m_pos;
};

// iphone-style amount chooser
class iAmountChooser : public iBaseCtrl, public IViewCmdHandler
{
public:
	iAmountChooser(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
	inline sint32 CurPos() const { return m_pos; }
	inline void SetMax(sint32 max) { m_max = max; }
	inline void SetCurPos(sint32 newval) { m_pos = iCLAMP<sint32>(0, m_max, newval); } 
	void OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	void OnMouseTrack(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	void OnCompose();
private: 
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
private:
	sint32	m_pos;
	sint32  m_max;
};


class iAmountChooserGlow : public iBaseCtrl, public IViewCmdHandler
{
public:
	iAmountChooserGlow(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, bool bDeluxe = false);
	inline sint32 CurPos() const { return m_pos; }
	inline void SetMax(sint32 max) { m_max = max; }
	inline sint32 GetMax() const { return m_max; }
	inline void SetCurPos(sint32 newval) { m_pos = iCLAMP<sint32>(0, m_max, newval); } 
	void OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	void OnMouseTrack(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	void OnCompose();
	void UpdateSize();
public:
	static const iSize sm_defSize;
	static const sint32 sm_offset;
private: 
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
private:
	sint32	m_pos;
	sint32  m_max;
	iButton *m_pBtnLeft;
	iButton *m_pBtnRight;
    bool m_bDeluxe;
};


class iRater : public iBaseCtrl, public IViewCmdHandler
{
public:
	iRater(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
	inline sint32 CurPos() const { return m_pos; }
	inline void SetCurPos(sint32 newval) 
	{ m_pos = iCLAMP<sint32>(0, 4, newval); } 
	void OnMouseTrack(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	void OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	void OnCompose();
	void UpdateSize();
	inline uint8& Alpha() { return m_alpha; }
public:
	static const iSize sm_defSize;
private: 
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
private:
	sint32	m_pos;
	uint8	m_alpha;
};


// twitter paner
class iTwitterPane: public iBaseCtrl, public IViewCmdHandler
{
public:
	iTwitterPane(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
	void OnMouseTrack(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	void OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	void OnCompose();
	void UpdateSize();
	sint32 SelectedRating() const;
	void Reset();
	void ShowRating();
	inline uint8& RatingAlpha() { return m_pRater->Alpha(); }
public:
	static const iSize sm_defSize;
private: 
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
private:
	iRater* m_pRater;
	iButton* m_pBtn;
	IViewCmdHandler *m_pHandler;
};

 /*
 *	Specialized controls
 */

// Tool bar tabbed switch
class iBarTabSwitch : public iTabbedSwitch
{
public:
	iBarTabSwitch(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 tabcnt, uint32 uid);
	void SetTabIcon(SpriteId sid);
	void ComposeTabItem(uint32 idx, uint32 itemState, const iRect& rect);
	
private:
	iSimpleArray<SpriteId>	m_Tabs;
};

///-------------------------------------------------------------------------------------------------
/// Hero portrait button. 
///-------------------------------------------------------------------------------------------------

class iHeroPortBtn : public iIphoneBtn 	
{
public:

	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, manager for pointer to view. 
	/// \param [in,out]	pCmdHandler	If non-null, the pointer to command handler. 
	/// \param	rect				The rectangle. 
	/// \param	uid					The uid. 
	///-------------------------------------------------------------------------------------------------

	// SHL TODO!!! iHeroPortBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, bool bUnclick, bool bShowMoves = false, Alignment align = AlignCenter);
	iHeroPortBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect rect, uint32 uid, bool bUnclick, bool bShowMoves = false, Alignment align = AlignCenter
#ifndef PC_VERSION
                    , bool bGUI = true
#endif
                 );

	///-------------------------------------------------------------------------------------------------
	/// Executes the compose action. 
	///-------------------------------------------------------------------------------------------------

	void OnCompose();

	///-------------------------------------------------------------------------------------------------
	/// Sets a hero. 
	///
	/// \param [in,out]	pHero	If non-null, the pointer to hero. 
	///-------------------------------------------------------------------------------------------------

	void SetHero(iHero* pHero);

	///-------------------------------------------------------------------------------------------------
	/// Gets the hero. 
	///
	/// \return	null if it fails, else the current hero. 
	///-------------------------------------------------------------------------------------------------

	inline const iHero* Hero() const { return m_pHero; }

	///-------------------------------------------------------------------------------------------------
	/// Gets the hero. 
	///
	/// \return	null if it fails, else the hero. 
	///-------------------------------------------------------------------------------------------------

	inline iHero* Hero() { return m_pHero; }

private:
	iHero*	m_pHero;	///< The pointer to hero	
	bool	m_bUnclickable; ///< the button is unclickable
	bool	m_bShowMoves;
#ifndef PC_VERSION
    bool	m_bGUI;
#endif
};



///-------------------------------------------------------------------------------------------------
/// Big hero portrait button. 
///-------------------------------------------------------------------------------------------------

class iBigHeroPortBtn : public iButton	
{
public:

	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, manager for pointer to view. 
	/// \param [in,out]	pCmdHandler	If non-null, the pointer to command handler. 
	/// \param	rect				The rectangle. 
	/// \param	uid					The uid. 
	///-------------------------------------------------------------------------------------------------

	iBigHeroPortBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);

	///-------------------------------------------------------------------------------------------------
	/// Executes the compose action. 
	///-------------------------------------------------------------------------------------------------

	void OnCompose();

	///-------------------------------------------------------------------------------------------------
	/// Sets a hero. 
	///
	/// \param [in,out]	pHero	If non-null, the pointer to hero. 
	///-------------------------------------------------------------------------------------------------

	void SetHero(iHero* pHero);

	///-------------------------------------------------------------------------------------------------
	/// Gets the hero. 
	///
	/// \return	null if it fails, else the current hero. 
	///-------------------------------------------------------------------------------------------------

	inline const iHero* Hero() const { return m_pHero; }

	///-------------------------------------------------------------------------------------------------
	/// Gets the hero. 
	///
	/// \return	null if it fails, else the hero. 
	///-------------------------------------------------------------------------------------------------

	inline iHero* Hero() { return m_pHero; }

	///-------------------------------------------------------------------------------------------------
	/// Sets a button is highlighted. 
	///
	/// \param	bHigh	true if highlighted. 
	///-------------------------------------------------------------------------------------------------

	inline void SetHighlighted(bool bHigh) { m_bHighlighted = bHigh; Invalidate(); }

private:
	iHero*	m_pHero;	///< The pointer to hero	
	bool	m_bHighlighted;
};

///-------------------------------------------------------------------------------------------------
/// Castle glyph button. 
///-------------------------------------------------------------------------------------------------

class iCastleGlyphBtn : public iImgBtn 	
{
public:

	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, pointer to view manager. 
	/// \param [in,out]	pCmdHandler	If non-null, the pointer to command handler. 
	/// \param	rect				The rectangle. 
	/// \param	uid					The uid. 
	/// \param	unclk				true if button is unclickable. 
	///-------------------------------------------------------------------------------------------------

	iCastleGlyphBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, bool unclk = false
#ifndef PC_VERSION
                    , bool bGUI = true
#endif
                    );

	///-------------------------------------------------------------------------------------------------
	/// Sets a castle. 
	///
	/// \param [in,out]	pCastle	If non-null, the pointer to castle. 
	///-------------------------------------------------------------------------------------------------

	void SetCastle(iCastle* pCastle);

	///-------------------------------------------------------------------------------------------------
	/// Gets the castle. 
	///
	/// \return	null if it fails, else the castle. 
	///-------------------------------------------------------------------------------------------------

	inline const iCastle* Castle() const { return m_pCastle; }

	///-------------------------------------------------------------------------------------------------
	/// Gets the castle. 
	///
	/// \return	null if it fails, else the castle. 
	///-------------------------------------------------------------------------------------------------

	inline iCastle* Castle() { return m_pCastle; }

private:
	iCastle*	m_pCastle;  ///< The pointer to castle
#ifndef PC_VERSION
    bool		m_bGUI;
#endif
};


///-------------------------------------------------------------------------------------------------
/// Big castle glyph button. 
///-------------------------------------------------------------------------------------------------

class iBigCastleGlyphBtn : public iButton 	
{
public:

	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param [in,out]	pViewMgr	If non-null, pointer to view manager. 
	/// \param [in,out]	pCmdHandler	If non-null, the pointer to command handler. 
	/// \param	rect				The rectangle. 
	/// \param	uid					The uid. 
	/// \param	unclk				true if button is unclickable. 
	///-------------------------------------------------------------------------------------------------

	iBigCastleGlyphBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);

	///-------------------------------------------------------------------------------------------------
	/// Sets a castle. 
	///
	/// \param [in,out]	pCastle	If non-null, the pointer to castle. 
	///-------------------------------------------------------------------------------------------------

	void SetCastle(iCastle* pCastle);

	///-------------------------------------------------------------------------------------------------
	/// Gets the castle. 
	///
	/// \return	null if it fails, else the castle. 
	///-------------------------------------------------------------------------------------------------

	inline const iCastle* Castle() const { return m_pCastle; }

	///-------------------------------------------------------------------------------------------------
	/// Gets the castle. 
	///
	/// \return	null if it fails, else the castle. 
	///-------------------------------------------------------------------------------------------------

	inline iCastle* Castle() { return m_pCastle; }

	///-------------------------------------------------------------------------------------------------
	/// Executes the compose action. 
	///-------------------------------------------------------------------------------------------------

	void OnCompose();

	///-------------------------------------------------------------------------------------------------
	/// Sets a button is highlighted. 
	///
	/// \param	bHigh	true if highlighted. 
	///-------------------------------------------------------------------------------------------------

	inline void SetHighlighted(bool bHigh) { m_bHighlighted = bHigh; Invalidate(); }
private:
	iCastle*	m_pCastle;  ///< The pointer to castle
	bool		m_bHighlighted; ///< if a button is highlighted
};


// Spell button
class iSpellBtn : public iButton 
{
public:
	iSpellBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
	void OnCompose();
	inline void SetSpell(const iBaseSpell* pSpell) { m_pSpell = pSpell; SetEnabled(m_pSpell!=NULL); }
	inline const iBaseSpell* GetSpell() const { return m_pSpell; }

private:
	const iBaseSpell*	m_pSpell;
};

// Popup list box
class iPopupList : public iPopupView, public IViewCmdHandler
{
public:
	typedef void(PopupListHandler)(sint32);
public:
	// c-tor
	iPopupList(iViewMgr* pViewMgr, PopupListHandler* pHandler, const iSize& itemMetrics, sint32 itemsCount, sint32 selItem, sint32 pageSize);

	// iPopupView overrides
	iSize PopupViewSize();
	iSize PopupViewMinSize();
	void OnTrackPopup(const iRect& clRect);
	void OnHidePopup();

	// iView overrides
	void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnCompose();

	// Scrollbar command handler
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

	// virtuals
	virtual void DrawItem(sint32 itemId, iDib& surf, const iRect& rc, uint32 flags)  =0;

private:
	PopupListHandler*	m_pHandler;
	iPHScrollBar*		m_pScrollBar;
	sint32				m_scrPos;
	sint32				m_sidx;
	sint32				m_itemsCount;
	sint32				m_pageSize;
	sint32				m_selItem;
	iSize				m_itemMetrics;
};

// Hero list popup
class iHeroListPopup : public iPopupList
{
public:
	// c-tor
	iHeroListPopup(iViewMgr* pViewMgr);

	// iPopupList overrides
	void DrawItem(sint32 itemId, iDib& surf, const iRect& rc, uint32 flags);
};


// Castle list popup
class iCastleListPopup : public iPopupList
{
public:
	// c-tor
	iCastleListPopup(iViewMgr* pViewMgr);

	// iPopupList overrides
	void DrawItem(sint32 itemId, iDib& surf, const iRect& rc, uint32 flags);
};

// Game popup menu
class iGamePopupMenu : public iPopupList
{
public:
	// c-tor
	iGamePopupMenu(iViewMgr* pViewMgr);

	// iPopupList overrides
	void DrawItem(sint32 itemId, iDib& surf, const iRect& rc, uint32 flags);
};

#endif //PHEROES_GAME_COMMONCONTROLS_H_
