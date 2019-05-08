#include "stdafx.h"
#include "Dlg_RiseSkeletons.h"

iDlg_RiseSkeletons::iDlg_RiseSkeletons(iViewMgr* pViewMgr, PLAYER_ID pid, CREATURE_TYPE ct, sint32 quant)
: iBaseGameDlg(pViewMgr), m_ct(ct), m_quant(quant)
{}

void iDlg_RiseSkeletons::OnCreateDlg()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(DRC_OK);
		return;
	}		
#endif

	iRect clRect = ClientRect();
	//AddChild(new iTextButton(m_pMgr, this, iRect((clRect.x+clRect.w/2)-20, clRect.y2()-DEF_BTN_HEIGHT, 40, DEF_BTN_HEIGHT), TRID_OK, DRC_OK));
}

void iDlg_RiseSkeletons::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();
	// title
	rc.y += gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(),gTextMgr[TRID_HSKILL_NECROMANCY], rc);
	
	// text	
	rc.y += gTextComposer.TextBoxOut(dlgfc_splain, gApp.Surface(), iFormat(gTextMgr[TRID_MSG_RISE_SKELETONS], m_quant, gTextMgr[TRID_CREATURE_PEASANT_F3+m_ct*3]), rc);	

	// icon and quantity
	ComposeCreatureCell(gApp.Surface(), iRect(iPoint((rc.x+rc.w/2)-18, rc.y), CreatureCellSize()), m_ct, m_quant, VL_EXPERT);
}

iSize iDlg_RiseSkeletons::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();

	// title
	h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_HSKILL_NECROMANCY], w, dlgfc_hdr).h;
	// text
	h += gTextComposer.GetTextBoxSize(iFormat(gTextMgr[TRID_MSG_RISE_SKELETONS], m_quant, gTextMgr[TRID_CREATURE_PEASANT_F3+m_ct*3]), w, dlgfc_splain).h;	
	
	// icon and quantity
	h += 42;	

	return iSize(w,h);
}

void iDlg_RiseSkeletons::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == DRC_OK) {
		EndDialog(DRC_OK);
	}
}

void iDlg_RiseSkeletons::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {
	EndDialog(DRC_OK);
}

