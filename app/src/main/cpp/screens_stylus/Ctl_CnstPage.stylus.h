#ifndef PHEROES_GAME_CTL_CNSTPAGE_STYLUS_H_
#define PHEROES_GAME_CTL_CNSTPAGE_STYLUS_H_
#pragma once

class iCnstListBox_stylus : public iCnstListBox
{
public:
	iCnstListBox_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
	virtual uint32 LBItemHeight() const;
	virtual void ComposeLBBackground(const iRect& rect);
	virtual void ComposeLBItem(uint32 iIdx, bool bSel, const iRect& irc);	
};

class iCnstDescView_stylus: public iCnstDescView
{
public:
	iCnstDescView_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect);
	virtual void OnCompose();	
};

class iCnstTab_stylus: public iCnstTab
{
public:
	iCnstTab_stylus(iViewMgr* pViewMgr, iCastleView* pCastleView);
	virtual void UpdateSize();
	virtual void Process(fix32 t) {}
};
#endif //PHEROES_GAME_CTL_CNSTPAGE_STYLUS_H_
