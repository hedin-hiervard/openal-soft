#include "gxl.inc.h"
#ifdef OS_WINCE
//#include <gx.h>
#include "gapi_help.h"
#endif //OS_WINCE
#include "gxl.ctr.cbuff.h"
#include "gxl.ctr.array.h"
#include "gxl.window.h"
#include "gxl.input.h"
#include "gxl.timer.h"
#include "gxl.display.h"
#include "gxl.view.h"
#include "gxl.stdctrl.h"
#include "gxl.dialog.h"
#include "gxl.viewmgr.h"
#include "gxl.application.h"

/*
 *	Base view-port based control
 */
iBaseCtrl::iBaseCtrl(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 clsId, uint32 uid, uint32 state)
: iView(pViewMgr, rect, clsId, uid, state ), m_pCmdHandler(pCmdHandler) {}

void iBaseCtrl::SetCommandHandler(IViewCmdHandler* pCmdHandler)
{
	m_pCmdHandler = pCmdHandler;
}

/*
 *	Generic push putton
 */

bool IsOneHanded();

iButton::iButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 state)
: iBaseCtrl(pViewMgr, pCmdHandler, rect, PUSH_BUTTON, uid, state), m_btnLastClick(0), m_state(0), m_bHoldable(false)
#if defined( PC_VERSION )	
, m_bEntered(false)
#endif
{}

void iButton::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if(mID != 0 && m_bHoldable) { 
		m_pParent->OnMouseDown(pos, mID, mbID); return; 
	}
	
	if(m_bHoldable) {
		if(IsOneHanded() || m_bSwitch) {
			if(m_state & Pressed) {
				m_state ^= Pressed;		
				m_pCmdHandler->iCMDH_ControlCommand(this,CCI_RELEASE,0);
			} else {
				m_state |= Pressed;
				m_pCmdHandler->iCMDH_ControlCommand(this,CCI_HOLD,0);
			}
		} else {
			m_state |= Pressed;
			m_pCmdHandler->iCMDH_ControlCommand(this,CCI_HOLD,0);			
		}
	} else {
		m_state |= Pressed;
	}

	OnBtnDown();
	Invalidate();
}

void iButton::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if(mID != 0 && m_bHoldable) { 
		m_pParent->OnMouseUp(pos, mID, mbID); return; 
	}

	if(m_bHoldable) {
		if(!(IsOneHanded() || m_bSwitch)) {
			if (m_state & Pressed) {
				m_state ^= Pressed;
				OnBtnUp();
				Invalidate();
			}
			m_pCmdHandler->iCMDH_ControlCommand(this,CCI_RELEASE,0);
		}
	} else {
		if (m_state & Pressed) {
			m_state ^= Pressed;
			OnBtnUp();
			Invalidate();
		}
	}	
#if defined( PC_VERSION )	
	m_bEntered = false;
#endif
}

void iButton::Unhighlight() 
{
	if(m_bHoldable && !(IsOneHanded() || m_bSwitch)) {
		if(m_state & Pressed) {
			m_state ^= Pressed;
		}
		m_TrackAnchor[0] = cInvalidPoint;
	}
}

void iButton::Release()
{
	if(m_state & Pressed)
		m_state ^= Pressed;
}

void iButton::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if(mID != 0 && m_bHoldable) { m_pParent->OnMouseClick(pos, mID, mbID); return; }
	 

	if (m_pCmdHandler && IsEnabled()) {
		uint32 nt = GetTickCount();
		if ( m_btnLastClick && nt > m_btnLastClick && (nt-m_btnLastClick) < 500) {
			m_btnLastClick = 0;
			m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNDBLCLICK,0);
		} else {
			m_btnLastClick = nt;
			m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK,0);
		}
	}
}

void iButton::OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if(mID != 0 && m_bHoldable) { 
		if(m_state & Pressed)
			m_pParent->OnMouseTrack(pos, mID, mbID); 
		return; 
	}

//	if (GetScrRect().PtInRect(pos)) {
//		if ( (m_state & Pressed) != Pressed) {
//			m_state |= Pressed;
//			if(m_bHoldable)
//				m_pCmdHandler->iCMDH_ControlCommand(this,CCI_HOLD,0);
//			Invalidate();
//		}
//	} else if (m_state & Pressed) {
//		m_state ^= Pressed;
//		if(m_bHoldable)
//			m_pCmdHandler->iCMDH_ControlCommand(this,CCI_RELEASE,0);
//		 
//		Invalidate();
//	}
}

