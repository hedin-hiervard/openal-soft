#include "stdafx.h"
#include "Dlg_LeaveGuards.h"
#include "ArmyList.touch.h"
#include "CastleView.h"

iDlg_LeaveGuards::iDlg_LeaveGuards(iViewMgr* pViewMgr, iArmyOwner* pMainOwner, iArmyOwner* pLeaveBehind, TextResId title)
: iBaseGameDlg(pViewMgr), m_pMainOwner(pMainOwner), m_pLeaveBehind(pLeaveBehind), m_title(title)
{}

void iDlg_LeaveGuards::OnCreateDlg()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(DRC_OK);
		return;
	}		
#endif
	iRect clRect = ClientRect();
	
	AddChild(m_pArmyExchange = new iArmyExchange(&gApp.ViewMgr(), this, iRect(), 100, true, true));	
	m_pArmyExchange->SetBottomOwner(m_pMainOwner, false);
	m_pArmyExchange->SetTopOwner(m_pLeaveBehind, false);


//	sint32 txtoff = gTextComposer.GetTextSize(gTextMgr[m_title], dlgfc_hdr).h;	
    iSize txtsize = gTextComposer.GetTextSize(gTextMgr[m_title], dlgfc_hdr);
    sint32 txtoff;
    
    if( clRect.w > txtsize.w )    
        txtoff = gTextComposer.GetTextSize(gTextMgr[m_title], dlgfc_hdr).h;	
    else
        txtoff = gTextComposer.GetTextSize(gTextMgr[m_title], dlgfc_hdr).h*2;
    
	m_pArmyExchange->SetRect(AlignChild(m_pArmyExchange->GetDefSize(2), AlignTop) + iPoint(0, txtoff));

	m_pArmyExchange->UpdateSize();	


	sint32 bw = gTextComposer.GetTextSize(gTextMgr[TRID_OK], dlgfc_hdr).w + 30;
//	AddChild(new iTextButton(m_pMgr, this, AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom), TRID_OK, DRC_OK));
    sint32 hw = 
#ifdef PC_VERSION
    iIphoneBtn::sm_defSize.h;
#else
    0;
#endif
    
    AddChild( m_pBtnOk = new iImgBtn(m_pMgr, this, AlignChild(iIphoneBtn::sm_defSize, AlignBottom)+ iPoint(0, -hw), DRC_OK, PDGG(BTN_OK_DELUXE), -1) );
}

#ifdef PC_VERSION
void iDlg_LeaveGuards::UpdateSize(){

    Center();
    
    iRect clRect = ClientRect();
    
    iSize txtsize = gTextComposer.GetTextSize(gTextMgr[m_title], dlgfc_hdr);
    sint32 txtoff;
    
    if( clRect.w > txtsize.w )    
        txtoff = gTextComposer.GetTextSize(gTextMgr[m_title], dlgfc_hdr).h;	
    else
        txtoff = gTextComposer.GetTextSize(gTextMgr[m_title], dlgfc_hdr).h*2;
	
	m_pArmyExchange->SetRect(AlignChild(m_pArmyExchange->GetDefSize(2), AlignTop) + iPoint(0, txtoff));
    
    sint32 hw = iIphoneBtn::sm_defSize.h;
    m_pBtnOk->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottom)+ iPoint(0, -hw));
}
#endif
void iDlg_LeaveGuards::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);	
	// title
	gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), gTextMgr[m_title], rc);	
}

iSize iDlg_LeaveGuards::ClientSize() const
{

	sint32 w = iArmyExchange::GetDefSize(2).w + 15;
    w = iCLAMP<sint32>(iBaseGameDlg::GetDefaultWidth(), gApp.Surface().GetWidth(), w);	
    sint32 h = iBaseGameDlg::GetTopOffset();


	// title
	h += gTextComposer.GetTextBoxSize(gTextMgr[m_title], w, dlgfc_hdr).h;

	h += iArmyExchange::GetDefSize(2).h;

	// buttons
	h += DEF_BTN_HEIGHT;

	return iSize(w,h);
}

void iDlg_LeaveGuards::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == DRC_OK) {
		EndDialog(DRC_OK);
	}
}


