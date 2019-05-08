#ifndef PHEROES_GAME_TRANSITIONVIEW_H_
#define PHEROES_GAME_TRANSITIONVIEW_H_

class iPageDisplay: public iView {
public:
	iPageDisplay(iViewMgr* pViewMgr, const iRect& rect);
	inline void SetNumPages(sint32 num) { m_numPages = num; Invalidate(); }
	inline void SetCurPage(sint32 num) { m_curPage = num; Invalidate(); }
private:
	void OnCompose();
	sint32 m_numPages;
	sint32 m_curPage;
};

class iTransitionView: public iDialog {
public: 
	enum TV_MODE {
		TVM_HRZ_SLIDE_RIGHT,
		TVM_HRZ_SLIDE_LEFT,
		TVM_VRT_OVERLAY
	};
	
	enum MOTION_MODE {
		MM_MANUAL,
		MM_AUTO
	};
	
	enum TV_RESULT {
		TVR_LEFT,
		TVR_CHANGED,
		TVR_UP,
		TVR_DOWN
	};
	
	virtual iSize GetDlgMetrics() const;
	virtual void OnCreateDlg();
	virtual void OnPlace(iRect& rect) {}
	
	// Virtuals
	virtual bool KeyDown(sint32 key) { return false; }
	virtual bool KeyUp(sint32 key) { return false; }
	
	iTransitionView(iViewMgr* viewMgr, TV_MODE viewMode, MOTION_MODE motionMode, iDib& dib1, iDib& dib2);
	
	void OnCompose();
	bool Process(fix32 t);
	
	void OnSwipeUp(sint32);
	void OnSwipeLeft(sint32);
	void OnSwipeRight(sint32);
	void OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	
private:
	sint32 m_globalHeight, m_globalWidth;
	sint32 m_speed;
	iDib& m_dib1, m_dib2;
	TV_MODE m_viewMode;
	MOTION_MODE m_motionMode;
	iPoint m_position;
	fix32 m_timer;
	sint32 m_start;
};

#endif //PHEROES_GAME_TRANSITIONVIEW_H_
