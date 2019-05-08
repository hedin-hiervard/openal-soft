#ifndef PHEROES_GAME_SETTINGS_H_
#define PHEROES_GAME_SETTINGS_H_

//////////////////////////////////////////////////////////////////////////
enum ButtonActionType {
	BAT_INVALID = -1,
	BAT_HELP_MODE = 0,
	BAT_HAND_MODE,
	BAT_SURVEY_MODE,
	BAT_MINIMIZE_APP,
	BAT_MAKE_SCREENSHOT,
    BAT_QUICK_SAVE,
	BAT_COUNT
};

//////////////////////////////////////////////////////////////////////////
enum ConfigEntryType {
	CET_SFXVOLUME,
	CET_MSCVOLUME,
	CET_DISPGAMMA,
	CET_MAPSCROLLSPEED,
	CET_HEROSPEED,
	CET_COMBATSPEED,
	CET_QUICKCOMBAT,
	CET_ENDTURNCONF,
	CET_LHANDMAODE,
	CET_OVRLANDGRID,
	CET_COMBATGRID,
	CET_TRVPTSMODE,
	CET_SURVMAPFILTR,
	CET_SURVMAPSCALE,
	CET_NGDSORT,
	CET_NGDPOS,
	CET_AUTOSAVE,           // CotullaTODO: should we remove AUTOZOOM ?
	CET_AUTOZOOM,
	CET_SCROLL_AUTOZOOMED,
    CET_ADVANCED_COMBAT,
	CET_STICKY_PATH,
	CET_ONE_HANDED_MODE,
	CET_COUNT
};

//////////////////////////////////////////////////////////////////////////
struct file_size_struct { uint32 name; uint32 size; };
typedef iSimpleArray<file_size_struct> file_sizez;

enum InterfaceType {
	IT_STYLUS,
	IT_TOUCH
};

//////////////////////////////////////////////////////////////////////////
class iSettings
{
public:
	iSettings();
	~iSettings();
	bool Init(const iStringT& cmdLine);
	bool FirstLaunch();
	void Save();

	void SetActivationKey(const iStringT& newKey);
	void SetLanguage(GAME_LANG lng);
	inline GAME_LANG GetLanguage() {return m_lang;}
	InterfaceType GetInterfaceType();

	const iStringT& GetActivationKey() const { return m_ActKey; }
	bool HasActivationKey() const { return !m_ActKey.Empty(); }

	void SetGameSN(uint32 newSN) { m_gameSN = newSN; }
	uint32 GetGameSN() const { return m_gameSN; }
	
	void SetMagicNumbers(uint32 nums[4]) { memcpy(m_magNums, nums, sizeof(m_magNums)); }
#ifndef OS_APPLE
	uint32 GetMagicNumber(uint32 idx) const { check(idx<4); return m_magNums[idx]; }
#else
	uint32 GetMagicNumber(uint32 idx) const { return 0xC4A6F682; }	
#endif
	sint32 GetEntryValue(ConfigEntryType entry) const { return m_cfgEntries[entry]; }
	void SetEntryValue(ConfigEntryType entry, sint32 val);

	inline sint32 ActionKey(ButtonActionType bat) const { return m_actionKeys[bat]; }
	inline void AssignActionKey(ButtonActionType bat, iKbdKey key) { m_actionKeys[bat] = key; }
	inline ButtonActionType GetActionByKey(iKbdKey key) { for (sint32 nn=0; nn<BAT_COUNT; ++nn) if (m_actionKeys[nn] == key) return (ButtonActionType)nn; return BAT_INVALID; }

	inline bool ForceNoSound() const { return m_bNoSound; }
	inline bool FogOfWar() const { return m_bFogOfWar; }
	inline bool ShowEnemyTurn() const { return m_bShowEnemyTurn; }
	inline bool MapSpriteFile() const { return m_bMapSpriteFile; }
	
    inline uint32 LaunchCount() const { return m_launchCounter; }
#ifdef OS_WIN32
    inline bool IsFullScreen() const { return m_bFullScreen;};

#endif
protected:
	sint32		m_cfgEntries[CET_COUNT];
	sint32		m_actionKeys[BAT_COUNT];
	bool		m_bFogOfWar;
	bool		m_bShowEnemyTurn;
	bool		m_bMapSpriteFile;
	bool		m_bNoSound;
#ifdef OS_WIN32
    bool        m_bFullScreen;
#endif
	iStringT	m_ActKey;
	uint32		m_gameSN;
	uint32		m_magNums[4];
	GAME_LANG   m_lang;
    uint32      m_launchCounter;
    
	file_sizez	m_filez;
	struct {
		uint32 fingerShown;
	} m_tutorial;
};


#endif //PHEROES_GAME_SETTINGS_H_