/*
 * Tabbed switch control	
 */
iTabbedSwitch::iTabbedSwitch(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 tabcnt, uint32 uid, uint32 state)
: iBaseCtrl(pViewMgr, pCmdHandler, rect, TABBED_SWITCH, uid, state)
, m_TabsCount(tabcnt), m_CurTab(0), m_FocTab(-1), m_CurFocTab(-1)
{
	m_tabStates = new uint32[tabcnt]; for (uint32 xx=0; xx<tabcnt; ++xx) m_tabStates[xx] = 1;
	m_ItemWidth = m_Rect.w / m_TabsCount;
	check(m_ItemWidth);
	check((rect.w%m_TabsCount) == 0);
}

iTabbedSwitch::~iTabbedSwitch()
{
	delete[] m_tabStates;
}


void iTabbedSwitch::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	sint32 ntab = GetTabByPos(pos-GetScrRect().point());
	if (ntab >= 0 && ntab < m_TabsCount && m_tabStates[ntab]) m_CurFocTab = m_FocTab = ntab;
	Invalidate();
}

void iTabbedSwitch::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	sint32 tab = GetTabByPos(pos-GetScrRect().point());
	if (tab == m_FocTab){
		if (m_CurTab == m_FocTab) {
			if (m_pCmdHandler && IsEnabled()) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK,m_CurTab);
		} else {
			m_CurTab = m_FocTab;
			if (m_pCmdHandler && IsEnabled()) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_TABCHANGED, m_CurTab);
		}
		Invalidate();
	}
	m_CurFocTab = m_FocTab = -1;
}

void iTabbedSwitch::OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	sint32 tab = GetTabByPos(pos-GetScrRect().point());
	if (tab != m_CurFocTab) Invalidate();
	if (tab == m_FocTab) m_CurFocTab = tab;
	else m_CurFocTab = -1;
}

void iTabbedSwitch::OnCompose()
{
	iRect rc = GetScrRect();
	rc.w = m_ItemWidth;
	for (sint32 xx=0; xx<m_TabsCount; ++xx) {
		uint32 state=0;
		if (xx==GetFocusedTab()) state |= iButton::Pressed;
		if (xx==GetCurrentTab()) state |= iButton::Selected;
		if (!m_tabStates[xx]) state |= iButton::Disabled;
		ComposeTabItem(xx, state, rc);
		rc.x += m_ItemWidth;
	}
}

/*
 *	Scroll Bar
 */
iScrollBar::iTBButton::iTBButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 state, iScrollBar* pScrollBar, Element el)
: iButton(pViewMgr, pCmdHandler, rect, uid, state), m_pScrollBar(pScrollBar), m_el(el) {}

void iScrollBar::iTBButton::OnCompose()
{
	m_pScrollBar->ComposeSBElement(m_el, GetScrRect(), GetButtonState());
}

//
iScrollBar::iScrollBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 flags)
: iBaseCtrl(pViewMgr, pCmdHandler, rect, SCROLL_BAR, uid, Visible|Enabled)
, m_flags(flags), m_totSiz(0), m_pagSiz(0), m_curPos(0), m_bThumbTrack(false)
{
	if (IsHorizontal()) {
		AddChild(m_pUp = new iTBButton(pViewMgr,this,iRect(0,0,GetDefBtnSiz(),rect.h),uid+1, Visible | Enabled, this, EL_BtnLeft));
		AddChild(m_pDown = new iTBButton(pViewMgr,this,iRect(rect.w-GetDefBtnSiz(),0,GetDefBtnSiz(),rect.h),uid+2, Visible | Enabled, this, EL_BtnRight));
	} else {
		AddChild(m_pUp = new iTBButton(pViewMgr,this,iRect(0,0,rect.w,GetDefBtnSiz()),uid+1, Visible | Enabled, this, EL_BtnUp));
		AddChild(m_pDown = new iTBButton(pViewMgr,this,iRect(0,rect.h-GetDefBtnSiz(),rect.w,GetDefBtnSiz()),uid+2, Visible | Enabled, this, EL_BtnDown));
	}
	CalcThumbMetrix();
}

