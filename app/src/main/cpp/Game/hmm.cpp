#include "stdafx.h"
#include "game.h"
#include "gxl.cmdline.h"
#include "gxl.power.h"
#include "Dlg_Register.h"
#include "Dlg_GameLanguage.h"
#include <string>

//#include "xxc/xxc.dllfn.h"
//#include "xxl.guard.h"
//#include "xxc/xxc.sysinfo.h"
//#include "xxc/wce.dyncode.h"
//#include "xxc/xxc.security.h"
//#include "xxc/wce.detects.h"
//
#include <stdlib.h>
#include "IntroDlg.h"
#include "lang.h"

#include "Splash.h"
// #include "NewMapShop.h"

#include "OverlandView.h"
#include "OverlandView.touch.h"

#include "BattleView.h"
#include "BattleView.touch.h"

#include "Dlg_Save.h"
#include "GameMenuDlg.h"
#include "helpers.h"

#include "CastleView.h"
#include "Dlg_Spellbook.h"
#include "MinimapView.h"
#include "Dlg_Chest.h"
#include "iosdk/iosdk.h"
#include "FileAccessor/FileAccessor.h"

using namespace iosdk;
using namespace fileaccessor;
void log(const std::string& str);

class iDlg_Message;

struct Dummy {
	~Dummy() {
		mem_report();
	}
};

Dummy _dummy;

#ifdef _DEEP_DEBUG_
#define DBG( x ) x
void SEO( int stream )
{
   _CrtSetReportMode( stream, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG );
   _CrtSetReportFile( stream, _CRTDBG_FILE_STDOUT );
}

inline void InitDBG()
{
    DBG( SEO( _CRT_ERROR ) );
    DBG( SEO( _CRT_WARN ) );
    DBG( SEO( _CRT_ASSERT ) );

    DBG(_CrtSetDbgFlag( _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG ) | ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF )) );
}

inline void DoneDBG()
{
    DBG( _CrtCheckMemory() );
}

struct DebugCls
{
	DebugCls()
	{ InitDBG();}
	~DebugCls()
	{ DoneDBG(); }
};
DebugCls cls_;

#else
#define DBG( x )
#endif


RelativeFilePath		gDataPath;
RelativeFilePath		gSavePath;
RelativeFilePath		gAppDataPath;
RelativeFilePath		gBundledMapsPath;
RelativeFilePath		gDownloadedMapsPath;
RelativeFilePath    gMapStatFilePath;
RelativeFilePath		gMusicPath;
RelativeFilePath    gMapShopCfgFilePath;
RelativeFilePath    gConfigFilePath;
RelativeFilePath    gTutorialCfgFilePath;

iDibReader		gDibReader;
iTextManager	gTextMgr;

iGXApp&			gApp = iGXApp::Instance();;
iTextComposer&  gTextComposer = iTextComposer::Instance();
iGfxManager&	gGfxMgr = iGfxManager::Instance();
iSfxManager&	gSfxMgr = iSfxManager::Instance();
iMusicManager&	gMusicMgr = iMusicManager::Instance();
iAnimationHost	gAniHost;
#ifdef MULTIPLAYER
iMPManager		gMPMgr;
#endif
iGame			gGame;
iSettings		gSettings;
iTutorial       gTutorial;
iMapEnum        gMapEnum;
bool			gRegistered = false;
bool			gEnterNewKey = false;
bool			gMinimap = false;

uint32			gOemInfoHash;
iSecret*		pSecret = NULL;
uint32*			pSecNum = NULL;

void*			pDynCode = NULL;
uint8*			pBloomBits=NULL;
bool            gUseIpadUI = false;

/*void WaitFor(sint32 msec)
{
	do {
	  MSG  msg;
		if (::PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE )) {
			while (::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE)) {
				if ( msg.message == WM_QUIT ) return;
				DispatchMessage( &msg );
			}
		}
		Sleep(30);
		msec -= 30;
	} while (!gApp.Input().EntryCount() && msec > 0);
	}*/

void ShowProgressReport( uint32 curProg, bool initial = true )
{
	//
	iDisplay& display = gApp.Display();
	iRect rcScreen = display.SurfMetrics();
	iPoint ldrOrg  = rcScreen.Center();
	ldrOrg.y += rcScreen.h / 3;
	uint32 pbarWidth = (rcScreen.w * 5) / 6;
	iRect rcPbar( ldrOrg.x - pbarWidth/2, ldrOrg.y, pbarWidth, 10 );
	uint16 pbarColor = 48;
	if ( !initial ) pbarColor = 127;
	ComposeProgressBar( false, display.GetSurface(), rcPbar, pbarColor, curProg, 100 );
	//gGfxMgr.Blit(12,gApp.Display().GetSurface(), iPoint(0,0) );
	if ( !initial ) {
		rcPbar.MoveY( -16 );
		iTextComposer::FontConfig loadingFC (GetButtonFont(0));
		gTextComposer.TextOut( loadingFC, gApp.Display().GetSurface(), rcPbar.TopLeft(), _T("Prepare for battle...") );
	}
	display.DoPaint( display.SurfMetrics() );
#ifdef OS_WIN32
	Sleep(1);
#else
	//usleep(0.001f);
#endif
}

