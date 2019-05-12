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
#include "gxl.viewmgr.h"
#include "gxl.application.h"

iView::iView(iViewMgr* pViewMgr, const iRect& rect, uint32 clsId, uint32 uid, uint32 state)
: m_bNeedRedraw(true),  m_pParent(NULL)
#if defined(OS_APPLE) || defined(OS_ANDROID)
, m_lastClick(-1), m_bIsDoubleClicking(false)
#endif
{
	for(uint32 x=0; x<MAX_MICE_COUNT; x++) {
		m_bTracking[x] = false;
	
		m_BeginningTrackAnchor[x] = cInvalidPoint;
		m_TrackAnchor[x] = cInvalidPoint;
		m_EndTrackAnchor[x] = cInvalidPoint;
	}

	CreateView(pViewMgr,rect,clsId, uid, state); 
}

iView::~iView()
{ 
	Cleanup(); 
}

bool iView::CreateView(iViewMgr* pViewMgr, const iRect& rect, uint32 clsId, uint32 uid, uint32 state)
{
	if (!pViewMgr) return false;
	m_pMgr = pViewMgr;
	m_bVisible = (state & Visible)>0;
	m_bEnabled = (state & Enabled)>0;
	m_Rect = rect;
	m_clsId = clsId;
	m_UID = uid;
	return true;
}

void iView::Cleanup()
{
	for (uint32 xx=0; xx<m_Childs.GetSize(); ++xx) delete m_Childs[xx];
	m_Childs.RemoveAll();
}

bool iView::AddChild(iView* pChild)
{
	pChild->m_pParent = this;
	m_Childs.Add(pChild);
	return true;
}

bool iView::RemoveChild(iView* pChild)
{
	if (pChild && m_Childs.Remove(pChild)){
		pChild->m_pParent = NULL;
		return true;
	}
	return false;
}

iView* iView::GetChildByPos(const iPoint& pos) const
{
	// reverse find (in future remake to z-order)
	if (!m_Childs.GetSize()) return NULL;
	for (sint32 xx=m_Childs.GetSize()-1; xx>=0; --xx) {
		iView* v = m_Childs[xx];
		iRect r = m_Childs[xx]->m_Rect;
		if (m_Childs[xx]->m_bVisible && m_Childs[xx]->m_bEnabled && m_Childs[xx]->m_Rect.PtInRect(pos)) return m_Childs[xx];
	}
	return NULL;
}

iView* iView::GetChildById(uint32 uid) const
{
	for (uint32 xx=0; xx<m_Childs.GetSize(); ++xx) {
		if (m_Childs[xx]->m_UID == uid) return m_Childs[xx];
	}
	return NULL;
}

iView* iView::GetChildByIndex(uint32 index) const
{
    if (index < m_Childs.GetSize())
    {
        return m_Childs[index];
    }
    return NULL;
}


void iView::SetVisibleById(uint32 uid, bool state) const
{
    iView* p = GetChildById(uid);
    if (p) p->SetVisible(state);
}
//////////////////////////////////////////////////////////////////////////
void iView::Compose(iRect& rect)
{
	if (m_bVisible) {
		OnCompose();
		uint32 sz = m_Childs.GetSize();
		for (uint32 xx=0; xx<sz; ++xx) m_Childs[xx]->Compose(rect);
		if (m_bNeedRedraw) {
			rect += GetScrRect();
			m_bNeedRedraw = false;
		}
		AfterCompose();
	}
}

bool iView::MouseEntered(const iPoint& pos, MouseId mID){

#if defined( PC_VERSION )
	iView* vp = GetChildByPos(pos-GetScrRect().point());
	if ( !vp || !vp->MouseEntered(pos, mID) ) {
		/* *** */
		if( iGXApp::Instance().ViewMgr().CapturedEnt() && !iGXApp::Instance().ViewMgr().CapturedRect().PtInRect(pos) ){
			iView* v = iGXApp::Instance().ViewMgr().CapturedEnt();
			
			iGXApp::Instance().ViewMgr().ReleaseRectCapture();
			iGXApp::Instance().ViewMgr().ReleaseEntCapture();
			v->OnMouseLeave(pos, mID);
//			v->OnCompose();
		}
		else{
			iGXApp::Instance().ViewMgr().SetRectCapture( GetScrRect() );
			iGXApp::Instance().ViewMgr().SetEntCapture(this);
			OnMouseEntered(pos, mID);
//			OnCompose();
		}
		Invalidate();
	}
#endif
	return true;
}

bool iView::MouseLeave(const iPoint& pos, MouseId mID){

	iGXApp::Instance().ViewMgr().ReleaseViewCapture();
	CheckUpGestures();
	OnMouseLeave(pos, mID);

	return true;
}

