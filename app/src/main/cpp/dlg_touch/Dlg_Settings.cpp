#include "stdafx.h"
#include "Dlg_Settings.h"
#include "OverlandView.h"
#include "OverlandView.touch.h"
#include "GameMenuDlg.h"
#include "Dlg_Save.h"
#include "Dlg_ScenProps.h"
#include "helpers.h"
#include "Dlg_GameLanguage.h"

//#include "xxc/xxc.security.h"

void ShowLoadingBox();

//////////////////////////////////////////////////////////////////////////
class iAssignKeyDlg : public iBaseGameDlg
{
public:
	iAssignKeyDlg(iViewMgr* pViewMgr, const iStringT& hdr)
	: iBaseGameDlg(pViewMgr), m_hdr(hdr)
	{
	}

private:
	void OnCreateDlg()
	{
		iRect clRect = ClientRect();
		AddChild(new iPHLabel(m_pMgr, iRect(clRect.x,clRect.y,clRect.w,15), m_hdr, AlignCenter, dlgfc_topic));
		AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_CANCEL, DRC_CANCEL));
	}

	bool KeyUp(sint32 key)
	{
		//if (key == KEY_ENTER || key >= KEY_VKA) EndDialog(key);
		return true;
	}

	void DoCompose(const iRect& rect)
	{
		gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), gTextMgr[TRID_HKEY_PRESS_KEY], iRect(rect.x+10, rect.y+25, rect.w-20, rect.h-20));
	}

	iSize ClientSize() const
	{
		return gTextComposer.GetTextBoxSize(gTextMgr[TRID_HKEY_PRESS_KEY], 120, dlgfc_topic) + iSize(20,60);
	}

	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
	{
		if (pView->GetUID() == DRC_CANCEL) EndDialog(0xFF);
	}

	iStringT m_hdr;
};

//////////////////////////////////////////////////////////////////////////
class iHardKeysDlg : public iBaseGameDlg
{
public:
	iHardKeysDlg(iViewMgr* pViewMgr)
	: iBaseGameDlg(pViewMgr)
	{
	}

private:
	void OnCreateDlg()
	{
		iRect clRect = ClientRect();
		AddChild(new iPHLabel(m_pMgr, iRect(clRect.x,clRect.y,clRect.w,15), gTextMgr[TRID_HARDWARE_KEYS_SETTINGS], AlignCenter, dlgfc_hdr));
		sint32 xpos = clRect.x + 10;
		sint32 ypos = clRect.y + 20;
		for (uint16 nn=0; nn<BAT_COUNT; ++nn) {
			AddChild(new iPHLabel(m_pMgr, iRect(xpos,ypos,80,DEF_BTN_HEIGHT), gTextMgr[TRID_HKEY_ACT_HELPMODE + nn], AlignRight, dlgfc_topic));
			AddChild(m_keyBtn[nn] = new iTextButton(m_pMgr, this, iRect(xpos+85, ypos, 70, DEF_BTN_HEIGHT), TRID_HKEY_NAME_ENTER + gSettings.ActionKey((ButtonActionType)nn), 100+nn));
			ypos += DEF_BTN_HEIGHT + 3;
		}

		AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
	}

	void DoCompose(const iRect& rect)
	{}

	iSize ClientSize() const
	{
		return iSize(170, 20 + BAT_COUNT*(DEF_BTN_HEIGHT+3) + 25);
	}

	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
	{
		uint32 uid = pView->GetUID();
		if (uid == DRC_OK) {
			EndDialog(uid);
		} else if (uid >= 100 && uid < (100+BAT_COUNT)) {
			ButtonActionType bat = (ButtonActionType)(uid - 100);
			iAssignKeyDlg akdlg(m_pMgr, gTextMgr[TRID_HKEY_ACT_HELPMODE + bat]);
			iKbdKey res = (iKbdKey)akdlg.DoModal();
			if (res >= KEY_ENTER && res < KEY_COUNT && gSettings.ActionKey(bat) != res) {
				ButtonActionType oldBat = gSettings.GetActionByKey(res);
				if (oldBat != BAT_INVALID) {
					// reset old command
					gSettings.AssignActionKey(oldBat, KEY_INVALID);
					m_keyBtn[oldBat]->SetCaption(TRID_HKEY_NAME_NONE);
				}
				// Assign new key
				gSettings.AssignActionKey(bat, res);
				m_keyBtn[bat]->SetCaption(TRID_HKEY_NAME_ENTER + res);
			}
		}

	}

private:
	iTextButton*	m_keyBtn[BAT_COUNT];
};


