#ifndef PHEROES_GAME_CTL_MARKETPAGE_STYLUS_H_
#define PHEROES_GAME_CTL_MARKETPAGE_STYLUS_H_
#pragma once

class iMineralBtn_stylus: public iMineralBtn
{
public:
	iMineralBtn_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, MINERAL_TYPE mt)
	  : iMineralBtn(pViewMgr, pCmdHandler, rect, uid, mt) {}
	virtual void OnCompose();		
};

class iMarketView_stylus: public iMarketView
{
public:
	iMarketView_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, const iSize& cellSiz, bool is_harbor);
	virtual void OnCompose();
	virtual void UpdateControls();	
	virtual void SliderPosChanged();
	virtual void UpdateSize();
private:	
	virtual void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	virtual sint32 GetAmount() {
		return m_pSlider->CurPos();
	}
private:
	iDlgIconButton*		m_pMaxBtn;
	iTextButton*		m_pTradeBtn;
	iPHScrollBar*		m_pSlider;
};

class iMarketTab_stylus: public iMarketTab 
{
public:
	iMarketTab_stylus(iViewMgr* pViewMgr, bool is_harbor);
};
#endif //PHEROES_GAME_CTL_MARKETPAGE_STYLUS_H_
