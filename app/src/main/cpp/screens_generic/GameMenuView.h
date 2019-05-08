#ifndef PHEROES_GAME_GAMEMENUVIEW_H_
#define PHEROES_GAME_GAMEMENUVIEW_H_


class iGameMenuView : public iChildGameView
	{
	public:
		enum CHILD_MENU 
        {
			CM_SINGLE,
			CM_COUNT
		};
		iGameMenuView();
		~iGameMenuView();
		void OnCompose();

		void OnSwipeUp(sint32 length);
		void OnMouseDown(const iPoint &rect, MouseId mID, MouseButtonId mbID);
        virtual inline bool AlwaysReceiveMouse() { return true; }
		void UpdateSize();
        bool AllowGestures(const iPoint &pos);
        bool MouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        bool Process(fix32 t);
		void OnActivate(bool bActivate);
		virtual CHILD_VIEW Type() const { return CV_GAMEMENU; }
		virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); }		
#if defined( PC_VERSION )
		void SetLoadFlagTrue();
		void SetSaveFlagTrue();
#endif
	private:
		void ShowView(CHILD_MENU cm);
        virtual void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	
		bool m_bGestureComplete;
        bool m_bAllowGestures;		
        IMainMenuView* m_pChildren[CM_COUNT];
		sint32 m_pos;
        uint32 m_frame;
        fix32 m_timer;
	};
#endif //PHEROES_GAME_GAMEMENUVIEW_H_
