#include "stdafx.h"
#include "Dlg_SplitCreatures.h"


iDlg_SplitCreatures::iDlg_SplitCreatures(iViewMgr* pViewMgr, PLAYER_ID pid, iArmyOwner* own1, sint32 idx1, iArmyOwner* own2, sint32 idx2, uint32 flags )
: iBaseGameDlg(pViewMgr,pid), m_own1(own1), m_idx1(idx1), m_own2(own2), m_idx2(idx2), m_flags(flags)
{ 
	m_total = m_own1->Army().At(m_idx1).Count();
	if (m_own2->Army().At(m_idx2).IsValid()) m_total += m_own2->Army().At(m_idx2).Count();
	m_left = m_own1->Army().At(m_idx1).Count();
}

void iDlg_SplitCreatures::OnCreateDlg()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(DRC_OK);
		return;
	}		
#endif

	iRect clrc = ClientRect();
	// Slider
	AddChild(m_pSlider = new iPHScrollBar(m_pMgr, this, iRect(clrc.x + (clrc.w/2-65), clrc.y2()-20-20,130,15), 101, iScrollBar::Horizontal));
	uint32 slen = m_total + 1;
	if (m_flags&SafeG1) slen--;
	if (m_flags&SafeG2) slen--;
	m_pSlider->SetMetrics(slen,1);

	sint32 npos = m_total-m_left;
	if (m_flags&SafeG2) npos--;
	m_pSlider->SetCurPos(npos);
	// Button size 40x15 (3*40+10)
	iRect rc(clrc.x + (clrc.w/2-45), clrc.y2()-DEF_BTN_HEIGHT, 40, DEF_BTN_HEIGHT);

	AddChild(new iTextButton(m_pMgr,this,rc,TRID_OK, DRC_OK));
	rc.x+= rc.w + 5;
	AddChild(new iTextButton(m_pMgr,this,rc,TRID_CANCEL, DRC_CANCEL));
}

void iDlg_SplitCreatures::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);

	// title
	gTextComposer.TextOut(dlgfc_hdr, gApp.Surface(), rc.point(), iFormat(_T("%s %s"), gTextMgr[TRID_SPLIT], gTextMgr[m_own1->Army().At(m_idx1).Type()*3+TRID_CREATURE_PEASANT_F3]), rc, AlignTop);
	rc.DeflateRect(0,20,0,0);

	iRect orc(rc.x + (rc.w/2-65), rc.y,130,46);
	gTextComposer.TextOut(dlgfc_topic, gApp.Surface(), orc.point(), FormatNumber(m_left), iRect(orc.x,orc.y,42,46), AlignBottom);
	gTextComposer.TextOut(dlgfc_topic, gApp.Surface(), orc.point(), FormatNumber(m_total-m_left), iRect(orc.x+42+46,orc.y,42,46), AlignBottom);
	BlitIcon(gApp.Surface(), PDGG_MINIMON + m_own1->Army().At(m_idx1).Type(), iRect(orc.x + 42,orc.y,46,46));
}

iSize iDlg_SplitCreatures::ClientSize() const
{
	return iSize(180, 100 + DEF_BTN_HEIGHT);
}

void iDlg_SplitCreatures::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == 101) {
		m_left = m_total - m_pSlider->CurPos();
		if (m_flags&SafeG2) m_left--;
	} else {
		if (uid == DRC_OK) {
			if (m_left == m_total) {
				m_own1->SetArmyGroupCount(m_idx1, m_total);
				m_own2->ResetArmyGroup(m_idx2);
			} else if (!m_left) {
				m_own2->SetArmyGroupType(m_idx2, m_own1->Army().At(m_idx1).Type());
				m_own2->SetArmyGroupCount(m_idx2, m_total);
				m_own1->ResetArmyGroup(m_idx1);
			} else {
				m_own2->SetArmyGroupType(m_idx2, m_own1->Army().At(m_idx1).Type());
				m_own2->SetArmyGroupCount(m_idx2, m_total - m_left);
				m_own1->SetArmyGroupCount(m_idx1, m_left);
			}
			EndDialog(DRC_OK);
		} else if (uid == DRC_CANCEL) {
			EndDialog(DRC_CANCEL);
		}
	}
}


