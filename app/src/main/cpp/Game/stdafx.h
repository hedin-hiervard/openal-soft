#ifndef PHEROES_GAME_STDAFX_H_
#define PHEROES_GAME_STDAFX_H_

extern  bool            gUseIpadUI;

#if defined(OS_IPHONE) || defined(OS_MACOS)
#define OS_APPLE
#endif //(OS_IPHONE || OS_MACOS)

#if defined(OS_MACOS) && !defined (MACOS_LION)
//#define MACOS_SNOWLEO
#endif

#if defined(OS_WIN32) || defined(OS_MACOS)
#define PC_VERSION
#endif

#if _MSC_VER > 1000
#pragma once

#endif // _MSC_VER > 1000
#ifdef _WIN32_WCE
#	if (_WIN32_WCE <= 211)
#	error This project can not be built for H/PC Pro 2.11 or earlier platforms.
#	endif

#endif
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning(disable:4786)

#ifdef OS_WIN32
#include <windows.h>

#endif
//
// Define file size value ( "magic number" xor "file size")
//
//#define MAGIC_NUMBER (0x17a63e0f)
//#define FILE_SIZE_VALUE (MAGIC_NUMBER ^ 256782)

//#define _DEEP_DEBUG_
//#define DEBUG_LOG
//#define FOG_MODE 2
//#define TRANSLATOR_VERSION

#ifdef _DEEP_DEBUG_
#	include <crtdbg.h>
#	define _CRTDBG_MAP_ALLOC
#	define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#	define DEBUG_NEW new

#endif
#if (_MSC_VER < 1310) && !defined(for)
#define for if (0) {} else for


#endif
#ifdef OS_WINCE
//#	include <gx.h>
#include "gapi_help.h"
#if UNDER_CE == 300
#	include <dbgapi.H>
#elif UNDER_CE == 420
#	include <ASSERT.H>
#else
//#	error Unknown version of WindowsCE

#endif
#endif
#ifdef OS_IPHONE
#include <assert.h>

#endif
#ifdef OS_WIN32
#include <stdio.h>
#include <objidl.h>
#include <tchar.h>
#include <ASSERT.H>

#endif
#include "gxl.inc.h"
#include "gxl.rtti.h"
#include "gxl.window.h"
#include "gxl.ctr.cbuff.h"
#include "gxl.ctr.array.h"
#include "gxl.ctr.map.h"
#include "gxl.ctr.list.h"
#include "gxl.input.h"
#include "gxl.timer.h"
#include "gxl.display.h"

#include "gxl.view.h"
#include "gxl.dialog.h"
#include "gxl.topmostview.h"
#include "gxl.popupview.h"
#include "gxl.stdctrl.h"
#include "gxl.viewmgr.h"
#include "gxl.audio.h"
#include "gxl.tables.h"
#include "gxl.ctr.hash.h"

#include "cnst_text.h"
#include "Common/iphone/cnst_gfx.h"
#include "Common/ipad/cnst_gfx.h"
#include "cnst_sfx.h"

#include "gxl.application.h"
#include "gxl.dibfont.h"
#include "isometric.h"
#include "gxl.ctr.sortarray.h"
#include "TextManager.h"
#include "TextComposer.h"
#include "FileAccessor/FileAccessor.h"

#include "common.h"
#include "cm_creatures.h"
#include "cm_constructions.h"
#include "cm_magic.h"
#include "cm_castles.h"
#include "cm_events.h"
#include "cm_ships.h"
#include "GfxManager.h"
#include "SfxManager.h"
#include "MusicManager.h"
#if defined( OS_APPLE )
#endif

#include "Magic.h"
#include "PathFinding.h"
#include "fogmap.h"
#include "AvatarMgr.h"
#include "Army.h"
#include "ArmyOwner.h"
#include "treasury_cnst.h"
#include "ItemMgr.h"
#include "Objects.h"
#include "Artifacts.h"
#include "Constructions.h"
#include "CastleCnsts.h"
#include "Castles.h"
#include "hero.h"
#include "ship.h"
#include "player.h"
#include "AniObject.h"
#include "animation.h"

#include "Map.h"
#include "gxl.ctr.textmgr.h" // text hash manager for ML maps
#include "GameWorld.h"
#include "SoundMap.h"
#include "ai.h"
#include "BattleInfo.h"
#include "BattleFort.h"
#include "BattleUnit.h"
#include "BattleEngine.h"
#include "Game.h"
#include "Settings.h"
#include "MapEnum.h"
#include "CastleViewCommons.h"
#include "Tutorial.h"

#ifdef MULTIPLAYER
#include <StdoutLog.h>
#include <SocketHandler.h>
#include <TcpSocket.h>

#endif
#ifdef DEBUG_LOG
#include "gxl.logmgr.h"
extern iLogMgr gLogMgr;


#endif //DEBUG_LOG
struct iSecret {
	uint32	state;
	void*	fnKernelIoControl;
	void*	fnGetDeviceUniqueID;
	void*	fnGetFileAttributesEx;
};

#if (!defined(OS_WIN32) || defined(DEV_VER)) && !defined(OS_APPLE) && (!defined(FREE_VER))
//#	define NEED_REGISTRATION
//#pragma comment(linker, "/merge:.rdata=.text")
//#pragma comment(linker, "/merge:.data=.text")
//#pragma comment(linker, "/SECTION:.text,EWRX")

#endif
//#undef NEED_REGISTRATION

extern fileaccessor::RelativeFilePath    gDataPath;
extern fileaccessor::RelativeFilePath    gSavePath;
extern fileaccessor::RelativeFilePath    gAppDataPath;
extern fileaccessor::RelativeFilePath    gBundledMapsPath;
extern fileaccessor::RelativeFilePath    gDownloadedMapsPath;
extern fileaccessor::RelativeFilePath    gMapStatFilePath;
extern fileaccessor::RelativeFilePath    gMusicPath;
extern fileaccessor::RelativeFilePath    gMapShopCfgFilePath;
extern fileaccessor::RelativeFilePath    gConfigFilePath;
extern fileaccessor::RelativeFilePath    gTutorialCfgFilePath;

extern	iGXApp&			gApp;
extern	iDibReader		gDibReader;
extern	iTextComposer&	gTextComposer;
extern	iTextManager	gTextMgr;
extern	iGfxManager&		gGfxMgr;
extern	iSfxManager&		gSfxMgr;
extern  iMusicManager&	gMusicMgr;
extern	iAnimationHost	gAniHost;



#ifdef MULTIPLAYER
#include "PHSocket.h"
#include "GfxHlp.h"
#include "CommonDialogs.h"
#include "MPLogic.h"
#include "MPManager.h"
extern  iMPManager		gMPMgr;
#endif
extern	iGame			gGame;
extern	iSettings		gSettings;
extern  iTutorial       gTutorial;
extern	iMapEnum        gMapEnum;
extern	bool			gRegistered;
extern	bool			gEnterNewKey;

#ifndef OS_IPHONE
extern HMODULE* pCoreHandle;
#endif
extern iSecret* pSecret;
extern uint32* pSecNum;

#include "GfxHlp.h"
#include "Composer.h"

#include "ViewDefs.h"
#include "CommonControls.h"
#include "ArmyList.h"
#ifndef MULTIPLAYER
#include "CommonDialogs.h"
#endif
#include "PopupDlg.h"


extern sint32 cell_w;
extern sint32 cell_h;
extern sint32 im_coef;




#endif //PHEROES_GAME_STDAFX_H_
