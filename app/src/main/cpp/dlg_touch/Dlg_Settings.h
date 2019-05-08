#ifndef PHEROES_GAME_DLG_SETTINGS_H_
#define PHEROES_GAME_DLG_SETTINGS_H_

class iSettingsDlg : public iBaseGameDlg
{
public:
	iSettingsDlg(iViewMgr* pViewMgr, bool showSysMenu);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	void UpdateSliderLabel(uint32 sliderId);
	bool m_sysMenu;

private:
	iPHMiniSlider*	m_sfxVolumeSlider;
	iPHMiniSlider*	m_dispGammaSlider;
	iPHMiniSlider*	m_mapScrollSlider;
	iPHMiniSlider*	m_heroSpeedSlider;
	iPHMiniSlider*	m_combatSpeedSlider;
	
	iPHLabel*		m_sfxVolumeLabel;
	iPHLabel*		m_dispGammaLabel;
	iPHLabel*		m_mapScrollLabel;
	iPHLabel*		m_heroSpeedLabel;
	iPHLabel*		m_combatSpeedLabel;
	iCheckBox*		m_checkBoxes[9];
};


#endif //PHEROES_GAME_DLG_SETTINGS_H_
