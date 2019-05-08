#ifndef PHEROES_GAME_BATTLEVIEW_H_
#define PHEROES_GAME_BATTLEVIEW_H_

enum BATTLE_CONTROLS_IDS {
	BCI_DEFAULT = 1024,
	BCI_ASSAULTER_BTN,
	BCI_DEFENDER_BTN,
	BCI_MODE_SWITCH,
	BCI_WAIT_BTN,
	BCI_DEFEND_BTN,
	BCI_MSGLOG_BTN,
	BCI_AUTOBATTLE_BTN,
	BCI_CASTSPELL_BTN,
	BCI_SETTINGS_BTN,
	BCI_INFO_BTN,
#ifdef MULTIPLAYER
	, BCI_KICK_BTN
#endif
};


//////////////////////////////////////////////////////////////////////////
class iCreatInfoPopup : public iFramePopupView
{
public:
	iCreatInfoPopup(iViewMgr* pViewMgr, PLAYER_ID pid, const iBattleGroup* pGroup);
	void DoCompose(const iRect& clRect);
	iSize ClientSize();
private:
	const iBattleGroup*	m_pGroup;
};


//////////////////////////////////////////////////////////////////////////
struct iBatObstEntry {
	iBatObstEntry(uint16 _obstId, const iPoint& _cell) : obstId(_obstId), cell(_cell) {}
	uint16	obstId;
	iPoint	cell;
};
typedef iSimpleArray<iBatObstEntry> iBatObstList;

//////////////////////////////////////////////////////////////////////////
class iBattleToolBar;
class iCastSpellToolBar;
class iAutoBattleToolBar;

class iBattleView : public iChildGameView
{
public:
	enum StateFlags {
		Nothing		= 0x00,
		EnemyTurn	= 0x01,
		Acting		= 0x02,
		CanMelee	= 0x04,
		CanShoot	= 0x08,
		CanInfo		= 0x10,
		CanCast		= 0x20,
		CanWait		= 0x40,
		SpellBook	= 0x80,
#ifdef MULTIPLAYER
		, RemoteTurn = 0x100
#endif
	};

	static iPoint anchor;
	inline static iPoint Map2Screen(const iPoint& pos)
	{ 
		return iPoint(anchor.x+pos.x*sm_hexWidth - (pos.y&1)*sm_hexHalfWidth ,anchor.y + pos.y*sm_hexHeight); 
	}
	inline static iPoint Map2ScreenCenter(const iPoint& pos)
	{
		return Map2Screen(pos) + iPoint(sm_hexHalfWidth, sm_hexHeight / 2);
	}
	inline static iPoint Screen2Map(const iPoint& pos)
	{ 		
		sint32 y = (pos.y - anchor.y)/sm_hexHeight;
		if(y < 0 || y > 10) return cInvalidPoint;
		sint32 x = iCLAMP<sint32>(0, 12, (pos.x - anchor.x + (y&1)*sm_hexHalfWidth)/sm_hexWidth);		
		return iPoint(x,y);
	}


	iBattleView();
	~iBattleView();

	virtual void BeginBattle(iBattleWrapper* pBattle, SURF_TYPE st, BATTLE_BACK_TYPE background, BATTLE_SKY_TYPE sky) =0;
	void Start();
	void EndBattle(iBattleResult br);

	virtual bool OnKeyDown(sint32 key) { return true; };
	virtual bool OnKeyUp(sint32 key) { return true; };

	virtual void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) =0;
	virtual void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID) =0;
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID) =0;
	
	virtual void SetInfoMode(bool bInfoMode) =0;
	
	virtual CHILD_VIEW Type() const { return CV_BATTLE; }
	
	virtual bool Process(fix32 t);
	void OnActivate(bool bActivate);
	void PrepareSurface();
	void DoCompose();
	virtual void UpdateSize() {};

	void AddLogEvent(const iStringT& msg);
	void AddCellEvent(const iStringT& msg, const iPoint& pos);
	
	bool AlwaysReceiveMouse() { return true; }
	virtual bool OnGroupChanged();
	inline bool IsAni() const { return m_bAni; }
	void BeginAni();
	void EndAni();
	
	void SpellDlg();

	static iPoint CreatPos(iPoint anchor, iBattleGroup::ORIENT orient, uint8 size);
	static iPoint CreatCorpsePos(iPoint anchor, iBattleGroup::ORIENT orient, uint8 size);
	static iPoint FortElemPos( iPoint pos, iCastleFort::ElementType target, iCastleFort::ElementState state);
	static iPoint FortElemHitPos( iPoint pos, iCastleFort::ElementType target, iCastleFort::ElementState state);