//////////////////////////////////////////////////////////////////////////
iSettingsDlg::iSettingsDlg(iViewMgr* pViewMgr, bool showSysMenu)
: iBaseGameDlg(pViewMgr), m_sysMenu(showSysMenu)
{}

void iSettingsDlg::OnCreateDlg()
{
	iRect clRect = ClientRect();
	sint32 xpos = clRect.x + 10;
	sint32 ypos = clRect.y + 18;

	// Label
	AddChild(new iPHLabel(m_pMgr,iRect(clRect.x, clRect.y, clRect.w, 15), gTextMgr[TRID_MENU_GAMESETTINGS], AlignTop, dlgfc_hdr));

	// Track bars

	// Sfx volume
	AddChild(m_sfxVolumeLabel = new iPHLabel(m_pMgr,iRect(xpos, ypos, 120, 12), iStringT(), AlignTop, dlgfc_splain)); ypos += 13;
	AddChild(m_sfxVolumeSlider = new iPHMiniSlider(m_pMgr,this,iRect(xpos, ypos , 120, 11), 101)); ypos += 15;
	m_sfxVolumeSlider->SetMetrics(11,1);
	if (gSettings.ForceNoSound()) {
		m_sfxVolumeSlider->SetEnabled(false);
		m_sfxVolumeSlider->SetCurPos(0);
	} else {
		m_sfxVolumeSlider->SetCurPos(gSettings.GetEntryValue(CET_SFXVOLUME));
	}
	UpdateSliderLabel(101);

	// Display gamma (not needed anymore as of 1.04. Hedin)
	//AddChild(m_dispGammaLabel = new iPHLabel(m_pMgr,iRect(xpos, ypos, 120, 12), iStringT(), AlignTop, dlgfc_splain)); ypos += 13;
	//AddChild(m_dispGammaSlider = new iPHMiniSlider(m_pMgr,this,iRect(xpos, ypos , 120, 12), 102)); ypos += 15;
	//m_dispGammaSlider->SetMetrics(5,1);
	//m_dispGammaSlider->SetCurPos(gSettings.GetEntryValue(CET_DISPGAMMA));
	//UpdateSliderLabel(102);

	// Map scroll speed
	AddChild(m_mapScrollLabel = new iPHLabel(m_pMgr,iRect(xpos, ypos, 120, 12), iStringT(), AlignTop, dlgfc_splain)); ypos += 13;
	AddChild(m_mapScrollSlider = new iPHMiniSlider(m_pMgr,this,iRect(xpos, ypos , 120, 12), 103)); ypos += 15;
	m_mapScrollSlider->SetMetrics(5,1);
	m_mapScrollSlider->SetCurPos(gSettings.GetEntryValue(CET_MAPSCROLLSPEED));
//	AddChild(m_mapScrollSlider = new iPHMiniSlider(m_pMgr,this,iRect(xpos, ypos , 120, 12), 103)); ypos += 15;
	UpdateSliderLabel(103);

	// Hero speed
	AddChild(m_heroSpeedLabel = new iPHLabel(m_pMgr,iRect(xpos, ypos, 120, 12), iStringT(), AlignTop, dlgfc_splain)); ypos += 13;
	AddChild(m_heroSpeedSlider = new iPHMiniSlider(m_pMgr,this,iRect(xpos, ypos , 120, 12), 104)); ypos += 15;
	m_heroSpeedSlider->SetMetrics(5,1);
	m_heroSpeedSlider->SetCurPos(gSettings.GetEntryValue(CET_HEROSPEED));
//	AddChild(m_heroSpeedSlider = new iPHMiniSlider(m_pMgr,this,iRect(xpos, ypos , 120, 12), 104)); ypos += 15;
	UpdateSliderLabel(104);

	// Combat speed
	AddChild(m_combatSpeedLabel = new iPHLabel(m_pMgr,iRect(xpos, ypos, 120, 12), iStringT(), AlignTop, dlgfc_splain)); ypos += 13;
	AddChild(m_combatSpeedSlider = new iPHMiniSlider(m_pMgr,this,iRect(xpos, ypos , 120, 12), 105)); ypos += 15;
	m_combatSpeedSlider->SetMetrics(5,1);
	m_combatSpeedSlider->SetCurPos(gSettings.GetEntryValue(CET_COMBATSPEED));
//	AddChild(m_combatSpeedSlider = new iPHMiniSlider(m_pMgr,this,iRect(xpos, ypos , 120, 12), 105)); ypos += 15;
	UpdateSliderLabel(105);

	// Game Language
	AddChild(new iTextButton(m_pMgr,this,iRect(xpos, ypos, 120, DEF_BTN_HEIGHT),TRID_GAME_LANGUAGE, 102));	ypos += DEF_BTN_HEIGHT;

	// Checkboxes
	xpos = clRect.x + (clRect.w/2) + 10;
	ypos = clRect.y + 29;

	AddChild(m_checkBoxes[0] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 12), gTextMgr[TRID_CFGENTRY_ENDTURN], 201, Visible|Enabled, gSettings.GetEntryValue(CET_ENDTURNCONF)>0)); ypos += 16;
	AddChild(m_checkBoxes[1] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 12), gTextMgr[TRID_CFGENTRY_MAPGRID], 202, Visible|Enabled, gSettings.GetEntryValue(CET_OVRLANDGRID)>0)); ypos += 16;
	AddChild(m_checkBoxes[2] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 12), gTextMgr[TRID_CFGENTRY_COMBATGRID], 203, Visible|Enabled, gSettings.GetEntryValue(CET_COMBATGRID)>0)); ypos += 16;
	AddChild(m_checkBoxes[3] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 12), gTextMgr[TRID_CFGENTRY_QUICKCOMBAT], 204, Visible|Enabled, gSettings.GetEntryValue(CET_QUICKCOMBAT)>0)); ypos += 16;

	AddChild(m_checkBoxes[4] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 12), gTextMgr[TRID_CFGENTRY_PATHMODE], 205, Visible|Enabled, gSettings.GetEntryValue(CET_TRVPTSMODE)>0)); ypos += 16;
