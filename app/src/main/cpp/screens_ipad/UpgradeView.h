#pragma once

#include "GameViewInterface.h"

class iCreatDescView;
class iArmyExchange;
class iArmyOwner;

namespace UI_IPAD
{
    
    class iCreatUpgradeView: public iView, IViewCmdHandler
    {
    public:
        iCreatUpgradeView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rc, uint32 uid, iCreatGroup::UpgradeWay way);
        void SetCreatureGroup(iArmyOwner *pOwner, sint32 idx);
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        void UpdateSize();
        CREATURE_TYPE inline GetCT() { return m_idx == -1 ? CREAT_UNKNOWN : m_pArmyOwner->Army().At(m_idx).Type(); }
    private:
        iArmyOwner *m_pArmyOwner;
        sint32 m_idx;
        iImgBtn *m_pBtn;
        iCreatDescView *m_pDescView;
        iCreatGroup::UpgradeWay m_way;
        IViewCmdHandler *m_pHandler;
    };
    
    class iUpgradeView : public IUpgradeView
    {
    public:
        /// Default constuctor
        iUpgradeView();
        
        /// Default destructor
        ~iUpgradeView() {}
        
        void SetHero(iHero* pHero);
        
        /// Called when needs a redraw
        void DoCompose();
        
        /// Called when a user swipes mouse up
        void OnSwipeUp(sint32 length);
        
        void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        
        virtual inline bool AlwaysReceiveMouse() { return true; }
        
        void OnActivate(bool bActivate);
        
        virtual CHILD_VIEW Type() const { return CV_UPGRADE; }
    private:
        /// Updates the size and pos of all controls
        void UpdateSize();
        
        /// Processes the user interaction
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        
        virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); }
    private:
        static const iSize sm_defCentralSize;
        private	:	
        sint32 m_idx;
        bool				m_bGestureComplete;
        iHero				*m_pHero;
        iCreatUpgradeView	*m_pUpgView1, *m_pUpgView2;
        iArmyExchange		*m_pArmyExchange;
        iButton				*m_pExitBtn;
        
    };
    
}
