#ifndef PHEROES_GAME_CTL_DOCKPAGE_STYLUS_H_
#define PHEROES_GAME_CTL_DOCKPAGE_STYLUS_H_
#pragma once

class iDockView_stylus: public iDockView
{
public:
	iDockView_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 dt);
};

class iDockTab_stylus: public iDockTab
{
public:
	iDockTab_stylus(iViewMgr* pViewMgr);
};
#endif //PHEROES_GAME_CTL_DOCKPAGE_STYLUS_H_
