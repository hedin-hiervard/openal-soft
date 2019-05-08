#ifndef PHEROES_GAME_MPLOGINVIEW_H_
#define PHEROES_GAME_MPLOGINVIEW_H_
class iMPLoginView : public iChildGameView
{
public:
	iMPLoginView();
	~iMPLoginView();

	void OnCompose();

private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

private:
	IViewCmdHandler*	m_pCmdHandler;
};
#endif //PHEROES_GAME_MPLOGINVIEW_H_
