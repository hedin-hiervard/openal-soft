#include "stdafx.h"
#include "CastleView.h"
#include "Ctl_DockPage.h"
#include "Ctl_DockPage.stylus.h"

iDockView_stylus::iDockView_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 dt)
: iDockView(pViewMgr, pCmdHandler, rect, uid, dt)
{		
	// Build ship and Occupy ship buttons
	AddChild(m_pBtn = new iTextButton(m_pMgr, this, iRect((rect.w - 80) / 2, rect.h - 2 * DEF_BTN_HEIGHT, 80, DEF_BTN_HEIGHT), TRID_BTN_BUILD, 102));        	
	SwitchState(DS_CANT);
}

iDockTab_stylus::iDockTab_stylus(iViewMgr* pViewMgr)
: iDockTab(pViewMgr, iRect(0, 15,gApp.Surface().GetWidth(), gApp.Surface().GetHeight() - 36))
{
	uint32 hdx = m_Rect.w / 2;		
	AddChild( m_pLDockView = new iDockView_stylus(pViewMgr, this, iRect(5, 5, hdx - 10, m_Rect.h - 10), 100, 0));
	AddChild( m_pRDockView = new iDockView_stylus(pViewMgr, this, iRect(hdx + 5, 5, hdx - 10, m_Rect.h - 10), 101, 1));
}
