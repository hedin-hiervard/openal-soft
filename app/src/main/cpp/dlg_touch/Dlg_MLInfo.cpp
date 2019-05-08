#include "stdafx.h"
#include "Dlg_MLInfo.h"

iMLInfoDlg::iMLInfoDlg(iViewMgr* pViewMgr, bool bLuck, sint8 val, PLAYER_ID pid)
: iBaseGameDlg(pViewMgr), m_bLuck(bLuck), m_val(val)
{
	if (m_bLuck){
		m_title = gTextMgr[TRID_SKILL_LUCK];
		if (val < 0) m_text = gTextMgr[TRID_INFO_BAD_LUCK];
		else if (val > 0) m_text = gTextMgr[TRID_INFO_GOOD_LUCK];
		else m_text = gTextMgr[TRID_INFO_NEUT_LUCK];
	} else {
		m_title = gTextMgr[TRID_SKILL_MORALE];
		if (val < 0) m_text = gTextMgr[TRID_INFO_BAD_MORALE];
		else if (val > 0) m_text = gTextMgr[TRID_INFO_GOOD_MORALE];
		else m_text = gTextMgr[TRID_INFO_NEUT_MORALE];
	}
}

void iMLInfoDlg::OnCreateDlg()
{
	iRect clRect = ClientRect();	
}

void iMLInfoDlg::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();

	if (!m_title.Empty()) {
		rc.y += gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), m_title, rc);		
	}

	if (!m_text.Empty()) {
		rc.y += gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), m_text, rc);		
	}
	
	rc.h = 30;
	if (m_bLuck) {
		ComposeLuckGlyph(gApp.Surface(), rc, m_val, false);
	} else {
		ComposeMoraleGlyph(gApp.Surface(), rc, m_val, false);
	}

}

iSize iMLInfoDlg::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();

	// title
	if (!m_title.Empty()) {
		h += gTextComposer.GetTextBoxSize(m_title, w, dlgfc_hdr).h;		
	}

	// text
	if (!m_text.Empty()) {
		h += gTextComposer.GetTextBoxSize(m_text, w, dlgfc_plain).h;				
	}

	// icon	
	h += 50;	

	return iSize(w, h);
}

void iMLInfoDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	EndDialog(uid);
}

void iMLInfoDlg::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {
	EndDialog(0);
}


