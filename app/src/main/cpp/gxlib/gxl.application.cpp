/*#ifdef OS_WIN32
#include <windows.h>
#endif
*/

#ifdef OS_WINCE
//#	ifdef HPC_JORNADA
//#	else
#		include <aygshell.h>
//#	endif
//#include <gx.h>
#include "gapi_help.h"
#endif //OS_WINCE
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

#if defined( OS_APPLE )
#include "iphone_common.h"
#include <pthread.h>
#endif

#ifdef OS_WIN32
#include "gxl.inc.h"
#endif

#ifdef OS_WIN32
// ceGCC SDK lacks this define
void WINAPI	SystemIdleTimerReset( void );

typedef void (WINAPI *sitr_fn)(void);
#endif

#if defined( OS_MACOS )
#import <pthread.h>

pthread_mutex_t suspend_mutex;
pthread_mutex_t processing_mutex;
pthread_mutex_t exit_mutex;
extern pthread_mutex_t initMutex;
extern pthread_mutex_t backbufMutex;
#endif
//
iGXApp::iGXApp()
: m_viewMgr(this)
, m_pGame(NULL)
, m_bInited(false)
, m_bSuspended(false)
, m_bExit(false)
, m_CycleDelay(60)
, m_processTimer(0)
{
#ifdef OS_WIN32
    m_mode_fullscreen = false;
	m_bFullScreen = true;
#endif
}

//
iGXApp::~iGXApp()
{
	check(!m_bInited);
}

