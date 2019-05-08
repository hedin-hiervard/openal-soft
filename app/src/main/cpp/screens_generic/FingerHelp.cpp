/*
 *  FingerHelp.cpp
 *  pheroes
 *
 *  Created by Hedin on 7/20/11.
 *  Copyright 2011 Palm Heroes Team. All rights reserved.
 *
 */
#include "stdafx.h"
#include "FingerHelp.h"

const iSize dlgSize = iSize(400, 300);

iFingerHelp::iFingerHelp(iViewMgr* pViewMgr, uint32 flags): 
iDialog(pViewMgr), m_timer(fix32::zero), m_flags(flags)
{
	m_underSrf.Init(gApp.Surface());
}

bool iFingerHelp::Process(fix32 t)
{
	m_timer += t;
	if(m_timer >= fix32(1.0f))
		EndDialog(0);
	return true;
}

sint32 iFingerHelp::DoModal()
{	
	iRect rc = iRect(iPoint(), gApp.Surface().GetSize());
	iRect r = AlignRect(dlgSize, rc, AlignCenter);
	SetRect(r);
	OnCreateDlg();
	SetVisible();
	m_pMgr->PushModalDlg(this);
	while (m_pMgr->App()->Cycle() && m_retCode == DRC_UNDEFINED) {}
	iDialog* pDlg = m_pMgr->PopModalDlg();
	check(pDlg == this);
	
	return m_retCode;
}

void iFingerHelp::OnCreateDlg()
{
	
}

iSize iFingerHelp::GetDlgMetrics() const
{
	return iSize(400, 300);
}

void iFingerHelp::OnCompose()
{
	m_underSrf.CopyToDibXY(gApp.Surface(), iPoint());
	iRect rc = GetScrRect();
	sint16 sid = PDGG(FINGER);
	iSize sz = gGfxMgr.Dimension(sid);
	iRect trc = AlignRect(sz, rc, AlignCenter) + iPoint(0, 30);
	gGfxMgr.Blit(sid, gApp.Surface(), trc);

	if(m_flags & FH_LEFT) {
		sid = PDGG(FINGER_ARROW_L);
		sz = gGfxMgr.Dimension(sid);
		gGfxMgr.Blit(sid, gApp.Surface(), trc + iPoint(-sz.w, 0));
	}

	if(m_flags & FH_RIGHT) {
		sid = PDGG(FINGER_ARROW_R);
		sz = gGfxMgr.Dimension(sid);
		gGfxMgr.Blit(sid, gApp.Surface(), trc + iPoint(sz.w, 0));
	}

	if(m_flags & FH_UP) {
		sid = PDGG(FINGER_ARROW_U);
		sz = gGfxMgr.Dimension(sid);
		gGfxMgr.Blit(sid, gApp.Surface(), trc + iPoint(-15, -sz.h));
	}
	
	
}

