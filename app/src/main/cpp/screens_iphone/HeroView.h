#pragma once

#include "GameViewInterface.h"

class iArtClickItem;
class iArtItemCtrl_touch;
class iSpellBookView;
class iArmyExchange;
class iScreenSlider;


namespace UI_IPHONE
{
    
    /*	Primary Skills view
     */
    class iPrSkillBtn : public iButton
    {
    public:
        iPrSkillBtn(iViewMgr* pViewMgr, IViewCmdHandler *pHandler, const iRect& rect, uint32 uid, FURTHER_SKILLS skill);
        void OnCompose();
        void SetHero(iHero* pHero);
        
        iHero* 			m_pHero;
        FURTHER_SKILLS	m_skill;
    };
    //////////////////////////////////////////////////////////////////////////
    class iPrSkillsView : public iView, public IViewCmdHandler
    {
    public:
        iPrSkillsView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
        void OnCompose();
        void SetHero(iHero* pHero);
        void UpdateSize();
    public:
        static const iSize sm_defSize;
    protected:
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
    protected:
        iButton			*m_pBtns[8];
        iHero*			m_pHero;
        IViewCmdHandler *m_pHandler;
    };
    
    //////////////////////////////////////////////////////////////////////////
    class iHeroTopView : public iView
    {
    public:
        iHeroTopView(iViewMgr* pViewMgr, const iRect& rect, uint32 uid);
        void OnCompose();
        void SetHero(iHero* pHero);
    public:
        static const iSize sm_defSize;
    protected:
        iHero*			m_pHero;
    };
    
    
    class iSkillsTabBtn : public iButton
    {
    public:
        iSkillsTabBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
        inline void SetHero(const iHero *pHero);
        const iHero* Hero() const;
    protected:
        const iHero* m_pHero;
    };
    
    class iSecSkillBtn : public iButton
    {
    public:
        iSecSkillBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
        inline void SetSkill(const iSecSkillEntry* pSkill);
        inline const iSecSkillEntry* Skill() const;
        void OnCompose();
    private:
        const iSecSkillEntry* m_pSkill;
    };
    
    class iExpBtn : public iSkillsTabBtn
    {
    public:
        iExpBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
        void OnCompose();
    };
    
    class iManaBtn : public iSkillsTabBtn
    {
    public:
        iManaBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
        void OnCompose();
    };
    
    class iActPtsBtn : public iSkillsTabBtn
    {
    public:
        iActPtsBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
        void OnCompose();
    };
    
    class iMoraleBtn : public iSkillsTabBtn
    {
    public:
        iMoraleBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
        void OnCompose();
    };
    
    class iLuckBtn : public iSkillsTabBtn
    {
    public:
        iLuckBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
        void OnCompose();
    };
    
    
    
    class iSecSkillsView: public iView, public IViewCmdHandler {
    public:
        iSecSkillsView(iViewMgr *pViewMgr, IViewCmdHandler* pCmdHandler, iPoint pnt, uint32 uid);
        
        void SetHero(iHero *pHero) ;
        void OnCompose();
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        void UpdateSize();
    public:
        static iSize sm_defSize;
    private:
        iSecSkillBtn*	m_pBtnSecSkill[8];
        iHero*			m_pHero;
        IViewCmdHandler* m_pCmdHandler;
        
    };
    
    
    class iExpLuckMoraleView: public iView, public IViewCmdHandler {
    public:
        iExpLuckMoraleView(iViewMgr *pViewMgr, IViewCmdHandler* pCmdHandler, iRect rc, uint32 uid);
        void SetHero(iHero *pHero);
        void OnCompose();
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        void UpdateSize();
    public:
        static const iSize sm_defSize;
    private:
        iSkillsTabBtn*	m_pSkillTabBtns[5];
        iHero*			m_pHero;
        IViewCmdHandler* m_pCmdHandler;
        
    };
    
    class iArtItemCtrl_Doll: public iArtItemCtrl_touch {
    public:
        iArtItemCtrl_Doll(iSimpleArray<iArtClickItem*>& competitors, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, const iSize& cellSiz, uint32 uid);
        void Highlight(iArtClickItem* pSource, uint16 artId);
        void Deselect();
    };
    
    class iHeroDollView: public iView, public IViewCmdHandler {
    private:
        static const sint32 doX = 55;
        static const sint32 doY = 10;
        static const sint32 acW = 30;
        static const sint32 acH = 28;
        
