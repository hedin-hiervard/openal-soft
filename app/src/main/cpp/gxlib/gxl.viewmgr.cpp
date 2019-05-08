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
#include "gxl.dialog.h"
#include "gxl.topmostview.h"
#include "gxl.popupview.h"
#include "gxl.viewmgr.h"
#include "gxl.application.h"


iViewMgr::iViewMgr(iGXApp* pApp)
: m_pApp(pApp), m_Surf(pApp->Display().GetSurface()), m_pCurView(NULL), m_pCapView(NULL), m_pPopupView(NULL), m_timerCounter(0)
#if defined( PC_VERSION )
, m_CapRect( iRect() )
#endif
{}

iViewMgr::~iViewMgr()
{}

bool iViewMgr::ProcessMessage(const iInput::iEntry& msg)
{

	if (m_pPopupView) {
		// In case of popup view
		switch(msg.taskType) {
			case iInput::iEntry::MouseMove: if (m_pCapView) m_pCapView->MouseTrack(iPoint(msg.px,msg.py), msg.mID, msg.mbID); else m_pPopupView->MouseTrack(iPoint(msg.px,msg.py),msg.mID, msg.mbID); break;
			case iInput::iEntry::MouseDown: 
				if (m_pCapView) m_pCapView->MouseDown(iPoint(msg.px,msg.py), msg.mID, msg.mbID); 
				else {
					if (!m_pPopupView->GetRect().PtInRect(msg.px,msg.py)) {
						m_pPopupView->HidePopup();
						if (m_pCurView) m_pCurView->Invalidate();
					} else m_pPopupView->MouseDown(iPoint(msg.px,msg.py), msg.mID, msg.mbID); 
				}
				break;
			case iInput::iEntry::MouseUp: 
				if(m_pCurView && m_pCurView->AlwaysReceiveMouse())
					m_pCurView->MouseUp(iPoint(msg.px, msg.py), msg.mID, msg.mbID);
				if (m_pCapView) m_pCapView->MouseUp(iPoint(msg.px,msg.py), msg.mID, msg.mbID); 
				else {
					if (m_pPopupView->IsVisible()) m_pPopupView->MouseUp(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
					else HidePopup();
				}
				break;
			case iInput::iEntry::KeyDown: m_pApp->KeyDown(msg.key);	break;
			case iInput::iEntry::KeyUp: m_pApp->KeyUp(msg.key);	break;
		}
	} else if (m_dlgStack.GetSize()) {
		// In case of dialog stack
		iDialog* pDlg = m_dlgStack.GetLast();
		switch(msg.taskType) {
			case iInput::iEntry::MouseMove: 
				if (m_pCapView)  
				{
					m_pCapView->MouseTrack(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
					if(m_pCapView != pDlg && pDlg->AlwaysReceiveMouse())
						pDlg->MouseTrack(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
				} else {
					pDlg->MouseTrack(iPoint(msg.px,msg.py), msg.mID, msg.mbID); 
				}
				break;
			case iInput::iEntry::MouseDown: 
				if (m_pCapView)  
				{
					m_pCapView->MouseDown(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
					if(m_pCapView != pDlg && pDlg->AlwaysReceiveMouse())
						pDlg->MouseDown(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
				} else {
					pDlg->MouseDown(iPoint(msg.px,msg.py), msg.mID, msg.mbID); 
				}
				break;
			case iInput::iEntry::MouseUp: 
				if (m_pCapView) { 
					m_pCapView->MouseUp(iPoint(msg.px,msg.py), msg.mID, msg.mbID); 
					// this was commented out because it crashed the menu
					// if fasttapping on the first button twice after launch
					// hedin, 23.05.11
					//
					// uncommented to make Dlg_CreatInfo close on tap. presumably, it doesn't cause crashes
					if(m_pCapView != pDlg && pDlg->AlwaysReceiveMouse())
						pDlg->MouseUp(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
				} else 
					pDlg->MouseUp(iPoint(msg.px,msg.py), msg.mID, msg.mbID); 
				break;
			case iInput::iEntry::KeyDown: if (!pDlg->KeyDown(msg.key)) m_pApp->KeyDown(msg.key); break;
			case iInput::iEntry::KeyUp: if (!pDlg->KeyUp(msg.key)) m_pApp->KeyUp(msg.key); break;
			case iInput::iEntry::MouseWheelDown:
					pDlg->MouseWheelDown(msg.px, msg.mID); 
				break;
			case iInput::iEntry::MouseWheelUp:
					pDlg->MouseWheelUp(msg.px, msg.mID);
				break;
		}
	} else if (m_pCurView) {
		// In general case
		switch(msg.taskType) {
			case iInput::iEntry::MouseMove: 
				if (m_pCapView)  
				{
					m_pCapView->MouseTrack(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
					if(m_pCapView != m_pCurView && m_pCurView->AlwaysReceiveMouse())
						m_pCurView->MouseTrack(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
				} else {
					m_pCurView->MouseTrack(iPoint(msg.px,msg.py), msg.mID, msg.mbID); 
				}
				break;
			case iInput::iEntry::MouseDown: 
				if (m_pCapView)  
				{
					m_pCapView->MouseDown(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
					if(m_pCapView != m_pCurView && m_pCurView->AlwaysReceiveMouse())
						m_pCurView->MouseDown(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
				} else {
					m_pCurView->MouseDown(iPoint(msg.px,msg.py), msg.mID, msg.mbID); 
				}
				break;
			case iInput::iEntry::MouseUp: 
				if (m_pCapView) { 
					m_pCapView->MouseUp(iPoint(msg.px,msg.py), msg.mID, msg.mbID); 
					if(m_pCapView != m_pCurView && m_pCurView->AlwaysReceiveMouse())
						m_pCurView->MouseUp(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
				} else 
					m_pCurView->MouseUp(iPoint(msg.px,msg.py), msg.mID, msg.mbID); 
				break;
			case iInput::iEntry::KeyDown: 
				if (!m_pCurView->KeyDown(msg.key)) {
					m_pApp->KeyDown(msg.key);
				}
				break;
			case iInput::iEntry::KeyUp: 
				if (!m_pCurView->KeyUp(msg.key)) m_pApp->KeyUp(msg.key);
				break;
			case iInput::iEntry::MouseEntered:
				if (m_pCapView)  
				{
					m_pCapView->MouseEntered(iPoint(msg.px, msg.py), msg.mID);
					if(m_pCapView != m_pCurView && m_pCurView->AlwaysReceiveMouse())
						m_pCurView->MouseEntered(iPoint(msg.px, msg.py), msg.mID);
				} else {
					m_pCurView->MouseEntered(iPoint(msg.px, msg.py), msg.mID);
				}
				break;
			case iInput::iEntry::MouseLeave:
/*				
				if (m_pCapView)  
				{
					m_pCapView->MouseLeave(iPoint(msg.px, msg.py), msg.mID);
					if(m_pCapView != m_pCurView && m_pCurView->AlwaysReceiveMouse())
						m_pCurView->MouseLeave(iPoint(msg.px, msg.py), msg.mID);
				} else {
					m_pCurView->MouseLeave(iPoint(msg.px, msg.py), msg.mID);
				}
*/
				break;
            case iInput::iEntry::MouseWheelDown:
                    m_pCurView->MouseWheelDown(msg.px, msg.mID);
				break;
			case iInput::iEntry::MouseWheelUp:
                    m_pCurView->MouseWheelDown(msg.px, msg.mID);
                break;
            case iInput::iEntry::MouseRightDown:
                    m_pCurView->MouseRightDown(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
                break;
            case iInput::iEntry::MouseRightUp:
                    m_pCurView->MouseRightUp(iPoint(msg.px,msg.py), msg.mID, msg.mbID);
                break;
		}
	} else {
		// 
		switch(msg.taskType) {
			case iInput::iEntry::KeyDown: m_pApp->KeyDown(msg.key);	break;
			case iInput::iEntry::KeyUp: m_pApp->KeyUp(msg.key);	break;
		}
	}

	return true;
}

void iViewMgr::SetCurView(iTopmostView* pCurView)
{
	m_pCurView = pCurView;
}

void iViewMgr::SetViewCapture(iView* pCapView)
{
	//check(!m_pCapView);
	m_pCapView = pCapView;
}

iView* iViewMgr::ReleaseViewCapture()
{
	if(m_pCapView == 0)
		return NULL;
	iView* nView = m_pCapView;
	m_pCapView = NULL;
	return nView;
}
#if defined (PC_VERSION)
void iViewMgr::SetRectCapture( iRect rect ) { 
	
		m_CapRect = rect;
}

iRect iViewMgr::ReleaseRectCapture(){

	iRect rect = m_CapRect;
	m_CapRect = iRect();
	return rect;
}

void iViewMgr::SetEntCapture( iView* pEntView ){

	m_pCapEntView = pEntView;
}

iView* iViewMgr::ReleaseEntCapture(){

	iView* pView = m_pCapEntView;
	m_pCapEntView = NULL;
	return pView;	
}
#endif
// Modal stack
void iViewMgr::PushModalDlg(iDialog* pDlg)
{
	m_dlgStack.Push(pDlg);
}

iDialog* iViewMgr::PopModalDlg()
{
	ReleaseViewCapture();
	if (m_pCurView) m_pCurView->Invalidate();
	return m_dlgStack.Pop();
}

iDialog* iViewMgr::PeepDlg()
{
	return m_dlgStack.GetLast();
}


// Popup windows
void iViewMgr::TrackPopup(iPopupView* pPopupView, const iPoint& pos, const iRect& bound, Alignment al)
{
	check(!m_pPopupView && pPopupView);
	m_pPopupView = pPopupView;
	m_pPopupView->TrackPopup(pos, bound, al);
}

void iViewMgr::HidePopup()
{
	check(m_pPopupView);
	delete m_pPopupView;
	m_pPopupView = NULL;
	if (m_pCurView) m_pCurView->Invalidate();
}

void iViewMgr::Compose(iRect& rect)
{
	bool bRedraw = false;
	// Compose current topmost view
	if (!m_pCurView || (m_pCurView && m_pCurView->NeedRedraw())) { 
		bRedraw = true;
		if(m_pCurView)
			m_pCurView->Compose(rect);
	}

	// Compose last item of dialog stack
	/*for (uint32 xx=0; xx< m_dlgStack.GetSize(); ++xx) */
		if (bRedraw && m_dlgStack.GetSize() > 0)
			m_dlgStack[m_dlgStack.GetSize() - 1]->Compose(rect);

	// Compose Popup view
	if (m_pPopupView && bRedraw) 
			m_pPopupView->Compose(rect);

	// Compose drag'n'drop glyph
	if (m_pDragGlyph && bRedraw) m_pDragGlyph->ComposeDragGlyph();
}

// Timer processing
void iViewMgr::Process(uint32 interval, fix32 t)
{
	if (m_timers.GetSize()) {
		m_timerCounter += interval;
		while (m_timers.GetSize() && m_timers.GetLast().timer <= m_timerCounter) {
			iTimerHandler item = m_timers.Pop();	
			item.pHandler->OnTimer(item.tid);
		}
	}
	if (m_dlgStack.GetSize()) {
		// In case of dialog stack
		iDialog* pDlg = m_dlgStack.GetLast();
		pDlg->Process(t);
	}
}

void iViewMgr::SetTimer(uint32 period, uint32 tid, iView* pHandler)
{
	if (!m_timers.GetSize()) {
		// reset timer counter
		m_timerCounter = 0;
		m_timers.Add(iTimerHandler(period, tid, pHandler));
	} else if (m_timers.GetLast().timer >= (period+m_timerCounter) ) {
		m_timers.Add(iTimerHandler(period+m_timerCounter, tid, pHandler));
	} else {
		uint32 ntimer = period+m_timerCounter;
		for (uint32 xx=0; xx<m_timers.GetSize(); ++xx) {
			if (ntimer >= m_timers[xx].timer) {
				m_timers.InsertBefore(xx, iTimerHandler(ntimer, tid, pHandler));
				break;
			}
		}
	}
}


void iViewMgr::CleanupTimers(iView* pHandler)
{
	if (!pHandler) m_timers.RemoveAll();
	else {
		iTimerList tmp;
		
		for (uint32 xx=0; xx<m_timers.GetSize(); xx++){
			if (m_timers[xx].pHandler != pHandler) 
				tmp.Add(m_timers.At(xx));
		}
		
		m_timers = tmp;
	}
}

void iViewMgr::UpdateDlgSizes()
{ 	
    for (uint32 xx=0; xx< m_dlgStack.GetSize(); ++xx) m_dlgStack[xx]->UpdateSize();
}
