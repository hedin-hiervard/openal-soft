#ifndef PHEROES_GAME_MPLOBBYVIEW_H_
#define PHEROES_GAME_MPLOBBYVIEW_H_
class iMPLobbyView : public iChildGameView
{
private:
	/*
	*	User List Box
	*/
	class iUserListBox : public iListBox
	{
	public:
		iUserListBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
			: iListBox(pViewMgr, pCmdHandler, rect, uid)
		{}

		inline sint32 LBTopOffset() const { return 7; }

		inline uint32 LBItemHeight() const
#ifndef OS_IPHONE
		{ return 16; }
#else
		{ return 25; }	

#endif
		inline uint32 LBItemsCount() const
		{ return gMPMgr.GetUserList().GetSize();  }

		
		void UpdateList()
		{ UpdateListBox(); }

	private:
		void ComposeLBBackground(const iRect& rect)
		{
			iRect rc = rect;
#ifndef OS_IPHONE
			gApp.Surface().Darken25Rect(rc);
#else
			ComposeDlgBkgndPanel(gApp.Surface(), rc, gGame.Map().ActPlayer()->PlayerId(), false);
#endif
		}

		void ComposeLBItem(uint32 iIdx, bool bSel, const iRect& irc)
		{
			iTextComposer::FontConfig fc(dlgfc_plain);
			iRect rc=irc;
#ifdef OS_IPHONE
			rc.x += 14;
			rc.w -= 28;

#endif
#ifndef OS_IPHONE
			ButtonFrame(gApp.Surface(),rc,iButton::Pressed);
#else
			gApp.Surface().HLine(iPoint(rc.x,rc.y + rc.h - 3),rc.x2(),0xFFFF,64);
#endif
			rc.DeflateRect(1);
			if (bSel) {
				gGfxMgr.BlitTile(PDGG(CTILE), gApp.Surface(),rc);
				ButtonFrame(gApp.Surface(),rc,0);
			}	
			iStringT temp = iStringT(CvtA2T<>(gMPMgr.GetUserList().At(iIdx).m_nickname.CStr()));
			gTextComposer.TextOut(gApp.Surface(), rc, temp);
		}

	private:		
	};


/*
	*	Game List Box
	*/
	class iGameListBox : public iListBox
	{
	public:
		iGameListBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
			: iListBox(pViewMgr, pCmdHandler, rect, uid)
		{}

		inline sint32 LBTopOffset() const { return 7; }

		inline uint32 LBItemHeight() const
#ifndef OS_IPHONE
		{ return 16; }
#else
		{ return 25; }	

#endif
		inline uint32 LBItemsCount() const
		{ return gMPMgr.GetGameList().GetSize();  }

		
		void UpdateList()
		{ UpdateListBox(); }

	private:
		void ComposeLBBackground(const iRect& rect)
		{
			iRect rc = rect;
#ifndef OS_IPHONE
			gApp.Surface().Darken25Rect(rc);
#else
			ComposeDlgBkgndPanel(gApp.Surface(), rc, gGame.Map().ActPlayer()->PlayerId(), false);
#endif
		}

		void ComposeLBItem(uint32 iIdx, bool bSel, const iRect& irc)
		{
			iTextComposer::FontConfig fc(dlgfc_plain);
			iTextComposer::FontConfig fc_started(dlgfc_plain);
			fc_started.cmpProps.faceColor = RGB16(128, 128, 128);
			iRect rc=irc;
#ifdef OS_IPHONE
			rc.x += 14;
			rc.w -= 28;

#endif
#ifndef OS_IPHONE
			ButtonFrame(gApp.Surface(),rc,iButton::Pressed);
#else
			gApp.Surface().HLine(iPoint(rc.x,rc.y + rc.h - 3),rc.x2(),0xFFFF,64);
#endif
			rc.DeflateRect(1);
			if (bSel) {
				gGfxMgr.BlitTile(PDGG(CTILE), gApp.Surface(),rc);
				ButtonFrame(gApp.Surface(),rc,0);
			}				
			const iMPManager::GameSessionData& g = gMPMgr.GetGameList()[iIdx];

			gTextComposer.TextOut(g.m_state == iMPManager::GS_OPEN ? fc : fc_started, gApp.Surface(), rc, iFormat(_T("%s %s %d/%d"), 
				CvtA2T<>(g.m_hostname.CStr()).str_, 
				CvtA2T<>(g.m_mapname.CStr()).str_, 
				g.m_totalSlots - g.m_freeSlots, g.m_totalSlots));
		}

	private:		
	};

public:
	iMPLobbyView();
	~iMPLobbyView();

	void OnCompose();
	void SetActive(bool bAct);
private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	iGameListBox* m_gameList;
	iButton*	  m_joinBtn;
	iButton*	  m_profileBtn;
	iButton*	  m_createBtn;
	iButton*	  m_cancelBtn;
private:
	IViewCmdHandler*	m_pCmdHandler;
};

#endif //PHEROES_GAME_MPLOBBYVIEW_H_
