#pragma once

//////////////////////////////////////////////////////////////////////////
// MainMenu.h
//////////////////////////////////////////////////////////////////////////
// this file contains new main menu code (for iPad)
//////////////////////////////////////////////////////////////////////////

#include "GameViewInterface.h"


//////////////////////////////////////////////////////////////////////////

namespace UI_IPAD
{
    void ComposeBadge(iDib& dib, const iPoint& pt, const iStringT& txt);
    
    class iMainMenuView : public IMainMenuView
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // internal classes
        class iMenuBtn;
        class iMenuTab;
        class iMenuTab_Settings;
        class iMenuTab_Credits;
        class iMenuTab_DownloadMaps;
        class iMenuTab_SingleNew;
        class iMenuTab_SingleLoad;
        class iMenuTab_SingleSave;
        class iMenuTab_SingleTutorial;
        //////////////////////////////////////////////////////////////////////////
  
        iMainMenuView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, Type tp, bool fInGame, uint32 uid);
        virtual bool KeyUp(sint32 key);
        bool Process(fix32 t);
        void OnActivate(bool bActivate);
        void OnCompose();
    private:
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        void UpdateControls();
        void ReportResult(uint32 res);
        void FillControlsForType();
        
    private:
        iSimpleArray<iMenuBtn*>	m_buttons;
        iSimpleArray<iMenuTab*>	m_tabs;
        iSimpleArray<Result>	m_results;
        IViewCmdHandler*        m_pCmdHandler;
        sint32                  m_activebtn;
        Type                    m_type;
        bool                    m_fInGame;
        bool					m_bDoneUpdatingBadges;
#if defined( PC_VERSION )
    public:
        bool m_bLoad;
        bool m_bSave;
#endif
    };
    
    
    //////////////////////////////////////////////////////////////////////////
    
    
    
    class iNewMenuDlg : public INewMenuDlg
    {
    public:
        
        iNewMenuDlg(iViewMgr* pViewMgr);
        virtual bool KeyUp(sint32 key);
        void OnCompose();
        
        void SetType(iMainMenuView::Type tp, bool fInGame) { m_type = tp; m_in_game = fInGame; };
        bool Process(fix32 t);
        void UpdateSize();
        void Center();
    private:
        void ReportRes(uint32 res);
        void OnCreateDlg();
        void DoCompose(const iRect& rect);
        iSize ClientSize() const;
        
        void UpdateControls();
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        virtual iSize GetDlgMetrics() const;
        //inline void EndDialogDelayed(sint32 res) { m_delayedRes = res; }
    private:
        iMainMenuView*           m_ctrl;
        iMainMenuView::Type      m_type;
        bool m_in_game;
        //sint32	m_delayedRes;
    };
}
// END OF FILE
