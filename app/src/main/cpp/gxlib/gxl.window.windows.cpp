#include "gxl.inc.h"

#ifdef OS_WINCE
//#include <gx.h>
#include "gapi_help.h"
#endif //OS_WINCE

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

bool Enter();
bool Esc();
bool Space();
bool CastSpell();
bool MiniMap(bool bDown);
void Num( unsigned short ch );
bool AutoCombat( bool bEnd );
bool QuickSave();
bool QuickLoad();
bool OnWait();
bool EndTurn();
bool Tab( bool bDown );
bool SetKeyUpFlag( bool flag );
bool SetKeyDownFlag( bool flag );
bool SetKeyLeftFlag( bool flag );
bool SetKeyRightFlag( bool flag );
bool ExitDlgClick();
void Minimize();
bool ShowLoadScreen();
bool ShowSaveScreen();
void onQuit();
void CheckOverland();
void RightDown();
void RightUp();
bool CastleBuild();
bool CastleRecruit();
bool CastleTavern();
bool CastleMarket();
bool NextPrevCastle( bool bNext );
bool Defender();

#ifdef OS_WINCE
//#	ifdef HPC_JORNADA
//#	else
#		include <aygshell.h>
		const int FMSG = WM_CREATE;
//#	endif
#endif
#ifdef OS_WIN32
	const int FMSG = WM_NCCREATE;
#define WND_STYLE_WINDOWING         WS_OVERLAPPEDWINDOW
#define WND_STYLE_FULLSCREEN        WS_POPUP
#define WND_EXSTYLE_WINDOWING       0
#define WND_EXSTYLE_FULLSCREEN      0//WS_EX_TOPMOST

#endif

//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static iWindow* cls = 0;
	LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
	if (cls == 0) {
		cls = (iWindow*)GetWindowLong(hWnd,GWL_USERDATA);
	}

	if ( cls ) 
    {
		return cls->OnMessage( hWnd, uMsg, wParam, lParam );
	}
    return DefWindowProc( hWnd, uMsg, wParam, lParam );;
}
//////////////////////////////////////////////////////////////////////////
iWindow::iWindow()
: m_hWnd(NULL)
, m_pOwner(NULL)
, m_bActive(false)
, m_bTrack(false)
{
#ifdef OS_WIN32
#endif
}
//////////////////////////////////////////////////////////////////////////
iWindow::~iWindow()
{
	check(!m_hWnd);
}

//////////////////////////////////////////////////////////////////////////
const WCHAR wndClassName[] = L"GXLMWND";

#ifdef OS_WINCE
bool iWindow::CheckSecondInstance()
{
    HWND hWnd = ::FindWindow(wndClassName, NULL);
    if (hWnd)
    {
        SetForegroundWindow(hWnd);
        return true;
    }

    return false;
}
#endif 

bool iWindow::Init(HINSTANCE hInst, LPCWSTR wndName, const iSize& siz, iGXApp* owner )
{
	check(!m_hWnd);
	check(owner != 0);
	m_pOwner = owner;
	// SiGMan: this one is proper (for PPC) app single instance check
	// first check if such window already exists (application is running)
	// NB:: Somehow in the windows application does not have any caption and search fails
	// 2ROBERT: WHY THERE IS NO WINDOW TITLE DISPLAYED???
	//HWND hOtherWnd = ::FindWindow( wndClassName, NULL );
	// this parameter belongs to config actually
    /*
    Cotulla: new code in CheckSecondInstance() above
#ifdef OS_WIN32
	bool allowMultipleInstances = true;
#else
	bool allowMultipleInstances = false;
#endif

	if ( (hOtherWnd != NULL) && !allowMultipleInstances ) {
		SetForegroundWindow( hOtherWnd );
		return false;
	}
    */
	// Create Window	
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, WndProc, 4, 4, hInst, NULL, LoadCursor(NULL,IDC_ARROW), hBrush, NULL, wndClassName };
    wndClass.hInstance = hInst;
	RegisterClass( &wndClass );
	m_Size = siz;
