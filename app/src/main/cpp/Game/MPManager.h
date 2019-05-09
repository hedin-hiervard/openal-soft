#ifndef PHEROES_GAME_MPMANAGER_H_
#define PHEROES_GAME_MPMANAGER_H_
class iMPStatusDlg;
class PHClientSocket;
class PHSocketHandler;

class PHSocket;

#include "HttpGetSocket.h"

class iMPManager: public MPLogic {
	
friend class PHClientSocket;

public:
	enum HostType {
		HT_HOST = 0,
		HT_JOINED
	};

	typedef uint32 UserId;
	typedef uint32 GSessionId;

	struct UserData {
		iStringA m_nickname;
		UserId	 m_id;
	};
	
	struct GameUserData {
		iStringA m_nickname;
		UserId	 m_id;
		PLAYER_ID m_color;	
		CTL_TYPE m_castle;
	};
	

	struct GameCmd {
		iStringA cmd;
		iDynamicBuffer data;
	};

	enum GameState {
		GS_INVALID = -1,
		GS_SETTING = 0,
		GS_OPEN,
		GS_PLAYING
	};

	struct GameSessionData {		
		GSessionId	 m_id;
		iStringA     m_hostname;
		iStringA     m_mapname;
		uint8		 m_freeSlots;
		uint8		 m_totalSlots;
		GameState    m_state;
		uint32		 m_lastActivity;
	};

	struct TrafficCount {
		uint32 down;
		uint32 up;
	};

	typedef iSimpleArray<UserData> UserList;
	typedef iSimpleArray<GameSessionData> GSessionList;
	typedef	iSimpleArray<GameUserData> GameUserList;

	class PHHttpGetSocket: public HttpGetSocket {
	public:	
		PHHttpGetSocket(ISocketHandler& h,const std::string& url,const std::string& to_file = "");			
		void OnConnect(); 
		void OnData(const char *buf,size_t len);
	};

public:
	iMPManager();
	~iMPManager();
	
	bool ConnectToServer();
	void DisconnectFromServer();
	bool PollSockets(fix32 t);
	void Init();
	
	void CreateGame(iMapInfo& mapInfo);
	void JoinGame(GSessionId game);
	void LaunchGame();
	void LeaveGame();
	void MarkReady(bool bReady = true);
	void OnHeroMoved(const iPoint& from, const iPoint& to, uint32 moves);
	void OnDisapObject(const iPoint& from);
	void OnEndTurn();
	void OnNewTurn(PLAYER_ID plr);
	void OnAttackHero(iHero* pAttacker, iHero* pDefender);
	void OnAttackVisCnst(iHero* pAttacker, iVisCnst* pVisCnst);
	void OnAttackMapGuard(iHero* pAttacker, iMapGuard* pMapGuard);
	void OnSiegeCastle(iHero* pAttacker, iCastle* pCastle);
	void OnAttackOwnCnst(iHero* pAttacker, iOwnCnst* pCnst);
	void OnCombatMove(const iPoint& to);
	void OnCombatAttack(const iPoint &to, sint32 meleedir);
	void OnCombatShoot(const iPoint& to);
	void OnCombatWait();
	void OnCombatDefend();
	void OnCombatCastSpell(iCombatSpell* pSpell, const iPoint& pos, iBattleGroup *pSpellTarget);
	void OnCombatEnd(iBattleResult& br);
	void OnCombatCatapultShot(iCastleFort::ElementType target);
	void OnCombatTurretShot(const iPoint& pos);
	void OnDig(const iPoint& cell);
	void OnHeroUpdate(iHero* pHero);
	void OnCastleUpdate(iCastle *pCastle);
	void OnRecruitHero(iHero* pHero, iCastle* pCastle);
	void OnResourcesUpdate(PLAYER_ID pid, iMineralSet& newres);
	void OnOwnCnstUpdate(iOwnCnst *pOwnCnst);
	void OnVisCnstUpdate(iVisCnst *pVisCnst);
	void OnMapItemUpdate(iMapItem *pMapItem);
	void OnMapEventRemove(iMapEvent *pMapEvent);
	void OnMapGuardUpdate(iMapGuard *pMapGuard);
	void OnOwnCnstChangedOwner(iOwnCnst *pCnst);
	void OnKeysUpdate(uint8 newkeys);
	void OnOpenObelisksChanged(uint16 newobelisks);
	void OnHeroDefeated(iHero* pHero, bool bResetArmy);
	void OnCastleCaptured(iCastle* pCastle, PLAYER_ID newowner);
	void OnPlayerDropped(PLAYER_ID pid);
	void OnPlayerWon(PLAYER_ID pid);
	void OnPlayerDefeated(PLAYER_ID pid);
	void OnCharmAdded(iHero* pHero, MAGIC_SPELL spell, sint32 flag);

