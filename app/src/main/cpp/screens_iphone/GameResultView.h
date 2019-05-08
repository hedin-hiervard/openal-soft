#pragma once

#include "GameViewInterface.h"

namespace UI_IPHONE
{
    
    class iGameResultView : public IGameResultView
    {
    public:
        /// Default constuctor
        iGameResultView();
        
        /// Default destructor
        ~iGameResultView() {}
        
        /// Called when needs a redraw
        void OnCompose();
        
        /// Called when a user swipes mouse up
        void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        
        virtual CHILD_VIEW Type() const { return CV_GAMERESULT; }
        
        virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); }        
        
        void SetTitle(iStringT title) { m_title = title; }
        void SetText(iStringT text) { m_text = text; }
        void SetType(GRType type) { m_type = type; }
        void SetMapName(iStringT mapName) { m_mapName = mapName; }
        
        void OnActivate(bool bActivate);
    private:
        /// Updates the size and pos of all controls
        void UpdateSize();
        
        /// Processes the user interaction
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        private	:	
        iStringT m_text;
        iStringT m_title;
        sint16	m_textGrad;
        GRType	m_type;
        iStringT m_mapName;
        
        iTwitterPane* m_pTPane;
        bool		m_bTweet;
    };
    
}
