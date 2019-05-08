#include "gxl.inc.h"
#ifdef OS_WINCE
//#include <gx.h>
#include "gapi_help.h"
#endif //OS_WINCE
#include "gxl.ctr.cbuff.h"
#include "gxl.ctr.array.h"
#include "gxl.window.h"
#include "gxl.input.h"
#include "gxl.timer.h"
#include "gxl.display.h"
#include "gxl.view.h"
#include "gxl.dialog.h"
#include "gxl.viewmgr.h"
#include "gxl.application.h"

iDialog::iDialog(iViewMgr* pViewMgr)
: iView(pViewMgr, iRect(0, 0, 0, 0), MODAL_DIALOG, 0, Enabled), m_retCode(DRC_UNDEFINED) { }

sint32 iDialog::DoModal()
{
	Center();
	OnCreateDlg();
	SetVisible();
	m_pMgr->PushModalDlg(this);	
	iGXApp::Instance().OnModalDlgAdded();	
	while (m_pMgr->App()->Cycle() && m_retCode == DRC_UNDEFINED) {}
	// this seems to fix the invalid object mouseup/down/track being called eventually due to local object address being stored
	iGXApp::Instance().ViewMgr().ReleaseViewCapture();
	iGXApp::Instance().OnModalDlgRemoved();
	iDialog* pDlg = m_pMgr->PopModalDlg();
	check(pDlg == this);
	return m_retCode;
}

void iDialog::Center()
{
	iRect rect = AlignRect(GetDlgMetrics(), m_pMgr->Metrics(), AlignCenter);
	OnPlace(rect);
	SetRect(rect);
}

bool iDialog::EndDialog(sint32 retCode)
{
	if (m_retCode != DRC_UNDEFINED)	return false;
	m_retCode = retCode;
	return true;
}

void iDialog::Invalidate()
{
	m_bNeedRedraw = true;
	if (m_pMgr->CurView())  m_pMgr->CurView()->Invalidate();
}

void iDialog::UpdateSize()
{
    Center();
}