bool EnsureMemAvail( DWORD mega )
{
#ifdef OS_WINCE
	uint32 bytesNeeded = mega * (1024*1024);
	MEMORYSTATUS mst;
	mst.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus( &mst );
	if ( mst.dwTotalPhys < bytesNeeded ) {
		// out of luck, just report failure
		return false;
	}
	if ( mst.dwAvailPhys < bytesNeeded ) {
		// try to free up closing apps
		HINSTANCE hAygShell = LoadLibrary( _T("aygshell.dll") );
		void* pCloseApps = hAygShell ? (void*)GetProcAddress( hAygShell, _T("SHCloseApps") ) : NULL;
		if ( pCloseApps != NULL ) {
			for( size_t ntries = 0; ntries != 2; ntries++ ) {
				if ( ((BOOL(*)(DWORD))pCloseApps)( bytesNeeded ) ) 	return true;
			}
			return false;
		}
	}
#endif
	// no aygshell, just report success
	return true;
}

#ifdef DEBUG_LOG
iLogMgr gLogMgr;
#endif //DEBUG_LOG

#ifdef OS_IPHONE
void OpenItunesLink();
#endif
void RenderFPS(DWORD count) {
	gTextComposer.TextOut(gApp.Surface(), iPoint(0, 50), iFormat(_T("FPS: %d"), count));
}

#if defined(OS_APPLE) || defined(OS_ANDROID)
void SetRotated(bool bRotated)
{
	gApp.SetOrientation(true, !bRotated);
}

void onMouseDown(int x, int y, MouseId mID, MouseButtonId mbID) {
	gApp.Input().msg_OnMouseDown(x, y, mID, mbID);
}

void onMouseMove(int x, int y, MouseId mID, MouseButtonId mbID) {
	gApp.Input().msg_OnMouseMove(x, y, mID, mbID);
}

void onMouseUp(int x, int y, MouseId mID, MouseButtonId mbID) {

	gApp.Input().msg_OnMouseUp(x, y, mID, mbID);
}

void onMouseEntered(int x, int y, MouseId mID){

	if( !gMinimap && !gApp.ViewMgr().HasModalDlg() && (gGame.ActViewType() == iChildGameView::CV_OVERLAND || gGame.ActViewType() == iChildGameView::CV_HERO || gGame.ActViewType() == iChildGameView::CV_CASTLE || gGame.ActViewType() == iChildGameView::CV_MEET ||  gGame.ActViewType() == iChildGameView::CV_BATTLE ||
                gGame.ActViewType() == iChildGameView::CV_UPGRADE) )
		gApp.Input().msg_OnMouseEntered(x, y, mID);
}

void onMouseLeave(int x, int y, MouseId mID){

//	if(gGame.ActViewType() == iChildGameView::CV_OVERLAND)
//		gApp.Input().msg_OnMouseLeave(x, y, mID);
}

void onMouseWheelDown(sint16 line, MouseId mID){

	gApp.Input().msg_OnMouseWheelDown(line, mID);
}

void onMouseWheelUp(sint16 line, MouseId mID){

	gApp.Input().msg_OnMouseWheelUp(line, mID);
}

void onRightMouseDown(int x, int y, MouseId mID, MouseButtonId mbID){
#if defined( PC_VERSION )
    gGame.bRightDown = true;
#endif
    gApp.Input().msg_OnMouseRightDown(x, y, mID, mbID);
}

void onRightMouseUp(int x, int y, MouseId mID, MouseButtonId mbID){
#if defined( PC_VERSION )
    gGame.bRightDown = false;
#endif
    gApp.Input().msg_OnMouseRightUp(x, y, mID, mbID);
}

void onKeyDown( sint32 key ){

	gApp.Input().msg_OnKeyDown(key);
}

bool IsActive()
{
	return gApp.IsActive();
}

void onSuspend()
{
	gApp.Suspend();
}

void onResume()
{
	gApp.Resume();
}

void onResize(){
#ifdef OS_MACOS
	gApp.UpdateSize();
#endif
/*
	iPoint cell = gGame.MainView()->Composer().GetAnchor();
	gGame.MainView()->Composer().SetAnchor(cell);
*/
	//gApp.UpdateSize();
}

