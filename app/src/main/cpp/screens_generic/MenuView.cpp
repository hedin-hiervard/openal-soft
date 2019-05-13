#include "stdafx.h"
#include "Credits.h"
#include "MenuView.h"
#include "Dlg_ScenList.h"
#include "Dlg_Save.h"
#include "Dlg_ScenProps.h"
#include "Dlg_HallOfFame.h"
#include "Dlg_Settings.h"
#include "RMG.h"
#include "Dlg_RandomMap.h"

#include "screens_ipad/MainMenuView.h"
#include "screens_iphone/MainMenuView.h"
#include "FileAccessor/FileAccessor.h"

//#include "pthread.h"

//#include "xxc/xxc.security.h"

using namespace fileaccessor;

/*
 *	Main dialog
 */
#ifdef OS_IPHONE
	void OpenItunesLink();
#endif

const iDib::pixel menuBtnText[15] = {
	RGB16(210,190,115), RGB16(214,192,110), RGB16(216,192,102), RGB16(219,193,96), RGB16(221,193,85),
	RGB16(224,194,76),  RGB16(228,196,67),  RGB16(231,195,59), RGB16(233,196,49), RGB16(236,196,40),
	RGB16(239,198,31), RGB16(242,198,23), RGB16(224,198,16), RGB16(247,199,0), RGB16(248,200,0)
};

class iMainMenuDlg : public iDialog, public  IViewCmdHandler
{
public:
	class iMainMenuBtn : public iButton
	{
	public:
		iMainMenuBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, TextResId textKey, uint32 uid, uint32 state = Visible|Enabled)\
		: iButton(pViewMgr, pCmdHandler, rect, uid, state), m_TextKey(textKey)
		{}

		void OnBtnDown() const
		{
			//gSfxMgr.PlaySound(CSND_BUTTON);
		}

		void OnCompose()
		{
			gApp.Surface().Darken50Rect(GetScrRect());
			// Compose outer frame
			iRect rect = GetScrRect();
			rect.InflateRect(1);
			uint16 cColor_Grey = RGB16(32,32,32);
			gApp.Surface().HLine(iPoint(rect.x+2, rect.y), rect.x+rect.w-3, cColor_Grey);
			gApp.Surface().HLine(iPoint(rect.x+2, rect.y+rect.h-1), rect.x+rect.w-3, cColor_Grey);
			gApp.Surface().VLine(iPoint(rect.x,rect.y+2), rect.y+rect.h-2, cColor_Grey);
			gApp.Surface().VLine(iPoint(rect.x+rect.w-1,rect.y+2), rect.y+rect.h-2, cColor_Grey);

			iDibFont::ComposeProps props = iDibFont::ComposeProps(iGradient(menuBtnText,15), cColor_Black, iDibFont::DecBorder);
			uint32 state = GetButtonState();
			if ( state & iButton::Disabled ) {
				props = iDibFont::ComposeProps(RGB16(128,100,0), cColor_Black, iDibFont::DecBorder);
			} else if ( state & iButton::Pressed ) {
				props = iDibFont::ComposeProps(RGB16(255,255,255), cColor_Black, iDibFont::DecBorder);
				gApp.Surface().Darken50Rect(GetScrRect());
			}
			iTextComposer::FontConfig fc(FS_LARGE, props );
			gTextComposer.TextOut(fc, gApp.Surface(), iPoint(), gTextMgr[m_TextKey], GetScrRect(), AlignCenter, (state&iButton::Pressed?iPoint(1,1):iPoint(0,0)) );
		}

	private:
		TextResId	m_TextKey;
	};

public:
	iMainMenuDlg(iViewMgr* pViewMgr, iMenuView::MenuLocation loc) : iDialog(pViewMgr), m_loc(loc) {}

	enum {
		BTN_DIST = 5
	};

