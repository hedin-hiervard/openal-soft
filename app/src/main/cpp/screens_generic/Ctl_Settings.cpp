//
// Ctl_Settings.cpp
// 

#include "stdafx.h"
//#include "GameMenuView.h"
#include "Credits.h"
#include "MenuView.h"
#include "TransitionView.h"
#include "helpers.h"
#include "Ctl_Settings.h"
#include "Dlg_GameLanguage.h"
#include "OverlandView.h"
#include "OverlandView.touch.h"
#include "GfxHlp.h"


extern sint16 lang_icons[GLNG_COUNT];


void iSettingsView::OnActivate() {
	m_sfxVolSlider->SetCurPos(gSettings.GetEntryValue(CET_SFXVOLUME));
	m_musicVolSlider->SetCurPos(gSettings.GetEntryValue(CET_MSCVOLUME));
#ifndef ROYAL_BOUNTY
	m_pCBEndTurn->SetChecked(gSettings.GetEntryValue(CET_ENDTURNCONF)>0);
#endif
	m_pCBAdvCombat->SetChecked(gSettings.GetEntryValue(CET_ADVANCED_COMBAT)>0);
	m_pCBQuickCombat->SetChecked(gSettings.GetEntryValue(CET_QUICKCOMBAT)>0);
#ifdef PC_VERSION
	m_pCBCombatGrid->SetChecked(gSettings.GetEntryValue(CET_COMBATGRID)>0);
#endif
	
#ifndef PC_VERSION
	m_pCBOverlandGrid->SetChecked(gSettings.GetEntryValue(CET_OVRLANDGRID)>0);
    if(gUseIpadUI)
    {
        m_pCBStickyPath->SetChecked(gSettings.GetEntryValue(CET_STICKY_PATH)>0);
        m_pCBOneHanded->SetChecked(gSettings.GetEntryValue(CET_ONE_HANDED_MODE)>0);
    }
#endif
	m_pLangBtn->SetSprite(lang_icons[gSettings.GetLanguage()]);
}

iSettingsView::iSettingsView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) 
    : iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pCmdHandler(pCmdHandler)
{       
    iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);

    sint32 xposC = clRect.x
#ifndef OS_IPHONE
    + 180;
#else
    + 160;
#endif
    sint32 xposL = xposC - 10;
    sint32 xposR = xposC
#ifndef OS_IPHONE
    + 10;
#else
    + 3;
#endif
    sint32 ypos = clRect.y
#ifndef OS_IPHONE
    + 18;
#else
    + 10;
#endif

    iTextComposer::FontConfig fc(newmenufc_lrg_text.fontSize-1, iDibFont::ComposeProps(RGB16(0,0,0)));
    sint32 offset;
    if(!gUseIpadUI)
        offset = 40;
    else
        offset = 55;
	
    sint32 textoff;
    if(!gUseIpadUI)
        textoff = 0;
    else
        textoff = 10;

        
    // 1. music volume 
    AddChild(new iPHLabel(m_pMgr,iRect(xposR, ypos + 10, 120, 12), gTextMgr[TRID_CFGENTRY_SFXVOL], AlignLeft, fc, TRID_CFGENTRY_SFXVOL));
#ifndef OS_IPHONE
    AddChild(m_sfxVolSlider = new iNMVolumeBar(m_pMgr,this,iRect(xposL - /*135*/160, ypos - 10, /*150*/180, 32), 103)); 
#else
    AddChild(m_sfxVolSlider = new iNMVolumeBar(m_pMgr,this,iRect(xposL - 160, ypos - 10, 180, 32), 103)); 
#endif
    ypos += 60;
    

    // 2. sound volume
    AddChild(new iPHLabel(m_pMgr,iRect(xposR, ypos + 10, 120, 12), gTextMgr[TRID_CFGENTRY_MUSICVOL], AlignLeft, fc, TRID_CFGENTRY_MUSICVOL));