	void DoKick(PLAYER_ID pid);

	inline const iMapInfo& MapInfo() const { return m_mapInfo; }
	inline const GameUserList& GetGameUserList() const { return m_gameUsers; }
	inline const bool* PlayersPresent() const { return m_bPlayersPresent; }
	inline const UserList& GetUserList() const { return m_lobbyUsers; }
	inline const GSessionList& GetGameList() const { return m_games; } 
	inline HostType GetHostType() const { return m_hostType; }	
	inline void SetBattleWrapper(iBattleWrapper* wr) { m_pBattleWrapper = wr; }

	inline const iStringA& GetNickname() const { return m_nickname; }

	inline bool IsMultiplayer() const { return m_socket != NULL; }
	inline bool IsPRA() const { return m_socket != NULL && m_pra; }
	inline void DisablePRA() { m_pra = false; }
	inline bool IsReplaying() const { return m_socket != NULL && m_bReplaying; }
	inline const iStringA& AuthKey() const { return m_authkey; }
	inline void SetPlayingAutobattle(bool ab) { m_bPlayingAutobattle = ab; }

	inline const TrafficCount& CurTraffic() const { return m_curTraf; }
	inline const TrafficCount& TotalTraffic() const { return m_totalTraf; }

	iStringT GetColorNickname(PLAYER_ID color);

	bool CheckUpdates();


	inline time_t LastActivity() const { return m_lastActivity; }
	inline bool IsCombat() const { return m_bIsCombat; }
	virtual void OnConnect();
	virtual void OnDisconnect();
	virtual void OnConnectTimeout();
	virtual void OnConnectFailed();
	virtual void OnLine(const iStringA& line);
	virtual void OnRawData(const char *buf, size_t n);	

	virtual void OnUpload(uint32 count);
	virtual void OnDownload(uint32 count);

private:
	Mutex m_qMutex;
	void LoadCredentials();
	void SaveCredentials();
	void ShowStatusDlg(const iStringT header, const iStringT txt, bool bShowStatusDlg);
	void HideStatusDlg();
	void EnqueueAndRun(const iStringA& string);
	void SendImpersonated(const iStringA& string);
	void ExecuteCmd(iPlayer *plr, iCharA cmd, iStringA recvd, iDynamicBuffer& data);
	void ExecuteCmdQueue();
	void StepSendCmdQueue();
	void SortGamesList();
private:
	
	iStringA m_authkey;		
	StdoutLog m_log;
	PHSocketHandler* m_handler;
	iMapInfo m_mapInfo;
	iMPStatusDlg *m_statusDlg;

	uint32 m_counter;
	uint32 m_rcvdCnt;	
	HostType m_hostType;	
	iDynamicBuffer m_buff;	
	iBattleWrapper* m_pBattleWrapper;	
	iSimpleArray<GameCmd> m_cmdQueue;	
	iSimpleArray<GameCmd> m_sendQueue;	
	PLAYER_ID m_ownColor;
	bool m_pra;
	UserId m_id;
	GSessionId m_gameId;
	UserList m_lobbyUsers;
	GSessionList m_games;
	GameUserList m_gameUsers;	
	iStringA m_nickname;
	bool m_bBlock;
	iDynamicBuffer m_tosend;
	bool m_bPlayersPresent[PID_COUNT];
	bool m_bReplaying;
	bool m_bPlayingAutobattle;
	time_t m_lastActivity;
	bool m_bIsCombat;
	TrafficCount m_curTraf;
	TrafficCount m_totalTraf;
	fix32 m_pingTimer;
	bool m_bDisconFlag;
};
#endif //PHEROES_GAME_MPMANAGER_H_
