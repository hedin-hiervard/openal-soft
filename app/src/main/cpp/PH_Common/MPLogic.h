#ifndef PH_COMMON_MPLOGIC_H_
#define PH_COMMON_MPLOGIC_H_
#define CMD_START 'A'

#define MPC_UPDATE_OWNCNST		(iCharA)(CMD_START + 0x01)
#define MPC_UPDATE_VISCNST	    (iCharA)(CMD_START + 0x02)
#define MPC_UPDATE_MAPGUARD     (iCharA)(CMD_START + 0x03)
#define MPC_REMOVE_MAPEVENT     (iCharA)(CMD_START + 0x04)
#define MPC_UPDATE_MAPITEM      (iCharA)(CMD_START + 0x05)
#define MPC_UPDATE_CASTLE       (iCharA)(CMD_START + 0x06)
#define MPC_UPDATE_HERO         (iCharA)(CMD_START + 0x07)
#define MPC_UPDATE_KEYS         (iCharA)(CMD_START + 0x08)
#define MPC_UPDATE_MINERALS     (iCharA)(CMD_START + 0x09)
#define MPC_UPDATE_OBELISKS     (iCharA)(CMD_START + 0x0A)
#define MPC_CASTLE_SIEGED       (iCharA)(CMD_START + 0x0B)
#define MPC_CBT_ATTACK          (iCharA)(CMD_START + 0x0C)
#define MPC_CBT_SPELLCAST       (iCharA)(CMD_START + 0x0D)
#define MPC_CBT_CATAPULT        (iCharA)(CMD_START + 0x0E)
#define MPC_CBT_DEFEND          (iCharA)(CMD_START + 0x0F)
#define MPC_CBT_END             (iCharA)(CMD_START + 0x10)
#define MPC_CBT_MOVE            (iCharA)(CMD_START + 0x11)
#define MPC_CBT_SHOOT           (iCharA)(CMD_START + 0x12)
#define MPC_CBT_TURRET          (iCharA)(CMD_START + 0x13)
#define MPC_CBT_WAIT            (iCharA)(CMD_START + 0x14)
#define MPC_CHANGE_OWNER        (iCharA)(CMD_START + 0x15)
#define MPC_ENDTURN             (iCharA)(CMD_START + 0x16)
#define MPC_HERO_ATTACKED       (iCharA)(CMD_START + 0x17)
#define MPC_HERO_MOVED          (iCharA)(CMD_START + 0x18)
#define MPC_RECRUIT             (iCharA)(CMD_START + 0x19)
#define MPC_REMOVE_HERO         (iCharA)(CMD_START + 0x1A)
// do not use 0x1B, since it's '\' symbol and is not supported by mysql
#define MPC_OBJ_DISAPPEARED     (iCharA)(CMD_START + 0x1C)
#define MPC_OWNCNST_ATTACKED    (iCharA)(CMD_START + 0x1D)
#define MPC_DIG				    (iCharA)(CMD_START + 0x1E)
#define MPC_PLAYER_DROPPED	    (iCharA)(CMD_START + 0x1F)
#define MPC_PLAYER_WON		    (iCharA)(CMD_START + 0x20)
#define MPC_RSEED               (iCharA)(CMD_START + 0x21)
#define MPC_GAME_DROPPED        (iCharA)(CMD_START + 0x22)
#define MPC_CASTLE_CAPTURED     (iCharA)(CMD_START + 0x23)
#define MPC_EVENT               (iCharA)(CMD_START + 0x24)
#define MPC_FAILED_TO_JOIN      (iCharA)(CMD_START + 0x25)
#define MPC_SAVE_FOLLOWS        (iCharA)(CMD_START + 0x26)
#define MPC_CHARM_ADDED			(iCharA)(CMD_START + 0x27)
#define MPC_VISCNST_ATTACKED	(iCharA)(CMD_START + 0x28)
#define MPC_MAPGUARD_ATTACKED	(iCharA)(CMD_START + 0x29)
#define MPC_PLAYER_JUMPED_IN	(iCharA)(CMD_START + 0x30)
#define MPC_PLAYER_DEFEATED     (iCharA)(CMD_START + 0x31)

#define BINARY_CMDS 7
extern char binary_cmds[BINARY_CMDS];

enum EVENT_TYPE {
	ET_GAME_DROPPED = 0,
	ET_DROPPED_BY_TIMEOUT
};

class MPLogic {

friend class PHSocket;

public:
	MPLogic(): m_socket(NULL), m_state(LS_AWT_AUTH) {}
protected:
	
	// the code means what we are waiting for (AWT_XXX) or what data we are receiving (RCV_XXX)
	enum LogicState {
		LS_AWT_AUTH = 0,     // wait for the remote side to authorize itself
		LS_AWT_REG,          // wait for result of register request
		LS_AWT_USR,          // wait for user data
		LS_AWT_GAME,         // wait for game data
		LS_AWT_LOGCNF,       // wait for log-in confirmation
		LS_AWT_LOBBYCMD,     // wait for lobby cmd
		LS_AWT_SETUPCMD,     // wait for game setup screen cmd
		LS_AWT_GAMECMD,      // wait for in-game cmd
		LS_AWT_GAMECMDMONAD, // wait for single in-game cmd (move hero, attack, etc)   
		LS_AWT_SAVEACCEPT,   // wait for remote side to accept savegame upload
		LS_AWT_SAVEOFFER,    // wait for remote side to offer savegame size
		LS_AWT_RESUMEDCSN,	 // wait for resume decision (yes/no)
		LS_AWT_BINACCEPT,    // wait for remote side to accept binary upload
		LS_AWT_BINCNFRM,	 // wait for remote side to confirm binary receive
		LS_AWT_SAVECNFRM,    // wait for remote side to confirm save receive

		LS_RCV_SAVE,         // receiving savegame data
		LS_RCV_CMDDATA       // receiving cmd data
	};
	
	LogicState m_state;
	PHSocket* m_socket;

private:	

	virtual void OnConnect() =0;
	virtual void OnDisconnect() =0;
	virtual void OnConnectTimeout() =0;
	virtual void OnConnectFailed() =0;
	virtual void OnLine(const iStringA& line) =0;
	virtual void OnRawData(const char *buf, size_t n) =0;	
	virtual void OnUpload(uint32 count) =0;
	virtual void OnDownload(uint32 count) =0;
};

#endif //PH_COMMON_MPLOGIC_H_