char* languages[GLNG_COUNT] = {
		"en",
		"ru",
		"pl",
		"sk",
		"de",
		"cz",
		"it",
		"fr",
		"es"
};

void SetLanguage(const char *str) {
	if(!strcmp(str, "en"))
		SetGameLanguage(GLNG_ENGLISH);
	else if(!strcmp(str, "ru"))
		SetGameLanguage(GLNG_RUSSIAN);
	else if(!strcmp(str, "pl"))
		SetGameLanguage(GLNG_POLISH);
	else if(!strcmp(str, "en"))
		SetGameLanguage(GLNG_SLOVAK);
	else if(!strcmp(str, "de"))
		SetGameLanguage(GLNG_GERMAN);
	else if(!strcmp(str, "cs"))
		SetGameLanguage(GLNG_CZECH);
	else if(!strcmp(str, "it"))
		SetGameLanguage(GLNG_ITALIAN);
	else if(!strcmp(str, "fr"))
		SetGameLanguage(GLNG_FRENCH);
	else if(!strcmp(str, "es"))
		SetGameLanguage(GLNG_SPANISH);
	else
		SetGameLanguage(GLNG_ENGLISH);

//	SetGameLanguage(GLNG_RUSSIAN);
}

void PleerPlay(){
#if defined( PC_VERSION )
	gMusicMgr.Play();
#endif
	if(gGame.Map().CurHero())
		gGame.SoundMap().SetupEnvSounds(gGame.Map().CurHero()->Pos());
}

void PleerPause(){
#if defined( PC_VERSION )
	gMusicMgr.Pause();
#endif
	gGame.SoundMap().ResetEnvSounds();
}


//inline bool IsInMenu() {
bool IsInMenu() {

	switch (gGame.ActViewType()){

		case iChildGameView::CV_GAMEMENU:
		case iChildGameView::CV_CASTLE:
		case iChildGameView::CV_HARBOR:
		case iChildGameView::CV_WARACADEMY:
		case iChildGameView::CV_HERO:
		case iChildGameView::CV_MINIMAP:
			return( true );
		default:
			return( false );
	}
}

/*
void SetToolbarInfo(int px, int py){

	if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && !gApp.ViewMgr().HasModalDlg() ){

		iIsoMetric im(im_coef);
		iPoint actPos = gGame.MainView()->Composer().ActAnchor(iPoint(px,py));

		gGame.MainView()->m_ncell = im.Screen2Map(actPos);
		gGame.MainView()->m_bMouseEntered = true;
	}
}
*/
#ifdef OS_IPHONE
void onQuit() {

}
#endif
#endif

#if defined( OS_WIN32 )
void RightDown(){

	gGame.bRightDown = true;
}

void RightUp(){

	gGame.bRightDown = false;
}
#endif

#if defined( PC_VERSION )
/*
void ExitApp() {

    if(gGame.ActViewType() != iChildGameView::CV_MENU &&
       gGame.ActViewType() != iChildGameView::CV_BATTLE &&
       gGame.Map().CurPlayer()->PlayerType() == PT_HUMAN) {
        iStringT saveDir = gSavePath.Left(gSavePath.Length()-1);
        bool bOk = false;
        iFilePtr pFile(CreateWin32File(gSavePath + _T("lastses.tmp")));
        check(pFile);
        bOk = (pFile && gGame.Map().SaveToFile(pFile.operator->()));
        if (bOk) iFile::Move(gSavePath + _T("lastses.tmp"), gSavePath + _T("lastses.phs"));
    }
    gGame.Quit();
}*/

void onQuit() {

    gGame.m_bQuit = true;
}

bool ShowSaveScreen(){

	if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && !gApp.ViewMgr().HasModalDlg() && !gMinimap  )
		gGame.m_bSaveScreen = true;

	return( gGame.m_bSaveScreen );
}

bool ShowLoadScreen(){

	if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && !gApp.ViewMgr().HasModalDlg() && !gMinimap  )
		gGame.m_bLoadScreen = true;

	return( gGame.m_bLoadScreen );
}

void Minimize(){

	gApp.Minimize();
}

bool ExitDlgClick(){

    if( gApp.ViewMgr().HasModalDlg() || gGame.ActViewType() == iChildGameView::CV_GAMERESULT )
        gGame.m_bExitDlg = true;

	return( gGame.m_bExitDlg );
}

bool SetKeyUpFlag( bool flag ){

	if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && !gApp.ViewMgr().HasModalDlg() && !gMinimap )
		gGame.m_bKeyUpFlag = flag;

	return( gGame.m_bKeyUpFlag );
}

bool SetKeyDownFlag( bool flag ){

	if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && !gApp.ViewMgr().HasModalDlg() && !gMinimap )
		gGame.m_bKeyDownFlag = flag;

	return( gGame.m_bKeyDownFlag );
}

