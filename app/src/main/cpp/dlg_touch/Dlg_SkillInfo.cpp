#include "stdafx.h"
#include "Dlg_SkillInfo.h"

iSecSkillInfoDlg::iSecSkillInfoDlg(iViewMgr* pViewMgr, SECONDARY_SKILLS skill, SECSKILL_LEVEL level, PLAYER_ID pid)
: iBaseGameDlg(pViewMgr), m_skill(skill), m_level(level)
{
}

void iSecSkillInfoDlg::OnCreateDlg()
{
	iRect clRect = ClientRect();
	//AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
}

void iSecSkillInfoDlg::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();
	const iStringT& skName = gTextMgr[TRID_HSKILL_ESTATES+m_skill];
	const iStringT& skLevel = gTextMgr[TRID_HSKILL_LEVEL_BASIC+m_level];
	const iStringT& skDesc = gTextMgr[TRID_HSKILL_ESTATES_BAS_DESC + (m_skill*SSLVL_COUNT) + m_level];

	// Skill name
	if (!skName.Empty()) {
		rc.y += gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), skName, rc);
		
	}
	// Level
	rc.y += gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(), skLevel, rc);
	
	// Icon
	iRect orc(rc.x + (rc.w/2 - 18), rc.y, 36, 30);
	gApp.Surface().Darken25Rect(orc);
	ButtonFrame(gApp.Surface(),orc,0);
	BlitIcon(gApp.Surface(), PDGG(SEC_SKILLS) + m_skill, orc);
	orc.InflateRect(1);
	FrameRoundRect(gApp.Surface(), orc, cColor_Black);
	rc.y += 42;


	// Description
	if (!skDesc.Empty()) {
		rc.y += gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), skDesc, rc);			
	}
}

iSize iSecSkillInfoDlg::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	const iStringT& skName = gTextMgr[TRID_HSKILL_ESTATES+m_skill];
	const iStringT& skLevel = gTextMgr[TRID_HSKILL_LEVEL_BASIC+m_level];
	const iStringT& skDesc = gTextMgr[TRID_HSKILL_ESTATES_BAS_DESC + (m_skill*SSLVL_COUNT) + m_level];

	sint32 h = iBaseGameDlg::GetTopOffset();

	// Skill name
	if (!skName.Empty()) {
		h += gTextComposer.GetTextBoxSize(skName, w, dlgfc_hdr).h;		
	}

	// Level
	h += gTextComposer.GetTextBoxSize(skLevel, w, dlgfc_topic).h;

	// Icon
	h += 42;

	// Description
	if (!skDesc.Empty()) {
		h += gTextComposer.GetTextBoxSize(skDesc, w, dlgfc_plain).h;
				
	}
	
	return iSize(w, h);
}

void iSecSkillInfoDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	EndDialog(uid);
}

void iSecSkillInfoDlg::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) 
{
	EndDialog(0);
}