private:
	iMenuView::MenuLocation m_loc;

	void OnCreateDlg()
	{
		iRect rc = GetDlgMetrics(); rc.h = DEF_BTN_HEIGHT+2;


		if(m_loc == iMenuView::ML_MAIN) {
#ifdef SINGLEPLAYER
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_MENU_SINGLEPLAYER, 100, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
#endif
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_MENU_MULTIPLAYER, 109, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;

#ifndef OS_IPHONE
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_MENU_GAMESETTINGS, 107, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
#endif
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_MENU_CREDITS, 103, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;

#ifndef OS_IPHONE
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_MENU_EXITGAME, 104, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
#else
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_HELP, 105, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
#endif
#ifdef IPHONE_LITE
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_REG_FULLVER, 115, Visible|Enabled));
#endif
		} else if(m_loc == iMenuView::ML_SINGLE) {
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_MENU_NEWSCENARIO, 100, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
#ifdef RMG
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_MENU_NEWRANDOM, 112, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
#endif
#ifndef IPHONE_LITE
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_MENU_LOADGAME, 101, (Visible|Enabled))); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
#endif
#ifdef SINGLEPLAYER
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_MENU_TUTORIAL, 111, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
#endif
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_MENU_HIGHSCORE, 102, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_HKEY_NAME_BACK, 108, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
		} else if(m_loc == iMenuView::ML_MULTI) {
#ifdef MULTIPLAYER
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_MENU_ONLINE, 106, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
#endif
#ifdef SINGLEPLAYER
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_MENU_MULTIPLAYER, 110, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
#endif
			AddChild(new iMainMenuBtn(m_pMgr, this, rc, TRID_HKEY_NAME_BACK, 108, Visible|Enabled)); rc.y+=DEF_BTN_HEIGHT+BTN_DIST;
		}
	}

	void OnPlace(iRect& rect)
	{
		rect.y += 40;
	}

	void OnCompose()
	{
	}

	iSize GetDlgMetrics() const
	{
#ifndef MULTIPLAYER
		return iSize(150,5*(DEF_BTN_HEIGHT+2) + 12);
#else
		return iSize(150,5*(DEF_BTN_HEIGHT+2) + 12 + 35);
#endif
	}

	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param) { EndDialog(pView->GetUID()); }
};

/*
 *
 */

iMenuView::iMenuView()
: iChildGameView(false, CV_UNDEFINED), m_bStarted(false), m_loc(ML_MAIN), m_dlg(NULL)
{
	m_crComposer.Init();
}

// TODO:: move const somewhere away OR otherwise automate!
//#define BLOOMBITS_HASH 0x7a89324b
// Hash updated to reflect current 'dumpbits.dat' file version!
//#define BLOOMBITS_HASH 0xe31751e0
#define BLOOMBITS_HASH 0x955884d1

void iMenuView::Start()
{
	if(m_bStarted) return;

#if defined(OS_IPHONE) || defined(OS_ANDROID)
	m_startTime = GetTickCount();
#endif

	/*if (gEnterNewKey) {
		if (registered) {
			iTextDlg tdlg(&gApp.ViewMgr(), gTextMgr[TRID_REG_SUCCEDED], gTextMgr[TRID_REG_DONE], PID_GREEN);
			tdlg.DoModal();
		} else {
			gSettings.SetActivationKey(iStringT());
			iTextDlg tdlg(&gApp.ViewMgr(), gTextMgr[TRID_REG_FAILED], gTextMgr[TRID_REG_INVALIDKEY], PID_RED);
			tdlg.DoModal();
		}
		gEnterNewKey = false;
	}
	*/
	m_bStarted = true;
	while (1) {
         sint32 res;

        iMainMenuDlg mdlg(&gApp.ViewMgr(), m_loc);
		mdlg.SetPos(iPoint(85, 100));
		res = mdlg.DoModal();

        if (res == 208)
        {
            // start new game
            return;
        }

// end of HACK

	// TRICK!
		if (res == 100) {
			while(1) {
				if(m_loc == ML_SINGLE) {
					// Start new game
					iScenListDlg sldlg(&gApp.ViewMgr(), SCL_SINGLE | SCL_MULTI);
					res = sldlg.DoModal();
					if (res == DRC_OK) {
						iMapInfo scenProps = sldlg.SelScen();
						iScenPropsDlg spdlg(&gApp.ViewMgr(), scenProps/*, false*/);
						if (spdlg.DoModal() == DRC_OK)	{
							scenProps.ReorderPlayers();
							gGame.StartNewGame(scenProps, true, true);
							return;
						}
					} else {
						break;
					}
				}
				else {
					m_loc = ML_SINGLE;
					break;
				}
			}
		} else if (res == 101) {
			// Load saved game
			iSaveDlg saveDlg(&gApp.ViewMgr(), false);
			res = saveDlg.DoModal();
			if (res == DRC_OK) {
				iMapInfo scenProps = saveDlg.SelScenario();
				//iScenPropsDlg spdlg(&gApp.ViewMgr(), scenProps, true);
				//if (spdlg.DoModal() == DRC_OK)	{
					scenProps.ReorderPlayers();
					gGame.StartNewGame(scenProps, false, false);
					break;
				//}
			} else {
				continue;
			}
		} else if (res == 102) {
// #ifndef OS_IPHONE
// 			iDlg_HallOfFame dlg(&gApp.ViewMgr(), gRootPath + _T("PalmHeroes.hsc"));
// #else
// 			iDlg_HallOfFame dlg(&gApp.ViewMgr(), gSavePath + _T("PalmHeroes.hsc"));
// #endif
// 			dlg.DoModal();
		} else if (res == 103) {
			StartCredits();
			break;
		} else if (res == 104) {
			// Quit to WM200x
			gGame.Quit();
			break;
		}
#if defined(OS_IPHONE) || defined(OS_ANDROID)
		else if (res == 105) {
			//StopCredits();
			gGame.ShowViewDelayed(iChildGameView::CV_SCROLL);
			break;
		}
//		} else if(res == 115) {
//			OpenItunesLink();
//		}
#endif
#ifdef MULTIPLAYER
		else if (res == 106) {
			if(gMPMgr.ConnectToServer())
				break;
			else {
				iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_FAILURE], gTextMgr[TRID_FAILED_CONNECTING], PID_NEUTRAL);
				dlg.DoModal();
			}
		}
