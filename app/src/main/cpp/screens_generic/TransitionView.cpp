#include "stdafx.h"
#include "CastleView.h"
#include "TransitionView.h"

iPageDisplay::iPageDisplay(iViewMgr* pViewMgr, const iRect& rect):
iView(&gApp.ViewMgr(), rect, GENERIC_VIEWPORT, 0, Visible|Enabled )
,m_curPage(0), m_numPages(0)
{
	
}
void iPageDisplay::OnCompose()
{
	iRect rc = iRect(GetScrPos(), iSize(7, 7));
	for(sint32 x=0; x<m_numPages; x++) {
		if(x==m_curPage)
			gGfxMgr.Blit(PDGG(STAR_SELECTED), gApp.Surface(), rc);
		else
			gGfxMgr.Blit(PDGG(STAR_DESELECTED), gApp.Surface(), rc);			
		rc.x += GetScrRect().w / m_numPages;
	}
}

iTransitionView::iTransitionView(iViewMgr* viewMgr, TV_MODE viewMode, MOTION_MODE motionMode, iDib& dib1, iDib& dib2):
	iDialog(viewMgr), m_start(0),
	m_viewMode(viewMode), m_motionMode(motionMode), m_dib1(dib1), 
	m_dib2(dib2), m_timer(fix32::zero), m_speed(motionMode == MM_AUTO ? 10 : 0)
{
	if(viewMode == TVM_VRT_OVERLAY) {
			if(motionMode == MM_AUTO)
				m_position = iPoint(0, -320);
			else
				m_position = iPoint(0, 0);
	}
	m_globalHeight = (sint32)gApp.Surface().GetHeight();
	m_globalWidth = (sint32)gApp.Surface().GetWidth();
}
	

void iTransitionView::OnCompose()
{
	if(m_viewMode == TVM_VRT_OVERLAY) {
		m_dib2.CopyToDibXY(gApp.Surface(), iPoint());
		m_dib1.CopyToDibXY(gApp.Surface(), m_position);
	} else if(m_viewMode == TVM_HRZ_SLIDE_RIGHT) {
		m_dib1.CopyToDibXY(gApp.Surface(), m_position);
		m_dib2.CopyToDibXY(gApp.Surface(), m_position + iPoint(m_globalWidth, 0));
	} else if(m_viewMode == TVM_HRZ_SLIDE_LEFT) {
		m_dib1.CopyToDibXY(gApp.Surface(), m_position - iPoint(m_globalWidth, 0));
		m_dib2.CopyToDibXY(gApp.Surface(), m_position);
	}
}

void iTransitionView::OnSwipeUp(sint32 length) 
{
	if(m_motionMode != MM_MANUAL)
		return;
	if(m_viewMode == TVM_VRT_OVERLAY) {
		m_speed = length / 10;
		m_position.y = iMIN<sint32>(0, m_start - length);
		Invalidate();
	} 
}

void iTransitionView::OnSwipeLeft(sint32 length) 
{
	if(m_motionMode != MM_MANUAL)
		return;
	if(m_viewMode == TVM_HRZ_SLIDE_RIGHT) {
		m_speed = length / 5;
		m_position.x = iCLAMP<sint32>(-m_globalWidth, 0, m_start - length);
		Invalidate();		
	}	
}

void iTransitionView::OnSwipeRight(sint32 length) 
{
	if(m_motionMode != MM_MANUAL)
		return;
	if(m_viewMode == TVM_HRZ_SLIDE_LEFT) {
		m_speed = length / 5;
		m_position.x = iCLAMP<sint32>(0, m_globalWidth, m_start + length);
		
		Invalidate();		
	}	
}

void iTransitionView::OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	if(mID > 0 || m_motionMode == MM_AUTO)
		return;
	m_speed = 0;
	//if(m_motionMode == MM_AUTO) m_motionMode = MM_MANUAL;
	if(m_viewMode == TVM_VRT_OVERLAY)
		m_start = m_position.y;	
	else if(m_viewMode == TVM_HRZ_SLIDE_RIGHT || m_viewMode == TVM_HRZ_SLIDE_LEFT)
		m_start = m_position.x;
}

//#define m_speed 10

bool iTransitionView::Process(fix32 t)
{
	if(ActiveFingers() > 0 && m_motionMode == MM_MANUAL)
		return true; 
	if(m_viewMode == TVM_VRT_OVERLAY) {
		if(m_motionMode == MM_MANUAL) {
			if(m_position.y <= -m_globalHeight) {
				EndDialog(TVR_UP);
				return true;
			} else if(m_position.y >= 0) {
				EndDialog(TVR_DOWN);
			 return true;
			}
			m_timer += t;
			if(m_timer > fix32(0.01f)) {
				m_speed++;
				m_position.y = iCLAMP<sint32>(-m_globalHeight, 0, m_position.y - m_speed);
				m_timer = fix32::zero;
				Invalidate();
			}
		} else {
			m_timer += t;
			if(m_timer > fix32(0.01f)) {
				m_speed += 5;
				m_position.y = iCLAMP<sint32>(-m_globalHeight, 0, m_position.y + m_speed);
				Invalidate();
				m_timer = fix32::zero;
				if(m_position.y >= 0) {
					EndDialog(TVR_DOWN);
					return true;
				}
			}			
		}
	} else if(m_viewMode == TVM_HRZ_SLIDE_RIGHT) {
		if(m_motionMode == MM_MANUAL) {
			if(m_position.x <= -m_globalWidth) {
				EndDialog(TVR_CHANGED);
				return true;
			} else if(m_position.x >= 0) {
				EndDialog(TVR_LEFT);
				return true;
			}
			m_timer += t;
			if(m_timer > fix32(0.01f)) {
				if(m_position.x < -m_globalWidth / 2) {
					m_speed += 5;
				}
				else if(m_position.x < 0) 
				{
					m_speed -= 5;
				}
				m_timer = fix32::zero;
				m_position.x = iCLAMP<sint32>(-m_globalWidth, 0, m_position.x - m_speed);
				Invalidate();
			}
		} 
	} else if(m_viewMode == TVM_HRZ_SLIDE_LEFT) {
		if(m_motionMode == MM_MANUAL) {
			if(m_position.x >= m_globalWidth) {
				EndDialog(TVR_CHANGED);
				return true;
			} else if(m_position.x <= 0) {
				EndDialog(TVR_LEFT);
				return true;
			}
			m_timer += t;
			if(m_timer > fix32(0.01f)) {
				if(m_position.x > m_globalWidth / 2) {
					m_speed += 5;
				}
				else if(m_position.x > 0) 
				{
					m_speed -= 5;
				}
				m_position.x = iCLAMP<sint32>(0, m_globalWidth, m_position.x + m_speed);
				m_timer = fix32::zero;
				Invalidate();

			}
		} 
	}	
	return true;
}

iSize iTransitionView::GetDlgMetrics() const { return gApp.Surface().GetSize(); }

void iTransitionView::OnCreateDlg() {
	
}