#ifdef OS_WINCE
//	m_hWnd = CreateWindow( wndClassName, wndName, WS_VISIBLE, 0, 0, m_Size.w, m_Size.h, 0L, 0, hInst, 0 );
    m_hWnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, wndClassName, wndName, WS_POPUP | WS_VISIBLE, 0, 0, m_Size.w, m_Size.h, 0L, 0, hInst, 0 );
//#if !defined(HPC_JORNADA)
	//::SHFullScreen( m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON );
	owner->ShellFullscreen( m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON );
//#endif
#endif
#ifdef OS_WIN32
	RECT wrc;
    //m_bFullScreen = m_pOwner->IsModeFullScreen();

    uint32 src_dx = GetSystemMetrics(SM_CXSCREEN);
    uint32 src_dy = GetSystemMetrics(SM_CYSCREEN);

    m_szSaved.cx = m_Size.w;
    m_szSaved.cy = m_Size.h;

	DWORD dwStyle, dwExStyle;// = WS_POPUP;//WS_OVERLAPPED  | WS_CAPTION | WS_SYSMENU;
    if (m_pOwner->IsModeFullScreen())
    {
        dwStyle = WND_STYLE_FULLSCREEN;
        dwExStyle = WND_EXSTYLE_FULLSCREEN;

        wrc.left = 0;
        wrc.top = 0;// + 300;
        wrc.right = src_dx;
        wrc.bottom = src_dy;
    }
    else
    {
        dwStyle = WND_STYLE_WINDOWING; // WS_OVERLAPPED  | WS_CAPTION | WS_SYSMENU;// | WS_OVERLAPPEDWINDOW;
        dwExStyle = WND_EXSTYLE_WINDOWING;

        wrc.left  = (src_dx - m_Size.w) / 2;    
        wrc.top = (src_dy - m_Size.h) / 2;  
        wrc.right = wrc.left + m_Size.w;
        wrc.bottom = wrc.top + m_Size.h;
    }
    
    dwStyle |= WS_VISIBLE;
	AdjustWindowRect(&wrc, dwStyle, FALSE);
		
	m_hWnd = ::CreateWindowEx(dwExStyle, wndClassName, wndName, dwStyle, wrc.left, wrc.top, wrc.right-wrc.left, wrc.bottom-wrc.top, 0L, 0, hInst, 0);
#endif
	if (!m_hWnd) return false;
	SetWindowLong(m_hWnd,GWL_USERDATA,(LONG)this);
	m_bActive = true;
#ifdef OS_WIN32
	ShowWindow(m_hWnd, SW_SHOW);
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////
void iWindow::SetSize(const iSize& siz)
{
	m_Size = siz;
#ifdef OS_WIN32
	//RECT wrc={100,100,100+siz.w,100+siz.h};
	int xofsset = (GetSystemMetrics(SM_CXSCREEN)-siz.w)/2;
	int yofsset = (GetSystemMetrics(SM_CYSCREEN)-siz.h)/2;

	RECT wrc = { xofsset, yofsset, xofsset+siz.w, yofsset+siz.h };

	AdjustWindowRect(&wrc,::GetWindowLong(m_hWnd,GWL_STYLE),FALSE);
	::SetWindowPos(m_hWnd,0,xofsset,yofsset,wrc.right-wrc.left,wrc.bottom-wrc.top,SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);
#endif
}

