#ifndef PHEROES_GAME_MEETVIEW_STYLUS_H_
#define PHEROES_GAME_MEETVIEW_STYLUS_H_
class iHeroBigPortBtn : public iButton 
{
public:
	iHeroBigPortBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
	: iButton(pViewMgr, pCmdHandler, rect, uid, Visible), m_pHero(NULL) {}

	void OnCompose()
	{
		if (m_pHero) {
			iRect rc = GetScrRect();
			SpriteId sid = m_pHero->Proto()->m_icn48;
			gGfxMgr.Blit(sid,gApp.Surface(), rc);
			ButtonFrame(gApp.Surface(),rc,GetButtonState());
			if (!IsEnabled()) gApp.Surface().Darken25Rect(rc);
			rc.InflateRect(1);
			gApp.Surface().FrameRect(rc,cColor_Black);
		}
	}

	void SetHero(iHero* pHero)
	{
		m_pHero = pHero;
		SetEnabled(m_pHero != NULL);
		Invalidate();
	}

	inline const iHero* Hero() const { return m_pHero; }
	inline iHero* Hero() { return m_pHero; }

private:
	iHero*	m_pHero;
};

class iHeroChild_stylus : public iHeroChild
{
public:
	iHeroChild_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, iSimpleArray<iArtDragDropItem*>& competitors, const iPoint& pos);
	void SetHero(iHero* pHero);
	void OnCompose();
private:		
	iHeroBigPortBtn*	m_pPortrait;	
};

class iMeetView_stylus : public iMeetView
{
public:
	iMeetView_stylus();
private:	
	iSimpleArray<iArtDragDropItem*>	m_Competitors;
	iHeroChild*						m_pChilds[2];
	iMeetToolBar*					m_pToolBar;
	IViewCmdHandler*				m_pCmdHandler;
};
#endif //PHEROES_GAME_MEETVIEW_STYLUS_H_
