#include "stdafx.h"
#include "Settings.h"

const uint32 CONFIG_FILE_HDR = 0xA173B7C3;
const uint32 CONFIG_FILE_NEWHDR = 0xA173B7C4;
const uint32 CONFIG_FILE_VER = 3;


//////////////////////////////////////////////////////////////////////////
const sint32 CONFIG_ENTRY_DESC[CET_COUNT][3] = { // min, max, default
	{0, 10,5},	// Sfx volume
	{0, /*4*/5, 2},	// Music volume
	{0, 4, 0},	// Display gamma
	{0, 4, 2},	// Map scroll speed
	{0, 4, 2},	// Hero speed
#ifndef IPHONE_DEBUG
	{0, 4, 3},	// Combat speed
#else
	{0, 4, 4},	// Combat speed
#endif
	{0, 1, 0},	// Quick combat
	{0, 1, 1},	// End turn confitmation
	{0, 1, 0},	// Lefthander mode
	{0, 1, 1},	// Overland grid
	{0, 1, 1},	// Combat grid
	{0, 1, 1},	// Travel points mode
	{0, 1, 1},	// Survey map filtration
	{0, 1, 0},	// Survey map scale
	{0, 3, 2},	// New game dialog list sort order
	{0, -1, 0},	// New game dialog list position
	{0, 1, 1},   // enables autosave
    {0, 1, 1},   // enable auto-zoom
    {0, 1, 1},   // scroll while auto-zoomed
    {0, 1, 0},   // advanced combat
	{0, 1, 0},	// sticky path
	{0, 1, 1},	// one handed mode
};

//////////////////////////////////////////////////////////////////////////
#ifdef OS_WINCE
const sint32 CONFIG_DEFKEYS[BAT_COUNT] = {
	KEY_ENTER,	// Help mode
	KEY_VKA,	// Hand mode
	KEY_VKB,	// Survey map
	KEY_VKC,	// Minimize
	KEY_INVALID,// Make screenshot
    KEY_INVALID	// Quick save
};
#endif
//////////////////////////////////////////////////////////////////////////
LPCTSTR const CONFIG_FILE = _T("PalmHeroes.cfg");

//////////////////////////////////////////////////////////////////////////
uint32 hashname( wchar_t* ptr, unsigned seed = 0x1234abcd )
{
	uint32 h = seed;
	wchar_t ch;
	while ( 0 != (ch = *ptr++) ) {
		h ^= ( (h << 5) + (ch&0xff) + (h >> 2) );
		h ^= ( (h << 5) + (ch>>8) + (h >> 2) );
	}
	return h;
}

void InitFileSizez(file_sizez& sizez)
{
	sizez.RemoveAll();
	iStringT strText =  gRootPath + _T("*.*");
	iStringT appName;
	iFileName::GetAppName(appName);
#ifdef OS_WIN32
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(strText.CStr(), &wfd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (appName.CompareIgnoreCase(wfd.cFileName) == 0) {
				file_size_struct fss;
				fss.name = hashname(wfd.cFileName);
				fss.size = wfd.nFileSizeLow;
				sizez.Add(fss);
			}
		} while (FindNextFile(hFind, &wfd));
		FindClose(hFind);
	}
#endif
}
//////////////////////////////////////////////////////////////////////////
iSettings::iSettings() : m_gameSN(0)
{}

iSettings::~iSettings()
{}

void iSettings::SetActivationKey(const iStringT& newKey)
{
	m_ActKey = newKey;
}

void iSettings::SetLanguage(GAME_LANG lng)
{
	m_lang = lng;
}


InterfaceType iSettings::GetInterfaceType()
{
	return IT_TOUCH;
}

bool iSettings::FirstLaunch()
{
	iFileStream pConfigFile(OpenWin32File(gConfigFilePath));
	if (!pConfigFile.IsValid()) return true;
	uint32 hdr; pConfigFile.Read(hdr);
	if (hdr != CONFIG_FILE_NEWHDR) return true;

	uint32 ver;
	pConfigFile.Read(ver);
	if(ver < CONFIG_FILE_VER) return true;
	return false;
}

