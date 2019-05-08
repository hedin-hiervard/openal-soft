#include "stdafx.h"
#include "Dlg_FightGuard.h"

iDlg_FightGuard::iDlg_FightGuard(iViewMgr* pViewMgr, const iStringT& itemName, const iArmy& guard, PLAYER_ID pid, VISION_LEVEL vl)
: iTextDlg(pViewMgr, iStringT(), iStringT(), pid ), m_itemName(itemName), m_guard(guard), m_vl(vl)
{}

void iDlg_FightGuard::OnCreateDlg()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(DRC_NO); // does not fight. if we actually said YES, it would be a separate command to start combat
		return;
	}		
#endif

	sint32 bw = gTextComposer.GetTextSize(gTextMgr[TRID_CFGENTRY_QUICKCOMBAT], btnfc_normal).w + 30;
	AddChild(m_pBtnYes = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, 0),TRID_YES, DRC_YES));
    if(!gSettings.GetEntryValue(CET_QUICKCOMBAT))
		AddChild(m_pBtnQuick = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom),TRID_CFGENTRY_QUICKCOMBAT, DRC_QUICK));
	AddChild(m_pBtnNo = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0),TRID_NO, DRC_NO));
}
#ifdef PC_VERSION
void iDlg_FightGuard::UpdateSize(){

    Center(); 
   
    sint32 bw = gTextComposer.GetTextSize(gTextMgr[TRID_CFGENTRY_QUICKCOMBAT], btnfc_normal).w + 30;
    
    m_pBtnYes->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, 0) );
    
    if(!gSettings.GetEntryValue(CET_QUICKCOMBAT))
        m_pBtnQuick->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) );
    
    m_pBtnNo->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0) );
}
#endif
void iDlg_FightGuard::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();
	
	// title
	rc.y += gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), m_itemName, rc);

	// message
	rc.y += gTextComposer.TextBoxOut(dlgfc_stopic, gApp.Surface(), iFormat(gTextMgr[TRID_OBJ_GUARDED_BY], m_itemName.CStr()), rc);

	// guards
	for (uint32 xx=0; xx<7; ++xx) {
		if (m_guard[xx].IsValid()) {
			rc.y += gTextComposer.TextBoxOut(dlgfc_splain, gApp.Surface(), CreatGroup2Text(m_guard[xx].Type(),m_guard[xx].Count(), m_vl), rc);			
		}
	}
	rc.y += 5;

	// question
	rc.y += gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(), gTextMgr[TRID_FIGHT_GUARD_QUESTION], rc);
}

iSize iDlg_FightGuard::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();

	// title
	h += gTextComposer.GetTextBoxSize(m_itemName, w, dlgfc_hdr).h;

	// message
	h += gTextComposer.GetTextBoxSize(iFormat(gTextMgr[TRID_OBJ_GUARDED_BY], m_itemName.CStr()), w, dlgfc_stopic).h;

	// guards
	for (uint32 xx=0; xx<7; ++xx) {
		if (m_guard[xx].IsValid()) {
			h += gTextComposer.GetTextBoxSize(CreatGroup2Text(m_guard[xx].Type(),m_guard[xx].Count(), m_vl), w, dlgfc_splain).h;			
		}
	}

	h += iBaseGameDlg::GetTopOffset();

	// question
	h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_FIGHT_GUARD_QUESTION], w, dlgfc_topic).h;

	// ok button
	h += DEF_BTN_HEIGHT + 15;

	return iSize(w,h);
}

void iDlg_FightGuard::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	{
		EndDialog(pView->GetUID());
	}
}

