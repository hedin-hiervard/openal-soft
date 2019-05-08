#pragma once

/*
 *  Generic GameView
 */
class iChildGameView : public iTopmostView, public IViewCmdHandler
{
public:
    enum CHILD_VIEW {
        CV_UNDEFINED    = -1,
        CV_MENU         = 0,
        CV_OVERLAND,
        CV_BATTLE,
        CV_CASTLE,
        CV_HARBOR,
        CV_WARACADEMY,
        CV_HERO,
        CV_MEET,
        CV_MINIMAP,
        CV_GAMEMENU,
        CV_SCROLL,
        CV_UPGRADE,
        CV_GAMERESULT,
        
#ifdef MULTIPLAYER
        CV_MPLOGIN,
        CV_MPLOBBY,
        CV_MPGAMESETUP,
#endif
        CV_COUNT
    };
    
public:
    iChildGameView(bool bEternal, CHILD_VIEW parentView);
    virtual ~iChildGameView() {}
    inline bool Eternal() const { return m_bEternal; }
    inline CHILD_VIEW ParentView() const { return m_parentView; }
    virtual bool Process(fix32 t) { return true; }
    virtual void OnActivate(bool bActivate) {}
    virtual void OnModalDlgAdded() {}
    virtual void OnModalDlgRemoved() {}
    virtual CHILD_VIEW Type() const =0;
    
    void FallDown();
    
    void OnCompose();
    void AfterCompose();
    
    virtual void DoCompose(){};
    
#if defined( PC_VERSION )
    virtual void SetLoadFlagTrue() {};
    virtual void SetSaveFlagTrue() {};
#endif
private:
    CHILD_VIEW  m_parentView;
    bool        m_bEternal;
protected:
    bool bFirstDarken;
};


class IHarborView: public iChildGameView
{
public:
    using iChildGameView::iChildGameView;
    
    virtual void SetHero(iHero* pHero) =0;
    virtual void SetHarbor(IHarbor* pHarbor) =0;
};


class IUpgradeView: public iChildGameView
{
public:
    using iChildGameView::iChildGameView;

    virtual void SetHero(iHero* pHero) =0;
};

class IHeroView: public iChildGameView
{
public:
    using iChildGameView::iChildGameView;
    virtual void SetHero(iHero* pHero) =0;
};

class IResourceBar: public iView
{
public:
    using iView::iView;
    
    virtual void SetMinerals(const iMineralSet* ms) =0;
    virtual void OnCompose() =0;
    virtual iSize GetDefSize() =0;
    virtual void AnimateResources(const iMineralSet& was, const iMineralSet& now) =0;
};

class IMeetView: public iChildGameView
{
public:
    using iChildGameView::iChildGameView;
    
    virtual void SetHeroes(iHero* pHero1, iHero* pHero2) =0;

};

class ICastleView: public iChildGameView
{
public:
    enum CastleScreen {
        RecruitScreen,
        TavernScreen,
        MarketScreen,
        MGuildScreen
    };
    
    using iChildGameView::iChildGameView;
    virtual void SetCastle(iCastle* pCastle) =0;
    
    virtual void Update() =0;
    virtual void SetScreen(CastleScreen scr) {}
    virtual void SetRecruitCreatureType(CREATURE_TYPE ct) {}
};

class IGameResultView: public iChildGameView
{
public:
    using iChildGameView::iChildGameView;
    
    enum GRType {
        GRWin = 0,
        GRDefeat
    };
    
    virtual void SetTitle(iStringT title)  =0;
    virtual void SetText(iStringT text)  =0;
    virtual void SetType(GRType type)  =0;
    virtual void SetMapName(iStringT mapName)  =0;
};

class IMainMenuView: public iView, public IViewCmdHandler
{
public:
    enum Result
    {
        ResExit = 0,
        ResSingle = 1,
        ResMulti = 2,
        ResMain = 3,
        ResContinue = 4,
        ResContinueNoHide = 5,
        ResTab = 6,
        ResTutorial = 7,
        ResSingleMap
#if defined(PC_VERSION)
        = 8,
        ResExitGame
#endif
    };
    
    enum Type
    {
        TypeMain = 0,
        TypeSingle = 1,
        TypeInGame = 2,   // this one used during game, have save game menu
        TypeMulti= 3,
        TypeSingleMap = 4, // for a RoyalBounty project,
        TypeSingleMapInGame = 5
    };

    using iView::iView;
    
    virtual void OnActivate(bool bActivate) =0;
    virtual bool Process(fix32 t)=0;
};

class IOverlandToolbar: public iView, public IViewCmdHandler
{
public:
    using iView::iView;
    
    virtual iSize GetDefSize() const =0;
    virtual void SetHero(iHero* pHero)=0;
    virtual void SetCastle(iCastle* pCastle)=0;
    virtual void SetMapPos(iPoint pos) =0;
    virtual void ComposeInfoBar()=0;
    virtual void UpdateMinimap()=0;
    virtual void CenterMinimap()=0;
    virtual void AnimateExp(sint32 oldperc, sint32 newperc)=0;
    virtual void FinishAnimatingExp()=0;
    virtual void SetInfoMode(bool bInfoMode)=0;
    virtual void SetHighlighted( bool flag )=0;
    
};

class INewMenuDlg: public iDialog, public IViewCmdHandler
{
public:
    using iDialog::iDialog;
    
    virtual void SetType(IMainMenuView::Type tp, bool fInGame) =0;
};

