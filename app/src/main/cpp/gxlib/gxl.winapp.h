#ifndef GXLIB_GXL_WINAPP_H_
#define GXLIB_GXL_WINAPP_H_

class iWinApp
{
protected:
	bool	m_bExit;
	bool	m_bInited;
	HWND	m_hWnd;
	uint32	m_Flags;
	iDib	m_BackBuff;
public:
	iWinApp();
	~iWinApp();
	bool Init(HINSTANCE hInst, LPCWSTR appName, uint32 flags);
	sint32 Run();
	void Exit(uint32 code);
	void Destroy();
	void DoPaint();

	//
	virtual void msg_MouseMove(const iPoint& pos){};
	virtual void msg_MouseDown(const iPoint& pos){};
	virtual void msg_MouseUp(const iPoint& pos){};

	//
	inline operator HWND()
	{ return m_hWnd; }
	
	//
	LRESULT OnMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};


#endif //GXLIB_GXL_WINAPP_H_
