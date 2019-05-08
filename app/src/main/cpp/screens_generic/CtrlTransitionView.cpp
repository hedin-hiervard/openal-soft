/*
 *  CtrlTransitionView.cpp
 *  pheroes
 *
 *  Created by Hedin on 6/27/11.
 *  Copyright 2011 Palm Heroes Team. All rights reserved.
 *
 */

#include "stdafx.h"
#include "CtrlTransitionView.h"

iCtrlTransitionView::iCtrlTransitionView(iViewMgr* viewMgr, TV_MODE viewMode, MOTION_MODE motionMode, iView *pV1, iView* pV2): 
iDialog(viewMgr), 
m_start(0),
m_viewMode(viewMode), 
m_motionMode(motionMode), 
m_pV1(pV1), 
m_pV2(pV2), 
m_timer(fix32::zero), 
m_speed(motionMode == MM_AUTO ? 10 : 0)
{
	m_globalHeight = (sint32)gApp.Surface().GetHeight();
	m_globalWidth = (sint32)gApp.Surface().GetWidth();
}

void iCtrlTransitionView::OnCompose()
{
	iRect rc;
	//m_pV1->Compose(rc);
	//m_pV2->Compose(rc);
}

void iCtrlTransitionView::UpdateControls()
{
	if(m_viewMode == TVM_HRZ_SLIDE_RIGHT) {
		iRect rc = GetScrRect();
		rc.x = m_position.x;
		m_pV1->SetRect(rc);
		rc.x += rc.w;
		m_pV2->SetRect(rc);
	} else if(m_viewMode == TVM_HRZ_SLIDE_LEFT) {
		iRect rc = GetScrRect();
		rc.x = m_position.x;
		m_pV1->SetRect(rc);
		rc.x -= rc.w;
		m_pV2->SetRect(rc);
	}
}

void iCtrlTransitionView::OnSwipeLeft(sint32 length) 
{
	if(m_motionMode != MM_MANUAL)
		return;
	if(m_viewMode == TVM_HRZ_SLIDE_RIGHT) {
		m_speed = length / 5;
		m_position.x = iCLAMP<sint32>(-m_globalWidth, 0, m_start - length);
		UpdateControls();
		Invalidate();		
	}	
}

void iCtrlTransitionView::OnSwipeRight(sint32 length) 
{
	if(m_motionMode != MM_MANUAL)
		return;
	if(m_viewMode == TVM_HRZ_SLIDE_LEFT) {
		m_speed = length / 5;
		m_position.x = iCLAMP<sint32>(0, m_globalWidth, m_start + length);
		UpdateControls();
		Invalidate();		
	}	
}

void iCtrlTransitionView::OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	if(mID > 0 || m_motionMode == MM_AUTO)
		return;
	m_speed = 0;
	if(m_viewMode == TVM_HRZ_SLIDE_RIGHT || m_viewMode == TVM_HRZ_SLIDE_LEFT)
		m_start = m_position.x;
}

bool iCtrlTransitionView::Process(fix32 t)
{
	if(ActiveFingers() > 0 && m_motionMode == MM_MANUAL)
		return true; 
	if(m_viewMode == TVM_HRZ_SLIDE_RIGHT) {
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
				UpdateControls();
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
				UpdateControls();
				Invalidate();
				
			}
		} 
	}	
	return true;
}

iSize iCtrlTransitionView::GetDlgMetrics() const { return gApp.Surface().GetSize(); }

void iCtrlTransitionView::OnCreateDlg() {
	
}