bool iSettings::Init(const iStringT& cmdLine)
{
	//
	InitFileSizez(m_filez);
	// Reset config entries
	m_bMapSpriteFile = false;
	m_bShowEnemyTurn = false;
	m_bFogOfWar = true;
	m_bNoSound = false;
	m_launchCounter = 0;

#ifdef OS_WIN32
    m_bFullScreen = false;
#endif
	for (uint32 nn=0; nn<CET_COUNT; ++nn) m_cfgEntries[nn] = CONFIG_ENTRY_DESC[nn][2];
#ifdef OS_WINCE
	for (uint32 nn=0; nn<BAT_COUNT; ++nn) m_actionKeys[nn] = CONFIG_DEFKEYS[nn];
#endif

	// Parse command line
	if (cmdLine.Length()) {
		sint32 spos = 0;
		while (spos < (sint32)cmdLine.Length()) {
			sint32 epos = cmdLine.Find(_T("--"), spos+1);
			if (epos == -1) epos = cmdLine.Length();
			iStringT arg(cmdLine.CStr()+spos, epos - spos);
			arg.Trim();
			if (arg.Compare(_T("--show_enemy_turn")) == 0) {
				m_bShowEnemyTurn = true;
			} //else if (arg.Compare(_T("--disable_fow")) == 0) { //disabled as of 1.04b (Hedin)
				//m_bFogOfWar = false;
			else if (arg.Compare(_T("--no_sound")) == 0) {
				m_bNoSound = true;
			} else if (arg.Compare(_T("--map_sprite_file")) == 0) {
				m_bMapSpriteFile = true;
			}
#ifdef OS_WIN32
        else if (arg.Compare(_T("--fullscreen")) == 0) {
            m_bFullScreen = true;
        }
#endif
			spos = epos;
		}
	}
	// Read the config file
	iFileStream pConfigFile(OpenWin32File(gConfigFilePath));
	if (!pConfigFile.IsValid()) return true;
	uint32 hdr; pConfigFile.Read(hdr);
	if (hdr != CONFIG_FILE_NEWHDR) return true;

	uint32 ver;
	pConfigFile.Read(ver);
	if(ver < CONFIG_FILE_VER) return true;

	for (uint32 nn = 0; nn<CET_COUNT; ++nn) pConfigFile.Read(m_cfgEntries[nn]);
	for (uint32 nn = 0; nn<BAT_COUNT; ++nn) pConfigFile.Read(m_actionKeys[nn]);
	pConfigFile.Read(m_ActKey);
	uint8 tmp;
	pConfigFile.Read(tmp);
	m_lang = (GAME_LANG)(GLNG_ENGLISH + tmp);

    uint32 lc;
    if(pConfigFile.Read(lc))
        m_launchCounter = lc;
    m_launchCounter++;

	// set the default combat speed
	m_cfgEntries[CET_COMBATSPEED] = CONFIG_ENTRY_DESC[CET_COMBATSPEED][2];
	// and hero speed
	m_cfgEntries[CET_HEROSPEED] = CONFIG_ENTRY_DESC[CET_HEROSPEED][2];

    Save();

	return true;
}

void iSettings::Save()
{
	//
	iFileStream pConfigFile(CreateWin32File(gConfigFilePath));
	if (!pConfigFile.IsValid()) return;
	pConfigFile.Write(CONFIG_FILE_NEWHDR);
	pConfigFile.Write(CONFIG_FILE_VER);
	for (uint32 nn = 0; nn<CET_COUNT; ++nn) pConfigFile.Write(m_cfgEntries[nn]);
	for (uint32 nn = 0; nn<BAT_COUNT; ++nn) pConfigFile.Write(m_actionKeys[nn]);
	pConfigFile.Write(m_ActKey);
	pConfigFile.Write((uint8)(m_lang - GLNG_ENGLISH));
    pConfigFile.Write(m_launchCounter);
}

void iSettings::SetEntryValue(ConfigEntryType entry, sint32 val)
{
	if (CONFIG_ENTRY_DESC[entry][1] == -1) m_cfgEntries[entry] = iMAX<sint32>(CONFIG_ENTRY_DESC[entry][0], val);
	else m_cfgEntries[entry] = iCLAMP<sint32>(CONFIG_ENTRY_DESC[entry][0],CONFIG_ENTRY_DESC[entry][1], val);
}