bool SetKeyLeftFlag( bool flag ){

    if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && !gApp.ViewMgr().HasModalDlg() && !gMinimap ||
        flag && gGame.ActViewType() == iChildGameView::CV_HERO && !gApp.ViewMgr().HasModalDlg() ||
        flag && gGame.ActViewType() == iChildGameView::CV_CASTLE && !gApp.ViewMgr().HasModalDlg() )
		gGame.m_bKeyLeftFlag = flag;

	return( gGame.m_bKeyLeftFlag );
}

bool SetKeyRightFlag( bool flag ){

	if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && !gApp.ViewMgr().HasModalDlg() && !gMinimap ||
        flag && gGame.ActViewType() == iChildGameView::CV_HERO && !gApp.ViewMgr().HasModalDlg() ||
        flag && gGame.ActViewType() == iChildGameView::CV_CASTLE && !gApp.ViewMgr().HasModalDlg() )
		gGame.m_bKeyRightFlag = flag;

    return( gGame.m_bKeyRightFlag );
}

bool NextPrevCastle( bool bNext ){

    if( gGame.ActViewType() == iChildGameView::CV_HERO && !gApp.ViewMgr().HasModalDlg() ||
        gGame.ActViewType() == iChildGameView::CV_CASTLE && !gApp.ViewMgr().HasModalDlg()){

        if( bNext ){

            gGame.m_bKeyRightFlag = true;
			return true;
		}
        else{
			gGame.m_bKeyLeftFlag = true;
			return true;
		}
    }
	return false;
}

bool flag = true;

bool Tab( bool bDown ){

	if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && !gApp.ViewMgr().HasModalDlg() && !gMinimap &&
       gGame.Map().CurPlayer() == gGame.Map().ActPlayer()){

		if( bDown ){

			if( !flag )
                return( true );

			gGame.MainView()->HeroCastleSelectView()->SetVisible(true);

			if(gGame.Map().CurPlayer()) {

				if(gGame.Map().CurPlayer()->CurHero())
					gGame.Map().CurPlayer()->NextHero();
				else if(gGame.Map().CurPlayer()->HeroCount() > 0)
					gGame.Map().CurPlayer()->SetCurHero(*gGame.Map().CurPlayer()->HeroFirst());
				else if(gGame.Map().CurPlayer()->CastleCount() > 0)
					gGame.Map().CurPlayer()->SetCurCastle(*gGame.Map().CurPlayer()->CastleFirst());

				iSize dlg_size = gGame.MainView()->HeroCastleSelectView()->GetSize(gGame.Map().CurPlayer()->HeroCount(), gGame.Map().CurPlayer()->CastleCount());
#if defined( OS_MACOS )
				gGame.MainView()->HeroCastleSelectView()->SetRect(gGame.MainView()->HeroCastleSelectView()->AlignChild(dlg_size, AlignCenter) + iPoint(macos_getCurScreenWidth()/2 - dlg_size.w/2, macos_getCurScreenHeight()/2 + 25 - dlg_size.h/2));
				gGame.MainView()->HeroCastleSelectView()->SetRect(gGame.MainView()->HeroCastleSelectView()->AlignChild(dlg_size, AlignCenter) + iPoint(macos_getCurScreenWidth()/2 - dlg_size.w/2, macos_getCurScreenHeight()/2 + 25 - dlg_size.h/2));
#else
				HWND hWnd = gApp.Window().GetHWND();
				RECT rect;

				GetWindowRect( hWnd, &rect );

				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;

				gGame.MainView()->HeroCastleSelectView()->SetRect(gGame.MainView()->HeroCastleSelectView()->AlignChild(dlg_size, AlignCenter) + iPoint(width/2 - dlg_size.w/2, height/2 + 25 - dlg_size.h/2));
				gGame.MainView()->HeroCastleSelectView()->SetRect(gGame.MainView()->HeroCastleSelectView()->AlignChild(dlg_size, AlignCenter) + iPoint(width/2 - dlg_size.w/2, height/2 + 25 - dlg_size.h/2));
#endif
			}
			gGame.MainView()->HeroCastleSelectView()->Update();
			flag = false;
		}
		else{

			gGame.MainView()->HideHCSelect();
			flag = true;
		}

		gGame.MainView()->UpdateButtons();

		return( true );
	}

	return( false );
}

bool EndTurn(){

	if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && !gApp.ViewMgr().HasModalDlg() && !gMinimap  )
		gGame.MainView()->m_bEndTurn = true;

	return( gGame.MainView()->m_bEndTurn );
}

bool Enter(){

    if( (gApp.ViewMgr().HasModalDlg() || gGame.Map().CurPlayer() && gGame.ActViewType() == iChildGameView::CV_OVERLAND) && !gMinimap )
        gGame.m_bEnter = true;

    return( gGame.m_bEnter );
}