void iScrollBar::SetMetrics(sint32 totSiz, sint32 pagSiz)
{
	m_totSiz = totSiz;
	m_pagSiz = pagSiz;
	m_curPos = 0;
	CalcThumbMetrix();
}

bool iScrollBar::SetCurPos(sint32 nVal)
{
	nVal = iCLAMP<sint32>(0,iMAX<sint32>(0,m_totSiz-m_pagSiz),nVal);
	if (!m_totSiz || nVal==m_curPos) return false;
	m_curPos = nVal;
	CalcThumbMetrix();
	return true;
}
	
bool iScrollBar::PageUp() 
{ 
	if (SetCurPos(m_curPos-m_pagSiz)) {
		if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_SBPAGEUP,m_curPos);
		Invalidate();
		return true;
	}
	return false;
}

bool iScrollBar::PageDown() 
{ 
	if (SetCurPos(m_curPos+m_pagSiz)) {
		if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_SBPAGEDOWN,m_curPos);
		Invalidate();
		return true;
	}
	return false;
}

void iScrollBar::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	HitTestRes res = HitTest(pos - GetScrRect().point());
	
	if (res == HTR_Thumb) {
		m_bThumbTrack = true;
		m_trackAnchor = pos;
		m_trackPos = m_curPos;
		Invalidate();
	} else if (res == HTR_PgUp){
		if (PageUp()) Invalidate();
	} else if (res == HTR_PgDown){
		if (PageDown()) Invalidate();
	}
}

void iScrollBar::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	m_bThumbTrack = false;
	Invalidate();
}

void iScrollBar::OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if (m_bThumbTrack) {
		sint32 n;
		if (IsHorizontal()) n = pos.x - m_trackAnchor.x;
		else n = pos.y - m_trackAnchor.y;
		if (SetCurPos(m_trackPos + NItems(n))) {
			if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_SBTRACKING,m_curPos);
			Invalidate();
		}
	}
}

void iScrollBar::OnCompose()
{
	iRect rc(GetScrRect());
	if (IsHorizontal()){
		ComposeSBElement(EL_Bkgnd, iRect(rc.x+m_thumbMetrix.es,rc.y,m_thumbMetrix.h,rc.h), 0);
		ComposeSBElement(EL_Thumb, iRect(rc.x+m_thumbMetrix.es+m_thumbMetrix.s1,rc.y,m_thumbMetrix.th,rc.h), (m_bEnabled?0:iButton::Disabled) | (m_bThumbTrack?iButton::Pressed:0));
	} else {
		ComposeSBElement(EL_Bkgnd, iRect(rc.x,rc.y+m_thumbMetrix.es,rc.w,m_thumbMetrix.h), 0);
		ComposeSBElement(EL_Thumb, iRect(rc.x,rc.y+m_thumbMetrix.es+m_thumbMetrix.s1,rc.w,m_thumbMetrix.th), (m_bEnabled?0:iButton::Disabled) | (m_bThumbTrack?iButton::Pressed:0));
	}
}

iScrollBar::HitTestRes iScrollBar::HitTest(const iPoint& pos)
{
	sint32 val;
	if (IsHorizontal()) val = pos.x - m_thumbMetrix.es;
	else val = pos.y - m_thumbMetrix.es;
	if (val < m_thumbMetrix.s1) return HTR_PgUp;
	val -= m_thumbMetrix.s1;
	if (val < m_thumbMetrix.th) return HTR_Thumb;
	return HTR_PgDown;
}
	
void iScrollBar::CalcThumbMetrix()
{
	if (IsHorizontal()){
		m_thumbMetrix.es = GetDefBtnSiz();
		m_thumbMetrix.h = ((sint32)m_Rect.w) - m_thumbMetrix.es*2;
	} else {
		m_thumbMetrix.es = GetDefBtnSiz();
		m_thumbMetrix.h = ((sint32)m_Rect.h) - m_thumbMetrix.es*2;
	}

	if (m_totSiz == 0) {
		m_thumbMetrix.th = m_thumbMetrix.h;
		m_thumbMetrix.s1 = m_thumbMetrix.s2 = 0; 
	} else {
		m_thumbMetrix.th	= iMAX<sint32>(ItemsHeight(iMIN(m_pagSiz,m_totSiz)),m_thumbMetrix.es);
		if (!m_curPos) m_thumbMetrix.s1 = 0;
		else {
			sint32 fpx = m_thumbMetrix.h - m_thumbMetrix.th;
			m_thumbMetrix.s1 = (fpx*m_curPos)/(m_totSiz-m_pagSiz);
		}
		m_thumbMetrix.s2 = m_thumbMetrix.h - m_thumbMetrix.th - m_thumbMetrix.s1;
	}
}

