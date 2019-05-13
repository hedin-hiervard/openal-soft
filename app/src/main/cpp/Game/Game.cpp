#include "stdafx.h"
#include "BattleView.h"
#include "BattleView.touch.h"
#include "MinimapView.h"
#include "CastleView.h"
#include "Dlg_CreatInfo.h"

#include "ArtifactList.h"
#include "ArtifactList.touch.h"
#include "OverlandView.h"
#include "OverlandView.touch.h"

#include "GameMenuView.h"
#include "Credits.h"
#include "MenuView.h"
#include "game.h"
#include "AutoBattle.h"
#include "InteractBattle.h"
#include "Dlg_HallOfFame.h"
#include "Dlg_Save.h"
#include "helpers.h"
//#include "NewMapShop/NewMapShop.h"

#include "transitionview.h"

#ifdef MULTIPLAYER
#include "HiddenBattle.h"
#include "MPLoginView.h"
#include "MPLobbyView.h"
#include "Dlg_ScenProps.h"
#include "MPGameSetupView.h"
#endif

#include "GameMenuView.h"
#include "ScrollingView.h"
#include "CastleView.h"
#include "HeroView.h"
#include "MeetView.h"
#include "HarborView.h"
#include "UpgradeView.h"
#include "GameResultView.h"

#include "GameMenuDlg.h"

#include "OverlandUI.h"
#ifdef OS_MACOS
#include <pthread.h>
#endif
extern void*  pDynCode;
extern uint8* pBloomBits;

#include "screens_ipad/CastleView.h"
#include "screens_ipad/GameResultView.h"
#include "screens_ipad/HarborView.h"
#include "screens_ipad/HeroView.h"
#include "screens_ipad/MainMenuView.h"
#include "screens_ipad/MeetView.h"
#include "screens_ipad/OverlandUI.h"
#include "screens_ipad/UpgradeView.h"

#include "screens_iphone/CastleView.h"
#include "screens_iphone/GameResultView.h"
#include "screens_iphone/HarborView.h"
#include "screens_iphone/HeroView.h"
#include "screens_iphone/MainMenuView.h"
#include "screens_iphone/MeetView.h"
#include "screens_iphone/OverlandUI.h"
#include "screens_iphone/UpgradeView.h"

#include "iosdk/analytics/analytics.h"

#include "FileAccessor/FileAccessor.h"

using namespace iosdk;
using namespace fileaccessor;

/*
*  BlackBack
*/
iBlackBackView::iBlackBackView() : iTopmostView(&gApp.ViewMgr())
{
}


iSize iBlackBackView::GetDefaultSize() const
{
	return gApp.ViewMgr().Metrics();
}


void iBlackBackView::OnCompose()
{
    iRect rect = GetScrRect();
    gGfxMgr.BlitTile(PDGG(BKTILE), gApp.Surface(), rect);
    gApp.Surface().Darken50Rect(rect);
}

/*
*  Game
*/
iGame::iGame()
: m_bInited(false)
, m_bStarted(false)
, m_tActView(iChildGameView::CV_UNDEFINED)
, m_pMainView(NULL)
, m_pBattle(NULL)
, m_bGoToMainMenu(false)
, m_hmChannel(-1)
, m_hmSound(0xFFFF)
, m_bDelayedStart(false)
, m_delayedCV(iChildGameView::CV_UNDEFINED)
, m_delayedShowCV(iChildGameView::CV_UNDEFINED)
, m_bNextTurn(false)
#if defined( PC_VERSION )
, m_bKeyUpFlag(false)
, m_bKeyLeftFlag(false)
, m_bKeyRightFlag(false)
, m_Timer(0)
, m_bSaveScreen(false)
, m_bSpell(false)
, m_bLoadScreen( false )
, m_bNumMove( false )
, pos( iPoint(0,0) )
, bRightDown(false)
, m_bQuickLoad(false)
, m_bQuickSave(false)
, m_bQuit(false)
, m_bExitDlg(false)
, m_bEnter(false)
, m_bEsc(false)
, m_bMinimapEsc(false)
, m_bCenter(false)
, m_bShowCastle(false)
#endif
{
}
#ifdef OS_MACOS
extern pthread_mutex_t processing_mutex;
extern pthread_mutex_t exit_mutex;
#endif
iGame::~iGame()
{
    SaveLastSes();
#ifdef OS_MACOS
    pthread_mutex_lock(&exit_mutex); // lock the final mutex and wait for the processing mutex to eventually release
    pthread_mutex_lock(&processing_mutex);
#endif
    Cleanup();
}

// workaround compiler problem
static void setup_serial_( uint32 serial )
{
    uint32 keys[4];
    uint8* bitsPointer = pBloomBits;
//    uint32 mnum = xxc::bloom_probe( bitsPointer, serial );
//    xxc::scramble_magic( PBKEY_COMMON, mnum, keys );
//    gSettings.SetMagicNumbers(keys);
}

static uint32 do_antidbg_call()
{
    uint32 some_fake_data = 0x882;
    some_fake_data  <<= 1;
#if defined(UNDER_CE)
    return ((uint32(WINAPI*)(int))pDynCode)(some_fake_data);
#endif
}

