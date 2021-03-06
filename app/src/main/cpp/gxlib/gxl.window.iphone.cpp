#include "gxl.inc.h"

#include "gxl.ctr.array.h"
#include "gxl.ctr.cbuff.h"
#include "gxl.window.h"
#include "gxl.input.h"
#include "gxl.timer.h"
#include "gxl.display.h"
#include "gxl.view.h"
#include "gxl.dialog.h"
#include "gxl.viewmgr.h"
#include "gxl.application.h"

//////////////////////////////////////////////////////////////////////////
LRESULT  WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
}
//////////////////////////////////////////////////////////////////////////
iWindow::iWindow()
: m_hWnd(NULL)
, m_pOwner(NULL)
, m_bActive(false)
, m_bTrack(false)
{
}
//////////////////////////////////////////////////////////////////////////
iWindow::~iWindow()
{
	check(!m_hWnd);
}

//////////////////////////////////////////////////////////////////////////
const WCHAR wndClassName[] = L"GXLMWND";

bool iWindow::Init(const iSize& siz, iGXApp* owner )
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
void iWindow::SetSize(const iSize& siz)
{
	m_Size = siz;
}

//////////////////////////////////////////////////////////////////////////
void iWindow::Destroy()
{
	check(m_hWnd);
	m_hWnd = NULL;
}
//////////////////////////////////////////////////////////////////////////
LRESULT iWindow::OnMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
}

//////////////////////////////////////////////////////////////////////////
void iWindow::SetOwner(iGXApp* pApp)
{
	m_pOwner = pApp;
}