void iScrollBar::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == m_UID+1) {
		// Scroll up
		if (SetCurPos(m_curPos-1)) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_SBLINEUP,m_curPos);
	} else if (uid == m_UID+2) {
		// Scroll down
		if (SetCurPos(m_curPos+1)) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_SBLINEDOWN, m_curPos);
	}
}

/*
 *	List Box (virtual)
 */
iListBox::iListBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
: iBaseCtrl(pViewMgr, pCmdHandler, rect, LIST_BOX, uid, Visible|Enabled)
, m_selItem(-1)
, m_initialPos(0)
, m_scrollPos(0)
, m_velocity(0)
, m_timer(fix32::zero)
, m_holdTimer(fix32::zero)
{}

void iListBox::UpdateListBox()
{
	if (LBItemsCount()) m_selItem = 0;
	else m_selItem = -1;
	if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_LBSELCHANGED, m_selItem);
	Invalidate();
}

void iListBox::SetCurSel(sint32 idx, bool bNotify)
{
	check(idx == -1 || IsValidIdx(idx));
	if (idx != m_selItem) {
		m_selItem = idx;
		if (bNotify && m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_LBSELCHANGED, m_selItem);
		/*if (idx != -1) {
			if (m_selItem < m_scrVal) {
				if (m_pScrollBar) m_pScrollBar->SetCurPos(m_selItem);
				SetCurPos(m_selItem);
			} else if (m_selItem > (m_scrVal+(sint32)PageSize()-1) ) {
				sint32 npos = m_selItem-PageSize()+1;
				if (m_pScrollBar) m_pScrollBar->SetCurPos(npos);
				SetCurPos(npos);
			}
		}*/
		Invalidate();
	}
}

void iListBox::SelPrev()
{
	if (m_selItem > 0) SetCurSel(m_selItem - 1, true);
}

void iListBox::SelNext()
{
	if ((m_selItem+1) < LBItemsCount()) SetCurSel(m_selItem + 1, true);
}

void iListBox::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	SetCurPos(param);
}

void iListBox::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	m_initialPos = m_scrollPos;
	m_velocity = 0;
	m_holdTimer = fix32::zero;
}

void iListBox::SetScrollPos(sint32 newpos)
{
	iRect rc = GetScrRect();
#ifdef OS_IPHONE
    m_scrollPos = iCLAMP<sint32>(-(sint32)rc.h/2, (LBItemsCount()) * LBItemHeight(), newpos);
#else
	m_scrollPos = iCLAMP<sint32>(0, (LBItemsCount()) * LBItemHeight() - GetScrRect().h, newpos);
#endif
}

void iListBox::OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	SetCurSel(-1, true);
	if(m_TrackAnchor[0] != cInvalidPoint) {
		//SetCurPos(iCLAMP<sint32>(0, iMAX<sint32>(0, LBItemsCount() - PageSize()), m_trackScrVal + (sint32)(m_BeginningTrackAnchor[0].y - m_TrackAnchor[0].y) / (sint32)LBItemHeight() ));
		/*m_composeOffset = m_initialPos + (sint32)(-m_BeginningTrackAnchor[0].y + m_TrackAnchor[0].y);
		m_velocity = -m_BeginningTrackAnchor[0].y + m_TrackAnchor[0].y;
		if(m_scrVal == 0) 
			m_composeOffset = iMIN<sint32>(0, m_composeOffset);
		if(m_scrVal == iMAX<sint32>(0, LBItemsCount() - PageSize())) 
			m_composeOffset = iMAX<sint32>(0, m_composeOffset);
		if(m_composeOffset > (sint32)LBItemHeight()) {
			SetCurPos(iCLAMP<sint32>(0, iMAX<sint32>(0, LBItemsCount() - PageSize()), m_scrVal - 1));
			m_composeOffset = 0;
			m_initialPos = m_BeginningTrackAnchor[0].y - m_TrackAnchor[0].y;
		}
		else if(m_composeOffset < -(sint32)LBItemHeight()) {
			SetCurPos(iCLAMP<sint32>(0, iMAX<sint32>(0, LBItemsCount() - PageSize()), m_scrVal + 1));
			m_composeOffset = 0;
			m_initialPos = m_BeginningTrackAnchor[0].y - m_TrackAnchor[0].y;
		}

		//SetCurSel(-1, false);
		*/
		SetScrollPos(m_initialPos - (sint32)(-m_BeginningTrackAnchor[0].y + m_TrackAnchor[0].y));		
		m_velocity = m_BeginningTrackAnchor[0].y - m_TrackAnchor[0].y;

		Invalidate();
	}
}

