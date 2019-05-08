#include "stdafx.h"
#include "Ctl_ScenProps.h"
#include "Dlg_ScenProps.h"
//#include "xxc/xxc.security.h"


//////////////////////////////////////////////////////////////////////////
iScenPropsDlg::iScenPropsDlg(iViewMgr* pViewMgr, iMapInfo& scProps)
: iBaseGameDlg(pViewMgr), m_scProps(scProps)
{}

void iScenPropsDlg::OnCreateDlg()
{
	iRect clRect = ClientRect();
    sint32 yp = clRect.y;

    clRect.h -= 20;
    //clRect.DeflateSize(50);
	//AddChild(m_ctrl = new iScenPropsView(m_pMgr, this, m_scProps, clRect, 109) );
}

iSize iScenPropsDlg::ClientSize() const
{
    return iSize(270,150 + DEF_BTN_HEIGHT + 20); 
}

void iScenPropsDlg::DoCompose(const iRect& rect)
{
    iRect rc(rect);
}

void iScenPropsDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
    if (uid == 109)
    {
        // End dialog
        EndDialog(param); 
    }
}

// END OF FILE

