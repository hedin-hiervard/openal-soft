#ifndef PHEROES_GAME_CTL_RECRPAGE_STYLUS_H_
#define PHEROES_GAME_CTL_RECRPAGE_STYLUS_H_
#pragma once
/*
 *	RecrButton
 */
class iRecrButton_stylus : public iRecrButton
	{
	public:
		iRecrButton_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid):
		  iRecrButton(pViewMgr, pCmdHandler, rect, uid) {}
		virtual void OnCompose();				
	};


class iRecrSet_stylus : public iRecrSet
{
public:
	iRecrSet_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, uint32 uid, iPoint offset);
private:
	void iRecrSet_stylus::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
};

/*
*	Recruit page
*/
class iRecrTab_stylus : public iRecrTab
{
public:
	iRecrTab_stylus(iViewMgr* pViewMgr);
	void UpdateSize();	
private:
	
};
#endif //PHEROES_GAME_CTL_RECRPAGE_STYLUS_H_