bool iGame::Init(const iStringT& fname)
{
    check(!m_bInited);
    uint32 localyRegistered = 182;

    // Load items
    if (!LoadResources(m_itemMgr) ) return false;
    localyRegistered = (localyRegistered-12)*31415;
    localyRegistered >>= 2;
    //gRegistered = true;

	// create all child view
    for(uint32 xx=0; xx<iChildGameView::CV_COUNT; xx++)
	{
        if(gUseIpadUI)
        {
            switch(xx) {
                case iChildGameView::CV_MENU: m_pChildView[xx] = new iMenuView(); break;
                case iChildGameView::CV_GAMEMENU: m_pChildView[xx] = new iGameMenuView(); break;
                case iChildGameView::CV_OVERLAND: m_pChildView[xx] = new iMainView_touch(); m_pMainView = (iMainView*)m_pChildView[xx]; break;
                case iChildGameView::CV_BATTLE: m_pChildView[xx] = new iBattleView_touch; break;
                case iChildGameView::CV_HARBOR: m_pChildView[xx] = new UI_IPAD::iHarborView(); break;
                case iChildGameView::CV_UPGRADE: m_pChildView[xx] = new UI_IPAD::iUpgradeView(); break;
                case iChildGameView::CV_CASTLE: m_pChildView[xx] = new UI_IPAD::iCastleView(); break;
                case iChildGameView::CV_HERO: m_pChildView[xx] = new UI_IPAD::iHeroView(); break;
                case iChildGameView::CV_MEET:  m_pChildView[xx] = new UI_IPAD::iMeetView(); break;
                case iChildGameView::CV_MINIMAP: m_pChildView[xx] = new iMinimapView(); break;
                case iChildGameView::CV_GAMERESULT: m_pChildView[xx] = new UI_IPAD::iGameResultView(); break;
            }
        } else
        {
            switch(xx) {
                case iChildGameView::CV_MENU: m_pChildView[xx] = new iMenuView(); break;
                case iChildGameView::CV_GAMEMENU: m_pChildView[xx] = new iGameMenuView(); break;
                case iChildGameView::CV_OVERLAND: m_pChildView[xx] = new iMainView_touch(); m_pMainView = (iMainView*)m_pChildView[xx]; break;
                case iChildGameView::CV_BATTLE: m_pChildView[xx] = new iBattleView_touch; break;
                case iChildGameView::CV_HARBOR: m_pChildView[xx] = new UI_IPHONE::iHarborView(); break;
                case iChildGameView::CV_UPGRADE: m_pChildView[xx] = new UI_IPHONE::iUpgradeView(); break;
                case iChildGameView::CV_CASTLE: m_pChildView[xx] = new UI_IPHONE::iCastleView(); break;
                case iChildGameView::CV_HERO: m_pChildView[xx] = new UI_IPHONE::iHeroView(); break;
                case iChildGameView::CV_MEET:  m_pChildView[xx] = new UI_IPHONE::iMeetView(); break;
                case iChildGameView::CV_MINIMAP: m_pChildView[xx] = new iMinimapView(); break;
                case iChildGameView::CV_GAMERESULT: m_pChildView[xx] = new UI_IPHONE::iGameResultView(); break;
            }
        }

	}

    // check last session
        RelativeFilePath lastsesPath = RelativeFilePath("lastses.phsd", FileLocationType::FLT_DOCUMENT);
        RelativeFilePath lastsesUsePath = RelativeFilePath("lastses.use", FileLocationType::FLT_DOCUMENT);

        if(FileAccessor::sharedFileAccessor()->fileExists(lastsesPath)) {
            // here is the protection agains corrupts saves: first we move lastses.phs to lastses.use, then
            // load the latter. in case it is broken, after relaunch on crash, we will boot to menu
            iFile::Move(lastsesPath, lastsesUsePath);


#ifdef HMM_COMPOVERSION
            iFilePtr pSaveFile = xxc::OpenXFile(gSavePath + _T("lastses.use"),HMM_COMPOCODE);
#else
            iFilePtr pSaveFile = OpenWin32File(lastsesPath);
#endif
            check(pSaveFile);
            if (pSaveFile) {
                uint32 fourcc; pSaveFile->Read(&fourcc,sizeof(fourcc));
                iMapInfo mapInfo;
                mapInfo.m_FileName = lastsesPath;
                mapInfo.m_bNewGame = false;
                if(mapInfo.ReadMapInfo(pSaveFile.get())) {
                    mapInfo.ReorderPlayers();
                    if (StartNewGame(mapInfo, false, false)) {
                        pSaveFile.reset();
                        iFile::Delete(lastsesUsePath);
                        return m_bInited = true;
                    }
                }
                pSaveFile.reset();
                iFile::Delete(lastsesUsePath);
            }
    }

    // Init main menu
    ShowView(iChildGameView::CV_MENU);
    ((iMenuView*)m_pChildView[iChildGameView::CV_MENU])->NewStart(false);


    return m_bInited = true;
}

void iGame::Quit()
{
    SaveLastSes();
    gApp.Exit(0);
}

void iGame::SaveLastSes()
{
    if(ActViewType() != iChildGameView::CV_MENU &&
	   ActViewType() != iChildGameView::CV_BATTLE &&
          Map().CurPlayer() &&
	   Map().CurPlayer()->PlayerType() == PT_HUMAN) {

        Crashsave();
    }
}

void iGame::RealMainMenu()
{
    //if(ActViewType() != iChildGameView::CV_MENU)
    m_bGoToMainMenu = true;
}

void iGame::ShowMainMenu(bool fInGame)
{
    ShowView(iChildGameView::CV_MENU);
    ((iMenuView*)m_pChildView[iChildGameView::CV_MENU])->NewStart(fInGame);
}


void iGame::MainMenu()
{
#ifdef MULTIPLAYER
    if(gMPMgr.IsMultiplayer())
        gMPMgr.DisconnectFromServer();
    //else
#endif
    RealMainMenu();
}

void ShowProgressReport( uint32 curProg, bool initial = true );

bool iGame::StartNewGame(const iMapInfo& mapInfo, bool bNewGame, bool bUpdateVisitors)
{
	m_bStarted = false;
    ShowProgressReport( 0, false );
    // Load map info
    iMapInfo _mapInfo;
#ifdef HMM_COMPOVERSION
    iFileI* nakedFile;
    //if ( bNewGame ) nakedFile = OpenWin32File( mapInfo.m_FileName ); else
    nakedFile = xxc::OpenXFile( mapInfo.m_FileName, HMM_COMPOCODE );
    iFilePtr pFile( nakedFile );
#else
    iFilePtr pFile( OpenWin32File( mapInfo.m_FileName ));
#endif
    check(pFile);
    ShowProgressReport( 10, false );
    uint32 fourcc; pFile->Read(&fourcc,sizeof(fourcc));
    if (fourcc != GMAP_FILE_HDR_KEY || !_mapInfo.ReadMapInfo(pFile.get(), false)) {
        return false;
    }

    ShowProgressReport( 25, false );
    // Show Main view

    ShowView(iChildGameView::CV_OVERLAND);

	gGame.MainView()->CleanupAni();

    ShowProgressReport( 30, false );
    // Load map
    if(!m_Map.LoadFromFile(mapInfo, pFile.get(), bUpdateVisitors, !bNewGame)) {
        gGame.MainMenu();
        return false;
    }

    ShowProgressReport( 50, false );
    // Init Pass map
    m_Map.InitPassMap();

    ShowProgressReport( 60, false );
    // Setup ultimate artifact position
    m_Map.InitUltimateArt();

	// setup losing condition construction owners
	if(bNewGame)
		m_Map.SetupVC();

    ShowProgressReport( 75, false );
    // Init Sound map
    m_soundMap.Init(&m_Map);

    ShowProgressReport( 85, false );

    //gTutorial.Trigger(TUT_BEGIN);
    // Start the game
    m_Map.StartGame(mapInfo.m_curPlayerId, !bNewGame);

    ShowProgressReport( 100, false );
	m_bStarted = true;

	MainView()->OnGameStarted();

    Analytics::sharedInstance()->TrackEvent("game", mapInfo.m_gameMode == iMapInfo::GM_SPLAYER ? "map started: single" : "map started: hotseat", CvtT2A<>(mapInfo.MapEnglishName().CStr()).Unsafe());

	/*char dif[256];
	snprintf(dif, sizeof(dif), "%d", mapInfo.m_Difficulty);
	Analytics::sharedInstance()->TrackEvent("game", "difficulty", dif);
	if (mapInfo.m_gameMode == iMapInfo::GM_HOTSEAT)
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "%d", mapInfo.HumanPlayers());
        Analytics::sharedInstance()->TrackEvent("game", "hotseat started", buf);
    }
    */
/*	if(bNewGame)
		gGame.Autosave();
*/
    return true;
}

bool iGame::StartTutorialMap()
{
	iStringT filename;

  auto tutorialPath = gBundledMapsPath.pathByAppendingString("break.phmd");

    iFilePtr pMapFile = OpenWin32File(tutorialPath);
	check(pMapFile);
	if (pMapFile) {
		uint32 fourcc; pMapFile->Read(&fourcc,sizeof(fourcc));
		iMapInfo mapInfo;
		mapInfo.m_FileName = tutorialPath;
		mapInfo.m_bNewGame = true;
		if(mapInfo.ReadMapInfo(pMapFile.get())) {
			mapInfo.m_Difficulty = DFC_NORMAL;
			mapInfo.ReorderPlayers();
			return StartNewGame(mapInfo, true, true);
		} else
			return false;
	} else
		return false;
}


