#include "stdafx.h"
#include "RMG.h"
#include "Dlg_RandomMap.h"


iRandomMapDlg::iRandomMapDlg(iViewMgr* pViewMgr):
iBaseGameDlg(pViewMgr)
{

}

iRandomMapDlg::~iRandomMapDlg()
{

}

void iRandomMapDlg::OnCreateDlg()
{
	iRect clRect = ClientRect();

	// Buttons
	sint32 npos = clRect.x + (clRect.w/2-80);
	AddChild(new iTextButton(m_pMgr,this,iRect(npos,clRect.y2()-DEF_BTN_HEIGHT,50,DEF_BTN_HEIGHT),TRID_OK, DRC_OK,iButton::Visible|Enabled));	
	AddChild(new iTextButton(m_pMgr,this,iRect(npos+110,clRect.y2()-DEF_BTN_HEIGHT,55,DEF_BTN_HEIGHT),TRID_CANCEL, DRC_CANCEL));
}

void iRandomMapDlg::DoCompose(const iRect& rect)
{

}

iSize iRandomMapDlg::ClientSize() const
{
	return iSize(200, 150);
}

void iRandomMapDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == DRC_OK || uid == DRC_CANCEL){
		EndDialog(uid); 
	}
}
	