// Cotulla 11.04.2010 removed
//	AddChild(m_checkBoxes[5] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 12), gTextMgr[TRID_CFGENTRY_SURVMAPFILT], 206, Visible|Enabled, gSettings.GetEntryValue(CET_SURVMAPFILTR)>0)); ypos += 16;
//	AddChild(m_checkBoxes[6] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 12), gTextMgr[TRID_CFGENTRY_SURVMAPSCALE], 207, Visible|Enabled, gSettings.GetEntryValue(CET_SURVMAPSCALE)>0)); ypos += 16;
	AddChild(m_checkBoxes[7] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 12), gTextMgr[TRID_CFGENTRY_AUTOSAVE], 208, Visible|Enabled, gSettings.GetEntryValue(CET_AUTOSAVE)>0)); ypos += 16;
	AddChild(m_checkBoxes[8] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 12), gTextMgr[TRID_CFGENTRY_LHANDER], 209, Visible|Enabled, gSettings.GetEntryValue(CET_LHANDMAODE)>0)); ypos += 16;

	/*
	AddChild(m_checkBoxes[0] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 17), gTextMgr[TRID_CFGENTRY_ENDTURN], 201, Visible|Enabled, gSettings.GetEntryValue(CET_ENDTURNCONF)>0)); ypos += 21;
	AddChild(m_checkBoxes[1] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 17), gTextMgr[TRID_CFGENTRY_MAPGRID], 202, Visible|Enabled, gSettings.GetEntryValue(CET_OVRLANDGRID)>0)); ypos += 21;
	AddChild(m_checkBoxes[2] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 17), gTextMgr[TRID_CFGENTRY_COMBATGRID], 203, Visible|Enabled, gSettings.GetEntryValue(CET_COMBATGRID)>0)); ypos += 21;
	AddChild(m_checkBoxes[3] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 17), gTextMgr[TRID_CFGENTRY_QUICKCOMBAT], 204, Visible|Enabled, gSettings.GetEntryValue(CET_QUICKCOMBAT)>0)); ypos += 21;

	AddChild(m_checkBoxes[5] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 17), gTextMgr[TRID_CFGENTRY_SURVMAPFILT], 206, Visible|Enabled, gSettings.GetEntryValue(CET_SURVMAPFILTR)>0)); ypos += 21;
	AddChild(m_checkBoxes[6] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 17), gTextMgr[TRID_CFGENTRY_SURVMAPSCALE], 207, Visible|Enabled, gSettings.GetEntryValue(CET_SURVMAPSCALE)>0)); ypos += 21;
	AddChild(m_checkBoxes[7] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 17), gTextMgr[TRID_CFGENTRY_AUTOSAVE], 208, Visible|Enabled, gSettings.GetEntryValue(CET_AUTOSAVE)>0)); ypos += 21;
#ifndef OS_IPHONE
	AddChild(m_checkBoxes[8] = new iCheckBox(m_pMgr,this,iRect(xpos, ypos, 120, 17), gTextMgr[TRID_CFGENTRY_LHANDER], 209, Visible|Enabled, gSettings.GetEntryValue(CET_LHANDMAODE)>0)); ypos += 21;
#endif
	*/
	// Hardware keys and Ok button
	if(m_sysMenu) {
		AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-57) - 40,clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
		AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-57) + 5,clRect.y2()-DEF_BTN_HEIGHT,70,DEF_BTN_HEIGHT),TRID_HARDWARE_KEYS, 301));
		AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-57) + 80,clRect.y2()-DEF_BTN_HEIGHT,90,DEF_BTN_HEIGHT),TRID_SYSTEM_MENU, 302));
	} else {
		AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-57),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
		AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-57) + 45,clRect.y2()-DEF_BTN_HEIGHT,70,DEF_BTN_HEIGHT),TRID_HARDWARE_KEYS, 301));
	}
}