bool iGame::StartSingleMap()
{
// 	iStringT filename;
// #ifdef ROYAL_BOUNTY
// 	filename = _T("RoyalBounty.phmd");
// #endif
// 	iFilePtr pMapFile = OpenWin32File(gBundledMapsPath + filename);
// 	check(pMapFile);
// 	if (pMapFile) {
// 		uint32 fourcc; pMapFile->Read(&fourcc,sizeof(fourcc));
// 		iMapInfo mapInfo;
// 		mapInfo.m_FileName = gBundledMapsPath + filename;
// 		mapInfo.m_bNewGame = true;
// 		if(mapInfo.ReadMapInfo(pMapFile.get())) {
// 			mapInfo.m_Difficulty = DFC_NORMAL;
// 			mapInfo.ReorderPlayers();
// 			return StartNewGame(mapInfo, true, true);
// 		} else
// 			return false;
// 	} else
		return false;
}

void iGame::DelayedStartGame(const iMapInfo& mapInfo, bool bNewGame) {
    m_delMapInfo = mapInfo;
    m_bDelNewGame = bNewGame;
    m_bDelayedStart = true;
}

void iGame::ExitGame(bool bChangeView)
{
    if (!m_bStarted) return;

    m_soundMap.Cleanup();
    m_Map.CleanupGameWorld();
    m_itemMgr.OnGameEnd();

	iFile::Delete(RelativeFilePath("lastses.tmp", FileLocationType::FLT_DOCUMENT));

    if (m_pBattle)
    {
        delete m_pBattle;
        m_pBattle = NULL;
    }
    if (bChangeView)
    {
        ShowView(iChildGameView::CV_MENU);
        ((iMenuView*)m_pChildView[iChildGameView::CV_MENU])->NewStart(false);
    }
}

void iGame::Cleanup()
{
    /*for (uint32 xx=0; xx<iChildGameView::CV_COUNT; ++xx){
        if (m_pChildView[xx]) {
            delete m_pChildView[xx];
            m_pChildView[xx] = NULL;
        }
    }
    m_tActView = iChildGameView::CV_UNDEFINED;

    if (m_bInited) {
        m_itemMgr.Cleanup();
        m_bInited = false;
    }
	 */
}


void iGame::OnSuspend()
{
    //
    m_soundMap.ResetEnvSounds();
}

void iGame::OnResume()
{
    //xxc::sec_shuffle();
    //
    if (m_Map.CurHero()) SoundMap().SetupEnvSounds(m_Map.CurHero()->Pos());
}

// Child views
const char * viewName[] =
{
    "CV_MENU",
    "CV_OVERLAND",
    "CV_BATTLE",
    "CV_CASTLE",
    "CV_HARBOR",
    "CV_WARACADEMY",
    "CV_HERO",
    "CV_MEET",
    "CV_MINIMAP",
    "CV_GAMEMENU",
    "CV_SCROLL",
    "CV_UPGRADE",
    "CV_GAMERESULT",

#ifdef MULTIPLAYER
    "CV_MPLOGIN",
    "CV_MPLOBBY",
    "CV_MPGAMESETUP",
#endif
};


void iGame::ShowView(iChildGameView::CHILD_VIEW cv, bool bAfterHide)
{
    check (cv != iChildGameView::CV_UNDEFINED);
    if (cv == m_tActView) return;

    m_soundMap.ResetEnvSounds();

	// if showing overland, main menu, or battle, clear the view stack
	if(m_tActView == iChildGameView::CV_OVERLAND ||
	   m_tActView == iChildGameView::CV_MENU ||
	   iChildGameView::CV_BATTLE) {

		gGame.ClearScreenStack();
	}

    // Delete/Hide old active view
    if (m_tActView != iChildGameView::CV_UNDEFINED) {
        m_pChildView[m_tActView]->OnActivate(false);
        m_pChildView[m_tActView]->SetVisible(false);
        gApp.ViewMgr().SetCurView(NULL);
	}

    // Show new active view
    switch (cv) {
    case iChildGameView::CV_OVERLAND:
        if(m_Map.ActPlayer() && m_Map.ActPlayer()->PlayerType() == PT_HUMAN && gGame.Map().CurHero())
            m_soundMap.SetupEnvSounds(gGame.Map().CurHero()->Pos());
        m_pMainView->SetLHandMode(gSettings.GetEntryValue(CET_LHANDMAODE));
        break;
        case iChildGameView::CV_MENU:
        {
            static int counter = 0;
            counter++;
            // if(counter > 1)
            //     Adomatic::sharedInstance()->requestFullscreenAd();
            break;
        }
    case iChildGameView::CV_HARBOR:
    case iChildGameView::CV_HERO:
    case iChildGameView::CV_UPGRADE:
	case iChildGameView::CV_MEET:
	case iChildGameView::CV_GAMEMENU:
	case iChildGameView::CV_GAMERESULT:
		if(!bAfterHide)
            gGame.PushScreen();
		break;
    case iChildGameView::CV_CASTLE:
        if(!bAfterHide)
           gGame.PushScreen();
        break;
    case iChildGameView::CV_MINIMAP:
        ((iMinimapView*)m_pChildView[cv])->SetCenterCell(m_pMainView->Composer().GetCenterCell());
		break;
    case iChildGameView::CV_SCROLL:
        //check(!m_pChildView[cv]);
        if(!m_pChildView[cv]) {
            iScrollingView* pGMView = new iScrollingView();
            m_pChildView[cv] = pGMView;
        }
        break;
#ifdef MULTIPLAYER
    case iChildGameView::CV_MPLOGIN:
        if(!m_pChildView[cv]) {
            m_pChildView[cv] = new iMPLoginView();
        }
        break;
    case iChildGameView::CV_MPLOBBY:
        if(!m_pChildView[cv]) {
            m_pChildView[cv] = new iMPLobbyView();
        }
        break;
    case iChildGameView::CV_MPGAMESETUP:
        if(!m_pChildView[cv]) {
            m_pChildView[cv] = new iMPGameSetupView();
        }
        break;
#endif
    };

	if(!bAfterHide)
		m_viewStack.Push(cv);
	m_pChildView[cv]->SetVisible(true);
	m_pChildView[cv]->SetRect(AlignRect(m_pChildView[cv]->GetDefaultSize(), iRect(iPoint(), gApp.ViewMgr().Metrics()), AlignCenter));
    m_pChildView[cv]->UpdateSize();
    m_pChildView[cv]->OnActivate(true);

	if(cv != iChildGameView::CV_OVERLAND){

		PauseMoveSound();
//		MainView()->SetHighlighted(false);
	}
	//else
	//	ResumeMoveSound();

	gApp.ViewMgr().SetCurView(m_pChildView[cv]);

    //Analytics::sharedInstance()->TrackEvent("UX", "entered screen", viewName[cv]);

    m_tActView = cv;
}

void iGame::HideViewDelayed(iChildGameView::CHILD_VIEW cv)
{
    m_delayedCV = cv;
}

void iGame::ShowViewDelayed(iChildGameView::CHILD_VIEW cv)
{
    m_delayedShowCV = cv;
}



