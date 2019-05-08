#include "stdafx.h"
#include "CastleView.h"
#include "HarborView.h"
#include "HarborView.stylus.h"
#include "Ctl_MarketPage.h"
#include "Ctl_MarketPage.stylus.h"
#include "Ctl_DockPage.h"
#include "Ctl_DockPage.stylus.h"
#include "OverlandView.h"
#include "OverlandView.stylus.h"

iHarborView_stylus::iHarborView_stylus()
: iHarborView()
{
	AddChild(m_pChilds[0] = new iDockTab_stylus(&gApp.ViewMgr()));   // 0    
    AddChild(new iInfoBar(&gApp.ViewMgr(), this, iRect(0, 0, m_Rect.w, 15))); // 2
	AddChild(m_pChilds[1] = new iMarketTab_stylus(&gApp.ViewMgr(), true)); // 1    
}

bool iHarborView_stylus::OnKeyDown(sint32 key)
{
    if (key == KEY_LEFT){
        PrevTab();
    } else if (key == KEY_RIGHT){
        NextTab();
    } else {
        return false;
    }
    return true;
}
