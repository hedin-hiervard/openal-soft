#include "stdafx.h"
#include "Dlg_NewWeek.h"

iDlg_NewWeek::iDlg_NewWeek(iViewMgr* pViewMgr, PLAYER_ID pid, const iWeekDesc& weekDesc)
: iBaseGameDlg(pViewMgr), m_weekDesc(weekDesc)
{
	if (weekDesc.bNewMonth) {
		m_title = gTextMgr[TRID_MSG_NEWMONTH];
		if (weekDesc.wkType == iWeekDesc::Plague) m_subTitle = gTextMgr[TRID_MSG_MONTHOFPLAGUE];
		else m_subTitle = iFormat(gTextMgr[TRID_MSG_MONTHOF], gTextMgr[TRID_MONTHN_01+weekDesc.fparam]);
	} else {
		m_title = gTextMgr[TRID_MSG_NEWWEEK];
		m_subTitle = iFormat(gTextMgr[TRID_MSG_WEEKOF], gTextMgr[TRID_WEEKN_01+weekDesc.fparam]);
	}

	if (weekDesc.wkType == iWeekDesc::Plague) m_text = gTextMgr[TRID_MSG_DWHALFPOP];
	else m_text = gTextMgr[TRID_MSG_DWINCPOP];

	if (weekDesc.wkType == iWeekDesc::CreatDoubles) m_text2 = iFormat(gTextMgr[TRID_MSG_DOUBLEPOP], gTextMgr[TRID_CREATURE_PEASANT_F3+weekDesc.crType*3]);
	else if (weekDesc.wkType == iWeekDesc::CreatPlus) m_text2 = iFormat(gTextMgr[TRID_MSG_PLUSPOP], gTextMgr[TRID_CREATURE_PEASANT_F3+weekDesc.crType*3]);
}

void iDlg_NewWeek::OnCreateDlg()
{
	iRect clRect = ClientRect();
	gSfxMgr.PlaySound(CSND_NEW_WEEK);
	//AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
}

void iDlg_NewWeek::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();

	// title
	rc.y += gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), m_title, rc);

	// subtitle
	rc.y += gTextComposer.TextBoxOut(dlgfc_stopic, gApp.Surface(), m_subTitle, rc);

	// text 1
	rc.y += gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), m_text, rc);

	// text 2
	if (!m_text2.Empty()) 
		rc.y += gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), m_text2, rc);
}

iSize iDlg_NewWeek::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();


	// title
	h += gTextComposer.GetTextBoxSize(m_title, w, dlgfc_hdr).h;

	// subtitle
	h += gTextComposer.GetTextBoxSize(m_subTitle, w, dlgfc_stopic).h;

	// text 1
	h += gTextComposer.GetTextBoxSize(m_text, w, dlgfc_plain).h;

	// text 2
	if (!m_text2.Empty())
		h += gTextComposer.GetTextBoxSize(m_text2, w, dlgfc_plain).h;

	return iSize(w,h);
}

void iDlg_NewWeek::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	{
		EndDialog(pView->GetUID());
	}
}

void iDlg_NewWeek::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	EndDialog(0);
}