#ifndef OS_IPHONE
    AddChild(m_musicVolSlider = new iNMVolumeBar(m_pMgr,this,iRect(xposL - /*135*/160, ypos - 10, /*150*/180, 32), 104)); 
#else
    AddChild(m_musicVolSlider = new iNMVolumeBar(m_pMgr,this,iRect(xposL - 160, ypos - 10, 180, 32), 104)); 
#endif
    ypos += 60;    

#ifndef ROYAL_BOUNTY
    // 3. show hero reminder    
    AddChild(new iPHLabel(m_pMgr,iRect(xposR, ypos + textoff, 120, 12), gTextMgr[TRID_CFGENTRY_ENDTURN], AlignLeft, fc, TRID_CFGENTRY_ENDTURN));
    AddChild(m_pCBEndTurn = new iNMCheckBox(m_pMgr,this,iRect(xposL - 32, ypos - 13, offset, offset), iStringT(), 201, Visible|Enabled, gSettings.GetEntryValue(CET_ENDTURNCONF)>0)); 
    ypos += offset;
#endif	
	
    // 4. advanced combat
    AddChild(new iPHLabel(m_pMgr,iRect(xposR, ypos + textoff, 120, 12), gTextMgr[TRID_CFGENTRY_ADVCOMBAT], AlignLeft, fc, TRID_CFGENTRY_ADVCOMBAT));
    AddChild(m_pCBAdvCombat = new iNMCheckBox(m_pMgr,this,iRect(xposL - 32, ypos - 13, offset, offset), iStringT(), 205, Visible|Enabled, gSettings.GetEntryValue(CET_ADVANCED_COMBAT)>0)); 
    ypos += offset;

    // 5. quick combat    
    AddChild(new iPHLabel(m_pMgr,iRect(xposR, ypos + textoff, 120, 12), gTextMgr[TRID_CFGENTRY_QUICKCOMBAT], AlignLeft, fc, TRID_CFGENTRY_QUICKCOMBAT));
    AddChild(m_pCBQuickCombat = new iNMCheckBox(m_pMgr,this,iRect(xposL - 32, ypos - 13, offset, offset), iStringT(), 204, Visible|Enabled, gSettings.GetEntryValue(CET_QUICKCOMBAT)>0)); 
    ypos += offset;

#ifdef PC_VERSION
	// 6. combat grid
	AddChild(new iPHLabel(m_pMgr,iRect(xposR, ypos + textoff, 120, 12), gTextMgr[TRID_CFGENTRY_COMBATGRID], AlignLeft, fc, TRID_CFGENTRY_COMBATGRID));
	AddChild(m_pCBCombatGrid= new iNMCheckBox(m_pMgr,this,iRect(xposL - 32, ypos - 13, offset, offset), iStringT(), 209, Visible|Enabled, gSettings.GetEntryValue(CET_COMBATGRID)>0)); 
	ypos += offset;
#endif

#ifndef PC_VERSION
	// 7. overland grid
	AddChild(new iPHLabel(m_pMgr,iRect(xposR, ypos + textoff, 120, 12), gTextMgr[TRID_CFGENTRY_MAPGRID], AlignLeft, fc, TRID_CFGENTRY_MAPGRID));
	AddChild(m_pCBOverlandGrid= new iNMCheckBox(m_pMgr,this,iRect(xposL - 32, ypos - 13, offset, offset), iStringT(), 212, Visible|Enabled, gSettings.GetEntryValue(CET_OVRLANDGRID)>0)); 
    ypos += offset;
    
    if(gUseIpadUI)
    {
        // 8. sticky path
        AddChild(new iPHLabel(m_pMgr,iRect(xposR, ypos + textoff, 120, 12), gTextMgr[TRID_CFGENTRY_PATH_STICKS], AlignLeft, fc, TRID_CFGENTRY_PATH_STICKS));
        AddChild(m_pCBStickyPath = new iNMCheckBox(m_pMgr,this,iRect(xposL - 32, ypos - 13, offset, offset), iStringT(), 210, Visible|Enabled, gSettings.GetEntryValue(CET_STICKY_PATH)>0));
        ypos += offset;
        
        // 9. one-handed mode
        AddChild(new iPHLabel(m_pMgr,iRect(xposR, ypos + textoff, 120, 12), gTextMgr[TRID_CFGENTRY_ONE_HANDED], AlignLeft, fc, TRID_CFGENTRY_ONE_HANDED));
        AddChild(m_pCBOneHanded = new iNMCheckBox(m_pMgr,this,iRect(xposL - 32, ypos - 13, offset, offset), iStringT(), 211, Visible|Enabled, gSettings.GetEntryValue(CET_ONE_HANDED_MODE)>0));
        ypos += offset;
    }
