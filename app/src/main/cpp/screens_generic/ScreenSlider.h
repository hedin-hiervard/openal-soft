/*
 *  ScreenSlider.h
 *  pheroes
 *
 *  Created by Hedin on 6/23/11.
 *  Copyright 2011 Palm Heroes Team. All rights reserved.
 *
 */


class iSlide: public iView 
{
public:
	iSlide(iViewMgr* pViewMgr, iRect rc, uint32 uid);
};

class iScreenSlider: public iView 
{
public:
	iScreenSlider(iViewMgr* pViewMgr, IViewCmdHandler *pHandler, iRect rc, uint32 uid);
	uint32 AddSlide();
	inline uint32 CurrentSlide() const { return m_curSlide; }
	iSlide* GetSlide(uint32 uid);
	void EnableSlide(uint32 uid, bool bEnabled);
	void UpdateSize();
	
	bool IsInTransition() const;
	
	sint32 GetNextSlide() const;
	sint32 GetPrevSlide() const;
	
	void PrevSlide();
	void NextSlide();
	
	void SetCurrentSlide(uint32 uid);
	
	void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) { m_bGestureComplete = false; }
	void OnSwipeRight(sint32 len);
	void OnSwipeLeft(sint32 len);
private:
	bool m_bGestureComplete;
	sint32 m_curSlide;
	bool m_bTransition;
    IViewCmdHandler* m_pHandler;
};