//
#if defined(OS_APPLE) || defined(OS_ANDROID)
bool iGXApp::Init(IGame* pGame, uint32 cdelay, uint32 flags, iSize baseSize)
#else
bool iGXApp::Init(HINSTANCE hInst, LPCWSTR appName, IGame* pGame, uint32 cdelay, uint32 flags, iSize baseSize)
#endif
{
	check(!m_bInited);
	InitMath();
	m_pGame = pGame;
	m_CycleDelay = cdelay;
	m_Flags = flags;
#if (defined(OS_WINCE))
    m_BaseMetrics = iSize(320, 240);
    iSize scrsiz(320, 240);
    iSize win_siz(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
#elif defined(OS_WIN32)
    iSize osiz;
    iSize scrsiz;

    m_mode_fullscreen = flags & GXLF_FULLSCREEN ? true : false;

    m_saved_size = baseSize.Swap();
    m_cur_size = m_saved_size;
    osiz = m_cur_size;

    if (m_mode_fullscreen)
    {
        // full screen mode
        scrsiz = iSize(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        osiz = scrsiz;
        if (flags & GXLF_DOUBLESIZE)
        {
            scrsiz.w /= 2;
            scrsiz.h /= 2;
        }
    }
    else
    {
        // windowing mode
        scrsiz = osiz;
        if (m_Flags & GXLF_DOUBLESIZE)
        {
            osiz.w *= 2;
            osiz.h *= 2;
        }
    }
#elif defined (OS_APPLE) || defined(OS_ANDROID)
	m_BaseMetrics = baseSize;
	iSize scrsiz(baseSize);
	iSize osiz(baseSize);

	if (flags & GXLF_DOUBLESIZE)
	{
		scrsiz.w /= 2;
		scrsiz.h /= 2;
	}

	//The game must default run in fullscreen.
	m_bFullScreen = true;
#endif

	//bool bOk = false;
	do {
#ifdef OS_WIN32
		if (!m_Window.Init(hInst,appName,osiz, this)) break;
#else
		if (!m_Window.Init(osiz, this)) break;
#endif
		if (!m_Input.Init(osiz,flags)) break;
		if (!m_Timer.Init()) break;
		if (!m_Display.Init(m_Window, scrsiz, flags)) break;
		if ( (m_Flags & GXLF_ENABLESOUND) && !m_SoundPlayer.Init() ) break;
		m_Window.SetOwner(this);
		m_bInited = true;
	} while(0);


	if (!m_bInited) {
		m_SoundPlayer.Close();
		m_Display.Destroy();
		m_Input.Destroy();
		m_Window.Destroy();
	}

#ifdef OS_WINCE
/*    m_Display.SwitchResolution(true);
    m_Display.SwitchResolution(false);

    //m_Display.SetOrientation(false, true);
    m_Display.SetOrientation(false, false);
*/
/*  this can possibly cause bugs in video-output

	DEVMODE dev;
	ZeroMemory(&dev, sizeof(dev));
	dev.dmSize = sizeof(DEVMODE);
	dev.dmFields = DM_DISPLAYORIENTATION;
	dev.dmDisplayOrientation = DMDO_0;
	LONG res = ChangeDisplaySettingsEx(NULL, &dev, 0, 0, NULL);
*/
	m_coreDLL= LoadLibrary( L"COREDLL.DLL" );
	m_sysidletimer = (void*)GetProcAddress( m_coreDLL, L"SystemIdleTimerReset" );
	m_aygDLL = LoadLibrary( L"AYGSHELL.DLL" );
	m_shfullscreen = 0;
	if ( m_aygDLL != 0 ) {
		m_shfullscreen = (void*)GetProcAddress( m_aygDLL, L"SHFullScreen" );
	}
#endif
#if defined( OS_MACOS )
	pthread_mutex_init(&suspend_mutex, NULL);
    pthread_mutex_init(&processing_mutex, NULL);
    pthread_mutex_init(&exit_mutex, NULL);

#endif
	return m_bInited;
}

void iGXApp::Destroy()
{
	check(m_bInited);
#ifdef OS_WIN32
	FreeLibrary( m_coreDLL );
	FreeLibrary( m_aygDLL );
#endif
	m_Display.Destroy();
	m_Input.Destroy();
	m_Window.Destroy();
	m_pGame = NULL;
	m_bInited = false;
}

void iGXApp::SetOrientation(bool bLandscape, bool bLeftHander)
{
	if (bLandscape && (m_Flags & GXLF_LANDSCAPE) == 0) m_Flags |= GXLF_LANDSCAPE;
	else if (!bLandscape && (m_Flags & GXLF_LANDSCAPE) != 0) m_Flags ^= GXLF_LANDSCAPE;

#ifdef OS_WIN32
	iSize csiz = m_Window.GetSize();
	iSwap(csiz.w,csiz.h);
	m_Window.SetSize(csiz);
#endif //OS_WIN32

	m_Input.SetOrientation(bLandscape, bLeftHander);
	m_Display.SetOrientation(bLandscape, bLeftHander);
}

void iGXApp::ToggleOrientation()
{
	SetOrientation(!IsLandscape(), (m_Flags&GXLF_LHANDER) != 0);
}

void display_iphone_end_draw();

void RenderFPS(DWORD count);

void iGXApp::ForceCompose()
{
	iRect rc;
	m_viewMgr.Compose(rc);
	m_Display.DoPaint(m_Display.GetSurface().GetSize());
}

//////////////////////////////////////////////////////////////////////////
sint32 iGXApp::Cycle()
{
#if defined( OS_MACOS )
    if(pthread_mutex_trylock(&exit_mutex)) // we are exiting the game, unlock the mutex and fail
    {
        pthread_mutex_unlock(&processing_mutex);
        return true;
    } else {
        pthread_mutex_unlock(&exit_mutex); // release the locked mutex to give change for the destructor to take it
    }
#endif
	if(m_bExit) {
		Destroy();
#if defined( OS_MACOS )
        pthread_mutex_unlock(&processing_mutex);
#endif
		exit(0);
	}

	if(m_bSuspended) {
#if defined( OS_MACOS )
		usleep(30000);
#endif
		return true;
	}

#ifdef OS_WIN32
  MSG  msg;
  if (::PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE )) {
    while (::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE)) {
		if ( msg.message == WM_QUIT ) {
			this->Exit(0);
			return false;
		}
      DispatchMessage( &msg );
    }
  }
#endif

  while (!m_bSuspended && m_Input.EntryCount()) m_viewMgr.ProcessMessage(m_Input.Get());
    uint32 step = m_Timer.GetStep();

#ifdef OS_WINCE
	static uint32 pwrTimeout = 0;
	pwrTimeout += step;
	if (pwrTimeout >= 10000) {
		pwrTimeout = 0;
		//SystemIdleTimerReset();
		((void (WINAPI *)(void))m_sysidletimer)();
	}
#endif //OS_WINCE

#if defined( OS_MACOS )
    pthread_mutex_trylock(&processing_mutex); // this is "soft-lock" to prevent destructor from destroying data while we are processing
#endif
	if (!m_bSuspended) {

        if(m_pGame)
            m_pGame->Process(fix32((sint32)step)/1000L);
		m_viewMgr.Process(step, fix32((sint32)step)/1000L);

		if ( 1 || (step + m_processTimer) > m_CycleDelay ) {
			m_processTimer = 0;
			iRect rc;
#if defined( OS_MACOS )
            pthread_mutex_lock(&backbufMutex);
#endif
			m_viewMgr.Compose(rc);
#if defined( OS_MACOS )
			pthread_mutex_unlock(&backbufMutex);
#endif
#if 0
			static DWORD dwStartTime = 0, dwEndTime = 0, dwCurTime = 0, dwFrame = 0, dwLastFrame = 0;
			dwFrame++;
			dwCurTime = GetTickCount();
			if (dwCurTime - dwStartTime > 1000)
			{
				dwStartTime = dwCurTime;
				dwLastFrame = dwFrame;
				dwFrame = 0;
			}
			RenderFPS(dwLastFrame);
#endif

			/*if (!rc.IsEmpty()) */
//			pthread_mutex_lock(&suspend_mutex);

			m_Display.DoPaint(iRect());

//			pthread_mutex_unlock(&suspend_mutex);

		} else m_processTimer += step;

    /*    iRect rc;
        m_viewMgr.Compose(rc);
        m_Display.DoPaint(rc);
        dwFrame++;
        dwCurTime = GetTickCount();
        if (dwCurTime - dwStartTime > 1000)
        {
            //NKDbgPrintfW(L"FPS: %d\n", dwFrame);
            dwStartTime = dwCurTime;
			dwFPS = dwFrame;
            dwFrame = 0;
        }  */
       // if (m_bExit)
       //     m_Display.SwitchResolution(true);
	} else {
#if defined( OS_MACOS )
		usleep(50000); // in background mode, don't consume CPU
#endif
	}
#if defined( OS_MACOS )
    pthread_mutex_unlock(&processing_mutex);
#endif
#ifdef OS_WIN32
	Sleep(1);
#else
	//usleep(30000);
#endif

	return true;
}