#endif	
	
    AddChild(m_pLangBtn = new iIphoneBtn(&gApp.ViewMgr(), this, AlignRect(iIphoneBtn::sm_defSize, m_Rect, AlignLeft), 207, Visible | Enabled, lang_icons[gSettings.GetLanguage()]));
    ypos += 48; 

	OnActivate();
};


void iSettingsView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
    if (uid == 103) {
        // Sfx volume
        gSettings.SetEntryValue(CET_SFXVOLUME, param);
        gApp.SndPlayer().SetVolume(param*25);
        gSettings.Save();
    } 
    else if (uid == 104) {
        // Music volume
        gSettings.SetEntryValue(CET_MSCVOLUME, param);
        gMusicMgr.SetVolume(param);
        gSettings.Save();
    } else if (uid == 105) {
        // Combat speed
        gSettings.SetEntryValue(CET_COMBATSPEED, param);
        gSettings.Save();
    } else if (uid == 201) {
        // End turn confirm
        gSettings.SetEntryValue(CET_ENDTURNCONF, param);
        gSettings.Save();
    } else if (uid == 202) {
        // Map grid
        gSettings.SetEntryValue(CET_OVRLANDGRID, param);
        gGame.MainView()->Composer().Invalidate();
        gSettings.Save();
    } else if (uid == 203) {
        // Process if "Lefthander mode has been changed"
        bool bLeftHander = param;
        if (bLeftHander != (gSettings.GetEntryValue(CET_LHANDMAODE)>0) ) {
            gApp.SetOrientation(gApp.IsLandscape(), bLeftHander);
            gSettings.SetEntryValue(CET_LHANDMAODE, bLeftHander);
        }
        gSettings.Save();
    } else if (uid == 204) {
        // Quick combat
        gSettings.SetEntryValue(CET_QUICKCOMBAT, param);
        gSettings.Save();
    } else if (uid == 205) {
        // autozoom
        gSettings.SetEntryValue(CET_ADVANCED_COMBAT, param);
        gSettings.Save();
    }  else if (uid == 206) {
        // autocroll while autozoomed
        gSettings.SetEntryValue(CET_SCROLL_AUTOZOOMED, param);
        gSettings.Save();
	} else if(uid == 207) {
		// language
		iDlg_GameLanguage dlg(&gApp.ViewMgr());
		dlg.DoModal();
		m_pLangBtn->SetSprite(lang_icons[gSettings.GetLanguage()]);
		gSettings.Save();
		Invalidate();
	} else if (uid == 208) {
        // autosave
        gSettings.SetEntryValue(CET_AUTOSAVE, param);
        gSettings.Save();
	} else if (uid == 209) {
		// combat grid
		gSettings.SetEntryValue(CET_COMBATGRID, param);
		gSettings.Save();
	}  else if (uid == 210) {
		// sticky path
		gSettings.SetEntryValue(CET_STICKY_PATH, param);
		gSettings.Save();
	}  else if (uid == 211) {
		// one-handed mode
		gSettings.SetEntryValue(CET_ONE_HANDED_MODE, param);
		gSettings.Save();
	}  else if (uid == 212) {
		// overland grid
		gSettings.SetEntryValue(CET_OVRLANDGRID, param);
		gSettings.Save();
	}  	

}




// END OF FILE