bool CastleMarket(){

    if( gGame.ActViewType() == iChildGameView::CV_CASTLE && !gApp.ViewMgr().HasModalDlg() ){

        gGame.CastleView()->m_bMarket = true;
    }

    return true;
}

bool CastleTavern(){

    if( gGame.ActViewType() == iChildGameView::CV_CASTLE && !gApp.ViewMgr().HasModalDlg() ){

        gGame.CastleView()->m_bTavern = true;
    }

    return true;
}

bool CastleRecruit(){

    if( gGame.ActViewType() == iChildGameView::CV_CASTLE && !gApp.ViewMgr().HasModalDlg() ){

        gGame.CastleView()->m_bRecruit = true;
    }

    return true;
}

bool CastleBuild(){

    if( gGame.ActViewType() == iChildGameView::CV_CASTLE && !gApp.ViewMgr().HasModalDlg() ){

        gGame.CastleView()->m_bBuild = true;
    }

    return true;
}

bool Esc(){

    if( gApp.ViewMgr().HasModalDlg() ){

        gGame.m_bEsc = true;
    }
    else{

        if( gMinimap ){

            gMinimap = false;
            gGame.m_bEsc = true;
            gGame.m_bMinimapEsc = true;
        }
        else{

            //Exit Menu
            switch (gGame.ActViewType()){

                case iChildGameView::CV_GAMEMENU:
                case iChildGameView::CV_UPGRADE:
                case iChildGameView::CV_CASTLE:
                case iChildGameView::CV_HARBOR:
                case iChildGameView::CV_WARACADEMY:
                case iChildGameView::CV_HERO:
                case iChildGameView::CV_MINIMAP:
                case iChildGameView::CV_MEET:
                case iChildGameView::CV_GAMERESULT:
                case iChildGameView::CV_OVERLAND:
//                case iChildGameView::CV_BATTLE:
                    break;
                default:
                    return( gGame.m_bEsc );
            }
        }
        gGame.m_bEsc = true;
    }

    return( gGame.m_bEsc );
}

bool Defender(){

    if( gGame.ActViewType() == iChildGameView::CV_BATTLE && !gApp.ViewMgr().HasModalDlg() ){

        if( ((iBattleView_touch*)gGame.View(iChildGameView::CV_BATTLE))->IsAni() )
            return( false );

        ((iBattleView_touch*)gGame.View(iChildGameView::CV_BATTLE))->m_bDefender = true;
        return true;
	}
    return false;
}

bool Space(){

	if( Defender() ){

	}
    else if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && gGame.Map().CurHero() && !gGame.Map().CurHero()->Path().IsEmpty() && !gApp.ViewMgr().HasModalDlg() && !gMinimap )
		gGame.m_bNumMove = true;
    else if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && gGame.Map().CurHero() && !gApp.ViewMgr().HasModalDlg() && !gMinimap )
        gGame.m_bShowCastle = true;
	else
		return( false );

	return( true );
}

bool CenterMap(){

    if(gGame.Map().CurHero())
        gGame.m_bCenter = true;

    return( gGame.m_bCenter );
}

bool CastSpell(){

	if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && !gApp.ViewMgr().HasModalDlg() && !gMinimap ){

		gGame.m_bSpell = true;
        return( true );
    }
    else if( gGame.ActViewType() == iChildGameView::CV_BATTLE ){

        if( !((iBattleView_touch*)gGame.View(iChildGameView::CV_BATTLE))->IsAni() ){

            ((iBattleView_touch*)gGame.View(iChildGameView::CV_BATTLE))->m_bSpell = true;
            return( true );
        }
    }

	return( false );
}

bool MiniMap(bool bDown){

	if( !gMinimap ){

		if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && !gApp.ViewMgr().HasModalDlg() ){

			((iMainView_touch*)gGame.View(iChildGameView::CV_OVERLAND))->MinimapRawView()->PrepareSurface();
			((iMainView_touch*)gGame.View(iChildGameView::CV_OVERLAND))->MinimapRawView()->SetCenterCell(gGame.MainView()->Composer().GetCenterCell());
			((iMainView_touch*)gGame.View(iChildGameView::CV_OVERLAND))->MinimapRawView()->SetVisible(true);

			((iMainView_touch*)gGame.View(iChildGameView::CV_OVERLAND))->UpdateButtons();
			((iMainView_touch*)gGame.View(iChildGameView::CV_OVERLAND))->m_bMinimap = true;
			gMinimap = true;

			return( true );
		}
        else if( gGame.ActViewType() == iChildGameView::CV_CASTLE && !gApp.ViewMgr().HasModalDlg() ){

            CastleMarket();
        }
	}
	else{
		gMinimap = false;
		((iMainView_touch*)gGame.View(iChildGameView::CV_OVERLAND))->MinimapRawView()->SetVisible(false);
		((iMainView_touch*)gGame.View(iChildGameView::CV_OVERLAND))->UpdateButtons();
	}
	return( false );
}