sint32 iGXApp::Run()
{
	while (!m_bExit && Cycle()) {}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void iGXApp::Exit(uint32 code)
{
	m_bExit = true;
}
//////////////////////////////////////////////////////////////////////////
void iGXApp::Minimize()
{
#if defined(OS_WINCE)
	ShowWindow(m_Window.GetHWND(), SW_MINIMIZE);
#if !defined(OS_APPLE)
	ShellFullscreen( m_Window.GetHWND(), SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON | SHFS_SHOWSTARTICON );

#endif
#endif

	// TO Bring back discussion about proper way to minimize application
	// I should admit what there is definitely more than one way to do so:
	//
	// First is SHNavigateBack() API which is used to switch back to the
	// previously running application
	// Notably its not available on pure CE devices
	//
	// The other is implmented by bringing the 'Today' screen back foreground:
	// HWND hwnd = FindWindow(TEXT("DesktopExplorerWindow"), NULL);
	// if((NULL != hwnd) && (TRUE == IsWindow(hwnd)))
	// {
    //   ShowWindow(hwnd, SW_SHOWNA);
    //   SetForegroundWindow((HWND)(((DWORD)((HWND)hwnd)) | 0x01));
	// }
	//
	// >> PPC2003\Samples\Win32\GapiApp\GapiApp.cpp
}

void iGXApp::Suspend()
{
	//pthread_mutex_lock(&suspend_mutex);
	if (!m_bSuspended) {
#ifdef OS_WINCE
		m_pGame->OnSuspend();
		m_Display.msg_Suspend();
#endif //OS_WINCE
		m_bSuspended = true;
	}
	//pthread_mutex_unlock(&suspend_mutex);
}

void iGXApp::Resume()
{
	ODS(L"+ iGXApp::Resume() : ");
	if (m_bSuspended) {
#ifdef OS_WINCE
		// Clear key-press states
		m_Input.ClearKeyState();
        //ShowWindow(m_Window.GetHWND(), SW_SHOW);
#if !defined(OS_APPLE)
		//::SHFullScreen( m_Window.GetHWND(), SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON );
		ShellFullscreen( m_Window.GetHWND(), SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON );
#endif
		if (m_SoundPlayer.Inited()) {
			m_SoundPlayer.Close();
			m_SoundPlayer.Init();
		}
		m_Display.msg_Resume();
		m_pGame->OnResume();
#endif //OS_WINCE
		m_bSuspended = false;
	}
}

#if defined( OS_MACOS )
void iGXApp::UpdateSize(){

//	if( m_bFullScreen ){

    m_Display.msg_Resize();

    pthread_mutex_lock(&backbufMutex);
    m_viewMgr.UpdateDlgSizes();

    if( m_pGame )
        m_pGame->OnScreenModeChange();

    pthread_mutex_unlock(&backbufMutex);
//		m_bFullScreen = false;
//	}
//	else{

//		m_bFullScreen = true;
//	}
}
#endif

#ifdef OS_WIN32
BOOL
iGXApp::ShellFullscreen( HWND p1, DWORD p2 )
{
	if ( m_shfullscreen != 0 ) {
		return ((BOOL (*)(HWND, DWORD))m_shfullscreen)( p1, p2 );
	}
	return FALSE;
}
#endif

#ifdef OS_WIN32
// called from WndProc on F11 keypress
void iGXApp::SwitchScreenMode()
{
    iSize new_siz, win_siz;

    if (m_mode_fullscreen)
    {
        m_mode_fullscreen = false;
        new_siz = m_saved_size;
        win_siz.w = new_siz.w;
        win_siz.h = new_siz.h;

    }
    else
    {
        // save size before full screen
        m_saved_size = m_cur_size;

        m_mode_fullscreen = true;

        new_siz.w = GetSystemMetrics(SM_CXSCREEN);
        new_siz.h = GetSystemMetrics(SM_CYSCREEN);
        win_siz = new_siz;

        if (m_Flags & GXLF_DOUBLESIZE)
        {
            new_siz.w /= 2;
            new_siz.h /= 2;
        }
    }

	if( !m_bFullScreen ){

		new_siz = iSize( 1024, 768 );
		Window().SetSize( iSize( new_siz.w, new_siz.h) );
		Window().CenterWindow();
	}

    m_Window.OnSwitchMode(win_siz);
    m_cur_size = new_siz;
    m_Display.DoResize(new_siz);

    m_viewMgr.UpdateDlgSizes();

    if (m_pGame) m_pGame->OnScreenModeChange();


}


void iGXApp::DoSizeChange(const iSize &siz)
{
    if (m_mode_fullscreen)
    {
        return;
    }

    iSize new_siz = siz;

    if (m_Flags & GXLF_DOUBLESIZE)
    {
        new_siz.w /= 2;
        new_siz.h /= 2;
    }

    if (new_siz != m_cur_size)
    {
        m_cur_size = new_siz;
        m_Display.DoResize(new_siz);

        m_viewMgr.UpdateDlgSizes();

		if (m_pGame) m_pGame->OnScreenModeChange();
    }
}


void iGXApp::DoDebugCommand()
{
    //gGame.Map().DiscoverFullMap();
}

iSize iGXApp::GetWindowSize(){

	HWND hWnd = Window().GetHWND();
	RECT rect;

	GetWindowRect( hWnd, &rect );

	return iSize( rect.right - rect.left, rect.bottom - rect.top );
}

#endif
