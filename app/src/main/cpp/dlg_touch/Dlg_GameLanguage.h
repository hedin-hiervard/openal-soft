#ifndef PHEROES_GAME_DLG_GAMELANGUAGE_H_
#define PHEROES_GAME_DLG_GAMELANGUAGE_H_

class iLanguageSelectView;

class iDlg_GameLanguage : public iDialog, public IViewCmdHandler
{
public:
	iDlg_GameLanguage(iViewMgr* pViewMgr);
	void OnCreateDlg();
	void OnCompose();
	iSize GetDlgMetrics() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);	

private:
	iLanguageSelectView *m_pLSV;
};


#endif //PHEROES_GAME_DLG_GAMELANGUAGE_H_