//////////////////////////////////////////////////////////////////////////
void iWindow::Destroy()
{
	check(m_hWnd);
	DestroyWindow(m_hWnd);
	m_hWnd = NULL;
}
//////////////////////////////////////////////////////////////////////////
LRESULT iWindow::OnMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	POINT pt;
	switch( uMsg ) {
#if 1
		case WM_KILLFOCUS:
			if (m_pOwner) {
#ifndef OS_WIN32
				m_pOwner->Suspend();
#endif
			}
			return 0;
		case WM_SETFOCUS:
			if (m_pOwner) {
				m_pOwner->Resume();
			}
			return 0;
#else
        case WM_ACTIVATE:
            if (wParam != 0)
            {                
                if (m_pOwner) 
                {
                    m_pOwner->Resume();
                }
            }
            else
            {
                if (m_pOwner) 
                {
                    m_pOwner->Suspend();
                }
            }
            break;
#endif

#ifdef OS_WINCE
        // Cotulla: this code for restoring full screen after ActiveSync connection popup 
#define WM_NCACTIVATE                   0x0086
        case WM_NCACTIVATE:
            if (wParam != 0)
            {
                if (m_pOwner) 
                {
                    m_pOwner->ShellFullscreen(hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON );
                }                
            }
            break;
#endif

    /*    case WM_WINDOWPOSCHANGED:
            {
                WINDOWPOS *pwp = (WINDOWPOS*)lParam;
                pwp = pwp;
                NKDbgPrintfW(L"WM_WINDOWPOSCHANGED: %d : %d : %d\r\n", wParam,  
                    GetActiveWindow()  == hWnd, GetForegroundWindow() == hWnd);

            }     
            break; */
		/*
		case WM_ACTIVATE:
			if (m_pOwner) {
				if (LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) {
					OutputDebugString(L"+ WM_ACTIVATE 1\n");
					 m_pOwner->Resume();
				} else if (LOWORD(wParam) == WA_INACTIVE) {
					OutputDebugString(L"- WM_ACTIVATE 0\n");
					m_pOwner->Suspend();
				} else {
					check(0 == "Invalid wParam in WM_ACTIVATE");
				}

			}
			return 0;
		*/
   /*     case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
            {
                NKDbgPrintfW(L"SIZE_MINIMIZED!!!\r\n");
                if (m_pOwner) 
                {
#ifndef OS_WIN32
                    m_pOwner->Suspend();
#endif
                }
            }  
            break;  */
		//case WM_ERASEBKGND:
		//	return 0;
#ifdef OS_WIN32
        case WM_SIZE:
            {
				iSize size = iSize( LOWORD(lParam), HIWORD(lParam) );
				
				if (m_pOwner) m_pOwner->DoSizeChange(size);
            }
            break;  
        case WM_GETMINMAXINFO:
            {
                RECT rc;
                rc.left = 0;
                rc.top = 0;
                rc.right = 1024;                
                rc.bottom = 768;
                DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
                AdjustWindowRect(&rc, dwStyle, FALSE);
                LPMINMAXINFO p = (LPMINMAXINFO)lParam;
                p->ptMinTrackSize.x = rc.right - rc.left;
                p->ptMinTrackSize.y = rc.bottom - rc.top;
            }
            break;


/*        case WM_CREATE:
            CenterWindow();
            UpdateWindow(m_hWnd);
            break;
*/
#endif    

		case WM_PAINT:
			if (m_pOwner && m_pOwner->IsActive()){
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				m_pOwner->Display().msg_OnPaint(hdc);
				EndPaint(m_hWnd,&ps);
			}
			return 0;
		case WM_LBUTTONDOWN:
			if (m_bTrack) return 0;
			SetCapture(hWnd);
			if (m_pOwner) {
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);
				//ClientToScreen(m_hWnd, &pt);
				m_pOwner->Input().msg_OnMouseDown(pt.x,pt.y, 0, 0);
			}
			m_bTrack = true;
			return 0;
		case WM_LBUTTONUP:
			if (!m_bTrack) return 0;
			ReleaseCapture();
			if (m_pOwner) {
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);
				//ClientToScreen(m_hWnd, &pt);
				m_pOwner->Input().msg_OnMouseUp(pt.x, pt.y, 0 ,0);
			}
			m_bTrack = false;
			return 0;
		case WM_RBUTTONDOWN:
			if (m_pOwner){ 
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);
				RightDown();
				m_pOwner->Input().msg_OnMouseRightDown(pt.x, pt.y, 0, 0); //msg_OnKeyDown(VK_CLEAR);
			}
			return 0;
		case WM_RBUTTONUP:
			if (m_pOwner){ 
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);
				RightUp();
				m_pOwner->Input().msg_OnMouseRightUp(pt.x, pt.y, 0, 0);//msg_OnKeyUp(VK_CLEAR);
			}
			return 0;
		case WM_MOUSEMOVE:
			if (m_pOwner && (wParam&MK_LBUTTON)) {
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);
				//ClientToScreen(m_hWnd, &pt);
				m_pOwner->Input().msg_OnMouseMove(pt.x, pt.y, 0, 0);
			}
			else if( m_pOwner ){
			
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);

				m_pOwner->Input().msg_OnMouseEntered(pt.x, pt.y, 0);
			}
			return 0;
		case WM_MOUSEWHEEL:

			short del;
			del = HIWORD(wParam);

			if( del < 0 )
				m_pOwner->Input().msg_OnMouseWheelDown( (sint16)(del/12), 0 );
			else
				m_pOwner->Input().msg_OnMouseWheelDown( (sint16)(del/12), 0 );

			return 0;
		case WM_KEYDOWN:
#ifdef OS_WIN32

			//CheckOverland();
			//Ctrl+S
			if (wParam == VK_F5 || ((wParam == 83) && GetKeyState(VK_CONTROL) & 0x8000))
			{
				if( QuickSave() )
					return 0;
			}//Ctrl+L
			else if (wParam == VK_F9 || ((wParam == 76) && GetKeyState(VK_CONTROL) & 0x8000))
			{
				if( QuickLoad() )
					return 0;
			}//Ctrl+F
			else if (wParam == VK_F11 || (((wParam == 'F') || (wParam == 'f')) && GetKeyState(VK_CONTROL) & 0x8000))
            {
				
				if( m_pOwner->IsFullScreen() )
					m_pOwner->SetFullScreen( false );
				else
					m_pOwner->SetFullScreen( true );
			
                if (m_pOwner) m_pOwner->SwitchScreenMode();
                return 0;
            }//Ctrl+Shift+S       
			else if ( (wParam == 83) && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000) )
			{
				if( ShowSaveScreen() )
					return 0;
			}//Ctrl+Shift+L
			else if ( (wParam == 76) && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000) )
			{
				if( ShowLoadScreen() )
					return 0;
			}
			else if ( (wParam == 77) && ( GetKeyState(VK_CONTROL) & 0x8000 ) )
			{
				//minimize
					return 0;
			}//Market
			else if( wParam == 77 ){
				
				if( CastleMarket() )
					return 0;
			}//Recruit
			else if( wParam == 82 ){
				
				if( CastleRecruit() )
					return 0;
			}//Tavern
			else if( wParam == 84 ){
				
				if( CastleTavern() )
					return 0;
			}//Build
			else if( wParam == 66 ){
				
				if( CastleBuild() )
					return 0;
			}
			//Wait
			else if( wParam == 87 ){
			
				if( OnWait() )
					return 0;
			}
			//AutoCombat
			else if( wParam == 65 ){
			
				if( AutoCombat( false ) )
					return 0;
				if( NextPrevCastle( false ) )
					return 0;
			}//D
			else if( wParam == 68 ){
			
				if( NextPrevCastle( true ) )
					return 0;
				if( Defender() )
					return 0;
			}
			//CastSpell
			else if( wParam == 67 ){
			
				if( CastSpell() )
					return 0;
			}
			//End Turn
			else if( wParam == 69 ){
			
				if( EndTurn() )
					return 0;
			}
			//MiniMap
			else if( wParam == 77 ){
			
				if( MiniMap( true ) )
					return 0;
			}
			//Left
			else if( wParam == 37 ){
			
				SetKeyLeftFlag( true );
				return 0;
			}
			//Up
			else if( wParam == 38 ){
			
				SetKeyUpFlag( true );
				return 0;
			}
			//Right
			else if( wParam == 39){
			
				SetKeyRightFlag( true );
				return 0;
			}
			//Down
			else if( wParam == 40 ){
			
				SetKeyDownFlag( true );
				return 0;
			}
			//Enter
			else if( wParam == 13 ){
			
				if( Enter() )
					return 0;
			}
			//Tab
			else if( wParam == 9 ){
			
				if( Tab(true) )
					return 0;
			}
			//Space
			else if( wParam == 32 ){
			
				if( Space() )
					return 0;
			}
			//Esc
			else if( wParam == 27 ){
			
				if( Esc() )
					return 0;
			}		

			//End AutoCombat
			AutoCombat( true );

			//Num
			Num( wParam );

            // CotullaHACK:
            if (wParam == VK_F9 && GetFileAttributesW(L"E:\\VKDocuments\\My Dropbox") != 0xFFFFFFFF)
            {
                int i;
                iStringT scrndir = _T("E:\\VKDocuments\\My Dropbox\\Testing\\");
                for (i = 1; i < 999; i++)
                {   
                    iStringT fname = scrndir + iFormat(_T("test%03d.bmp"), i);
                    if (!iFile::Exists(fname))
                    {
                        SaveDibBitmap16(m_pOwner->Surface(), fname);                        
                        break;
                    }
                }    
                return 0;
            }

			if( !(GetKeyState(VK_CONTROL) & 0x8000 ) && !(GetKeyState(VK_SHIFT) & 0x8000) )
				ExitDlgClick();
