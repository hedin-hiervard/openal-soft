#ifndef GXLIB_GXL_WINDOW_H_
#define GXLIB_GXL_WINDOW_H_


/*
 *	
 */
struct iDispMsgHnd
{
	virtual void msg_Suspend() = 0;
	virtual void msg_Resume() = 0;
#ifdef OS_APPLE
        virtual void msg_OnPaint() = 0;
#else
	virtual void msg_OnPaint(HDC hdc) = 0;
#endif
};

struct iInputMsgHnd
{
	virtual void msg_OnKeyDown(sint16 key_code) = 0;
	virtual void msg_OnKeyUp(sint16 key_code) = 0;
	virtual void msg_OnMouseMove(sint16 px, sint16 py, MouseId mID, MouseButtonId mbID) = 0;
	virtual void msg_OnMouseDown(sint16 px, sint16 py, MouseId mID, MouseButtonId mbID) = 0;
	virtual void msg_OnMouseUp(sint16 px, sint16 py, MouseId mID, MouseButtonId mbID) = 0;
};

/*
 *	
 */
class iGXApp;
class iWindow
{
public:
	iWindow();
	~iWindow();
#ifndef OS_APPLE
	bool Init(HINSTANCE hInst, LPCWSTR wndName, const iSize& siz, iGXApp* pApp );
#else
	bool Init(const iSize& siz, iGXApp* pApp );
#endif
	void Destroy();

	LRESULT OnMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void SetOwner(iGXApp* pApp);
	void SetSize(const iSize& siz);

	inline iSize GetSize() const
	{ return m_Size; }

	inline HWND GetHWND()
	{ return m_hWnd; }

	inline operator HWND()
	{ return m_hWnd; }

#ifdef OS_WINCE
    static bool CheckSecondInstance();
#endif
#ifdef OS_WIN32
    void OnSwitchMode(iSize &siz);
    void CenterWindow();

#endif
private:
#ifdef OS_WIN32
    SIZE            m_szSaved;
#endif
	iSize			m_Size;
	bool			m_bTrack;
	bool			m_bActive;
	HWND			m_hWnd;
	iGXApp*			m_pOwner;
};


#endif //GXLIB_GXL_WINDOW_H_