#endif
		else if(res == 107) {
#ifndef OS_IPHONE
			iSettingsDlg sdlg(&gApp.ViewMgr(), false);
			sdlg.DoModal();
#endif
		} else if(res == 108) {
			m_loc = ML_MAIN;
		} else if(res == 109) {
			m_loc = ML_MULTI;
		} else if(res == 110) {
			// Hot-Seat
			iScenListDlg sldlg(&gApp.ViewMgr(), SCL_MULTI);
			res = sldlg.DoModal();
			if (res == DRC_OK) {
				iMapInfo scenProps = sldlg.SelScen();
				iScenPropsDlg spdlg(&gApp.ViewMgr(), scenProps/*, false*/);
				if (spdlg.DoModal() == DRC_OK)	{
					scenProps.ReorderPlayers();
					gGame.StartNewGame(scenProps, true, true);
					return;
				}
			} else {
				continue;
			}
		} else if(res == 111) {
			// Tutorial
			iScenListDlg sldlg(&gApp.ViewMgr(), SCL_TUTORIAL);
			res = sldlg.DoModal();
			if (res == DRC_OK) {
				iMapInfo scenProps = sldlg.SelScen();
				iScenPropsDlg spdlg(&gApp.ViewMgr(), scenProps/*, false*/);
				if (spdlg.DoModal() == DRC_OK)	{
					scenProps.ReorderPlayers();
					gGame.StartNewGame(scenProps, true, true);
					return;
				}
			} else {
				continue;
			}
		} else if(res == 112) {
			// New random
			iRandomMapDlg rdlg(&gApp.ViewMgr());
			res = rdlg.DoModal();
			if(res == DRC_OK) {
				iRandomMapGenerator rmg;
				rmg.Generate(rdlg.SelParam());
			}
		}

	}
	m_bStarted = false;
}

void OpenTutorialLink();

//////////////////////////////////////////////////////////////////////////
// Cotulla: new code for new menu, old code is saved
void iMenuView::NewStart(bool fInGame)
{
    if(m_bStarted) return;

#if defined(OS_IPHONE) || defined(OS_ANDROID)
    m_startTime = GetTickCount();
#endif

    /*if (gEnterNewKey) {
        bool registered = IS_REGISTERED();
        if (registered) {
            iTextDlg tdlg(&gApp.ViewMgr(), gTextMgr[TRID_REG_SUCCEDED], gTextMgr[TRID_REG_DONE], PID_GREEN);
            tdlg.DoModal();
        } else {
            gSettings.SetActivationKey(iStringT());
            iTextDlg tdlg(&gApp.ViewMgr(), gTextMgr[TRID_REG_FAILED], gTextMgr[TRID_REG_INVALIDKEY], PID_RED);
            tdlg.DoModal();
        }
        gEnterNewKey = false;
    }*/
    m_bStarted = true;

    IMainMenuView::Type tp;

	if(fInGame)
#ifdef ROYAL_BOUNTY
		tp = iMainMenuView::TypeSingleMapInGame;
#else
        tp = IMainMenuView::TypeInGame;
#endif
	else
#ifdef ROYAL_BOUNTY
		tp = iMainMenuView::TypeSingleMap;
#else
        tp =IMainMenuView::TypeMain;
#endif

    while (1)
    {
        sint32 res;

        if(gUseIpadUI)
        {
            m_dlg = new UI_IPAD::iNewMenuDlg(&gApp.ViewMgr());
        } else
        {
            m_dlg = new UI_IPHONE::iNewMenuDlg(&gApp.ViewMgr());
        }

        m_dlg->SetType(tp, fInGame);
        res = m_dlg->DoModal();

        delete m_dlg;
        m_dlg = NULL;

        if (res == IMainMenuView::ResExit)
        {
            gGame.Quit();
            break;
        }
        else if (res == IMainMenuView::ResContinue || res == IMainMenuView::ResContinueNoHide)
        {
			m_bStarted = false;
            return;
        }
        else if (res == IMainMenuView::ResMain)
        {
            tp = IMainMenuView::TypeMain;
        }
        else if (res == IMainMenuView::ResSingle)
        {
            tp = fInGame ? IMainMenuView::TypeInGame : IMainMenuView::TypeSingle;
        }
        else if (res == IMainMenuView::ResMulti)
        {
            tp = IMainMenuView::TypeMulti;
		} else if(res == IMainMenuView::ResTutorial)
		{
//#ifdef OS_IPHONE
			//OpenTutorialLink();

            // now we need to start new game for a Break map
            gTutorial.ResetDontShow();
            gGame.StartTutorialMap();
            break;
//#endif
		} else if(res == IMainMenuView::ResSingleMap) {
			gGame.StartSingleMap();
			break;
		}
    }
    m_bStarted = false;
}


