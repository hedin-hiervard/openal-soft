#ifndef GXLIB_GXL_VIEWMGR_H_
#define GXLIB_GXL_VIEWMGR_H_

class iGXApp;
class iTopmostView;
class iPopupView;

class iViewMgr
{
public:
	struct IDragGlyph {
		virtual void ComposeDragGlyph() =0;
	};

public:
	iViewMgr(iGXApp* pApp);
	~iViewMgr();

	bool ProcessMessage(const iInput::iEntry& msg);
	void Compose(iRect& rect);

	inline iGXApp* App() { return m_pApp; }
	inline iDib& Surface() { return m_Surf; }
	inline iSize Metrics() { return m_Surf.GetSize(); }

	void SetCurView(iTopmostView* pCurView);
	inline iView* CurView() { return (iView*)m_pCurView; }

	// Capture view
	void SetViewCapture(iView* pCapView);
	iView* ReleaseViewCapture();
	inline iView* CapturedView() { return m_pCapView; };
#if defined (PC_VERSION)
	// Capture rect
	void SetRectCapture( iRect rect );
	iRect ReleaseRectCapture();
	inline iRect CapturedRect() { return( m_CapRect ); }
	
	void SetEntCapture( iView* pEntView );
	iView* ReleaseEntCapture();
	inline iView* CapturedEnt() { return( m_pCapEntView ); }
#endif	
	// Modal stack
	inline bool HasModalDlg() const { return m_dlgStack.GetSize()>0; }
	void PushModalDlg(iDialog* pDlg);
	iDialog* PopModalDlg();
    void UpdateDlgSizes();
	iDialog* PeepDlg();

	// Popup windows
	void TrackPopup(iPopupView* pPopupView, const iPoint& pos, const iRect& bound, Alignment al);
	void HidePopup();

	// Drag'n'Drop glyph
	inline void SetDragGlyph(IDragGlyph* pDragGlyph) { m_pDragGlyph = pDragGlyph; }
	inline IDragGlyph* DragGlyph() { return m_pDragGlyph; }

	// Timer processing
	void Process(uint32 interval, fix32 t);
	void SetTimer(uint32 period, uint32 tid, iView* pHandler);
	void CleanupTimers(iView* pHandler = NULL);

private:
	iDib&				m_Surf;
	iGXApp*				m_pApp;
	iTopmostView*		m_pCurView;
	iView*				m_pCapView;
#if defined( PC_VERSION )
	iRect				m_CapRect;
	iView*				m_pCapEntView;
#endif
	IDragGlyph*			m_pDragGlyph;
	iPopupView*			m_pPopupView;

	typedef iSimpleArray<iDialog*>	iDialogStack;
	iDialogStack	m_dlgStack;

private:
	struct iTimerHandler {
		iTimerHandler(uint32 _timer, uint32 _tid, iView* _pHandler) : timer(_timer), tid(_tid), pHandler(_pHandler) {}
		uint32	timer;
		uint32	tid;
		iView*	pHandler;
	};
	typedef iSimpleArray<iTimerHandler>	iTimerList;
	iTimerList	m_timers;
	uint32		m_timerCounter;
};


#endif //GXLIB_GXL_VIEWMGR_H_
