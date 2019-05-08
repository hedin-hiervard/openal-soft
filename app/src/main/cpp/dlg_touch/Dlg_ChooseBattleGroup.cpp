#include "stdafx.h"
#include "Dlg_ChooseBattleGroup.h"
#include "stdafx.h"

iDlg_ChooseBattleGroup::iBattleGroupBtn::iBattleGroupBtn( iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, uint32 uid, iBattleGroup *grp, iStringT text ) :
iButton(pViewMgr, pCmdHandler, iRect(pos.x, pos.y, 45, 45), uid, Visible|Enabled), m_group(grp), m_text(text)
{
}

void iDlg_ChooseBattleGroup::iBattleGroupBtn::OnCompose()
{
	iRect rc = GetScrRect();		
	ComposeCreatureButtonWithText(gApp.Surface(), rc, m_group->Type(), m_text, VL_EXPERT, false, false,false);
}


iDlg_ChooseBattleGroup::iDlg_ChooseBattleGroup( iViewMgr* pViewMgr, iSimpleArray<iBattleGroup *> list, iSimpleArray<iStringT> texts, PLAYER_ID pid, uint32* idx ):
iBaseGameDlg(pViewMgr), m_list(list), m_idx(idx), m_texts(texts)
{
}

void iDlg_ChooseBattleGroup::OnCreateDlg()
{
	iRect clrc = ClientRect();
	clrc.y += iBaseGameDlg::GetTopOffset();
	clrc.y += gTextComposer.GetTextBoxSize(gTextMgr[TRID_CHOOSE_BATTLE_GROUP_MESSAGE], clrc.w, dlgfc_topic).h;

	sint32 offset = clrc.w / (m_list.GetSize()+1);
	for (uint32 xx=0; xx<m_list.GetSize(); xx++) {	
		iBattleGroupBtn *btn = new iBattleGroupBtn(m_pMgr, this, iPoint(clrc.x + (xx+1) * offset, clrc.y), 100 + xx, m_list[xx], m_texts[xx]);
		m_buttons.Add(btn);
		AddChild(m_buttons[xx]);
	}	
	
    AddChild(m_pBtnCancel = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSize, AlignBottom), DRC_CANCEL, PDGG(BTN_CANCEL_DELUXE), -1));
}
#ifdef PC_VERSION
void iDlg_ChooseBattleGroup::UpdateSize(){

    iRect clrc = ClientRect();
	clrc.y += 15;
	clrc.y += gTextComposer.GetTextBoxSize(gTextMgr[TRID_CHOOSE_BATTLE_GROUP_MESSAGE], clrc.w, dlgfc_topic).h;
    
	sint32 offset = clrc.w / (m_list.GetSize()+1);
	for (uint32 xx=0; xx<m_list.GetSize(); xx++) {	
        
		iPoint pos = iPoint(clrc.x + (xx+1) * offset, clrc.y);
		m_buttons.At(xx)->SetRect( iRect(pos.x, pos.y, 45, 45) );
		AddChild(m_buttons[xx]);
	}	
	
	m_pBtnCancel->SetRect( AlignChild(iSize(gTextComposer.GetTextSize(gTextMgr[TRID_CANCEL], btnfc_normal).w + 30, DEF_BTN_HEIGHT), AlignBottom) );
}
#endif
iSize iDlg_ChooseBattleGroup::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();

	// message
	h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_CHOOSE_BATTLE_GROUP_MESSAGE], w, dlgfc_topic).h;

	// icons
	h += 45;

	h += iBaseGameDlg::GetTopOffset();

	// cancel button
	h += DEF_BTN_HEIGHT;

	return iSize(w, h); 
}

void iDlg_ChooseBattleGroup::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();	
	if(uid == DRC_CANCEL) {
		*m_idx = -1;
		EndDialog(DRC_CANCEL);
	} else {
		*m_idx = uid - 100;
		EndDialog(DRC_OK);
	}
}

void iDlg_ChooseBattleGroup::DoCompose( const iRect& rect )
{
	iRect clrc(rect);
	clrc.y += iBaseGameDlg::GetTopOffset();
	clrc.y += gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(), gTextMgr[TRID_CHOOSE_BATTLE_GROUP_MESSAGE], clrc);
}


