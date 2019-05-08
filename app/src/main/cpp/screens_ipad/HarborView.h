#pragma once

#include "GameViewInterface.h"

namespace UI_IPAD
{
    
    enum HarborState {
        HS_NA = 0,
        HS_BUILD,
        HS_OCCUPY,
        HS_OCCUPIED
    };
    
    class iHarborDockView;
    
    class iHarborControlView: public iView, public IViewCmdHandler
    {
    public:
        iHarborControlView(iViewMgr *pViewMgr, const iRect& rc, uint32 uid, iHarborDockView* pParentDock);
        void OnCompose();
        void UpdateSize();
        void Update();
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
    public:
        static const iSize sm_defSize;
    private:
        iImgBtn* m_pBtn;
        iHarborDockView *m_pParentDock;
    };
    
    class iHarborDockView: public iView
    {
        friend class iHarborControlView;
    public:
        
    public:
        iHarborDockView(iViewMgr *pViewMgr, const iRect& rc, uint32 uid, uint32 dockId);
        void OnCompose();
        void UpdateSize();
        void SetHarbor(IHarbor* pHarbor);
        void SetHero(iHero* pHero);
    private:
        HarborState GetHarborState();
    private:
        iHarborControlView* m_pHCView;
        uint32 m_dockId;
        IHarbor *m_pHarbor;
        iHero*	m_pHero;
    };
    
    class iHarborView : public IHarborView
    {
    public:
        /// Default constuctor
        iHarborView();
        
        /// Default destructor
        ~iHarborView() {}
        
        void SetHero(iHero* pHero);
        void SetHarbor(IHarbor *pHarbor);
        
        /// Called when needs a redraw
        void DoCompose();
        
        void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        
        /// Called when a user swipes mouse up
        void OnSwipeUp(sint32 length);
        
        void OnActivate(bool bActivate);
        
        virtual inline bool AlwaysReceiveMouse() { return true; }
        
        virtual CHILD_VIEW Type() const { return CV_HARBOR; }
        virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); }
        
    private:	
        /// Updates the size and pos of all controls
        void UpdateSize();	
        
        /// Processes the user interaction
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);	
        
        
    private:
        bool				m_bGestureComplete;
        iHarborDockView		*m_pDockView1, *m_pDockView2;
        iButton				*m_pExitBtn;
        private	:		
    };
}
