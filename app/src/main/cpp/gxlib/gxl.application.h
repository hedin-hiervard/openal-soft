#ifndef GXLIB_GXL_APPLICATION_H_
#define GXLIB_GXL_APPLICATION_H_

#include "gxl.audio.h"

/*
 *	
 */
struct IGame
{
	virtual sint32 Process(fix32 t) 	=0;
	virtual void OnKeyDown(sint32 key)	=0;
	virtual void OnKeyUp(sint32 key)	=0;
	virtual void OnSuspend() 			=0;
	virtual void OnResume() 			=0;
	virtual void OnScreenModeChange()   =0;
	virtual void OnModalDlgAdded()		=0;
	virtual void OnModalDlgRemoved()	=0;
};

/*
 *	
 */
class iGXApp
{
public:	
#ifndef OS_APPLE
	bool Init(HINSTANCE hInst, LPCWSTR appName, IGame* pGame, uint32 cdelay, uint32 flags, iSize baseSize);
#else
	bool Init(IGame* pGame, uint32 cdelay, uint32 flags, iSize baseSize);
#endif
	void Destroy();
	sint32 Cycle();
	sint32 Run();
	void Exit(uint32 code);
	void Minimize();
	void Suspend();
	void Resume();

	//
	void KeyDown(sint32 key) { check(m_pGame); m_pGame->OnKeyDown(key); }
	void KeyUp(sint32 key) { check(m_pGame); m_pGame->OnKeyUp(key); }

	//
	inline iWindow& Window() { return m_Window; }
	inline iViewMgr& ViewMgr() { return m_viewMgr; }
	inline iInput& Input() { return m_Input; }
	inline iTimer& Timer() { return m_Timer; }
	inline iDisplay& Display() { return m_Display; }
	inline iDib& Surface() { return m_Display.GetSurface(); }
	inline iSoundPlayer& SndPlayer() { return m_SoundPlayer; }
	inline bool IsActive() const { return m_bInited && !m_bSuspended; }

	void ForceCompose();
	
	//
	void SetOrientation(bool bLandscape, bool bLeftHander);
	void ToggleOrientation();
	inline bool IsLandscape() const { return (m_Flags & GXLF_LANDSCAPE) != 0; }

	virtual void OnModalDlgAdded() { if(m_pGame) m_pGame->OnModalDlgAdded(); }
	virtual void OnModalDlgRemoved() { if(m_pGame) m_pGame->OnModalDlgRemoved(); }
#ifdef OS_WIN32
    void DoSizeChange(const iSize &siz);
    void SwitchScreenMode();
    bool IsModeFullScreen(){ return m_mode_fullscreen; };    
    void DoDebugCommand();
	void SetFullScreen( bool flag ) { m_bFullScreen = flag; } 
	bool IsFullScreen() { return m_bFullScreen; }
	iSize GetWindowSize();
#endif
#ifndef OS_APPLE
	BOOL ShellFullscreen( HWND, DWORD );
#endif
	
#if defined(OS_MACOS)
	void UpdateSize(); 
#endif
	
	static iGXApp& Instance()
	{
		static iGXApp theSingleInstance;
		return theSingleInstance;
	}
protected:
	//
	iSize			m_BaseMetrics;
	uint32			m_Flags;
	uint32			m_CycleDelay;
	uint32			m_processTimer;
	bool			m_bExit;

	IGame*			m_pGame;
	iWindow			m_Window;
	iInput			m_Input;
	iTimer			m_Timer;
	iDisplay		m_Display;

	iViewMgr		m_viewMgr;
	iSoundPlayer	m_SoundPlayer;
	bool			m_bInited;
	bool			m_bSuspended;

#ifdef OS_APPLE
	bool			m_bFullScreen;
#endif
	
#ifndef OS_APPLE
	HINSTANCE		m_aygDLL;
	HINSTANCE		m_coreDLL;

#endif
	void*			m_shfullscreen;
	void*			m_sysidletimer;

#ifdef OS_WIN32
    bool            m_mode_fullscreen;
	bool			m_bFullScreen;
    iSize           m_saved_size;
    iSize           m_cur_size;
#endif
private:
	/* singleton */
	iGXApp();
	~iGXApp();	
};


#endif //GXLIB_GXL_APPLICATION_H_