public:
	static uint8 sm_gridZoom;
	static uint16 sm_hexWidth;
	static uint16 sm_hexHalfWidth;
	static uint16 sm_hexHeight;	
	static uint16 sm_backOffset;
protected:
	virtual iPoint& SpellTrackCell() { return m_trackCell; }
	virtual void EnableControls(uint32 flags) {}
	virtual void UpdateToolbarMode() {}
	virtual bool IsMelee() { return true; }
	virtual bool IsShoot() { return true; }
	virtual void ComposeSpellCursor() {}
	virtual void ComposeBattleCursors() {}
	virtual bool IsAcceptingCmds() { return true; }
	virtual void SwitchToDefendBtn() {}
public:
	virtual void BeginAutobattle();
	virtual void EndAutobattle();
	//inline bool IsAutobattle() const { return m_pAutoBattleToolBar != NULL; }
    inline bool IsAutobattle() const { return m_pBattle->Engine().IsAutoBattle(); }
    inline bool IsAnyAutobattle() const { return m_pBattle->Engine().IsAnyAutoBattle(); }
protected:    
	virtual bool BeginSpellTrack(iCombatSpell* pSpell);
	virtual void EndSpellTrack(const iPoint& cell);
	iBattleGroup* GetSpellTarget(const iPoint& cell);
	inline bool SpellTracking() const { return m_pCastSpellToolBar != NULL; }	

	// recalc anchors and other resolution-dependant params
	void Recalc();		
protected:	
	virtual void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	virtual void HighlightMemberButton( uint32 type, bool bHighlight ) =0;
public:
	bool			m_bDontMove;
protected:	
	iPoint              m_MoveEntry;
	iPoint				m_trackPos;
	iPoint				m_trackCell;
	iStringT			m_toolTip;
	iSimpleArray<iPoint>	m_spellTargets;
	iBattleWrapper*		m_pBattle;	
	BATTLE_BACK_TYPE m_background;
	BATTLE_SKY_TYPE m_sky;
	
	// Melee
	typedef const iBattleGroup::iMeleeEntry* iMeleeEntryPtr;
	iMeleeEntryPtr	m_pMeleeTrack;
	sint32			m_meleeDir;

	// Shoot
	typedef const iBattleGroup::iShootEntry* iShootEntryPtr;
	iShootEntryPtr m_pShootTrack;

	// Creature Info popup view
	iCreatInfoPopup*	m_pCreatInfoPopup;

	iCastSpellToolBar*	m_pCastSpellToolBar;	
	iAutoBattleToolBar*	m_pAutoBattleToolBar;
    
	bool				m_bForceInfo;
	bool				m_bHumanTurn;	
	iDib				m_dibSurf;
	iDib				m_dibTurnGlyph;
	
	// Obstacles
	iBatObstList	m_obstacles;

	// Events
	iEventList				m_cellEvents;
	iSimpleArray<iStringT>	m_log;
	
	// Animation
	fix32			m_actTimer;
	bool			m_bAni;
	
	// blinking
	fix32			m_blinkTimer;
	bool			m_bHighlight;
	
	iPoint			m_highlightedTarget;
	iPoint			m_highlightedGroup;	
#ifdef PC_VERSION
public:
    bool            m_bDefender;
    bool            m_bBeginAutocombat;
    bool            m_bEndAutocombat;
    bool            m_bWait;
    bool            m_bSpell;
    iPoint          m_Entered;
#endif
};

/*
 * Spell toolbar
 */
class iCastSpellToolBar : public iView
{
public:
	iCastSpellToolBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, iHero* pCaster, iCombatSpell* pSpell);	
	inline void SetTarget(iBattleGroup* pTarget) { m_pTarget = pTarget; Invalidate(); }
	void OnCompose();

	//
	inline iCombatSpell* Spell() { return m_pSpell; }

private:
	iCombatSpell*			m_pSpell;
	iHero*					m_pCaster;
	iBattleGroup*			m_pTarget;
    iTextButton*            m_pBtnCancel;
};

/*
 * Autobattle toolbar
 */
class iAutoBattleToolBar : public iView
{
public:
	iAutoBattleToolBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect);
	void OnCompose();
};

#endif //PHEROES_GAME_BATTLEVIEW_H_
