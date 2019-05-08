#ifndef PHEROES_GAME_HEROVIEW_STYLUS_H_
#define PHEROES_GAME_HEROVIEW_STYLUS_H_
#pragma once

class iHeroViewChild_stylus: public iHeroViewChild
{
public:
	iHeroViewChild_stylus::iHeroViewChild_stylus(iViewMgr *pViewMgr)
		:iHeroViewChild(pViewMgr) {}
	void OnCompose();
};

class iHeroPrSkillView_stylus: public iHeroPrSkillView
{
public:
	iHeroPrSkillView_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect): iHeroPrSkillView(pViewMgr, pCmdHandler, rect) {}
	void OnCompose();
};

class iHeroTitleBar_stylus: public iHeroTitleBar
{
public:
	iHeroTitleBar_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect);
	void OnCompose();
};

class iHeroView_stylus: public iHeroView
{
public:
	iHeroView_stylus(CHILD_VIEW parentView);
	bool OnKeyDown(sint32 key);
	void NextTab();
	void PrevTab();
};
#endif //PHEROES_GAME_HEROVIEW_STYLUS_H_
