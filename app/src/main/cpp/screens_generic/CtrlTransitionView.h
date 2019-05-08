/*
 *  CtrlTransitionView.h
 *  pheroes
 *
 *  Created by Hedin on 6/27/11.
 *  Copyright 2011 Palm Heroes Team. All rights reserved.
 *
 */
#ifndef PHEROES_GAME_CTRLTRANSITIONVIEW_H_
#define PHEROES_GAME_CTRLTRANSITIONVIEW_H_

class iCtrlTransitionView: public iDialog {
public: 
	enum TV_MODE {
		TVM_HRZ_SLIDE_RIGHT,
		TVM_HRZ_SLIDE_LEFT,
	};
	
	enum MOTION_MODE {
		MM_MANUAL,
		MM_AUTO
	};
	
	enum TV_RESULT {
		TVR_LEFT,
		TVR_CHANGED,
	};
	
	virtual iSize GetDlgMetrics() const;
	virtual void OnCreateDlg();
	virtual void OnPlace(iRect& rect) {}
	
	// Virtuals
	virtual bool KeyDown(sint32 key) { return false; }
	virtual bool KeyUp(sint32 key) { return false; }
	
	iCtrlTransitionView(iViewMgr* viewMgr, TV_MODE viewMode, MOTION_MODE motionMode, iView *pV1, iView* pV2);
	
	bool Process(fix32 t);
	
	void OnCompose();
	void UpdateControls();

	
	void OnSwipeLeft(sint32);
	void OnSwipeRight(sint32);
	void OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	
private:
	sint32 m_globalHeight, m_globalWidth;
	sint32 m_speed;
	iView *m_pV1, *m_pV2;
	TV_MODE m_viewMode;
	MOTION_MODE m_motionMode;
	iPoint m_position;
	fix32 m_timer;
	sint32 m_start;
};

#endif //PHEROES_GAME_CTRLTRANSITIONVIEW_H_