void Num( unsigned short ch ){

	if( gGame.ActViewType() == iChildGameView::CV_OVERLAND && !gApp.ViewMgr().HasModalDlg() && !gMinimap ){

		if( gGame.Map().CurHero() ){

			iPoint pos = gGame.Map().CurHero()->Pos();

			switch (ch) {
#if defined( OS_MACOS )
				case 83: //Num1
#else
				case 97: //Num1
#endif
					pos.y += 1;
					break;
#if defined( OS_MACOS )
				case 84: //Num2
#else
				case 98: //Num2
#endif
					pos.x += 1;
					pos.y += 1;
					break;
#if defined( OS_MACOS )
				case 85:
#else
				case 99:
#endif
					pos.x += 1;
					break;
#if defined( OS_MACOS )
				case 86:
#else
				case 100:
#endif
					pos.x -= 1;
					pos.y += 1;
					break;
#if defined( OS_MACOS )
				case 87:
#else
				case 101:
#endif
					pos = iPoint(0,0);
					break;
#if defined( OS_MACOS )
				case 88:
#else
				case 102:
#endif
					pos.x += 1;
					pos.y -= 1;
					break;
#if defined( OS_MACOS )
				case 89:
#else
				case 103:
#endif
					pos.x -= 1;
					break;
#if defined( OS_MACOS )
				case 91:
#else
				case 104:
#endif
					pos.x -= 1;
					pos.y -= 1;
					break;
#if defined( OS_MACOS )
				case 92: //Num9
#else
				case 105:
#endif
					pos.y -= 1;
					break;
				default:
					return;
			}
#if defined( OS_MACOS )
			if( ch!=87 )
#else
			if( ch!=101 )
#endif
				gGame.Map().CurHero()->ResetPath();

			gGame.pos = pos;
			gGame.m_bNumMove = true;
		}
	}
}

bool AutoCombat( bool bEnd ){

	if( gGame.ActViewType() == iChildGameView::CV_BATTLE && !gApp.ViewMgr().HasModalDlg() ){

		if( bEnd )
            ( (iBattleView*)gGame.View(iChildGameView::CV_BATTLE) )->m_bEndAutocombat = true;
        else
            ( (iBattleView*)gGame.View(iChildGameView::CV_BATTLE) )->m_bBeginAutocombat = true;

		return( true );
	}

	return( false );
}

bool OnWait(){

	if( gGame.ActViewType() == iChildGameView::CV_BATTLE && !gApp.ViewMgr().HasModalDlg() ){
        //gGame.m_bWait = true;
        if( !((iBattleView_touch*)gGame.View(iChildGameView::CV_BATTLE))->IsAni() ){

            ( (iBattleView*)gGame.View(iChildGameView::CV_BATTLE) )->m_bWait = true;
            return( true );
        }
    }
	return( false );
}

bool QuickSave(){

	if(gGame.ActViewType() == iChildGameView::CV_OVERLAND &&
       /*gGame.ActViewType() != iChildGameView::CV_MENU &&
	   gGame.ActViewType() != iChildGameView::CV_BATTLE &&*/
	   gGame.Map().CurPlayer()->PlayerType() == PT_HUMAN &&
	   !gApp.ViewMgr().HasModalDlg() && !gMinimap ){

        gGame.m_bQuickSave = true;
	}
	return( gGame.m_bQuickSave );
}

bool QuickLoad(){

	if(gGame.ActViewType() == iChildGameView::CV_OVERLAND &&
       /*gGame.ActViewType() != iChildGameView::CV_MENU &&
	   gGame.ActViewType() != iChildGameView::CV_BATTLE &&*/
	   gGame.Map().CurPlayer()->PlayerType() == PT_HUMAN &&
	   !gApp.ViewMgr().HasModalDlg() && !gMinimap ){

		gGame.m_bQuickLoad = true;
	}
	return( gGame.m_bQuickLoad );
}

#endif

#if defined(OS_MACOS) || defined( OS_IPHONE ) || defined(OS_ANDROID)
u_int32_t GetWindowWidth();
u_int32_t GetWindowHeight();
#endif


bool IsIpad();

