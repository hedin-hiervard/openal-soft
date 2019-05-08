#ifndef PHEROES_GAME_MPGAMESETUPVIEW_H_
#define PHEROES_GAME_MPGAMESETUPVIEW_H_

class iMPGameSetupView : public iChildGameView
{
public:
	iMPGameSetupView();
	~iMPGameSetupView();

	void OnCompose();
	void UpdatePlayers();
	void SetActive(bool bAct);
private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

	class ParticipantItem: public iView, public IViewCmdHandler {
	public:
		ParticipantItem(iViewMgr* pViewMgr, uint32 uid, const iRect &rect, PLAYER_ID pid);
		void OnCompose();
		void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
		void UpdateData(const iMPManager::GameUserData& data);
	private:
		iPlayerBtn *m_pPlayerBtn;
		iNationBtn *m_pNationBtn;
		PLAYER_ID m_pid;
		iMPManager::GameUserData m_user;
	};


private:
	enum State {
		PRELIMINAL,
		SETUP_CLIENT,
		SETUP_HOST,
		READY_CLIENT
	};
	IViewCmdHandler*	m_pCmdHandler;
	iTextButton  *m_pStartBtn;
	State m_state;
};
#endif //PHEROES_GAME_MPGAMESETUPVIEW_H_
