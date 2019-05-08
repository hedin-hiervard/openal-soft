#ifndef PHEROES_GAME_GAME_H_
#define PHEROES_GAME_GAME_H_

#include "GameViewInterface.h"

class iMenuView;

/*
 *  BlackBack
 */
class iBlackBackView : public iTopmostView
{
public:
    iBlackBackView();
    void OnCompose();
	virtual iSize GetDefaultSize() const;
};


//////////////////////////////////////////////////////////////////////////
struct IMsgComposer
{
    virtual void AddMsg(const iStringT& text) =0;
};

/*
 *  Game 
 */

class iMainView;
class iMainView_touch;


class iGame : public IGame
{
public:
    // c-tor, d-tor
    iGame();
    ~iGame();

    // Init/cleanup game resources
    bool Init(const iStringT& fname);
    void Cleanup();

    // Game control
    void Quit();
    void SaveLastSes();
    void MainMenu();
    void RealMainMenu();
    void ShowMainMenu(bool fInGame);
    bool StartNewGame(const iMapInfo& mapInfo, bool bNewGame, bool bUpdateVisitors);
    bool StartTutorialMap();
    bool StartSingleMap();
    void DelayedStartGame(const iMapInfo& mapInfo, bool bNewGame);
    void ExitGame(bool bChangeView);

    // inlines
    inline bool Inited() const { return m_bInited; }

    inline iGameWorld& Map() { return m_Map; }
    inline GAME_LANG MapLanguage() { return m_mapLanguage; }
    inline iItemMgr& ItemMgr() { return m_itemMgr; }
    inline iSoundMap& SoundMap() { return m_soundMap; }
    void SetLHandMode(bool lhandmode);
    
    // Child views
    void ShowView(iChildGameView::CHILD_VIEW cv, bool bAfterHide = false);
    void ShowViewDelayed(iChildGameView::CHILD_VIEW cv);
    void HideView(iChildGameView::CHILD_VIEW cv);
    void HideViewDelayed(iChildGameView::CHILD_VIEW cv);
    inline iChildGameView* View(iChildGameView::CHILD_VIEW view) { return (view==iChildGameView::CV_UNDEFINED)?NULL:m_pChildView[view]; }
    inline iChildGameView* ActView() { return  View(m_tActView); }
	
    inline IHeroView* HeroView() { return (IHeroView*)View(iChildGameView::CV_HERO); }
	inline IMeetView* MeetView() { return (IMeetView*)View(iChildGameView::CV_MEET); }
	inline ICastleView* CastleView() { return (ICastleView*)View(iChildGameView::CV_CASTLE); }
	inline IUpgradeView* UpgradeView() { return (IUpgradeView*)View(iChildGameView::CV_UPGRADE); }
	inline IHarborView* HarborView() { return (IHarborView*)View(iChildGameView::CV_HARBOR); }
    inline IGameResultView* GameResultView() { return (IGameResultView*)View(iChildGameView::CV_GAMERESULT); }
    
    inline iMenuView* MenuView() { return (iMenuView*)View(iChildGameView::CV_MENU); }
	
    inline iMainView_touch* MainView() { return (iMainView_touch*)m_pMainView; }
    inline iChildGameView::CHILD_VIEW ActViewType() const { return m_tActView; }
    
    // Game process
    void BeginBattle(const iBattleInfo& bi, bool quickbattle = false);
    void EndBattle();
    void MeetHeroes(iHero* pHero1, iHero* pHero2, bool bAct);
    sint32 Process(fix32 t);
    void OnKeyDown(sint32 key);
    void OnKeyUp(sint32 key);
    void OnSuspend();
    void OnResume();

    // Msgs
    void AddMsg(const iStringT& msg);
    void AddCellMsg(const iStringT& msg, const iPoint& pos);

    // Animation
    void OnDisapObject(SpriteId sid, const iPoint& pos, const iPoint& offset=iPoint(0,0));

	// Game notifications
	void OnVictory(VICTORY_CONDITION_TYPE vc);
	void OnDefeat(LOSE_CONDITION_TYPE lc, bool bExitGame);
    void OnActorChanged(iPlayer* pNewActor, bool bAttack);
    void OnPlayerChanged(iPlayer* pNewPlayer, bool bAct);
    void OnPlayerVanquished(PLAYER_ID pid);
    
