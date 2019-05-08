#include "stdafx.h"
#include "Dlg_SplitCreatures.touch.h"


iDlg_SplitCreatures::iDlg_SplitCreatures(iViewMgr* pViewMgr, PLAYER_ID pid, iArmyOwner* own1, sint32 idx1, iArmyOwner* own2, sint32 idx2, uint32 flags )
: iBaseGameDlg(pViewMgr), m_own1(own1), m_idx1(idx1), m_own2(own2), m_idx2(idx2), m_flags(flags)
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
	clrc.y += iBaseGameDlg::GetTopOffset();

	// Slider
	AddChild(m_pAmount = new iAmountChooserGlow(m_pMgr, this, AlignChild(iAmountChooserGlow::sm_defSize, AlignCenter, clrc.y), 101
//#ifndef PC_VERSION
                                                , true
//#endif
                                                ));
	m_pAmount->UpdateSize();
	
	clrc.y += 168;

	uint32 slen = m_total + 1;
	if (m_flags&SafeG1) slen--;
	if (m_flags&SafeG2) slen--;
	m_pAmount->SetMax(slen - 1);

	sint32 npos = m_total-m_left;
	if (m_flags&SafeG2) npos--;
	m_pAmount->SetCurPos(npos);
	// Button size 40x15 (3*40+10)
	
    sint32 bw = iIphoneBtn::sm_defSize.w;
    sint32 hw = 
#ifdef PC_VERSION
                iIphoneBtn::sm_defSize.h;
#else
                0;
#endif
    AddChild(m_pBtnOk = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSize, AlignBottom) + iPoint(bw, -hw), DRC_OK, PDGG(BTN_OK_DELUXE), -1));
    AddChild(m_pBtnCancel = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSize, AlignBottom) + iPoint(-bw, -hw), DRC_CANCEL, PDGG(BTN_CANCEL_DELUXE), -1));
}
#ifdef PC_VERSION
void iDlg_SplitCreatures::UpdateSize(){

    Center();
    
    iRect clrc = ClientRect();
	clrc.y += 15;
    
    m_pAmount->SetRect( AlignChild(iAmountChooserGlow::sm_defSize, AlignCenter, clrc.y) );
    
    sint32 bw = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_OK], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_CANCEL], btnfc_normal).w) + 30;
    sint32 hw = iIphoneBtn::sm_defSize.h;
	m_pBtnOk->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, -hw) );	
	m_pBtnCancel->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(-bw, -hw) );
}
#endif
void iDlg_SplitCreatures::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();

	// title
	rc.y += gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), iFormat(_T("%s %s"), gTextMgr[TRID_SPLIT], gTextMgr[m_own1->Army().At(m_idx1).Type()*3+TRID_CREATURE_PEASANT_F3]), rc);
	
	ComposeCreatureCell(gApp.Surface(), AlignRect(CreatureCellSize(), m_Rect, AlignLeft) 
#ifdef PC_VERSION
                        + iPoint(25, 20),
#else
                        + iPoint(34, -60),
#endif
                        m_own1->Army().At(m_idx1).Type(), m_left, VL_EXPERT);
    ComposeCreatureCell(gApp.Surface(), AlignRect(CreatureCellSize(), m_Rect, AlignRight) 
#ifdef PC_VERSION
                        + iPoint(-25, 20), 
#else
                        + iPoint(-34, -60), 
#endif
                        m_own1->Army().At(m_idx1).Type(), m_total - m_left, VL_EXPERT);
}

iSize iDlg_SplitCreatures::ClientSize() const
{
	sint32 w = iMIN<sint32>(gApp.Surface().GetWidth(), iBaseGameDlg::GetDefaultWidth() + 50);
#ifdef PC_VERSION
    w = iMIN<sint32>(w, 1024 - 200);
#endif
	sint32 h = iBaseGameDlg::GetTopOffset();

	// title
	h += gTextComposer.GetTextBoxSize(iFormat(_T("%s %s"), gTextMgr[TRID_SPLIT], gTextMgr[m_own1->Army().At(m_idx1).Type()*3+TRID_CREATURE_PEASANT_F3]), w, dlgfc_hdr).h;

	h += 168; // slider

	// buttons
	h += 
#ifndef PC_VERSION
    DEF_BTN_HEIGHT;
#else
    2*DEF_BTN_HEIGHT;
#endif

	return iSize(w, h);	
}
#if defined( PC_VERSION )
void iDlg_SplitCreatures::OnClickYes(){

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
}
#endif

void iDlg_SplitCreatures::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == 101) {
		m_left = m_total - m_pAmount->CurPos();		
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

void iDlg_SplitCreatures::OnMouseWheelDown(sint16 line, MouseId mID){

    sint32 pos = m_pAmount->CurPos();
    pos += iSIGN(line);
    
    m_pAmount->SetCurPos(pos);
    
    m_left = m_total - m_pAmount->CurPos();		
    if (m_flags&SafeG2) m_left--;
    Invalidate();
}
