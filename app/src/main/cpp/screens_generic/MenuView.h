#ifndef PHEROES_GAME_MENUVIEW_H_
#define PHEROES_GAME_MENUVIEW_H_

class iNewMenuDlg;

//////////////////////////////////////////////////////////////////////////
class iMenuView : public iChildGameView
{
public:
	enum MenuLocation {
		ML_MAIN,
		ML_SINGLE,
		ML_MULTI,
		ML_HELP
	};
	iMenuView();
	void Start();
    void NewStart(bool fInGame);
	void OnCompose();
	void AfterCompose();
	bool Process(fix32 t);
	void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnActivate(bool bActivate);
	void StartCredits();
	void StopCredits();
#ifdef PC_VERSION
    void UpdateSize();
#endif
	virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); }

	virtual CHILD_VIEW Type() const { return CV_MENU; }
private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

private:
#if defined(OS_APPLE) || defined(OS_ANDROID)
	uint			m_startTime;

#endif
	iCreditsComposer	m_crComposer;
	bool m_bStarted;
	MenuLocation m_loc;
    INewMenuDlg        *m_dlg;
};



#endif //PHEROES_GAME_MENUVIEW_H_