bool iView::MouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if(mID == 0) 
		for(uint32 x=0; x<MAX_MICE_COUNT; x++)
		{
			m_bTracking[x] = false;
			m_TrackAnchor[x] = cInvalidPoint;
			m_BeginningTrackAnchor[x] = cInvalidPoint;
			m_EndTrackAnchor[x] = cInvalidPoint;
		}
	m_BeginningTrackAnchor[mID] = pos;
	m_TrackAnchor[mID] = pos;

	if (!m_bEnabled) {
		// only topmost window can receive messages in disabled state
		check(m_pParent == NULL);
		iGXApp::Instance().ViewMgr().SetViewCapture(this);
		m_bTracking[mID] = true;
		return true;
	}

	// this is a dirty solution for the problem with two-finger operated controls (like the Hero switcher)
	// this line should be moved to the place marked *** or it causes the finger to "stuck" pressed
	m_bTracking[mID] = true;

	
	iView* vp = GetChildByPos(pos-GetScrRect().point());
	if (!vp || !vp->MouseDown(pos, mID, mbID) || AlwaysReceiveMouse()) {
		/* *** */
		if(!AlwaysReceiveMouse()) 
			iGXApp::Instance().ViewMgr().SetViewCapture(this);
		OnMouseDown(pos, mID, mbID);
	}
	return true;
}

bool iView::MouseWheelDown(sint16 line, MouseId mID){

    if( IsScrolling() ){
    
        OnMouseWheelDown(line, mID);
    }
    else if( m_Childs.GetSize() ){
		
		for (sint32 xx=m_Childs.GetSize()-1; xx>=0; --xx){
			
			iView* vp = m_Childs[xx];
        
			if (!m_Childs[xx]->m_bVisible || !m_Childs[xx]->m_bEnabled )
				continue;

			if (!vp || !vp->MouseWheelDown(line, mID) || AlwaysReceiveMouse()) {
				
//				OnMouseWheelDown(line, mID);
			}
		}
	}
	
	return( true );
}

bool iView::MouseWheelUp(sint16 line, MouseId mID){

	if( m_Childs.GetSize() ){
		
		for (sint32 xx=m_Childs.GetSize()-1; xx>=0; --xx){
	
			iView* vp = m_Childs[xx];
		
			if (!m_Childs[xx]->m_bVisible || !m_Childs[xx]->m_bEnabled )
				continue;
			
			if (!vp || !vp->MouseWheelUp(line, mID) || AlwaysReceiveMouse()) {
			
				if(!AlwaysReceiveMouse()) 
					iGXApp::Instance().ViewMgr().SetViewCapture(this);
				OnMouseWheelUp(line, mID);
			}
		}
	}
	else 
		OnMouseWheelUp(line, mID);
	
	return( true );
}

uint8 iView::PerformedFingers() {
	uint8 count = 0;
	for(uint32 x=0; x<MAX_MICE_COUNT; x++)
		if(m_BeginningTrackAnchor[x] != cInvalidPoint)
			count++;
			
	return count;
}

uint8 iView::ActiveFingers() {
	uint8 count = 0;
	for(uint32 x=0; x<MAX_MICE_COUNT; x++)
		if(m_bTracking[x])
			count++;
	return count;
}

void iView::CheckTrackGestures() {
	uint8 fingers = ActiveFingers();
	if(fingers == 1) {
		//swipe right
		OnSwipeRight(m_TrackAnchor[0].x - m_BeginningTrackAnchor[0].x);
		//swipe left
		OnSwipeLeft(m_BeginningTrackAnchor[0].x - m_TrackAnchor[0].x);
		//swipe up
		OnSwipeUp(m_BeginningTrackAnchor[0].y - m_TrackAnchor[0].y);
		//swipe down
		OnSwipeDown(-m_BeginningTrackAnchor[0].y + m_TrackAnchor[0].y);

	} else if(fingers == 2) {
		if(m_BeginningTrackAnchor[0].GetDelta(m_BeginningTrackAnchor[1]) < 150 &&
		   m_TrackAnchor[0].GetDelta(m_TrackAnchor[1]) < 150) {
			//double swipe right
			OnDualSwipeRight(m_TrackAnchor[0].x - m_BeginningTrackAnchor[0].x);
			//double swipe left
			OnDualSwipeLeft(-m_TrackAnchor[0].x + m_BeginningTrackAnchor[0].x);
			//dual swipe up
			OnDualSwipeUp( m_BeginningTrackAnchor[0].y - m_TrackAnchor[0].y);
			//dual swipe down
			OnDualSwipeDown(m_TrackAnchor[0].y - m_BeginningTrackAnchor[0].y);
		}
		//dual track
	//	if(m_TrackAnchor[0].GetDelta(m_TrackAnchor[1]) < 110)
			OnDualMouseTrack(m_TrackAnchor[0]);

		// pinch out
		if(m_TrackAnchor[0] != cInvalidPoint && m_TrackAnchor[1] != cInvalidPoint) {
			sint32 newDelta = m_TrackAnchor[0].GetDelta(m_TrackAnchor[1]), 
			oldDelta  = m_BeginningTrackAnchor[0].GetDelta(m_BeginningTrackAnchor[1]);
			OnPinchOut(newDelta - oldDelta);
			// pinch in
			OnPinchIn(oldDelta - newDelta);
		}
	} else if(fingers == 3) {
		//triple swipe right
		OnTripleSwipeRight(m_TrackAnchor[0].x - m_BeginningTrackAnchor[0].x);
		//triple swipe left
		OnTripleSwipeLeft(-m_TrackAnchor[0].x + m_BeginningTrackAnchor[0].x);
	}
}

