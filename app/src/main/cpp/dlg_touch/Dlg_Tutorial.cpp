//
//  Dlg_Tutorial.cpp
//  pheroes
//
//  Created by Andrey Kiryushkin on 1/30/12.
//  Copyright (c) 2012 Palm Heroes Team. All rights reserved.
//

#include "stdafx.h"
#include "Dlg_Tutorial.h"

iDlg_Tutorial::iDlg_Tutorial(iViewMgr* pViewMgr, TextResId text, TUT_MSG tut): 
iBaseGameDlg(pViewMgr), m_text(text), m_tut(tut)
{}

void iDlg_Tutorial::OnCreateDlg()
{    
	iRect clRect = ClientRect();
    const sint16 msg = TRID_DONT_SHOW;
    
    sint32 bw = iMAX(gTextComposer.GetTextSize(gTextMgr[TRID_OK], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[msg], btnfc_normal).w) + 30;
	AddChild(new iTextButton(m_pMgr,this, AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw / 2, 0), TRID_OK, 100));
	AddChild(new iTextButton(m_pMgr,this, AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw / 2, 0), msg, 101));
}

void iDlg_Tutorial::DoCompose(const iRect& clRect)
{
    iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();

    // Message
	iStringT msg = gTextMgr[m_text];
	rc.y += gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), msg, rc) + 10;
}

iSize iDlg_Tutorial::ClientSize() const
{
    sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();
    
	// Message
	iStringT msg = gTextMgr[m_text];
	h += gTextComposer.GetTextBoxSize(msg, w, dlgfc_plain).h + 10;
    
    h += DEF_BTN_HEIGHT;
    
	return iSize(w, h);
}

void iDlg_Tutorial::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
    if(uid == 101) {
        
            gTutorial.MarkDontShow(m_tut);
    }
	EndDialog(uid);
}