void iGame::HideView(iChildGameView::CHILD_VIEW cv)
{
    check(cv != iChildGameView::CV_UNDEFINED);
    check(cv != iChildGameView::CV_OVERLAND);
    check(m_pChildView[cv]);
#ifdef PC_VERSION
	if( cv == iChildGameView::CV_CASTLE ){

        CastleView()->SetHighlighted(iCastleView::CVM_EMPTY);
        MainView()->SetHighlighted(false);
    }

    if( cv == iChildGameView::CV_HERO ){

        HeroView()->SetHighlighted(false);
        MainView()->SetHighlighted(false);
    }
#endif
    if(cv == iChildGameView::CV_GAMEMENU)
        MainView()->SetHighlighted(false);

    if(gSettings.GetInterfaceType() == IT_TOUCH)
        if(cv == iChildGameView::CV_HERO ||
       cv == iChildGameView::CV_CASTLE ||
        cv == iChildGameView::CV_MEET ||
            cv == iChildGameView::CV_GAMEMENU )
            gGame.PopScreen();

	m_viewStack.Pop();
	if(m_viewStack.GetSize() > 0)
		ShowView(m_viewStack.GetLast(), true);
	else
		ShowView(m_pChildView[cv]->ParentView(), true);
}

// Game process
void iGame::BeginBattle(const iBattleInfo& bi, bool quickbattle)
{
    check(!m_pBattle);
    quickbattle = quickbattle || gSettings.GetEntryValue(CET_QUICKCOMBAT);

    // save the game
#ifdef MULTIPLAYER
    if(!gMPMgr.IsMultiplayer()) {
#endif


	//	DISABLED before-battle crash save as it interferes with before-battle events (like teleport)
    // I hope it's very rare occasion and should be considered a map bug

	 if(gGame.Map().CurPlayer()->PlayerType() == PT_HUMAN) {
#ifndef ROYAL_BOUNTY
			gGame.Crashsave();
#ifdef OS_MACOS
   			gGame.Autosave();
#endif
#else
			gGame.Autosave();
#endif
        }


#ifdef MULTIPLAYER
    }
#endif



    // Assaulter cannot be neutral
    check(bi.m_pAssaulter->Owner() != PID_NEUTRAL);
    iPlayer* pAssaulter = m_Map.FindPlayer(bi.m_pAssaulter->Owner());
    iPlayer* pDefender = (bi.m_pDefender->Owner()==PID_NEUTRAL)?NULL:m_Map.FindPlayer(bi.m_pDefender->Owner());
    // Assaulter and Defender can't be owned by one player or side
    check(pAssaulter && pAssaulter != pDefender);

    // Reset env sounds
    if (pAssaulter->PlayerType() == PT_HUMAN) m_soundMap.ResetEnvSounds();

    // If one of side is controlled by human, the battle must be interactive
    bool bInt = pAssaulter->PlayerType() == PT_HUMAN || (pDefender && pDefender->PlayerType() == PT_HUMAN);

#ifdef MULTIPLAYER
TODO: == PT_HUMAN or PT_REMOTE for each
#endif

    // If both sides are controlled by human or remote, disable quickbattle
    if(pAssaulter->PlayerType() == PT_HUMAN && (pDefender && pDefender->PlayerType() == PT_HUMAN))
        quickbattle = false;

    if (bInt) {
        // Computer player attack the not active human player, so, change the active player first
        if (pDefender && pDefender->PlayerType() == PT_HUMAN && pAssaulter->PlayerType() != PT_HUMAN && pDefender != m_Map.ActPlayer()) {
            m_Map.SetNewActor(pDefender, true);
        }

        OnHeroStopMoving(gGame.Map().CurHero());
        if (quickbattle) {
            // Autobattle with result
            check(gGame.Map().CurHero() == bi.m_pAssaulter->SpellCaster());
            m_pBattle = new iAutoBattle(true);
        } else {
            // Interactive battle
            m_pBattle = new iInteractBattle();
        }
    } else {
#ifdef MULTIPLAYER
        // if one of the side is remote, the battle must be hidden
        if(pAssaulter->PlayerType() == PT_REMOTE || (pDefender && pDefender->PlayerType() == PT_REMOTE)) {
            m_pBattle = new iHiddenBattle();
        } else
#endif
            // Auto battle
            m_pBattle = new iAutoBattle();
    }

    m_pBattle->BeginBattle(bi);
}

void iGame::EndBattle()
{
    check(m_pBattle);
    delete m_pBattle;
    m_pBattle = NULL;

    if(gGame.Map().CurPlayer()->PlayerType() == PT_HUMAN) {
#ifndef ROYAL_BOUNTY
		gGame.Crashsave();
#else
	  //	gGame.Autosave();
#endif
	}
}

void iGame::MeetHeroes(iHero* pHero1, iHero* pHero2, bool bAct)
{
#ifdef MULTIPLAYER
    if(gMPMgr.IsReplaying())
        return;
#endif
    if (bAct){
		MeetView()->SetHeroes(pHero1, pHero2);
        ShowView(iChildGameView::CV_MEET);

    }
}

