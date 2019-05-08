#include "stdafx.h"

#include "ScrollingView.h"
#include "helpers.h"

iScrollingView::iScrollingView():iChildGameView(false, CV_MENU)
, m_pos(0)
{
	Invalidate();
}

iScrollingView::~iScrollingView()
{
	
}

void iScrollingView::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	if(m_BeginningTrackAnchor[0].GetDelta(m_TrackAnchor[0]) < 5)
		gGame.MainMenu();
		//gGame.HideViewDelayed(CV_SCROLL);
}

void iScrollingView::OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	m_fixPos = m_pos;
	m_startPos = pos;
}

void iScrollingView::OnSwipeUp(sint32 length)
{
	m_pos = iCLAMP<sint32>(320 - 1700, 0, m_fixPos - (m_startPos.y - m_TrackAnchor[0].y));
	Invalidate();
}

void iScrollingView::OnSwipeDown(sint32 length)
{
	
}


void iScrollingView::OnCompose()
{
	//gGfxMgr.Blit(PDGG(TUTORIAL), gApp.Surface(), iPoint(0, m_pos));
}

