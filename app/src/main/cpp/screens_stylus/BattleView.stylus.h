#ifndef PHEROES_GAME_BATTLEVIEW_STYLUS_H_
#define PHEROES_GAME_BATTLEVIEW_STYLUS_H_
//////////////////////////////////////////////////////////////////////////
enum BattleNavMode {
	BNM_MELEE = 0,
	BNM_SHOOT,
	BNM_INFO,
	BNM_COUNT
};

class iBattleToolBar;

class iBattleView_stylus: public iBattleView {
public:
	iBattleView_stylus();		
	void BeginBattle(iBattleWrapper* pBattle, SURF_TYPE st, BATTLE_BACK_TYPE background, BATTLE_SKY_TYPE sky);
	virtual bool OnKeyDown(sint32 key);
	virtual bool OnKeyUp(sint32 key);

	virtual void OnMouseDown(const iPoint& pos, MouseId mID);
	virtual void OnMouseUp(const iPoint& pos, MouseId mID);
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID);
	virtual bool Process(fix32 t) { return iBattleView::Process(t); }
	virtual void UpdateSize();
private:	
	virtual void BeginAutobattle();
	virtual void EndAutobattle();
	
	virtual bool IsMelee() { return m_battleMode == BNM_MELEE; }
	virtual bool IsShoot() { return m_battleMode == BNM_SHOOT; }

	virtual void UpdateToolbarMode();
	virtual bool BeginSpellTrack(iCombatSpell* pSpell);		
	virtual void EndSpellTrack(const iPoint& cell);	
		
	virtual void EnableControls(uint32 flags);
	virtual void ComposeSpellCursor();
	virtual void ComposeBattleCursors();	

	virtual void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

	iBattleToolBar*		m_pToolBar;
	BattleNavMode		m_battleMode;	
};

/*
*	Battle Toolbars
*/
class iBattleToolBar : public iView
{	
public:
	iBattleToolBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect);
	void OnCompose();
	void UpdateSize();
	void UpdateSizeHelper(uint32 index, iRect &rc);
	void EnableControls(uint32 flags);

	iIconButton*	m_pBtnWait;
	iIconButton*	m_pBtnDefend;
	iIconButton*	m_pBtnCastSpell;
	iIconButton*	m_pBtnAutoBattle;
	iIconButton*	m_pBtnMsgLog;
	iIconButton*	m_pBtnSettings;
	iHeroPortBtn*	m_pBtnAssaulter;
	iHeroPortBtn*	m_pBtnDefender;
	iBarTabSwitch*	m_pModeSwitch;
#ifdef MULTIPLAYER
	iButton*		m_pKickBtn;	
#endif
};
#endif //PHEROES_GAME_BATTLEVIEW_STYLUS_H_