sint32 iGame::Process(fix32 t)
{
    if (m_bGoToMainMenu) {
        m_bGoToMainMenu = false;
        ExitGame(true);
        return 0;
    }
#ifdef MULTIPLAYER
    if(gMPMgr.PollSockets(t)) return 0; //network system is awaiting response (blocked)
#endif
    if(m_delayedCV != iChildGameView::CV_UNDEFINED) {
        HideView(m_delayedCV);
        m_delayedCV = iChildGameView::CV_UNDEFINED;
    } else if(m_delayedShowCV != iChildGameView::CV_UNDEFINED) {
        ShowView(m_delayedShowCV);
        m_delayedShowCV = iChildGameView::CV_UNDEFINED;
    } else if(m_bDelayedStart) {
        m_bDelayedStart = false;
        ExitGame(false);
        StartNewGame(m_delMapInfo, m_bDelNewGame, true);
    }else if (ActView() && ActView()->Process(t) && m_tActView == iChildGameView::CV_OVERLAND && !gGame.BattleWrapper() && !gApp.ViewMgr().HasModalDlg() &&
        !gGame.IsExitingToMenu()) {
        m_Map.Process(t);
    }

//    gMapShop.GetStatus();

    // purge the delete list. safe if not in modal mode (Hedin)
    if(!gApp.ViewMgr().HasModalDlg()) {
        for(uint32 x=0; x<m_deleteList.GetSize(); x++)
            delete (iView*)m_deleteList[x];
        m_deleteList.RemoveAll();
    }

	// drive the animations
	gAniHost.Process(t);

	// auto next turn
	if(m_bNextTurn) {
		gGame.Map().NextPlayer();
		m_bNextTurn = false;
	}
#if defined( PC_VERSION )
	m_Timer += 400;

	//hot up,down,left,right keys
	if( m_Timer > 1000 ){

		if( m_bKeyUpFlag ){

			m_Timer = 0;

			iPoint cell = m_pMainView->Composer().GetAnchor();

			cell.y -=40;
			m_pMainView->Composer().SetAnchor(cell);
		}
		if( m_bKeyDownFlag ){

			m_Timer = 0;

			iPoint cell = m_pMainView->Composer().GetAnchor();

			cell.y +=40;
			m_pMainView->Composer().SetAnchor(cell);
		}
		if( m_bKeyLeftFlag ){

            if( ActViewType() == iChildGameView::CV_OVERLAND ){

                m_Timer = 0;

                iPoint cell = m_pMainView->Composer().GetAnchor();

                cell.x -=30;
                m_pMainView->Composer().SetAnchor(cell);
            }
            else if( ActViewType() == iChildGameView::CV_HERO ){

                m_bKeyLeftFlag = false;
                Map().CurPlayer()->PrevHero();
            }
            else if( ActViewType() == iChildGameView::CV_CASTLE ){

                m_bKeyLeftFlag = false;
                iCastleView::CastleViewMode mode = CastleView()->GetViewMode();

                if( mode != iCastleView::CVM_BUILD ){
                    Map().CurPlayer()->PrevCastle();

                    if( mode != iCastleView::CVM_MARKET && mode != iCastleView::CVM_TAVERNDOCK && mode != iCastleView::CVM_RECRUIT && mode != iCastleView::CVM_MGUILD )
                        CastleView()->SetHighlighted(iCastleView::CVM_MAIN);
                }
            }
		}
		if( m_bKeyRightFlag ){

            if( ActViewType() == iChildGameView::CV_OVERLAND ){

                m_Timer = 0;

                iPoint cell = m_pMainView->Composer().GetAnchor();

                cell.x +=30;
                m_pMainView->Composer().SetAnchor(cell);
            }
            else if( ActViewType() == iChildGameView::CV_HERO ){

                m_bKeyRightFlag = false;
                Map().CurPlayer()->NextHero();
            }
            else if( ActViewType() == iChildGameView::CV_CASTLE ){

                m_bKeyRightFlag = false;
                iCastleView::CastleViewMode mode = CastleView()->GetViewMode();

                if( mode != iCastleView::CVM_BUILD ){
                    Map().CurPlayer()->NextCastle();

                    if( mode != iCastleView::CVM_MARKET && mode != iCastleView::CVM_TAVERNDOCK && mode != iCastleView::CVM_RECRUIT && mode != iCastleView::CVM_MGUILD)
                        CastleView()->SetHighlighted(iCastleView::CVM_MAIN);
                }
            }
		}
	}

	if( m_bSaveScreen && ActViewType() == iChildGameView::CV_OVERLAND ){

		m_bSaveScreen = false;

		m_pChildView[9]->SetSaveFlagTrue();

		ShowView( iChildGameView::CV_GAMEMENU, false );

	}
	if( m_bLoadScreen && ActViewType() == iChildGameView::CV_OVERLAND ){

		m_bLoadScreen = false;

		m_pChildView[9]->SetLoadFlagTrue();

		ShowView( iChildGameView::CV_GAMEMENU, false );

	}
	//Cast spell
	if( m_bSpell ){

		m_bSpell = false;

		ShowView(iChildGameView::CV_HERO);

		HeroView()->m_bSpell = true;
		HeroView()->SpellDlg();
	}

	if( m_bNumMove ){

		m_bNumMove = false;

		if( pos != iPoint(0,0) ){

			MainView()->SetHeroPath(pos);
			pos = iPoint(0,0);
		}

		Map().CurHero()->Step();
	}

/*    if( m_bQuickLoad ){

        m_bQuickLoad = false;
        Quickload();
    }
*/
    if( m_bQuickSave ){

        m_bQuickSave = false;
        Quicksave();
    }


    if( m_bExitDlg ){

        m_bExitDlg = false;

        if( gApp.ViewMgr().HasModalDlg() )
            gApp.ViewMgr().PeepDlg()->OnDown();
        else if( ActViewType() == iChildGameView::CV_GAMERESULT )
            MainMenu();
    }

    if( m_bEnter ){

        m_bEnter = false;

        //In Modal Dlg
        if( gApp.ViewMgr().HasModalDlg() )
            gApp.ViewMgr().PeepDlg()->OnClickYes();
        //Show current hero's screen
        else if(Map().CurPlayer()) {

			if( Map().CurPlayer()->CurCastle() )
				ShowView(iChildGameView::CV_CASTLE);
			else if(Map().CurPlayer()->CurHero())
				ShowView(iChildGameView::CV_HERO);
        }
    }

    if( m_bEsc ){

        m_bEsc = false;

        //Exit Dialog
        if( gApp.ViewMgr().HasModalDlg() )
            gApp.ViewMgr().PeepDlg()->OnClickNo();
        else{
            //Exit Minimap
            if( m_bMinimapEsc ){

                m_bMinimapEsc = false;
                MainView()->MinimapRawView()->SetVisible(false);
                MainView()->UpdateButtons();
            }
            else{
                //Exit Menu
                switch (ActViewType()){

                    case iChildGameView::CV_GAMEMENU:
                        HideView(iChildGameView::CV_GAMEMENU);
                        break;
                    case iChildGameView::CV_UPGRADE:
                        HideView(iChildGameView::CV_UPGRADE);
                        break;
                    case iChildGameView::CV_CASTLE:
                        HideView(iChildGameView::CV_CASTLE);
                        break;
                    case iChildGameView::CV_HARBOR:
                        HideView(iChildGameView::CV_HARBOR);
                        break;
                    case iChildGameView::CV_WARACADEMY:
                        HideView(iChildGameView::CV_WARACADEMY);
                        break;
                    case iChildGameView::CV_HERO:
                        HideView(iChildGameView::CV_HERO);
                        break;
                    case iChildGameView::CV_MINIMAP:
                        //HideView(iChildGameView::CV_MINIMAP);
                        break;
                    case iChildGameView::CV_MEET:
                        HideView(iChildGameView::CV_MEET);
                        break;
                    case iChildGameView::CV_GAMERESULT:
                        ExitToMenu();
                        break;
                    case iChildGameView::CV_OVERLAND:
                    case iChildGameView::CV_BATTLE:
                        ShowView(iChildGameView::CV_GAMEMENU);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if(m_bCenter){

        m_bCenter = false;

        MainView()->Composer().CenterView(Map().CurHero()->Pos());
    }

    if( m_bShowCastle ){

        m_bShowCastle = false;

        if( gGame.Map().CurHero() )
            if( iMapCnst* mc = gGame.Map().CurHero()->GetLocation() )
                if (iCastle* pCastle = DynamicCast<iCastle*>(mc)){

                    CastleView()->SetCastle(pCastle);
                    ShowView(iChildGameView::CV_CASTLE);
                }
                else
                    gGame.Map().CurHero()->ActivateConstruction(mc);

//        ShowView(iChildGameView::CV_CASTLE);
    }
#endif
    return 0;
}

void iGame::OnKeyDown(sint32 key)
    {
}

void iGame::OnKeyUp(sint32 key)
{
	//xxc::sec_shuffle();

//     if (key == gSettings.ActionKey(BAT_MINIMIZE_APP)) {
//         gApp.Minimize();
//     } else if (key == gSettings.ActionKey(BAT_MAKE_SCREENSHOT)) {
//         int i;
//         iStringT scrndir = gRootPath + _T("Scrn\\");
//         iFile::DirCreate(scrndir);
//         for (i = 1; i < 999; i++)
//         {
//             iStringT fname = scrndir + iFormat(_T("screenshot%03d.bmp"), i);
//             if (!iFile::Exists(fname))
//             {
//                 SaveDibBitmap16(gApp.Surface(), fname);
//                 AddMsg(iStringT(_T("#F4B4")) + gTextMgr[TRID_MSG_SCREENSHOT_SAVED]);
//                 break;
//             }
//         }

//         //      iStringT fname = gRootPath + _T("screenshot.bmp");
//         //      SaveDibBitmap16(gApp.Surface(), fname);
//         //      AddMsg(iStringT(_T("#F4B4")) + gTextMgr[TRID_MSG_SCREENSHOT_SAVED]);
//     }
//     // Cotulla: NOT SUPPORTED FOR IPAD
// #if 0
//     else if (key == gSettings.ActionKey(BAT_QUICK_SAVE))
//     {
// #ifdef MULTIPLAYER
//         if (!gMPMgr.IsMultiplayer())
//         {
// #endif
//                 iStringT fname;
//                 iSaveDlg::GetSaveFileName(fname, SAVE_GAME_SLOTS - 2);
//                 SaveGameFile(fname, gGame.Map() );
// #ifdef MULTIPLAYER
//         }
// #endif
//     }
// #endif

}


// Msgs
void iGame::AddMsg(const iStringT& msg)
{
    if (m_pMainView) m_pMainView->AddMsg(msg);
}

void iGame::AddCellMsg(const iStringT& msg, const iPoint& pos)
{
    if (m_pMainView) m_pMainView->AddCellMsg(msg, pos);
}


// Animation
void iGame::OnDisapObject(SpriteId sid, const iPoint& pos, const iPoint& offset)
{
#ifdef MULTIPLAYER
    gMPMgr.OnDisapObject(pos);
#endif
    if (!gGame.Map().ActPlayer()->FogMap().IsHidden(pos)) {
        m_pMainView->Composer().SetAniObj(new iAniObj_FadeOut(&m_pMainView->Composer(), sid,pos));
    }
}

// Game notifications
void iGame::OnVictory( VICTORY_CONDITION_TYPE vc )
{
    if(gGame.IsExitingToMenu() ||
	   gGame.ActViewType() == iChildGameView::CV_GAMERESULT)
        return;
#ifdef MULTIPLAYER
        /* if we have observer, inform him */
        if(m_pObserver) {
            // if we have only computers left, terminate the game...
            bool havehuman = false;
            for (iGameWorld::iPLIt pit = m_Players.First(); pit != m_Players.End(); ++pit){
                if(pit->PlayerType() != PT_COMPUTER) {
                    havehuman = true;
                    break;
                }
            }
            if(!havehuman)
                gMPMgr.OnPlayerWon(m_Players.First()->PlayerId());

            iTextDlg tdlg(&gApp.ViewMgr(), _T("The game ended."), _T(""),PID_NEUTRAL);
            tdlg.DoModal();
            //gGame.MainMenu();
            gMPMgr.DisconnectFromServer();
            return;
        }
    gMPMgr.OnPlayerWon(gGame.Map().ActPlayer()->PlayerId());
#endif

	// Show victory dialog
	iTextDlg tdlg(&gApp.ViewMgr(), gTextMgr[TRID_VICTORY], gTextMgr[TRID_VICT_DEFEAT_ALL_ENEMIES + vc],PID_NEUTRAL);
    tdlg.DoModal();

	/*Hall of fame disabled
	uint32 ut = GetCurrentTimestamp();
    iHighScore::iEntry entry(m_Map.MapName(), ut, m_Map.m_CurDay, m_Map.CalcGameScore());

    iDlg_HallOfFame dlg(&gApp.ViewMgr(), gSettingsPath + _T("PalmHeroes.hsc"), entry);
    dlg.DoModal();
    */

	/* show game result screen */
#ifndef ROYAL_BOUNTY
	gGame.GameResultView()->SetTitle(gTextMgr[TRID_VICTORY_CAP]);
	gGame.GameResultView()->SetText(gTextMgr[TRID_VICT_SHORT]);
	gGame.GameResultView()->SetType(IGameResultView::GRWin);
	gGame.GameResultView()->SetMapName(gGame.Map().MapName());
#else
	gGame.GameResultView()->SetTitle(_T("Victory!"));
	gGame.GameResultView()->SetType(iGameResultView::GRWin);
#endif
	gGame.ShowView(iChildGameView::CV_GAMERESULT);
    if(gGame.Map().GameMode() == iMapInfo::GM_SPLAYER)
        Analytics::sharedInstance()->TrackEvent("game", "won map: single", CvtT2A<>(gGame.Map().MapEnglishName().CStr()).Unsafe());

	iFile::Delete(RelativeFilePath("lastses.tmp", FileLocationType::FLT_DOCUMENT));
}

void iGame::OnDefeat( LOSE_CONDITION_TYPE lc, bool bExitGame )
{
	if(Map().ActPlayer() && Map().ActPlayer()->PlayerType() != PT_OBSERVER) {
		if(!bExitGame) {
			iTextDlg tdlg(&gApp.ViewMgr(), gTextMgr[TRID_DEFEAT], gTextMgr[TRID_LOSE_DEFAULT + lc],PID_NEUTRAL);
			tdlg.DoModal();
		}
		else
		{
			iTextDlg tdlg(&gApp.ViewMgr(), gTextMgr[TRID_DEFEAT], gTextMgr[TRID_LOSE_DEFAULT + lc],PID_NEUTRAL);
			tdlg.DoModal();

			gGame.GameResultView()->SetTitle(gTextMgr[TRID_DEFEAT_CAP]);
			gGame.GameResultView()->SetText(gTextMgr[TRID_LOSE_SHORT]);
			gGame.GameResultView()->SetType(IGameResultView::GRDefeat);
			gGame.GameResultView()->SetMapName(gGame.Map().MapName());

			gGame.ShowView(iChildGameView::CV_GAMERESULT);
		}
    }
    if(gGame.Map().GameMode() == iMapInfo::GM_SPLAYER)
        Analytics::sharedInstance()->TrackEvent("game", "lost map: single", CvtT2A<>(gGame.Map().MapEnglishName().CStr()).Unsafe());
	   iFile::Delete(RelativeFilePath("lastses.tmp", FileLocationType::FLT_DOCUMENT));
}

void iGame::OnActorChanged(iPlayer* pNewActor, bool bAttack)
{
    m_pChildView[m_tActView]->SetVisible(false);
    iBlackBackView bbv;
    gApp.ViewMgr().SetCurView(&bbv);
    iStringT msg;
    if (bAttack) msg = iFormat(gTextMgr[TRID_MSG_PLAYER_UNDER_ATTACK], gTextMgr[TRID_PLAYER_RED+pNewActor->PlayerId()]);
    else msg = iFormat(gTextMgr[TRID_MSG_PLAYER_TURN], gTextMgr[TRID_PLAYER_RED+pNewActor->PlayerId()]);
    iIconDlg dlg(&gApp.ViewMgr(), iStringT(), msg, PDGG(EMBLEMS)+pNewActor->PlayerId(), pNewActor->PlayerId());
    dlg.DoModal();
    m_pChildView[m_tActView]->SetVisible(true);
    gApp.ViewMgr().SetCurView(m_pChildView[m_tActView]);
}

void iGame::OnPlayerChanged(iPlayer* pNewPlayer, bool bAct)
{
    m_soundMap.ResetEnvSounds();
#ifndef MULTIPLAYER
    m_pMainView->OnPlayerChanged(pNewPlayer,bAct);
#else
    m_pMainView->OnPlayerChanged(pNewPlayer,bAct && !gMPMgr.IsReplaying());
#endif
}

void iGame::OnPlayerVanquished(PLAYER_ID pid)
{
	if(Map().ActPlayer() && Map().ActPlayer()->PlayerType() != PT_OBSERVER) {
		iTextDlg tdlg(&gApp.ViewMgr(), _T(""), iFormat(gTextMgr[TRID_MSG_PLAYER_VANQUISHED], gTextMgr[TRID_PLAYER_RED+pid]),pid);
		tdlg.DoModal();
	}
}

void iGame::OnAddCastle(iCastle* pCastle)
{
    m_pMainView->OnCastleListChanged();
}

void iGame::OnDelCastle(iCastle* pCastle)
{
    m_pMainView->OnCastleListChanged();
}

void iGame::OnCastleChanged(iCastle *pCastle)
{
	m_pMainView->HighlightObject(pCastle, iMainView::HLT_AVAILABLE);
    if (!gGame.Map().ActPlayer()->FogMap().IsHidden(pCastle->Pos())) {
        m_pMainView->Composer().CenterView(pCastle->Pos(),iPoint());
    }

    if (m_tActView == iChildGameView::CV_CASTLE) {
        if (iCastle* pCurCastle = gGame.Map().ActPlayer()->CurCastle()) ((ICastleView*)(m_pChildView[iChildGameView::CV_CASTLE]))->SetCastle(pCurCastle);
    }
}

void iGame::OnAddHero(iHero* pHero)
{
    m_pMainView->OnHeroListChanged();
}

void iGame::OnDelHero(iHero* pHero)
{
    if (m_tActView == iChildGameView::CV_HERO) {
		HideView(iChildGameView::CV_HERO);
    }

    m_pMainView->OnHeroListChanged();
    if (!gGame.Map().ActPlayer()->FogMap().IsHidden(pHero->Pos()) && m_tActView == iChildGameView::CV_OVERLAND) {
        m_pMainView->Composer().SetAniObj(new iAniObj_FadeOut(&m_pMainView->Composer(), GetHeroSprite(pHero),pHero->Pos()));
        if (gGame.Map().CurPlayer()->PlayerType() == PT_HUMAN) {
            if (!gGame.Map().ActPlayer()->CurHero()) m_soundMap.ResetEnvSounds();
            gSfxMgr.PlaySound(CSND_DEL_GUARD);
        }
    }

    if (iAI_Player* pAIPlayer = DynamicCast<iAI_Player*>(gGame.Map().CurPlayer())) {
        pAIPlayer->OnHeroDead(pHero);
    }
}

void iGame::OnAddShip(iShip* pShip)
{
}

void iGame::OnDelShip(iShip* pShip)
{
}


void iGame::OnHeroChanged(iHero* pHero)
{
#ifndef MULTIPLAYER
    if (!gGame.Map().ActPlayer()->FogMap().IsHidden(pHero->Pos()) && gGame.Map().CurPlayer()->PlayerId() == pHero->Owner()->PlayerId()) {
#else
    if (!gGame.Map().ActPlayer()->FogMap().IsHidden(pHero->Pos())) {
#endif
        m_pMainView->Composer().CenterView(pHero->Pos());
    }

    if (gGame.Map().ActPlayer()->PlayerId() == pHero->Owner()->PlayerId()) {
		m_pMainView->ResetTapMode();
		m_pMainView->HighlightObject(pHero, iMainView::HLT_AVAILABLE);

        m_soundMap.SetupEnvSounds(pHero->Pos());
    }

    if (m_tActView == iChildGameView::CV_HERO) {
		if (iHero* pCurHero = gGame.Map().ActPlayer()->CurHero()) ((IHeroView*)(m_pChildView[iChildGameView::CV_HERO]))->SetHero(pCurHero);
    }
}

void iGame::OnHeroLevel(iHero* pHero, uint8 level, const iNewLevelInfo& linfo)
{
}

const uint16 MOV_SOUNDS[STYPE_COUNT] = {
    CSND_HMOVE00,   // water
    CSND_HMOVE03,   // sand (beach)
    CSND_HMOVE06,   // dirt
    CSND_HMOVE01,   // grass
    CSND_HMOVE01,   // swamp
    CSND_HMOVE04,   // lava
    CSND_HMOVE03,   // wasteland
    CSND_HMOVE05,   // desert
    CSND_HMOVE05,   // snow
    CSND_HMOVE05,   // new desert
    CSND_HMOVE03,   // pavement
    CSND_HMOVE03,   // new wasteland
};

uint16 iGame::GetMovSound(SURF_TYPE st, bool bRoad)
{
    if (bRoad) return MOV_SOUNDS[STYPE_DIRT];
    return MOV_SOUNDS[st];
}

void iGame::PauseMoveSound()
{
	if(m_hmChannel != -1)
		gApp.SndPlayer().Stop(m_hmChannel);
}

void iGame::ResumeMoveSound()
{
	if(m_hmChannel != -1)
		m_hmChannel = gSfxMgr.PlaySound(m_hmSound, true);
}


void iGame::OnHeroMoveTo(iHero* pHero, const iPath::iStep& step)
{
    if (pHero->Owner() && gGame.Map().ActPlayer()->PlayerId() == pHero->Owner()->PlayerId()) {
        if (step.m_action == iPath::MoveTo || step.m_action == iPath::Enter) {
            sint32 px = step.m_Pos.x; sint32 py = step.m_Pos.y;
            SURF_TYPE st = (SURF_TYPE)m_Map.GetAt(px,py).SurfNode(0);
            uint16 avatar = m_Map.GetAt(px,py).avatar;
            uint16 snd  = GetMovSound(st, m_Map.m_CoverMap.GetAt(px,py) != NULL  || (avatar != 0xFFFF && ( avatar & 0xFF00) == 0xFF00) );
            if (m_hmChannel == -1) {
                m_hmSound = snd;
                m_hmChannel = gSfxMgr.PlaySound(m_hmSound, true);
            } else {
                if (m_hmSound != snd) {
                    m_hmSound = snd;
                    gSfxMgr.ReplaceSound(m_hmChannel, m_hmSound);
                }
            }

            if (m_tActView == iChildGameView::CV_OVERLAND) {
                m_pMainView->EnableToolBar(false);
            }
        }
    }

    if ((!gGame.Map().ActPlayer()->FogMap().IsHidden(pHero->Pos()) && !gGame.Map().ActPlayer()->FogMap().IsHidden(step.m_Pos))) {
        if (step.m_action == iPath::MoveTo || step.m_action == iPath::Enter) {
            if (m_pMainView->Composer().AniObj()) m_pMainView->Composer().RemoveAniObj();
            m_pMainView->Composer().SetAniObj(new iAniObj_MovHero(&m_pMainView->Composer(), pHero));
            check(pHero->Moving());
        } else pHero->Step();
    } else {
        pHero->Step();
    }
}


void iGame::OnHeroStopMoving(iHero* pHero)
{
    if (pHero->Owner() && gGame.Map().ActPlayer()->PlayerId() == pHero->Owner()->PlayerId()) {
        if (m_hmChannel != -1) {
            gApp.SndPlayer().Stop(m_hmChannel);
            m_hmChannel = -1;
        }
        if (m_tActView == iChildGameView::CV_OVERLAND) {
            m_pMainView->EnableToolBar(true);
            ((iMainView_touch*)m_pMainView)->UpdateButtons();
        }

    }
}

void iGame::OnHeroPosChanged(iHero* pHero, const iPoint& npos)
{
    if (pHero->Owner()->PlayerType() == PT_HUMAN) {
        m_soundMap.SetupEnvSounds(pHero->Pos());
    }
}

void iGame::OnHeroTeleport(iHero* pHero, const iPoint& src, const iPoint& dst)
{
    if (pHero->Owner()->PlayerType() == PT_HUMAN || (!gGame.Map().ActPlayer()->FogMap().IsHidden(src) && !gGame.Map().ActPlayer()->FogMap().IsHidden(dst))) {
        if (m_pMainView->Composer().AniObj()) m_pMainView->Composer().RemoveAniObj();
        m_pMainView->Composer().SetAniObj(new iAniObj_Teleport(&m_pMainView->Composer()));
        if (gGame.Map().ActPlayer()->PlayerId() == pHero->Owner()->PlayerId()) {
            gSfxMgr.PlaySound(CSND_TELEPORT_OUT);
        }
    }
}

void iGame::OnAttackHero(iHero* pHero1, iHero* pHero2)
{
}

void iGame::SetLHandMode(bool lhandmode)
{
    ((iMainView_touch*)m_pMainView)->SetLHandMode(lhandmode);
}

void iGame::PushScreen()
{
    iDib n;
    n.Init(gApp.Surface().GetSize(), iDib::RGB);
    gApp.Surface().CopyToDibXY(n, iPoint());
	iRect all(iPoint(), gApp.Surface().GetSize());
	n.Darken50Rect(all);
	n.Darken50Rect(all);


	m_screenStack.Push(n);
/*    for(uint32 xx=0; xx<iChildGameView::CV_COUNT; xx++)
        if(m_pChildView[xx])
        {
            m_pChildView[xx]->UpdateSize();
            m_pChildView[xx]->Invalidate();
        }
*/
}

void iGame::UpdateViewSizes()
{
    for (uint32 xx = 0; xx < iChildGameView::CV_COUNT; ++xx)
    {
        if (m_pChildView[xx])
        {
#if defined( PC_VERSION )
			if( m_pChildView[xx]->Type() == iChildGameView::CV_CASTLE ||
                m_pChildView[xx]->Type() == iChildGameView::CV_BATTLE ||
                m_pChildView[xx]->Type() == iChildGameView::CV_HERO   ||
                m_pChildView[xx]->Type() == iChildGameView::CV_MEET
               ){
 #ifdef OS_WIN32
				HWND hWnd = gApp.Window().GetHWND();
				RECT rect;

				GetWindowRect( hWnd, &rect );

				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;

				if( gApp.IsFullScreen() )
					m_pChildView[xx]->SetRect(iRect( (GetSystemMetrics(SM_CXSCREEN)-width)/2+(width-1024)/2, (GetSystemMetrics(SM_CYSCREEN)-height)/2+(height-768)/2, 1024,768));
                else
                    m_pChildView[xx]->SetRect(iRect( (width-1040)/2, (height-806)/2, 1024,768));

#elif defined( OS_MACOS )
				if( isFullScreen() )
					m_pChildView[xx]->SetRect(iRect( (GetScreenWidth()-GetWindowWidth())/2+(GetWindowWidth()-1024)/2, (GetScreenHeight()-GetWindowHeight())/2+(GetWindowHeight()-768)/2, 1024,768));
                else
                    m_pChildView[xx]->SetRect(iRect( (GetWindowWidth()-1024)/2, (GetWindowHeight()-768)/2, 1024,768));
#endif

			}
            else
#endif
                m_pChildView[xx]->SetRect(gApp.Surface().GetSize());

            m_pChildView[xx]->UpdateSize();
            m_pChildView[xx]->Invalidate();
        }
    }

    if( Map().CurPlayer() ){

        if( Map().CurPlayer()->CurHero() ){

            MainView()->Composer().CenterView( Map().CurPlayer()->CurHero()->Pos() );
        }
        else if( Map().CurPlayer()->CurCastle() ){

            MainView()->Composer().CenterView( Map().CurPlayer()->CurCastle()->Pos() );
        }
    }
}

bool iGame::CheckPlayerWinAndLose( iPlayer *pPlayer )
{
	if(!gGame.Started() || !pPlayer) return false;
	VICTORY_CONDITION_TYPE vc = pPlayer->MeetsVictoryCondition();
	if(vc != VC_INVALID) {
		if(pPlayer == Map().ActPlayer()) {
			// active player won - show the win message
			gGame.OnVictory(vc);
		} else {
			// otherwise we lost because someone else won
			gGame.OnDefeat(LC_OTHER_WON, true);
		}
		return true;
	}

	LOSE_CONDITION_TYPE lc = pPlayer->MeetsLoseCondition();

	if(lc != LC_INVALID) {
		gGame.Map().RemovePlayer(pPlayer, lc);
		return true;
	}
	return false;
}


void iGame::Crashsave()
{
	bool bOk = false;
	iFilePtr pFile(CreateWin32File(RelativeFilePath("lastses.tmp", FileLocationType::FLT_DOCUMENT)));
	check(pFile);
	bOk = (pFile && gGame.Map().SaveToFile(pFile.operator->()));
	if (bOk)
      iFile::Move(
          RelativeFilePath("lastses.tmp", FileLocationType::FLT_DOCUMENT),
          RelativeFilePath("lastses.phsd", FileLocationType::FLT_DOCUMENT)
        );
}

void iGame::Autosave()
{
	RelativeFilePath fname;
	iSaveDlg::GetSaveFileName(fname,0);
	iFile::Delete(RelativeFilePath("lastses.tmp", FileLocationType::FLT_DOCUMENT));

	SaveGameFile( fname, gGame.Map() );
}

extern const uint32 SAVE_GAME_SLOTS;
#if defined( OS_MACOS )
void iGame::ShowSaveScreen(){

	HeroView()->SetHero(gGame.Map().CurHero());
	ShowView(iChildGameView::CV_HERO);
}
#endif
void iGame::Quicksave()
{
	RelativeFilePath path;
	iSaveDlg::GetSaveFileName(path, SAVE_GAME_SLOTS - 1);
	//iFile::Delete(gSavePath + _T("lastses.tmp"));

	SaveGameFile( path, gGame.Map() );
}

void iGame::Quickload()
{
  RelativeFilePath path;
	iSaveDlg::GetSaveFileName(path, SAVE_GAME_SLOTS - 1);
	iMapInfo mapInfo;
	mapInfo.m_bNewGame = false;
	mapInfo.m_FileName = path;

	iFilePtr pFile(OpenWin32File(mapInfo.m_FileName));

	if(!pFile)
		return;

	iQuestDlg qdlg(&gApp.ViewMgr(), iStringT(), gTextMgr[TRID_CONFIRM_RELOAD], PID_NEUTRAL);
	if (qdlg.DoModal() == DRC_YES) {
		uint32 fourcc; pFile->Read(&fourcc,sizeof(fourcc));
		if (fourcc == GMAP_FILE_HDR_KEY && mapInfo.ReadMapInfo(pFile.get())) {
			ExitGame(false);
			mapInfo.ReorderPlayers();
			StartNewGame(mapInfo, false, false);
		}
	}
}

bool IsOneHanded()
{
#if defined(PC_VERSION) || (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR)
	return true;
#endif
	return gSettings.GetEntryValue(CET_ONE_HANDED_MODE) > 0;
}