    public:
        iHeroDollView(iSimpleArray<iArtClickItem*>& competitors, iViewMgr *pViewMgr, iRect rc, uint32 uid);
        void SetHero(iHero *pHero);
        void OnCompose();
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        void UpdateSize();
        void Deselect();
    public:
        static const iSize sm_defSize;
    private:
        
        iHero*			m_pHero;
        IViewCmdHandler* m_pCmdHandler;
        iArtItemCtrl_Doll *m_pDollItems[AC_COUNT];
    };
    
    
    class iArtifactBag: public iArtClickItem, public iArtBackPackCtrl {
    public:
        iArtifactBag(iSimpleArray<iArtClickItem*>& competitors, iViewMgr* pViewMgr, uint32 uid);
        void SetHero(iHero *pHero);
        void OnCompose();
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        void UpdateSize();
        void Deselect();
        void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        void OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        
        inline void Highlight(iArtClickItem* pSource, uint16 artId) {};
        virtual void Unhighlight() {}
        
        virtual iArtCell Remove(sint32 cell);
        virtual iArtCell Replace(sint32 cell, const iArtCell& newArt);
        
        virtual uint16 ArtId() const;
        virtual	sint32 BackPackIdxByCell(sint32 cellIdx) const;
    public:
        static const iSize sm_defSize;
        static const iPoint contentOffset;
        static const iSize sm_contentSize;
    private:
        void UpdateItems();
        sint32 CellByPos(const iPoint& pos) const;
        uint32 MaxPage();
        void SyncArts();
        void UpdateContent(sint32 cellIdx);
        
        void RemoveTrailingSlots();
        
    private:
        iSimpleArray<uint16> m_contents;
        uint32 m_curPage;
        iSimpleArray<iArtClickItem*>&		m_Competitors;
        iHero*			m_pHero;
        iButton* m_pPrevPageBtn;
        iButton* m_pNextPageBtn;
        sint8		m_lastClickedItem;
    };
    
    class iKeysView: public iView {
    public:
        iKeysView(iViewMgr *pViewMgr, const iRect& rc, uint32 uid);
        void SetHero(iHero *pHero);
        void OnCompose();
    public:
        static const iSize sm_defSize;
    private:
        iHero*	m_pHero;
    };
    
    class iHeroView : public IHeroView
    {
    public:
        /// Default constuctor
        iHeroView();
        
        /// Default destructor
        ~iHeroView() {}
        
        void SetHero(iHero* pHero);
        
        /// Called when needs a redraw
        void OnCompose();
        
        /// Called when a user swipes mouse up
        void OnSwipeUp(sint32 length);
        
        void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        
        void OnActivate(bool bActivate);
        
        virtual inline bool AlwaysReceiveMouse() { return true; }
        virtual CHILD_VIEW Type() const { return CV_HERO; }
        
        virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); }
        
        void OnDualSwipeLeft(sint32 length);
        void OnDualSwipeRight(sint32 length);
    private:	
        /// Updates the size and pos of all controls
        void UpdateSize();	
        
        /// Processes the user interaction
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        
        bool Process(fix32 t);
        private	:	
        iSimpleArray<iArtClickItem*>		m_artCompetitors;	
        bool				m_bGestureComplete;
        iHero*				m_pHero;
        iSpellBookView*		m_pSpellbookView;
        iArtifactBag*		m_pArtBag;
        iHeroDollView*		m_pHeroDoll;	
        iButton*			m_pDigBtn;
        iButton*			m_pPuzzleBtn;
        iButton*			m_pExitBtn;
        iArmyExchange*		m_pArmyExchange;
        iButton*			m_pDismissBtn;
        iKeysView*			m_pKeysView;
        iSecSkillsView*		m_pSecSkillsView;
        iPrSkillsView*		m_pPrSkillsView;
        iPrSkillsView*		m_pPrSkillsView2;
        iExpBtn*			m_pExpBtn;
        iExpLuckMoraleView* m_pEMLView;
        iButton*			m_pPrevHeroBtn;
        iButton*			m_pNextHeroBtn;
        iButton*			m_pCastBtn;
        iHeroTopView*		m_pHeroTopView;
        iCheckButton*       m_pSplitBtn;
        iScreenSlider*		m_pScreenSlider;
        uint32				STATS_SLIDE, ARTIFACTS_SLIDE, SPELLBOOK_SLIDE;
    };
    
    
}