void iSettingsDlg::DoCompose(const iRect& rect)
{

}

iSize iSettingsDlg::ClientSize() const
{
	return iSize(280,175 + DEF_BTN_HEIGHT);
}

void iSettingsDlg::UpdateSliderLabel(uint32 sliderId)
{
#ifdef OS_IPHONE
	return;
#endif

	if (sliderId == 101) {
		// Sfx Volume
		if (!m_sfxVolumeSlider->CurPos()) m_sfxVolumeLabel->SetText(iStringT(gTextMgr[TRID_SFX_VOLUME]) + _T(": ") + gTextMgr[TRID_OFF]);
		else m_sfxVolumeLabel->SetText(iFormat(_T("%s: #FFF0%d"), gTextMgr[TRID_CFGENTRY_SFXVOL], m_sfxVolumeSlider->CurPos()));
	} /*else if (sliderId == 102) {
		// Display gamma
		//m_dispGammaLabel->SetText(iFormat(_T("%s: #FFF0%d"), gTextMgr[TRID_CFGENTRY_DISPGAMMA], m_dispGammaSlider->CurPos()));
		iDlg_

	} */else if (sliderId == 103) {
		// Map scroll speed
		m_mapScrollLabel->SetText(iStringT(gTextMgr[TRID_CFGENTRY_MAPSCROLLSPEED]) + _T(": #FFF0") + gTextMgr[TRID_CFG_SPEED_VSLOW + m_mapScrollSlider->CurPos()]);
	} else if (sliderId == 104) {
		// Hero speed
		m_heroSpeedLabel->SetText(iStringT(gTextMgr[TRID_CFGENTRY_HEROSPEED]) + _T(": #FFF0") + gTextMgr[TRID_CFG_SPEED_VSLOW + m_heroSpeedSlider->CurPos()]);
	} else if (sliderId == 105) {
		// Combat speed
		m_combatSpeedLabel->SetText(iStringT(gTextMgr[TRID_CFGENTRY_COMBATSPEED]) + _T(": #FFF0") + gTextMgr[TRID_CFG_SPEED_VSLOW + m_combatSpeedSlider->CurPos()]);
	}
}

void iSettingsDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == DRC_OK) {
#ifndef OS_IPHONE
		// Process if sound was been turned off
		if (!m_sfxVolumeSlider->CurPos() && gApp.SndPlayer().Inited()) {
			gGame.SoundMap().ResetEnvSounds();
			gApp.SndPlayer().SetVolume(0);
			gApp.SndPlayer().Close();
		}

		// Process if "Lefthander mode was been changed"
		bool bLeftHander = m_checkBoxes[8]->IsChecked();
		if (bLeftHander != (gSettings.GetEntryValue(CET_LHANDMAODE)>0) ) {
			gApp.SetOrientation(gApp.IsLandscape(), bLeftHander);
			gSettings.SetEntryValue(CET_LHANDMAODE, bLeftHander);
		}

		// Check if gamma was changed and we need to reload gfx. Disabled. Hedin
		/*uint32 newGamma = m_dispGammaSlider->CurPos();
		if ( newGamma != gSettings.GetEntryValue( CET_DISPGAMMA ) ) {
			ShowLoadingBox();
			gGfxMgr.SetGamma( newGamma );
			gGfxMgr.Reload();
			gSettings.SetEntryValue( CET_DISPGAMMA, newGamma );
		}
		*/
#endif
		EndDialog(uid);
	} else if (uid == 301) {
		iHardKeysDlg hkdlg(m_pMgr);
		hkdlg.DoModal();
	} else if (uid == 302) {
		iGameMenuDlg dlg(m_pMgr, true);
		EndDialog(dlg.DoModal());
	}else if (uid == 101) {
		// Sfx volume
		UpdateSliderLabel(101);
		gSettings.SetEntryValue(CET_SFXVOLUME, param);
		if (param && !gApp.SndPlayer().Inited()) {
			gSfxMgr.Init(fileaccessor::RelativeFilePath("game.sfx"));
			gApp.SndPlayer().Init();
			gApp.SndPlayer().SetVolume(param*25);
			if (gGame.Map().CurHero()) gGame.SoundMap().SetupEnvSounds(gGame.Map().CurHero()->Pos());
		} else {
			gApp.SndPlayer().SetVolume(param*25);
		}
	} else if (uid == 102) {
		//Pop the game language dialog
		iDlg_GameLanguage dlg(m_pMgr);
		dlg.DoModal();

		EndDialog(0);
		// Display gamma
		//UpdateSliderLabel(102);
		//gSettings.SetEntryValue(CET_DISPGAMMA, param);
		//gApp.Display().SetGamma(1.0f + 0.05f * param);
	} else if (uid == 103) {
		// Map scroll speed
		UpdateSliderLabel(103);
		gSettings.SetEntryValue(CET_MAPSCROLLSPEED, param);
	} else if (uid == 104) {
		// Hero speed
		UpdateSliderLabel(104);
		gSettings.SetEntryValue(CET_HEROSPEED, param);
	} else if (uid == 105) {
		// Combat speed
		UpdateSliderLabel(105);
		gSettings.SetEntryValue(CET_COMBATSPEED, param);
	} else if (uid == 201) {
		// End turn confirm
		gSettings.SetEntryValue(CET_ENDTURNCONF, param);
	} else if (uid == 202) {
		// Map grid
		gSettings.SetEntryValue(CET_OVRLANDGRID, param);
		if(gGame.MainView())
			gGame.MainView()->Composer().Invalidate();
	} else if (uid == 203) {
		// Combat grid
		gSettings.SetEntryValue(CET_COMBATGRID, param);
	} else if (uid == 204) {
		// Quick combat
		gSettings.SetEntryValue(CET_QUICKCOMBAT, param);
	} else if (uid == 205) {
		// Path mode
		gSettings.SetEntryValue(CET_TRVPTSMODE, param);
	} else if (uid == 206) {
		// Survey map filtration
		gSettings.SetEntryValue(CET_SURVMAPFILTR, param);
	} else if (uid == 207) {
		// Survey map scale
		gSettings.SetEntryValue(CET_SURVMAPSCALE, param);
	} else if (uid == 208) {
		gSettings.SetEntryValue(CET_AUTOSAVE, param);
	}
}


