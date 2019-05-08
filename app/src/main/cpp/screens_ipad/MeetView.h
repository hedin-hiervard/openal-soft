#pragma once

#include "GameViewInterface.h"

class iArmyExchange;
class iArtClickItem;


namespace UI_IPAD
{
    class iArtifactBag;

    class iPrSkillsView;
    
    class iHeroChildView: public iView, IViewCmdHandler {
    public:
        iHeroChildView(iViewMgr* pViewMgr, IViewCmdHandler* pHandler, const iRect& rc, uint32 uid, iSimpleArray<iArtClickItem*>& m_artCompetitors);
        void UpdateSize();
        void OnCompose();
        void SetHero(iHero *pHero);
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        inline iArmyExchange& ArmyExchange() const { return *m_pArmyExchange; }
        void AddArmyCompetitor(iArmyListEx* comp);
        void SetSplitChecked( sint32 checked );
        void SetSplitButton( iCheckButton * pSplitBtn );
    private:
        iHero			*m_pHero;
        iArtifactBag	*m_pArtBag;
        iArmyExchange	*m_pArmyExchange;
        iPrSkillsView	*m_pPrSkillsView;
        IViewCmdHandler *m_pHandler;
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
        void DoCompose();
        
        /// Called when a user swipes mouse up
        void OnSwipeUp(sint32 length);
        void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        
        void OnActivate(bool bActivate);
        
        virtual inline bool AlwaysReceiveMouse() { return true; }
        
        virtual CHILD_VIEW Type() const { return CV_MEET; }
        
        virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); }
    private:
        /// Updates the size and pos of all controls
        void UpdateSize();
        
        /// Processes the user interaction
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);	
        
        
        private	:	
        iSimpleArray<iArtClickItem*>		m_artCompetitors;	
        bool				m_bGestureComplete;
        iHero				*m_pHero1, *m_pHero2;
        iHeroChildView		*m_pChildView1, *m_pChildView2;
        iButton				*m_pExitBtn;
        iCheckButton		*m_pSplitBtn;
    };
}
