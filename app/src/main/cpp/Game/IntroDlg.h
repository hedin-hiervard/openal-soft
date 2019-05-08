#ifndef PHEROES_GAME_INTRODLG_H_
#define PHEROES_GAME_INTRODLG_H_


class iIntroDlg : public iDialog, public  IViewCmdHandler
{
public:
	iIntroDlg(iViewMgr* pViewMgr);
	
private:
	void OnCreateDlg() {}
	void OnCompose();
	iSize GetDlgMetrics();
	void OnTimer(uint32 tid);

	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param) {}

private:
	uint32	m_aniStep;
	iDib	m_dibLogo;
	
};

#endif //PHEROES_GAME_INTRODLG_H_