void iListBox::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	SetCurSel(-1, true);
}

void iListBox::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if (m_pCmdHandler && mID == 0 && pos.GetDelta(m_BeginningTrackAnchor[mID]) < 10) {
		sint32 nIdx = IdxByPos(pos);
		if (!IsValidIdx(nIdx)) nIdx = -1;
		SetCurSel(nIdx, true);
		m_pCmdHandler->iCMDH_ControlCommand(this,CCI_LBSELECTED,m_selItem);	
		SetCurSel(-1, true);
	}
}

void iListBox::OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if (m_pCmdHandler) {
		m_pCmdHandler->iCMDH_ControlCommand(this,CCI_LBSELDBLCLICK,m_selItem);	
	}
}

void iListBox::Process(fix32 t)
{
	m_timer += t;
	m_holdTimer += t;
	
	if(m_holdTimer >= fix32(0.1f)) {
		m_holdTimer = fix32::zero;
		if(m_TrackAnchor[0] != cInvalidPoint &&
		   m_TrackAnchor[0].GetDelta(m_BeginningTrackAnchor[0]) < 10) {
			sint32 nIdx = IdxByPos(m_TrackAnchor[0]);
			if (!IsValidIdx(nIdx)) nIdx = -1;
			if(nIdx != m_selItem)
				m_lastClick = -1;
			SetCurSel(nIdx, true);
		}
	}
	
	if(m_timer >= fix32(0.05f)) {
		m_timer = fix32::zero;

		m_velocity /= 2;//-= iSIGN(m_velocity);

		sint32 oldScroll = m_scrollPos;
		sint32 fh = iMAX<sint32>(0, LBItemsCount() * LBItemHeight() - GetScrRect().h);
		if(m_scrollPos < 0)	{		
			m_velocity += -m_scrollPos;			
		} else if(m_scrollPos > fh)
			m_velocity -= (m_scrollPos - fh);

		
		if(!IsMouseDown(0)) {
			SetScrollPos((m_scrollPos + m_velocity));		
			Invalidate();
		}
		if(oldScroll < 0 && m_scrollPos >= 0) {
			m_velocity = 0;
			m_scrollPos = 0;
		} else if(oldScroll > fh && m_scrollPos <= fh) {
			m_velocity = 0;
			m_scrollPos = fh;
		}
	}

	

	

	/*
	if(!IsMouseDown(0)) {
		m_composeOffset += (m_velocity * t).ceil();
		if(m_scrVal == 0) 
			m_composeOffset = iMIN<sint32>(0, m_composeOffset);
		if(m_scrVal == iMAX<sint32>(0, LBItemsCount() - PageSize())) 
			m_composeOffset = iMAX<sint32>(0, m_composeOffset);
		if(m_composeOffset > (sint32)LBItemHeight()) {
			SetCurPos(iCLAMP<sint32>(0, iMAX<sint32>(0, LBItemsCount() - PageSize()), m_scrVal - 1));
			m_composeOffset = 0;
		}
		else if(m_composeOffset < -(sint32)LBItemHeight()) {
			SetCurPos(iCLAMP<sint32>(0, iMAX<sint32>(0, LBItemsCount() - PageSize()), m_scrVal + 1));
			m_composeOffset = 0;
		}			

	}	
	*/
}