int preload_resources()
{
    // Adomatic::sharedInstance()->init("PK2", "4d01d62fb6a74af872ccd9d577ce56403386785a553d1a5ca8b9ce0eaa68c089");
        fileaccessor::FileAccessor::sharedFileAccessor()->init("Palm Kingdoms 2 Remastered", "iosoftware");

#ifdef OS_WINCE
        // check second instance of app
        if (iWindow::CheckSecondInstance())
        {
            return TRUE;
        }
#endif


        //gTwitter.Ping();
        // Root, Save and Maps folders
        // iFileName::GetAppPath(gRootPath);

        gDataPath = RelativeFilePath("Data/");
        gMusicPath = RelativeFilePath("Music/");
        gBundledMapsPath = RelativeFilePath("Maps/");
        gDownloadedMapsPath = RelativeFilePath("Maps/", FileLocationType::FLT_LIBRARY);
        gMapStatFilePath = RelativeFilePath("map.stat", FileLocationType::FLT_LIBRARY);
        gMapShopCfgFilePath = RelativeFilePath("mapshop.cfg", FileLocationType::FLT_LIBRARY);
        gTutorialCfgFilePath = RelativeFilePath("tutorial.cfg", FileLocationType::FLT_LIBRARY);
        gConfigFilePath = RelativeFilePath("PalmHeroes.cfg", FileLocationType::FLT_LIBRARY);
        gSavePath = RelativeFilePath("", FileLocationType::FLT_DOCUMENT);


        iFile::DirCreate(gSavePath);
        iFile::DirCreate(gDownloadedMapsPath);

#ifdef MULTIPLAYER
        // update updater if applicable
        iFile::Move(gRootPath + _T("updater.exe_"), gRootPath + _T("updater.exe"));
#endif

        // Init log manager
#ifdef DEBUG_LOG
        gLogMgr.Init(gRootPath+_T("game.log"));
#endif //DEBUG_LOG

#if defined(OS_APPLE) || defined(OS_WIN32) || defined(OS_ANDROID)
    LPCWSTR lpstrCmdLine = L"";
#endif


        if (!gSettings.Init(lpstrCmdLine)) {
            MessageBox(NULL, _T("Unable to init game settings!"), NULL, MB_OK);
        }

    // gMapShop.Init("PK2R", CvtT2A<>(gDownloadedMapsPath.CStr()).Unsafe());

        // Cotulla: is it right location? if not move
        if (!gMapEnum.Init())
        {
            return -1;
        }

        gTutorial.Init();

#ifdef MULTIPLAYER
        gMPMgr.Init();
#endif

        // Init application
        uint32 flags = GXLF_LANDSCAPE;
        if (!gSettings.ForceNoSound())
            flags |= GXLF_ENABLESOUND;
        if (gSettings.GetEntryValue(CET_LHANDMAODE)) flags |= GXLF_LHANDER;
#ifdef OS_WIN32

        // only for Win32 build
        // GXLF_FULLSCREEN - start application in fullscreen mode
        // no GXLF_FULLSCREEN - start application in windowed mode
        // note: this flag only checked in iGXApp::Init and have no effect later
        //if (gSettings.IsFullScreen())
        {
            //    flags |= GXLF_FULLSCREEN;
        }
#endif
    iSize sz = iSize(GetWindowWidth(), GetWindowHeight());
    if(IsIpad()) {
        if(sz.w >= 4096 && sz.h >= 3072) {
            flags |= GXLF_QUADSIZE;
        } else
        if(sz.w >= 2048 && sz.h >= 1536) {
            flags |= GXLF_DOUBLESIZE;
        }
    } else { // phone
        if (sz.w >= 1920 && sz.h >= 960) {
            flags |= GXLF_QUADSIZE;
        } else if (sz.w >= 960 && sz.h >= 480) {
            flags |= GXLF_DOUBLESIZE;
        }
    }

#if defined(OS_IPHONE) || defined(OS_ANDROID) // iOS
    if (!gApp.Init(&gGame, 60, flags, sz))
    {
        return -1;
    }
#else // Windows
    flags |= GXLF_FULLSCREEN;
    if (!gApp.Init((HINSTANCE)-1,L"PK2: Remastered", &gGame,30,flags,
                   iSize(768, 1024)
                   ))
    {
        return -1;
    }
#endif

        // ShowLogo and intro image (disabled for this version)
        //iIntroDlg idlg(&gApp.ViewMgr());
        //idlg.DoModal();


        // Setup global sfx volume
        if (gApp.SndPlayer().Inited()) {
            gApp.SndPlayer().SetVolume(gSettings.GetEntryValue(CET_SFXVOLUME) * 25);
        }

        // Setup initial gamma value
        gApp.Display().SetGamma(1.0f + 0.05f * gSettings.GetEntryValue(CET_DISPGAMMA));

        // Init secret dllLib
        pSecret = (iSecret*)malloc(sizeof(iSecret));
        memset(pSecret, 0, sizeof(iSecret));

#ifdef SPLASH
        // show the splash screen
        iSplash prldr;
        prldr.Show();
#endif
        // init music manager
        if(!gMusicMgr.Init()) {
            return -1;
        }

        // Init text composer
        if (!gTextComposer.Init(RelativeFilePath("Data/fonts_decoded.dat"), FS_COUNT, PBKEY_COMMON, PBKEY_FNT, GFNT_FILE_HDR_KEY, GFNT_FILE_VERSION)) {
            MessageBox(NULL, _T("Unable to init text composer!"), NULL, MB_OK);
            return -1;
        }

        // init font sizes
        InitFonts();
        // Init text resources
        if (!gTextMgr.Init()) {
            MessageBox(NULL, _T("Unable to init text manager!"), NULL, MB_OK);
            return -1;
        }

        gUseIpadUI = IsIpad();

        init_creat_desc();
        init_castle_cnsts();
        init_ships();

        // Init gfx resources and fill secret dll
        gGfxMgr.SetGamma( gSettings.GetEntryValue(CET_DISPGAMMA) );
        if (!gGfxMgr.Load(0,RelativeFilePath(std::string("Data/") + (gUseIpadUI ? "ipad/" : "iphone/") + "game.gfx"), gSettings.MapSpriteFile()?(iGfxManager::LM_MappedFile):(iGfxManager::LM_Memory))) {
            MessageBox(NULL, _T("Unable to open sprite file!"), NULL, MB_OK);
            return -1;
        }

        // Init sfx resources
        if (!gSfxMgr.Init(RelativeFilePath("Data/game.sfx"))) {
            MessageBox(NULL, _T("Unable to open sound resources file!"), NULL, MB_OK);
            return -1;
        }

        gMusicMgr.SetVolume(gSettings.GetEntryValue(CET_MSCVOLUME));

        gSettings.Save();
        SetGameLanguage(gSettings.GetLanguage());

        //////////////////////
        // debug: output creatures info into text file
        /*SetGameLanguage(GLNG_ENGLISH);
         FILE *f = fopen("debug.txt", "w+");
         fprintf(f, "name&attack&defence&speed&size&transportation&shots&hp&mindamage&maxdamage&buy:gold&buy:ore&buy:wood&buy:merc&buy:gems&buy:cryst&buy:sulfur&growth&perks&power\n");
         for(uint32 xx=0; xx<CREAT_COUNT; xx++) {
         iStringT perks = _T("");
         for (uint16 nn=0; nn<CPERKS_COUNT; ++nn) {
         if (CREAT_DESC[xx].perks & (1<<nn))
         perks += iFormat(_T("%s:::"), gTextMgr[TRID_CPERK_DOUBLESHOT+nn]);
         }
         fprintf(f, "%s&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%s&%d\n", iStringA(CvtT2A<>(gTextMgr[TRID_CREATURE_PEASANT_F1 + xx*3])).CStr(), CREAT_DESC[xx].attack,
         CREAT_DESC[xx].defence, CREAT_DESC[xx].speed,CREAT_DESC[xx].size,CREAT_DESC[xx].transType,CREAT_DESC[xx].shots,CREAT_DESC[xx].hits,CREAT_DESC[xx].damage_min, CREAT_DESC[xx].damage_max,
         CREAT_DESC[xx].cost.quant[0],CREAT_DESC[xx].cost.quant[1],CREAT_DESC[xx].cost.quant[2],CREAT_DESC[xx].cost.quant[3],CREAT_DESC[xx].cost.quant[4],CREAT_DESC[xx].cost.quant[5],
         CREAT_DESC[xx].cost.quant[6], CREAT_DESC[xx].growth, iStringA(CvtT2A<>(perks.CStr())).CStr(), CREAT_DESC[xx].pidx);
         }
         fclose(f);
         */
        //////////////////////
        log("preloading finished");
        return 0;
}

int pheroes_main()
    {
        log("main started");
        LPCWSTR lpstrCmdLine = L"";

        bool bFirstLaunch = false;

        //determine first launch
        if(gSettings.LaunchCount() == 0)
        {
            bFirstLaunch = true;
        }


#ifdef PLAYCITY
        if(bFirstLaunch)
            SetGameLanguage(GLNG_RUSSIAN);
#endif


	// Initialize and start new game
	if (!gGame.Init(lpstrCmdLine) )
    {
		return -1;
	}

#ifdef OS_WINCE
	iLightHolder	lightHolder;
#endif //OS_WINCE



#ifdef SPLASH
        // remove the splash screen
        prldr.Disappear();
#endif



    log("starting main loop");

	// Core loop
	/*sint32 retCode =  */gApp.Run();


	// Cleanup
	gApp.Destroy();

	log("main loop finished");
	return 0;//retCode;
}

#ifdef WIN32
int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
	)
{
	preload_resources();
	return pheroes_main();
}
#endif