#endif
			if (m_pOwner && (lParam & 0x40000000) == 0) m_pOwner->Input().msg_OnKeyDown(wParam);
			return 0;
		case WM_KEYUP:
#ifdef OS_WIN32
            if (wParam == VK_F11 || wParam == VK_F9)
            {
                return 0;
            }
			//Tab
			else if( wParam == 9 ){
			
				if( Tab(false) )
					return 0;
			}
			//Left
			else if( wParam == 37 ){
			
				SetKeyLeftFlag( false );
				return 0;
			}
			//Up
			else if( wParam == 38 ){
			
				SetKeyUpFlag( false );
				return 0;
			}
			//Right
			else if( wParam == 39){
			
				SetKeyRightFlag( false );
				return 0;
			}
			//Down
			else if( wParam == 40 ){
			
				SetKeyDownFlag( false );
				return 0;
			}
#endif
			if (m_pOwner) m_pOwner->Input().msg_OnKeyUp(wParam);
			return 0;

		case WM_CLOSE:	
			onQuit();
			PostQuitMessage(0); 
			return 0;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////////
void iWindow::SetOwner(iGXApp* pApp)
{
	m_pOwner = pApp;
}

#ifdef OS_WIN32

void iWindow::OnSwitchMode(iSize &siz)
{
    DWORD dw;
    RECT rc;

    if (m_pOwner->IsModeFullScreen())
    {
        GetWindowRect(m_hWnd, &rc);
        m_szSaved.cx = rc.right - rc.left;
        m_szSaved.cy = rc.bottom - rc.top;

        dw = GetWindowLong(m_hWnd, GWL_STYLE);
        dw &= ~WND_STYLE_WINDOWING;
        dw |=  WND_STYLE_FULLSCREEN;
        SetWindowLong(m_hWnd, GWL_STYLE, dw);

        dw = GetWindowLong(m_hWnd, GWL_EXSTYLE);
        dw &= ~WND_EXSTYLE_WINDOWING;
        dw |=  WND_EXSTYLE_FULLSCREEN;
        SetWindowLong(m_hWnd, GWL_EXSTYLE, dw);

        SetWindowPos(m_hWnd, 0, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CXSCREEN), 
                     SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    }
    else
    {
        DWORD dwStyle;
        dw = GetWindowLong(m_hWnd, GWL_STYLE);
        dw &= ~WND_STYLE_FULLSCREEN;
        dw |=  WND_STYLE_WINDOWING;                
        SetWindowLong(m_hWnd, GWL_STYLE, dw);
        dwStyle = dw;

        dw = GetWindowLong(m_hWnd, GWL_EXSTYLE);       
        dw &= ~WND_EXSTYLE_FULLSCREEN;
        dw |=  WND_EXSTYLE_WINDOWING;
        SetWindowLong(m_hWnd, GWL_EXSTYLE, dw);
        
        rc.left = 0;
        rc.right = siz.w;
        rc.top = 0;
        rc.bottom = siz.h;
        AdjustWindowRect(&rc, dwStyle, FALSE);
        SetWindowPos(m_hWnd, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);
        CenterWindow();
    }
}

void iWindow::CenterWindow()
{
    RECT rc;
    sint32 x, y;
    uint32 dx = GetSystemMetrics(SM_CXSCREEN);
    uint32 dy = GetSystemMetrics(SM_CYSCREEN);    

    GetWindowRect(m_hWnd, &rc);
    x = (dx - (rc.right - rc.left)) / 2;    
    y = (dy - (rc.bottom - rc.top)) / 2;    
    SetWindowPos(m_hWnd, 0, x, y, 0, 0, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOSIZE);
}
#endif