void iListBox::OnCompose()
{
	iRect rc=GetScrRect();
	ComposeLBBackground(rc);
	sint32 ih = LBItemHeight();
	sint32 ic = LBItemsCount();
	sint32 ps = PageSize();
	iRect irc(rc.x, rc.y + LBTopOffset(), rc.w, ih);
	
	
	for (sint32 xx=TopIdx(m_scrollPos); xx<TopIdx(m_scrollPos + rc.h); ++xx){
		ComposeLBItem(xx,xx==m_selItem,irc);
		irc.y += ih;
	}


	/*if(m_composeOffset < 0)
		irc.y += ih;
	for (sint32 xx=m_scrVal + (m_composeOffset < 0 ? 1 : 0); xx<iMIN(ic,m_scrVal+ps - (m_composeOffset > 0 ? 1 : 0)); ++xx){
		ComposeLBItem(xx,xx==m_selItem,irc);
		irc.y += ih;
	}
	
	if(m_scrVal > 0 || (m_scrVal == 0 && m_composeOffset != 0)) 
		ComposeUpArrow();
	if(m_scrVal + ps < ic || (m_scrVal + ps == ic && m_composeOffset != 0))
		ComposeDownArrow();
		*/
}

sint32 iListBox::IdxByPos(const iPoint& pos)
{	
	sint32 offs = 0;
#ifdef PC_VERSION
    if( LBItemsCount() > 5 ){
#else
        if( LBItemsCount() > 3 ){
#endif
	sint32 fl = LBItemHeight() * LBItemsCount();
	if(m_scrollPos > 0 && m_scrollPos < LBItemsCount() * LBItemHeight())
		offs = (m_scrollPos % LBItemHeight());
	else {
		if(m_scrollPos < 0)
			offs = m_scrollPos;
		else if(m_scrollPos >= fl)
			offs = (m_scrollPos - fl + LBItemHeight());
	}
    
    }
   return iCLAMP<sint32>((sint32)0, (sint32)(LBItemsCount() - 1), (sint32)(TopIdx(m_scrollPos) + (pos.y + offs - GetScrRect().y - LBTopOffset()) / (sint32)LBItemHeight()));
	//return m_scrVal + (pos.y-GetScrRect().y-LBTopOffset() - m_composeOffset)/LBItemHeight();
}

sint32 iListBox::TopIdx(sint32 scroll)
{
	return iCLAMP<sint32>(0, (sint32)LBItemsCount() - 1, scroll / (sint32)LBItemHeight());
}

void iListBox::SetCurPos( sint32 sval )
{
	m_scrollPos = sval * LBItemHeight();
	Invalidate();
}


//////////////////////////////////////////////////////////////////////////
// new iListBoxEx with smooth scroll
//////////////////////////////////////////////////////////////////////////

// Cotulla: TEMP HERE: move later.
/*
*	List Box Ex (virtual)
*   with smooth scroll
*/


void iListBoxEx::OnCompose()
{
	iRect rc = GetScrRect();

    ComposeLBBackground(rc);

	if(!LBItemsCount()) return;

    sint32 ih = LBItemHeight();
    sint32 ic = LBItemsCount();
    sint32 ps = PageSize();
	sint32 fl = LBItemHeight() * LBItemsCount();
    iRect irc(rc.x, rc.y + LBTopOffset(), rc.w, ih);
    sint32 start, end, full = 0, extra = 0;

    bool part_draw_begin = false;
    bool part_draw_end = false;

    // scroll down: m_composeOffset < 0, m_scrVal = start
    // scroll up:   m_composeOffset > 0, m_scrVal = start + 1

	if(m_scrollPos > 0 && m_scrollPos < LBItemsCount() * LBItemHeight())
		irc.y -= (m_scrollPos % LBItemHeight());
#if defined( OS_IPHONE )
	else {
		if(m_scrollPos < 0)
			irc.y -= m_scrollPos;
		else if(m_scrollPos >= fl)
			irc.y -= (m_scrollPos - fl/2);
	}
#endif
	iDib tmpdib;	
	tmpdib.Init(iSize(m_Rect.w, ih), iDib::RGB);
	

	for (sint32 xx = TopIdx(m_scrollPos); xx < TopIdx(m_scrollPos + rc.h) + 1; ++xx)
	{
		if(rc.y - irc.y > ih) continue;
		if(irc.y2() - rc.y2() > ih) continue;

		if(IsValidIdx(xx)) {					
			
			iRect src_rc(0, 0, irc.w, irc.h);
			iPoint dst_pt(irc.x, irc.y);

			if(irc.y < rc.y) {
				src_rc.DeflateRect(0, rc.y - irc.y, 0, 0);
				dst_pt.y += rc.y - irc.y;
			} else if(irc.y2() > rc.y2()) {
				src_rc.DeflateRect(0, 0, 0, irc.y2() - rc.y2());
				//dst_pt.y += (irc.y2() - rc.y2());
			}
			m_dib.CopyRectToDibXY(tmpdib, iRect(dst_pt, tmpdib.GetSize()), iPoint());
			ExComposeLBItem(tmpdib, xx, xx == m_selItem, iRect(0, 0, irc.w, ih));	

			tmpdib.CopyRectToDibXY(m_dib, src_rc, dst_pt);

			irc.y += ih;
		}
	}

	/*{
		iDib tmpdib;
		tmpdib.Init(iSize(m_Rect.w, ih), iDib::RGB);
		iRect rect(0, 0, m_Rect.w, ih);
		iRect src_rc(0, -m_composeOffset, m_Rect.w, ih + m_composeOffset);
		iPoint dst_pt(rc.x, rc.y);
		ExComposeLBItem(tmpdib, m_scrVal, m_scrVal == m_selItem, rect);
		tmpdib.CopyRectToDibXY(m_dib, src_rc, dst_pt);
		tmpdib.Cleanup();

		extra = (m_Rect.h - (ih + m_composeOffset)) % LBItemHeight();
		full = (m_Rect.h - (ih + m_composeOffset)) / LBItemHeight();

		start = m_scrVal + 1;
		end = iMIN<sint32>(start + full, ic);
		irc.y += ih + m_composeOffset;
	}*/

  /*  if (m_composeOffset < 0)
    {
        iDib tmpdib;
        tmpdib.Init(iSize(m_Rect.w, ih), iDib::RGB);
        iRect rect(0, 0, m_Rect.w, ih);
        iRect src_rc(0, -m_composeOffset, m_Rect.w, ih + m_composeOffset);
        iPoint dst_pt(rc.x, rc.y);
        ExComposeLBItem(tmpdib, m_scrVal, m_scrVal == m_selItem, rect);
        tmpdib.CopyRectToDibXY(m_dib, src_rc, dst_pt);
        tmpdib.Cleanup();

        extra = (m_Rect.h - (ih + m_composeOffset)) % LBItemHeight();
        full = (m_Rect.h - (ih + m_composeOffset)) / LBItemHeight();

        start = m_scrVal + 1;
        end = iMIN<sint32>(start + full, ic);
        irc.y += ih + m_composeOffset;
    }
    else if (m_composeOffset > 0)
    {
        iDib tmpdib;
        tmpdib.Init(iSize(m_Rect.w, ih), iDib::RGB);
        iRect rect(0, 0, m_Rect.w, ih);
        iRect src_rc(0, ih - m_composeOffset, m_Rect.w, m_composeOffset);
        iPoint dst_pt(rc.x, rc.y);
        ExComposeLBItem(tmpdib, (m_scrVal - 1), (m_scrVal - 1) == m_selItem, rect);
        tmpdib.CopyRectToDibXY(m_dib, src_rc, dst_pt);
        tmpdib.Cleanup();

        extra = (m_Rect.h - m_composeOffset) % LBItemHeight();
        full = (m_Rect.h - m_composeOffset) / LBItemHeight();

        start = m_scrVal;
        end = iMIN<sint32>(start + full, ic);
        irc.y += m_composeOffset;
    }
    else
    {
        extra = m_Rect.h % LBItemHeight();
        full = m_Rect.h / LBItemHeight();

        start = m_scrVal;
        end = iMIN<sint32>(start + full, ic);
    }

    for (sint32 xx = start; xx < end; ++xx)
    {
        ExComposeLBItem(m_dib, xx, xx == m_selItem, irc);
        irc.y += ih;
    }

    // check if we need draw last item in box
    if (extra && end < ic)
    {
        iDib tmpdib;
        tmpdib.Init(iSize(m_Rect.w, ih), iDib::RGB);
        iRect rect(0, 0, m_Rect.w, ih);
        iRect src_rc(0, 0, m_Rect.w, extra);
        iPoint dst_pt(irc);//rc.x, rc.y);
        ExComposeLBItem(tmpdib, end, end == m_selItem, rect);
        tmpdib.CopyRectToDibXY(m_dib, src_rc, dst_pt);
        tmpdib.Cleanup();
    }
	*/
}




// END OF FILE