    void OnAddCastle(iCastle* pCastle);
    void OnDelCastle(iCastle* pCastle);
    void OnCastleChanged(iCastle *pCastle);

    void OnAddShip(iShip* pShip);
    void OnDelShip(iShip* pShip);

    void OnAddHero(iHero* pHero);
    void OnDelHero(iHero* pHero);
    void OnHeroChanged(iHero* pHero);
    void OnHeroLevel(iHero* pHero, uint8 level, const iNewLevelInfo& linfo);
    void OnHeroMoveTo(iHero* pHero, const iPath::iStep& step);
    void OnHeroStopMoving(iHero* pHero);
    void OnHeroPosChanged(iHero* pHero, const iPoint& npos);
    void OnHeroTeleport(iHero* pHero, const iPoint& src, const iPoint& dst);
    void OnAttackHero(iHero* pHero1, iHero* pHero2);

    inline bool IsExitingToMenu() {return m_bGoToMainMenu;}
	inline void ExitToMenu() { m_bGoToMainMenu = true; }
    inline /*const*/ iBattleWrapper* BattleWrapper() /*const*/ { return m_pBattle; }

	virtual void OnScreenModeChange() { UpdateViewSizes(); }
    void UpdateViewSizes();

	void Autosave();
	void Crashsave();
	void Quicksave();
	void Quickload();
#if defined( OS_MACOS )
	void ShowSaveScreen();
#endif	
    void PushScreen();
    inline void ClearScreenStack() { m_screenStack.RemoveAll(); }
    inline iDib PopScreen() { return m_screenStack.Pop(); }
    inline iDib& PeekScreen() { return m_screenStack.At(m_screenStack.GetSize()-1); }
	inline bool Started() const { return m_bStarted; }

	bool CheckPlayerWinAndLose(iPlayer *pPlayer);	
	inline void AutoNextTurn() { m_bNextTurn = true; }
	
	void PauseMoveSound();
	void ResumeMoveSound();

	virtual void OnModalDlgAdded() { PauseMoveSound(); if(ActView()) ActView()->OnModalDlgAdded(); }
	virtual void OnModalDlgRemoved() { ResumeMoveSound(); if(ActView()) ActView()->OnModalDlgRemoved();}
private:
        // 
    static uint16 GetMovSound(SURF_TYPE st, bool bRoad);
	bool            m_bGoToMainMenu;
    iMapInfo        m_delMapInfo;
    bool            m_bDelNewGame;
    bool            m_bDelayedStart;
    
    iItemMgr        m_itemMgr;
    iGameWorld      m_Map;
    iSoundMap       m_soundMap;
    iBattleWrapper* m_pBattle;
    bool            m_bInited;
    bool            m_bStarted;

    sint32          m_hmChannel;
    uint16          m_hmSound;
    
    iMainView*      m_pMainView;
    iChildGameView* m_pChildView[iChildGameView::CV_COUNT];
    iChildGameView::CHILD_VIEW      m_tActView;
    GAME_LANG       m_mapLanguage;
    iChildGameView::CHILD_VIEW m_delayedCV;
    iChildGameView::CHILD_VIEW m_delayedShowCV;
    iSimpleArray<void*> m_deleteList;
    iSimpleArray<iDib> m_screenStack;

	iSimpleArray<iChildGameView::CHILD_VIEW> m_viewStack;	
	bool			m_bNextTurn;
#if defined( PC_VERSION )
public: //for HotKeys
	//hot up,down,left,right keys
	bool m_bKeyUpFlag;
	bool m_bKeyDownFlag;
	bool m_bKeyLeftFlag;
	bool m_bKeyRightFlag;
	
	bool m_bSaveScreen;
	bool m_bLoadScreen;
	//Cast spell
	bool m_bSpell;
	
	bool m_bNumMove;
	iPoint pos;
	uint32 m_Timer;
    bool bRightDown;
    bool m_bQuickLoad;
    bool m_bQuickSave;
    
    bool m_bQuit;
    bool m_bExitDlg;
    bool m_bEnter;
    bool m_bEsc;
    bool m_bMinimapEsc;
    bool m_bCenter;
    bool m_bShowCastle;
#endif
};



#endif //PHEROES_GAME_GAME_H_
