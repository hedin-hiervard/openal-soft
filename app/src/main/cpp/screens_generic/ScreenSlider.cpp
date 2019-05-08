/*
 *  ScreenSlider.cpp
 *  pheroes
 *
 *  Created by Hedin on 6/23/11.
 *  Copyright 2011 Palm Heroes Team. All rights reserved.
 *
 */

#include "stdafx.h"
#include "ScreenSlider.h"
#include "CtrlTransitionView.h"
#include "Dlg_Spellbook.h"
/////
// SLIDE
/////

iSlide::iSlide(iViewMgr* pViewMgr, iRect rc, uint32 uid) :
iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled)
{
	
}


/////
// SLIDER
/////

iScreenSlider::iScreenSlider(iViewMgr* pViewMgr, IViewCmdHandler* pHandler, iRect rc, uint32 uid) :
iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled)
, m_bGestureComplete(false), m_curSlide(0)
, m_bTransition(false)
, m_pHandler(pHandler)
{
	
}

uint32 iScreenSlider::AddSlide()
{
	uint32 topuid = m_Childs.GetSize();
	iView* sl;
	AddChild(sl = new iSlide(&gApp.ViewMgr(), m_Rect, topuid));
	sl->SetVisible(topuid == 0);
	return topuid;
}

iSlide* iScreenSlider::GetSlide(uint32 uid) 
{ 
	check(uid < m_Childs.GetSize());
	return (iSlide*)GetChildById(uid);
}

void iScreenSlider::EnableSlide(uint32 uid, bool bEnabled)
{
	//check(uid < m_pSlides.GetSize());
	GetChildById(uid)->SetEnabled(bEnabled);
}

void iScreenSlider::UpdateSize()
{
	iRect rc = GetScrRect();
	for(uint32 xx=0; xx<m_Childs.GetSize(); xx++)
	{
		m_Childs[xx]->SetRect(rc);
		m_Childs[xx]->UpdateSize();
		m_Childs[xx]->SetVisible(xx == m_curSlide);
	}
}

sint32 iScreenSlider::GetNextSlide() const
{
	sint32 cur = m_curSlide;
	do {
		cur++;
		if(cur >= m_Childs.GetSize()) cur = 0;
	} while(!m_Childs[cur]->IsEnabled());
    
	return cur;
}

sint32 iScreenSlider::GetPrevSlide() const
{
	sint32 cur = m_curSlide;
	do {
		cur--;
		if(cur < 0) cur = m_Childs.GetSize() - 1;
	} while(!m_Childs[cur]->IsEnabled());
	return cur;
}

void iScreenSlider::PrevSlide()
{
	m_Childs[m_curSlide]->SetVisible(false);
	m_curSlide = GetPrevSlide();
	m_Childs[m_curSlide]->SetVisible(true);	
    
    m_pHandler->iCMDH_ControlCommand(this, CCI_TABCHANGED, m_curSlide);
}

void iScreenSlider::NextSlide()
{
	m_Childs[m_curSlide]->SetVisible(false);
	m_curSlide = GetNextSlide();
	m_Childs[m_curSlide]->SetVisible(true);
    
    m_pHandler->iCMDH_ControlCommand(this, CCI_TABCHANGED, m_curSlide);
}

void iScreenSlider::OnSwipeRight(sint32 length)
{
	if(dynamic_cast<iAmountChooserGlow*>(gApp.ViewMgr().CapturedView()))
		return;
	
	if(length < 20)
		return;
	
	if(!m_bGestureComplete) {
		m_bGestureComplete = true;
		iRect rc = GetScrRect();
		m_Childs[GetPrevSlide()]->SetVisible(true);
		rc.x -= rc.w;
		m_Childs[GetPrevSlide()]->SetRect(rc);
		iCtrlTransitionView trView(&gApp.ViewMgr(), 
								   iCtrlTransitionView::TVM_HRZ_SLIDE_LEFT,
								   iCtrlTransitionView::MM_MANUAL, 						
								   m_Childs[m_curSlide],
								   m_Childs[GetPrevSlide()]);
		trView.TransferMouseState(this);
		gApp.ViewMgr().SetViewCapture(&trView);
		m_bTransition = true;
		iCtrlTransitionView::TV_RESULT res = (iCtrlTransitionView::TV_RESULT)trView.DoModal();
		m_bTransition = false;
		if(res == iCtrlTransitionView::TVR_CHANGED)
			PrevSlide();
		UpdateSize(); // to return slides to normal positions
		m_bGestureComplete = false;
	}
}

void iScreenSlider::OnSwipeLeft(sint32 length)
{
	if(dynamic_cast<iAmountChooserGlow*>(gApp.ViewMgr().CapturedView()))
		return;

	if(length < 20)
		return;
		
	if(!m_bGestureComplete) {
		m_bGestureComplete = true;
		iRect rc = GetScrRect();
		m_Childs[GetNextSlide()]->SetVisible(true);
		rc.x += rc.w;
		m_Childs[GetNextSlide()]->SetRect(rc);
		iCtrlTransitionView trView(&gApp.ViewMgr(), 
								   iCtrlTransitionView::TVM_HRZ_SLIDE_RIGHT,
								   iCtrlTransitionView::MM_MANUAL, 						
								   m_Childs[m_curSlide],
								   m_Childs[GetNextSlide()]);
		trView.TransferMouseState(this);
		gApp.ViewMgr().SetViewCapture(&trView);
		m_bTransition = true;
		iCtrlTransitionView::TV_RESULT res = (iCtrlTransitionView::TV_RESULT)trView.DoModal();
		m_bTransition = false;
		if(res == iCtrlTransitionView::TVR_CHANGED)
			NextSlide();
		UpdateSize(); // to return slides to normal positions
		m_bGestureComplete = false;
	}
}

void iScreenSlider::SetCurrentSlide(uint32 uid) 
{
	m_curSlide = uid;
	for(uint32 xx=0; xx<m_Childs.GetSize(); xx++)
		m_Childs[xx]->SetVisible(uid == xx);
    m_pHandler->iCMDH_ControlCommand(this, CCI_TABCHANGED, uid);
}

bool iScreenSlider::IsInTransition() const
{
	return m_bTransition;
}
