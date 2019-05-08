#ifndef PHEROES_GAME_POPUPDLG_H_
#define PHEROES_GAME_POPUPDLG_H_

//////////////////////////////////////////////////////////////////////////
class iTextPopupDlg : public iFramePopupView
{
public:
	iTextPopupDlg(iViewMgr* pViewMgr, const iStringT& text, PLAYER_ID pid);
	void DoCompose(const iRect& clRect);
	iSize ClientSize();

private:
	iStringT	m_Text;
};

//////////////////////////////////////////////////////////////////////////
class iPopupDlg_Guard : public iFramePopupView
{
public:
	iPopupDlg_Guard(iViewMgr* pViewMgr, iMapGuard* pGuard, VISION_LEVEL vl, iHero* pHero);
	void DoCompose(const iRect& clRect);
	iSize ClientSize();

private:
	iMapGuard*		m_pGuard;
	iStringT		m_text1;
	iStringT		m_text2;
};

//////////////////////////////////////////////////////////////////////////
class iPopupDlg_VisCnst : public iFramePopupView
{
public:
	iPopupDlg_VisCnst(iViewMgr* pViewMgr, iVisCnst* pVisCnst, VISION_LEVEL vl, const iHero* pVisitor);
	void DoCompose(const iRect& clRect);
	iSize ClientSize();

private:
	iVisCnst*		m_pVisCnst;
	VISION_LEVEL	m_vLevel;
	const iHero*	m_pVisitor;
};
//////////////////////////////////////////////////////////////////////////
class iPopupDlg_OwnCnst : public iFramePopupView
{
public:
	iPopupDlg_OwnCnst(iViewMgr* pViewMgr, iOwnCnst* pOwnCnst, VISION_LEVEL vl);
	void DoCompose(const iRect& clRect);
	iSize ClientSize();

private:
	iOwnCnst* m_pOwnCnst;
	VISION_LEVEL	m_vLevel;
};
//////////////////////////////////////////////////////////////////////////
class iPopupDlg_Castle: public iFramePopupView
{
public:
	iPopupDlg_Castle(iViewMgr* pViewMgr, iCastle* pCastle, VISION_LEVEL vl);
	void DoCompose(const iRect& clRect);
	iSize ClientSize();

private:
	iCastle* m_pCastle;
	VISION_LEVEL	m_vLevel;
};
//////////////////////////////////////////////////////////////////////////
class iPopupDlg_Hero : public iFramePopupView
{
public:
	iPopupDlg_Hero(iViewMgr* pViewMgr, iHero* pHero, VISION_LEVEL vl);
	void DoCompose(const iRect& clRect);
	iSize ClientSize();

private:
	iHero*	m_pHero;
	VISION_LEVEL	m_vLevel;
};


#endif //PHEROES_GAME_POPUPDLG_H_
