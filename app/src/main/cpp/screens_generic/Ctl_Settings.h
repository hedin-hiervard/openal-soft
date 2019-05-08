#ifndef PHEROES_GAME_CTL_SETTINGS_H_
#define PHEROES_GAME_CTL_SETTINGS_H_
//
// Ctl_Settings.h
// 

#pragma once


class iSettingsView: public iView, public IViewCmdHandler
{
public:
    iSettingsView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);    
    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	void OnActivate();
private:
    IViewCmdHandler*    m_pCmdHandler; 
    iIphoneBtn*         m_pLangBtn;
    iNMVolumeBar *m_heroSpeedSlider, *m_combatSpeedSlider, *m_musicVolSlider, *m_sfxVolSlider;
    iNMCheckBox	 *m_pCBEndTurn, *m_pCBQuickCombat, *m_pCBAutosave, *m_pCBAdvCombat, *m_pCBCombatGrid,
				*m_pCBStickyPath, *m_pCBOneHanded, *m_pCBOverlandGrid;
};



// END OF FILE
#endif //PHEROES_GAME_CTL_SETTINGS_H_
