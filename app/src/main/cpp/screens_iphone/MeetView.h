#pragma once

#include "GameViewInterface.h"

class iArmyExchange;
class iArtClickItem;
class iScreenSlider;


namespace UI_IPHONE
{
    class iPrSkillsView;
    class iArtifactBag;
    
    class iHeroInfoView: public iView, public IViewCmdHandler
    {
    public:
        iHeroInfoView(iViewMgr* pViewMgr, iRect rc, uint32 uid);
        void SetHero(iHero* pHero);
        void UpdateSize();
        void OnCompose();
        void iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param );
    private:
        iPrSkillsView* m_pPrSkillsView;
        iHero*		   m_pHero;
    };
    
    class iHeroArtView: public iView, public IViewCmdHandler
    {
    public:
        iHeroArtView(iSimpleArray<iArtClickItem*>& competitors, iViewMgr* pViewMgr,  iRect rc, uint32 uid);
        void SetHero(iHero* pHero);
        void UpdateSize();
        void iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param );
        void OnCompose();
    private:
        iArtifactBag *m_pArtBag;
        iHero		 *m_pHero;
    };
    
    class iMeetView : public IMeetView
    {
    public:
        /// Default constuctor
        iMeetView();
        
        /// Default destructor
        ~iMeetView() {}
        
        void SetHeroes(iHero* pHero1, iHero* pHero2);
        
        /// Called when needs a redraw
        void OnCompose();
        
        /// Called when a user swipes mouse up
        void OnSwipeUp(sint32 length);
        void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        
        void OnActivate(bool bActivate);
        
        virtual CHILD_VIEW Type() const { return CV_MEET; }
        
        virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); }
        
        virtual inline bool AlwaysReceiveMouse() { return true; }
        
    private:
        /// Updates the size and pos of all controls
        void UpdateSize();
        
        /// Processes the user interaction
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        
        
        private	:
        iSimpleArray<iArtClickItem*>		m_artCompetitors;
        bool				m_bGestureComplete;
        iHero				*m_pHero1, *m_pHero2;
        iButton				*m_pExitBtn;
        iArmyExchange		*m_pArmyExchange;
        iCheckButton*       m_pSplitBtn;
        iHeroInfoView		*m_pHeroInfoView1, *m_pHeroInfoView2;
        iHeroArtView		*m_pHeroArtView1, *m_pHeroArtView2;
        iScreenSlider		*m_pScreenSlider;
        sint32				ARMY_SLIDE, ARTIFACTS_SLIDE;
    };
    
}