void iMenuView::StartCredits()
{
	m_crComposer.StartCredits();
}

void iMenuView::StopCredits()
{
	m_crComposer.StopCredits();
	//NewStart();
}

bool iMenuView::Process(fix32 t)
{
	static fix32 timer = fix32::zero;

	timer += t;
	if (m_crComposer.IsCreaditsStarted() && m_crComposer.IsCreaditsEnd()) StopCredits();
	//if(timer >= fix32(0.005f)) {
		Invalidate();
		timer = fix32::zero;
	//}
    m_crComposer.Process(t);
	return true;
}

void iMenuView::OnCompose()
{
	m_crComposer.Compose(gApp.Surface(),iPoint(0,0));

//	gGfxMgr.Blit(PDGG(LOGO), gApp.Surface(), iPoint(44,2));
//	gGfxMgr.Blit(PDGG(LOGO2), gApp.Surface(), iPoint(174,3));

#if 0
	gTextComposer.TextOut(
			iTextComposer::FontConfig(iTextComposer::FS_MEDIUM, iDibFont::ComposeProps(cColor_White, cColor_Black, iDibFont::DecBorder ) ),
			gApp.Surface(), iPoint(), _T("›ÍÒÍÎ˛ÁË‚Ì‡ˇ ‚ÂÒËˇ ‰Îˇ ˜ËÚ‡ÚÂÎÂÈ ÊÛÌ‡Î‡ Mobi (www.mobi.ru)"),
			iRect(0,m_Rect.y2()-15,m_Rect.w, 15), AlignCenter);
#endif

#if 0
	gTextComposer.TextOut(
						  iTextComposer::FontConfig(iTextComposer::FS_LARGE, iDibFont::ComposeProps(cColor_Red, cColor_Black, iDibFont::DecBorder ) ),
						  gApp.Surface(), iPoint(), _T("Please support the development by donating at: "),
						  iRect(0,m_Rect.y2()-40,m_Rect.w, 15), AlignCenter);
	gTextComposer.TextOut(
						  iTextComposer::FontConfig(iTextComposer::FS_LARGE, iDibFont::ComposeProps(cColor_Red, cColor_Black, iDibFont::DecBorder ) ),
						  gApp.Surface(), iPoint(), _T("http://iphone.palmheroes.com"),
						  iRect(0,m_Rect.y2()-20,m_Rect.w, 15), AlignCenter);
#endif

/*
#if MULTIPLAYER
	gTextComposer.TextOut(
						  iTextComposer::FontConfig(iTextComposer::FS_LARGE, iDibFont::ComposeProps(cColor_Red, cColor_Black, iDibFont::DecBorder ) ),
						  gApp.Surface(), iPoint(), _T("MULTIPLAYER BETA"),
						  iRect(0,m_Rect.y2()-10,m_Rect.w, 10), AlignCenter);
#endif
*/
#ifdef PLAYCITY
    iStringT text = _T("Copyright 2013 MyPlayCity, Inc., iosoftware LLC");
    iSize sz = gTextComposer.GetTextSize(text, dlgfc_plain);
    iRect copyrightrc = AlignRect(sz, gApp.Surface().GetSize(), AlignBottomRight);
    gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), text, copyrightrc);
#endif
}

void iMenuView::AfterCompose()
{
/*#ifdef OS_IPHONE
	// black descreen on appear
	uint32 len = 1000;
	uint32 ticks;
	if((ticks = GetTickCount()) - m_startTime < len) {
		gApp.Surface().FillRect(gApp.Surface().GetSize(), cColor_Black, 255-255*(ticks - m_startTime)/len);
	}
#endif
*/
}
#ifdef PC_VERSION
void iMenuView::UpdateSize()
{
    if(m_dlg) {
     //   m_dlg->SetRect(gApp.ViewMgr().Metrics());
       // m_dlg->UpdateSize();
        m_dlg->Center();
    }
}
#endif
void iMenuView::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	StopCredits();
}

void iMenuView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	//uint32 uid = pView->GetUID();
}


void iMenuView::OnActivate(bool bActivate)
{
		gMusicMgr.Play(RelativeFilePath("Music/Relent.mp3"));
}
