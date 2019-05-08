#ifndef PHEROES_GAME_SCROLLINGVIEW_H_
#define PHEROES_GAME_SCROLLINGVIEW_H_
class iScrollingView : public iChildGameView
	{
	public:
		iScrollingView();
		~iScrollingView();
		void OnCompose();
		
		void OnSwipeUp(sint32 length);
		void OnSwipeDown(sint32 length);
		void OnMouseUp(const iPoint &rect, MouseId mID, MouseButtonId mbID);
		void OnMouseDown(const iPoint &rect, MouseId mID, MouseButtonId mbID);
		
		virtual void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param) {}
		virtual CHILD_VIEW Type() const { return CV_SCROLL; }
		
		virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); }
		//virtual iSize GetDefaultSize() const { return iSize(iMIN<sint32>(1024, gApp.ViewMgr().Metrics().w), iMIN<sint32>(768, gApp.ViewMgr().Metrics().h)); }

	private:
		sint32 m_pos;
		sint32 m_fixPos;
		iPoint m_startPos;
	};
#endif //PHEROES_GAME_SCROLLINGVIEW_H_