void iView::CheckUpGestures() {
	// 'sand-glass' gesture
	if(PerformedFingers() != 2)
		return;
	if(/*iABS<sint16>(m_BeginningTrackAnchor[0].x - m_BeginningTrackAnchor[1].x) < 30 &&*/
		m_BeginningTrackAnchor[0].GetDelta(m_EndTrackAnchor[1]) < 50 &&
	   m_BeginningTrackAnchor[1].GetDelta(m_EndTrackAnchor[0]) < 50);
		//OnSandGlass();
}


bool iView::MouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	m_EndTrackAnchor[mID] = pos;

	if (m_bTracking[mID]) {
		iGXApp::Instance().ViewMgr().ReleaseViewCapture();
		CheckUpGestures();
		OnMouseUp(pos,  mID, mbID);
		m_bTracking[mID] = false;
		if (GetScrRect().PtInRect(pos)) {
			if(mID != 0
				|| GetTickCount() - m_lastClick > 500 || m_lastClick == -1) {
				OnMouseClick(pos, mID, mbID);
				//if(m_BeginningTrackAnchor[mID] == m_EndTrackAnchor[mID])
				m_lastClick = GetTickCount();
			} else {
				m_bIsDoubleClicking = true;
				OnMouseClick(pos, mID, mbID);
				m_bIsDoubleClicking = false;			
				OnMouseDoubleClick(pos, mID, mbID);

				m_lastClick = -1;
			}
		}
		if(ActiveFingers() == 0)
			for(uint32 x=0; x<MAX_MICE_COUNT; x++)
				m_BeginningTrackAnchor[x] = cInvalidPoint;
		
	} else {
		//check(0);
	}
	
	return true;
}

bool iView::MouseRightDown(const iPoint& pos, MouseId mID, MouseButtonId mbID){

    OnMouseRightDown(pos, mID, mbID);
    
    return true;
}
bool iView::MouseRightUp(const iPoint& pos, MouseId mID, MouseButtonId mbID){

    OnMouseRightUp(pos, mID, mbID);
    
    return true;
}  

bool iView::MouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
     m_TrackAnchor[mID] = pos;    
        CheckTrackGestures();
       // m_lastClick = -1;

	if (m_bTracking[mID] && m_bEnabled) OnMouseTrack(pos, mID, mbID);
	iView* vp = GetChildByPos(pos-GetScrRect().point());
	if (!vp || !vp->MouseTrack(pos, mID, mbID) || AlwaysReceiveMouse()) {
//		if(!AlwaysReceiveMouse()) m_pMgr->SetViewCapture(this);

	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
void iView::SetSize(const iSize& nsiz)
{
	m_Rect.w = nsiz.w;
	m_Rect.h = nsiz.h;
	OnRectChanged(m_Rect);
}

void iView::SetPos(const iPoint& npos)
{
	m_Rect.x = npos.x;
	m_Rect.y = npos.y;
	OnRectChanged(m_Rect);
}

void iView::SetRect(const iRect& rect)
{
	m_Rect = rect; 
	OnRectChanged(m_Rect);
}

void iView::Invalidate()
{
	m_bNeedRedraw = true;
	if (m_pParent) m_pParent->Invalidate();
}

void iView::TransferMouseState(iView *fromView)
{
	for(uint32 x=0; x<MAX_MICE_COUNT; x++) {
		m_TrackAnchor[x] = fromView->m_TrackAnchor[x];
		m_BeginningTrackAnchor[x] = fromView->m_BeginningTrackAnchor[x];
		m_EndTrackAnchor[x] = fromView->m_EndTrackAnchor[x];
		m_bTracking[x] = fromView->m_bTracking[x];
	}
}


